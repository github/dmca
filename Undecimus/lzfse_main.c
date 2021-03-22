/*
Copyright (c) 2015-2016, Apple Inc. All rights reserved.

Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:

1.  Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.

2.  Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer
    in the documentation and/or other materials provided with the distribution.

3.  Neither the name of the copyright holder(s) nor the names of any contributors may be used to endorse or promote products derived
    from this software without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*/

// LZFSE command line tool

#include <stdio.h>

#if !defined(_POSIX_C_SOURCE) || (_POSIX_C_SOURCE < 200112L)
#  undef _POSIX_C_SOURCE
#  define _POSIX_C_SOURCE 200112L
#endif

#if defined(_MSC_VER)
#  if !defined(_CRT_NONSTDC_NO_DEPRECATE)
#    define _CRT_NONSTDC_NO_DEPRECATE
#  endif
#  if !defined(_CRT_SECURE_NO_WARNINGS)
#    define _CRT_SECURE_NO_WARNINGS
#  endif
#  if !defined(__clang__)
#    define inline __inline
#  endif
#endif

#include "lzfse.h"
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#if defined(_MSC_VER)
#  include <io.h>
#  include <windows.h>
#else
#  include <sys/time.h>
#  include <unistd.h>
#endif

static double get_time() {
#if defined(_MSC_VER)
  LARGE_INTEGER count, freq;
  if (QueryPerformanceFrequency(&freq) && QueryPerformanceCounter(&count)) {
    return (double)count.QuadPart / (double)freq.QuadPart;
  }
  return 1.0e-3 * (double)GetTickCount();
#else
  struct timeval tv;
  if (gettimeofday(&tv, 0) != 0) {
    perror("gettimeofday");
    exit(1);
  }
  return (double)tv.tv_sec + 1.0e-6 * (double)tv.tv_usec;
#endif
}

//--------------------

enum { LZFSE_ENCODE = 0, LZFSE_DECODE };

void usage(int argc, char **argv) {
  fprintf(
      stderr,
      "Usage: %s -encode|-decode [-i input_file] [-o output_file] [-h] [-v]\n",
      argv[0]);
}

#define USAGE(argc, argv)                                                      \
  do {                                                                         \
    usage(argc, argv);                                                         \
    exit(0);                                                                   \
  } while (0)
#define USAGE_MSG(argc, argv, ...)                                             \
  do {                                                                         \
    usage(argc, argv);                                                         \
    fprintf(stderr, __VA_ARGS__);                                              \
    exit(1);                                                                   \
  } while (0)

int main(int argc, char **argv) {
  const char *in_file = 0;  // stdin
  const char *out_file = 0; // stdout
  int op = -1;              // invalid op
  int verbosity = 0;        // quiet

  // Parse options
  for (int i = 1; i < argc;) {
    // no args
    const char *a = argv[i++];
    if (strcmp(a, "-h") == 0)
      USAGE(argc, argv);
    if (strcmp(a, "-v") == 0) {
      verbosity++;
      continue;
    }
    if (strcmp(a, "-encode") == 0) {
      op = LZFSE_ENCODE;
      continue;
    }
    if (strcmp(a, "-decode") == 0) {
      op = LZFSE_DECODE;
      continue;
    }

    // one arg
    const char **arg_var = 0;
    if (strcmp(a, "-i") == 0 && in_file == 0)
      arg_var = &in_file;
    else if (strcmp(a, "-o") == 0 && out_file == 0)
      arg_var = &out_file;
    if (arg_var != 0) {
      // Flag is recognized. Check if there is an argument.
      if (i == argc)
        USAGE_MSG(argc, argv, "Error: Missing arg after %s\n", a);
      *arg_var = argv[i++];
      continue;
    }

    USAGE_MSG(argc, argv, "Error: invalid flag %s\n", a);
  }
  if (op < 0)
    USAGE_MSG(argc, argv, "Error: -encode|-decode required\n");

  // Info
  if (verbosity > 0) {
    if (op == LZFSE_ENCODE)
      fprintf(stderr, "LZFSE encode\n");
    if (op == LZFSE_DECODE)
      fprintf(stderr, "LZFSE decode\n");
    fprintf(stderr, "Input: %s\n", in_file ? in_file : "stdin");
    fprintf(stderr, "Output: %s\n", out_file ? out_file : "stdout");
  }

  // Load input
  size_t out_size = 0;     // used in OUT
  int in_fd = -1;          // input file desc

  if (in_file != 0) {
    // If we have a file name, open it, and allocate the exact input size
    struct stat st;
#if defined(_WIN32)
    in_fd = open(in_file, O_RDONLY | O_BINARY);
#else
    in_fd = open(in_file, O_RDONLY);
#endif
    if (in_fd < 0) {
      perror(in_file);
      exit(1);
    }
  } else {
    in_fd = 0;
#if defined(_WIN32)
    if (setmode(in_fd, O_BINARY) == -1) {
      perror("setmode");
      exit(1);
    }
#endif
  }

  int out_fd = -1;
  if (out_file) {
#if defined(_WIN32)
    out_fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC | O_BINARY,
      S_IWRITE);
#else
    out_fd = open(out_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
#endif
    if (out_fd < 0) {
      perror(out_file);
      exit(1);
    }
  } else {
    out_fd = 1; // stdout
#if defined(_WIN32)
    if (setmode(out_fd, O_BINARY) == -1) {
      perror("setmode");
      exit(1);
    }
#endif
  }
  //  Encode/decode
  //  Compute size for result buffer; we assume here that encode shrinks size,
  //  and that decode grows by no more than 4x.  These are reasonable common-
  //  case guidelines, but are not formally guaranteed to be satisfied.

  double c0 = get_time();
  if (op == LZFSE_ENCODE) {
      fprintf(stderr, "encode not implemented yet\n");
      exit(1);
  } else {
      FILE *outfile = fdopen(out_fd, "w");
      FILE *infile = fdopen(in_fd, "r");
      int error;
      out_size = lzfse_decode_file(outfile, infile, 0, &error, NULL);
      fclose(infile);
      fclose(outfile);
  }

  double c1 = get_time();

  if (out_file != 0) {
    close(out_fd);
    out_fd = -1;
  }

  return 0; // OK
}
