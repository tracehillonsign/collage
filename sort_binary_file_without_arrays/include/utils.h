#pragma once

#include "common.h"

FILE *fopen_or_exit(const char *filename, const char *mode);
void init_file();