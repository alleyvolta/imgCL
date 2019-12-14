//
#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <limits.h>
#include <ctype.h>
#include <getopt.h>
#include <dirent.h>

#ifndef UTILSFLDR
#define UTILSFLDR

#ifdef __cplusplus
extern "C" {
#endif

int read_from_dir (char *input_dir_path);
int write_to_dir (char *output_dir_path, mode_t permissionMode);

#ifdef __cplusplus
}
#endif

#endif