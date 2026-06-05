#pragma once

#include <stdio.h>

#define STACK_SIZE 5

typedef struct {
	int data[STACK_SIZE];
	int top;
} stack_t;