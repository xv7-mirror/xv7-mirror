// On-disk file system format.
// Both the kernel and user programs use this header file.

typedef unsigned int uint;
typedef unsigned short ushort;

#define ROOTINO 1 // root i-number
#define BSIZE 512 // block size

// Disk layout:
// [ boot block | super block | log | inode blocks |
//                                          free bit map | data blocks]
//
// mkfs computes the super block and builds an initial file system. The
// super block describes the disk layout:
struct superblock {
    uint size; // Size of file system image (blocks)
    uint nblocks; // Number of data blocks
    uint ninodes; // Number of inodes.
    uint nlog; // Number of log blocks
    uint logstart; // Block number of first log block
    uint inodestart; // Block number of first inode block
    uint bmapstart; // Block number of first free map block
    int mounttime;
    int modtime;
};

#define NDIRECT 10
#define NINDIRECT (BSIZE / sizeof(uint))
#define NDINDIRECT (NINDIRECT * NINDIRECT)
#define NTINDIRECT (NDINDIRECT * NINDIRECT)

#define MAX_DIRECT_BLOCKS NDIRECT
#define MAX_SIND_BLOCKS NINDIRECT
#define MAX_DIND_BLOCKS NDINDIRECT
#define MAX_TIND_BLOCKS NTINDIRECT

#define MAXFILE                                                                \
    (MAX_DIRECT_BLOCKS + MAX_SIND_BLOCKS + MAX_DIND_BLOCKS + MAX_TIND_BLOCKS)

// On-disk inode structure
struct dinode {
    short type; // File type
    short major; // Major device number (T_DEV only)
    short minor; // Minor device number (T_DEV only)
    short nlink; // Number of links to inode in file system
    uint size; // Size of file (bytes)
    uint addrs[NDIRECT + 3]; // Data block addresses
    int atime;
    int mtime;
};

// Inodes per block.
#define IPB (BSIZE / sizeof(struct dinode))

// Block containing inode i
#define IBLOCK(i, sb) ((i) / IPB + sb.inodestart)

// Bitmap bits per block
#define BPB (BSIZE * 8)

// Block of free map containing bit for block b
#define BBLOCK(b, sb) (b / BPB + sb.bmapstart)

// Directory is a file containing a sequence of dirent structures.
#define DIRSIZ 14

struct dirent {
    ushort inum;
    char name[DIRSIZ];
};
