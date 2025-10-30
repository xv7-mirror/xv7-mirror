#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <assert.h>
#include <sys/stat.h>

#define stat xv7_stat  // avoid clash with host struct stat
#include "types.h"
#include "fs.h"
#include "stat.h"
#include "param.h"

#ifndef static_assert
#define static_assert(a, b) do { switch (0) case 0: case (a): ; } while (0)
#endif

#define NINODES 200

// Disk layout:
// [ boot block | sb block | log | inode blocks | free bit map | data blocks ]

int nbitmap = FSSIZE/(BSIZE*8) + 1;
int ninodeblocks = NINODES / IPB + 1;
int nlog = LOGSIZE;
int nmeta;    // Number of meta blocks (boot, sb, nlog, inode, bitmap)
int nblocks;  // Number of data blocks

int fsfd;
struct superblock sb;
char zeroes[BSIZE];
uint freeinode = 1;
uint freeblock;
uint ensure_path(uint parent, char *path);

void balloc(int);
void wsect(uint, void*);
void winode(uint, struct dinode*);
void rinode(uint inum, struct dinode *ip);
void rsect(uint sec, void *buf);
uint ialloc(ushort type);
void iappend(uint inum, void *p, int n);
void add_dir(uint parent, const char *path, const char *name);
void add_path(uint parent, const char *path, const char *name);

// convert to intel byte order
ushort
xshort(ushort x)
{
  ushort y;
  uchar *a = (uchar*)&y;
  a[0] = x;
  a[1] = x >> 8;
  return y;
}

uint
xint(uint x)
{
  uint y;
  uchar *a = (uchar*)&y;
  a[0] = x;
  a[1] = x >> 8;
  a[2] = x >> 16;
  a[3] = x >> 24;
  return y;
}

int
main(int argc, char *argv[])
{
  int i;
  uint rootino, off;
  struct dirent de;
  char buf[BSIZE];
  struct dinode din;


  static_assert(sizeof(int) == 4, "Integers must be 4 bytes!");

  if(argc < 2){
    fprintf(stderr, "Usage: mkfs fs.img files...\n");
    exit(1);
  }

  assert((BSIZE % sizeof(struct dinode)) == 0);
  assert((BSIZE % sizeof(struct dirent)) == 0);

  fsfd = open(argv[1], O_RDWR|O_CREAT|O_TRUNC, 0666);
  if(fsfd < 0){
    perror(argv[1]);
    exit(1);
  }

  // 1 fs block = 1 disk sector
  nmeta = 2 + nlog + ninodeblocks + nbitmap;
  nblocks = FSSIZE - nmeta;

  sb.size = xint(FSSIZE);
  sb.nblocks = xint(nblocks);
  sb.ninodes = xint(NINODES);
  sb.nlog = xint(nlog);
  sb.logstart = xint(2);
  sb.inodestart = xint(2+nlog);
  sb.bmapstart = xint(2+nlog+ninodeblocks);

  printf("nmeta %d (boot, super, log blocks %u inode blocks %u, bitmap blocks %u) blocks %d total %d\n",
         nmeta, nlog, ninodeblocks, nbitmap, nblocks, FSSIZE);

  freeblock = nmeta;     // the first free block that we can allocate

  for(i = 0; i < FSSIZE; i++)
    wsect(i, zeroes);

  memset(buf, 0, sizeof(buf));
  memmove(buf, &sb, sizeof(sb));
  wsect(1, buf);

  rootino = ialloc(T_DIR);
  assert(rootino == ROOTINO);

  bzero(&de, sizeof(de));
  de.inum = xshort(rootino);
  strcpy(de.name, ".");
  iappend(rootino, &de, sizeof(de));

  bzero(&de, sizeof(de));
  de.inum = xshort(rootino);
  strcpy(de.name, "..");
  iappend(rootino, &de, sizeof(de));

  #undef stat
  for(i = 2; i < argc; i++){
    struct stat st;
    char *hostpath = argv[i];

    if(stat(hostpath, &st) < 0){
      perror(hostpath);
      exit(1);
    }

    char imgpath[512];
    strncpy(imgpath, hostpath, sizeof(imgpath));
    imgpath[sizeof(imgpath)-1] = 0;
    if(strncmp(imgpath, "userspace/", 10) == 0)
      memmove(imgpath, imgpath + 10, strlen(imgpath + 10) + 1);

    int ilen = strlen(imgpath);
    while(ilen > 0 && imgpath[ilen-1] == '/')
      imgpath[--ilen] = '\0';

    char *slash = strrchr(imgpath, '/');
    uint parent = rootino;
    char *basename = imgpath;
    char parentcopy[512];

    if(slash){
      int prefix_len = slash - imgpath;
      strncpy(parentcopy, imgpath, prefix_len);
      parentcopy[prefix_len] = 0;
      parent = ensure_path(rootino, parentcopy);
      basename = slash + 1;
    }

    if(basename[0] == '_')
      ++basename;

    struct dirent de;
    char buf[BSIZE];
    int fd, cc;
    uint inum;

    if(S_ISREG(st.st_mode)){
      if((fd = open(hostpath, 0)) < 0){
        perror(hostpath);
        exit(1);
      }

      inum = ialloc(T_FILE);

      bzero(&de, sizeof(de));
      de.inum = xshort(inum);
      strncpy(de.name, basename, DIRSIZ);
      iappend(parent, &de, sizeof(de));

      while((cc = read(fd, buf, sizeof(buf))) > 0)
        iappend(inum, buf, cc);

      close(fd);
    }
  }
  #define stat xv7_stat

  // fix size of root inode dir
  rinode(rootino, &din);
  off = xint(din.size);
  off = ((off/BSIZE) + 1) * BSIZE;
  din.size = xint(off);
  winode(rootino, &din);

  balloc(freeblock);

  exit(0);
}

