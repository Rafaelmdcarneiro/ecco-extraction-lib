#pragma once

#include "lstring.h"

typedef struct t_program
{
	t_glyph **language;
} t_program;

t_program *create_program(void);
void release_program(t_program *program);