#pragma once

#define _CRT_SECURE_NO_WARNINGS

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef char t_glyph;

t_glyph *create_string(int length);
t_glyph *hex_to_string(int x);
t_glyph *float_to_string(float x);

t_glyph *copy_string(const t_glyph *x);

t_glyph *append_string(const t_glyph *x, const t_glyph *y);
t_glyph *append_string_x(t_glyph *x, const t_glyph *y);
t_glyph *append_string_y(const t_glyph *x, t_glyph *y);

t_glyph *string_to_line(const t_glyph *x);

int append_line(t_glyph **x, const t_glyph *y);
int append_line_hex(t_glyph **w, const t_glyph *x, int y);
int append_line_int(t_glyph **w, const t_glyph *x, int y);
int append_line_int(t_glyph **w, const t_glyph *x, float y);

int append_log(const t_glyph *filename, const t_glyph *log);