void
wsect(uint sec, void *buf)
{
  if(lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE){
    perror("lseek");
    exit(1);
  }
  if(write(fsfd, buf, BSIZE) != BSIZE){
    perror("write");
    exit(1);
  }
}

void
winode(uint inum, struct dinode *ip)
{
  char buf[BSIZE];
  uint bn;
  struct dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((struct dinode*)buf) + (inum % IPB);
  *dip = *ip;
  wsect(bn, buf);
}

void
rinode(uint inum, struct dinode *ip)
{
  char buf[BSIZE];
  uint bn;
  struct dinode *dip;

  bn = IBLOCK(inum, sb);
  rsect(bn, buf);
  dip = ((struct dinode*)buf) + (inum % IPB);
  *ip = *dip;
}

void
rsect(uint sec, void *buf)
{
  if(lseek(fsfd, sec * BSIZE, 0) != sec * BSIZE){
    perror("lseek");
    exit(1);
  }
  if(read(fsfd, buf, BSIZE) != BSIZE){
    perror("read");
    exit(1);
  }
}

uint
ialloc(ushort type)
{
  uint inum = freeinode++;
  struct dinode din;

  bzero(&din, sizeof(din));
  din.type = xshort(type);
  din.nlink = xshort(1);
  din.size = xint(0);
  winode(inum, &din);
  return inum;
}

void
balloc(int used)
{
  uchar buf[BSIZE];
  int i;

  printf("balloc: first %d blocks have been allocated\n", used);
  assert(used < BSIZE*8);
  bzero(buf, BSIZE);
  for(i = 0; i < used; i++){
    buf[i/8] = buf[i/8] | (0x1 << (i%8));
  }
  printf("balloc: write bitmap block at sector %d\n", sb.bmapstart);
  wsect(sb.bmapstart, buf);
}

#define min(a, b) ((a) < (b) ? (a) : (b))

void
iappend(uint inum, void *xp, int n)
{
  char *p = (char*)xp;
  uint fbn, off, n1;
  struct dinode din;
  char buf[BSIZE];
  uint indirect[NINDIRECT];
  uint x;

  rinode(inum, &din);
  off = xint(din.size);
  // printf("append inum %d at off %d sz %d\n", inum, off, n);
  while(n > 0){
    fbn = off / BSIZE;
    assert(fbn < MAXFILE);
    if(fbn < NDIRECT){
      if(xint(din.addrs[fbn]) == 0){
        din.addrs[fbn] = xint(freeblock++);
      }
      x = xint(din.addrs[fbn]);
    } else {
      if(xint(din.addrs[NDIRECT]) == 0){
        din.addrs[NDIRECT] = xint(freeblock++);
      }
      rsect(xint(din.addrs[NDIRECT]), (char*)indirect);
      if(indirect[fbn - NDIRECT] == 0){
        indirect[fbn - NDIRECT] = xint(freeblock++);
        wsect(xint(din.addrs[NDIRECT]), (char*)indirect);
      }
      x = xint(indirect[fbn-NDIRECT]);
    }
    n1 = min(n, (fbn + 1) * BSIZE - off);
    rsect(x, buf);
    bcopy(p, buf + off - (fbn * BSIZE), n1);
    wsect(x, buf);
    n -= n1;
    off += n1;
    p += n1;
  }
  din.size = xint(off);
  winode(inum, &din);
}

uint
dirlookup(uint dirinum, const char *name)
{
  struct dinode din;
  char buf[BSIZE];
  struct dirent *de;
  int i, j;

  rinode(dirinum, &din);

  for(i = 0; i < NDIRECT; i++){
    uint addr = xint(din.addrs[i]);
    if(addr == 0)
      continue;
    rsect(addr, buf);
    de = (struct dirent*)buf;
    for(j = 0; j < BSIZE / sizeof(struct dirent); j++){
      ushort inum = xshort(de[j].inum);
      if(inum == 0)
        continue;
      char nm[DIRSIZ+1];
      memmove(nm, de[j].name, DIRSIZ);
      nm[DIRSIZ] = 0;
      int k = DIRSIZ - 1;
      while(k >= 0 && (nm[k] == 0 || nm[k] == ' ')) { nm[k] = 0; k--; }
      if(strcmp(nm, name) == 0)
        return inum;
    }
  }
  return 0;
}

uint
ensure_path(uint parent, char *path)
{
  char *tok;
  char copy[512];
  struct dirent de;
  uint inum;

  if(path == 0 || *path == 0)
    return parent;

  strncpy(copy, path, sizeof(copy));
  copy[sizeof(copy)-1] = 0;

  tok = strtok(copy, "/");
  while(tok){
    uint found = dirlookup(parent, tok);
    if(found == 0){
      inum = ialloc(T_DIR);

      bzero(&de, sizeof(de));
      de.inum = xshort(inum);
      strncpy(de.name, tok, DIRSIZ);
      iappend(parent, &de, sizeof(de));

      bzero(&de, sizeof(de));
      de.inum = xshort(inum);
      strcpy(de.name, ".");
      iappend(inum, &de, sizeof(de));

      bzero(&de, sizeof(de));
      de.inum = xshort(parent);
      strcpy(de.name, "..");
      iappend(inum, &de, sizeof(de));

      parent = inum;
    } else {
      parent = found;
    }
    tok = strtok(0, "/");
  }
  return parent;
}
