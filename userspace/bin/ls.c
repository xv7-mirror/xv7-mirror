/*
 * ls.c - lsprogram for xv7
 * Copyright (c) 2025 Vladislav Prokopenko
 *
 */
#include "types.h"
#include "stat.h"
#include "fs.h"
#include "user.h"

void
ls (char *path)
{
  int fd;
  struct stat st;
  struct dirent de;
  char buf[512], *p;

  if ((fd = open (path, 0)) < 0)
    {
      printf (2, "ls: %s does not exist\n", path);
      return;
    }

  if (fstat (fd, &st) < 0)
    {
      printf (2, "ls: could not stat %s\n", path);
      close (fd);
      return;
    }

  /*
   * It's a single file
   */
  if (st.type == T_FILE)
    {
      /* Just print the name*/
      char *name = path + strlen (path);
      while (name > path && *(name - 1) != '/')
        name--;
      printf (1, "%s\n", name);
      close (fd);
      return;
    }

  /*
   * It's a directory
   */
  while (read (fd, &de, sizeof (de)) == sizeof (de))
    {
      if (de.inum == 0)
        continue;

      /* Skip . and .. */
      if (de.name[0] == '.'
          && (de.name[1] == 0 || (de.name[1] == '.' && de.name[2] == 0)))
        continue;

      /* Build path for stat */
      strcpy (buf, path);
      p = buf + strlen (buf);
      if (*(p - 1) != '/')
        *p++ = '/';
      memmove (p, de.name, DIRSIZ);
      p[DIRSIZ] = 0;

      if (stat (buf, &st) < 0)
        {
          printf (2, "ls: cannot stat %s\n", buf);
          continue;
        }

      /* Print / if it is a directory*/
      printf (1, "%s%s ", de.name, st.type == T_DIR ? "/" : "");
    }

  close (fd);
}

/*
 * Main program
 */
int
main (int argc, char *argv[])
{
  if (argc < 2)
    ls (".");
  else
    for (int i = 1; i < argc; i++)
      ls (argv[i]);
  printf(1, "\n");
  exit ();
}