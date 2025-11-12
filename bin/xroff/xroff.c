/*
 * xv7 roff
 * Copyright (c) 2025 Vladislav Prokopenko
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to
 * deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or
 * sell copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS
 * IN THE SOFTWARE.
 */

#include <ctype.h>
#include <err.h>
#include <fcntl.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 512 /* default buffer size */
#define MAX_LINES                                                             \
  1024 /* maximum line number.                                                \
        * used to allocate buf for                                            \
        * reading.                                                            \
        */

struct fmtstate
{
  char buf[BUF_SIZE]; /* current line being built */
  int buf_pos;        /* position in buf[] */
  int line_width;     /* maximum width of line */
};

struct fmtstate state = {
  .buf_pos = 0,
  .line_width = 72 // Default to 72 characters
};

/*
 * print output and reset variables
 */
void
roff_flush ()
{
  if (state.buf_pos > 0)
    {
      state.buf[state.buf_pos] = '\0';
      printf ("%s\n", state.buf);
      state.buf_pos = 0;
    }
}

/*
 * trim trailing whitespace
 */
char *
roff_trim (char *s)
{
  char *end;
  while (isspace ((unsigned char)*s))
    s++;
  if (*s == 0)
    return s;
  end = s + strlen (s) - 1;
  while (end > s && isspace ((unsigned char)*end))
    end--;
  *(end + 1) = '\0';
  return s;
}

/*
 * add text to buf
 */
void
roff_add (const char *text)
{
  const char *word = text;

  while (*word)
    {
      while (*word && isspace ((unsigned char)*word))
        word++;
      /* end of string */
      if (!*word)
        break;

      const char *word_end = word;
      while (*word_end && !isspace ((unsigned char)*word_end))
        word_end++;
      int word_len = word_end - word;

      int required_space = state.buf_pos > 0 ? word_len + 1 : word_len;

      if (state.buf_pos + required_space > state.line_width)
        roff_flush ();

      if (state.buf_pos > 0)
        state.buf[state.buf_pos++] = ' ';

      if (state.buf_pos + word_len < BUF_SIZE)
        {
          strncpy (state.buf + state.buf_pos, word, word_len);
          state.buf_pos += word_len;
        }

      word = word_end;
    }
}

/*
 * process a macro
 */
void
roff_process (char *macro, char arg_trimmed[])
{
  if (strcmp (macro, "sp") == 0)
    {
      roff_flush ();
      int num_lines = atoi (roff_trim (arg_trimmed));
      if (num_lines == 0)
        num_lines = 1;
      for (int i = 0; i < num_lines; i++)
        {
          printf ("\n");
        }
    }
  else if (strcmp (macro, "br") == 0)
    {
      printf ("\n");
      roff_flush ();
    }
  else if (strcmp (macro, "\"") == 0)
  {
      return;
  }
  else
    {
      printf ("unhandled macro .%s", macro);
    }
}

/*
 * Parse file.
 */
void
roff_parse (FILE *fp)
{
  char line[MAX_LINES];
  int line_no = 0;

  while (fgets (line, sizeof (line), fp))
    {
      line_no++;
      char *s = roff_trim (line);

      if (*s == '\0')
        {
          roff_flush ();
          printf ("\n");
          continue;
        }

      if (s[0] == '.')
        {
          char macro[3] = { 0 };
          char *arg_start = NULL;
          char arg_trimmed[MAX_LINES] = { 0 };

          if (strlen (s) >= 3)
            {
              macro[0] = s[1];
              macro[1] = s[2];
              arg_start = s + 3;

              while (*arg_start != '\0' && isspace ((unsigned char)*arg_start))
                {
                  arg_start++;
                }
              strcpy (arg_trimmed, arg_start);
            }
          roff_process (macro, arg_trimmed);
        }
      else
        {
          /* it's text */
          roff_add (s);
        }
    }

  // Flush any remaining text at the end of the file
  roff_flush ();
}

/*
 * Main function.
 * open the file and start processing
 */
int
main (int argc, char **argv)
{
  if (argc < 2)
    err (1, "argc < 2");

  FILE *file = fopen (argv[1], O_RDONLY);
  if (file == NULL)
    err (1, "file == NULL");
  roff_parse (file);
  fclose (file);
  return 0;
}
