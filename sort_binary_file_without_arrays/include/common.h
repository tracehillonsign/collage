#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FILENAME "file.bin"

typedef struct
{
    char *data;
    size_t size;
    size_t seek_start;
} word_t;