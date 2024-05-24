#include "blue-dream.h"

char *create_string(int length)
{
	if (length == 0) return NULL;

	char *x = (char *)malloc((length + 1) * sizeof(char));
	if (x == NULL) return NULL;

	int index;
	for (index = 0; index < length; index++)
		x[index] = ' ';

	x[length] = '\0';
	return x;
}

char *hex_string(int x)
{
	int y_length = snprintf(NULL, 0, "%#010x", x);
	char *y = (char *)malloc((y_length + 1) * sizeof(char));

	int result = snprintf(y, y_length + 1, "%#010x", x);
	if (result <= 0) return NULL;

	return y;
}

char *int_string(int x)
{
	int y_length = snprintf(NULL, 0, "%d", x);
	char *y = (char *)malloc((y_length + 1) * sizeof(char));

	int result = snprintf(y, y_length + 1, "%d", x);
	if (result <= 0) return NULL;

	return y;
}

char *float_string(float x)
{
	int y_length = snprintf(NULL, 0, "%f", x);
	char *y = (char *)malloc((y_length + 1) * sizeof(char));

	int result = snprintf(y, y_length + 1, "%f", x);
	if (result <= 0) return NULL;

	return y;
}

void clone_string(const char *x, char *y)
{
	size_t length = strlen(x);
	memmove(y, x, length);
}

char *copy_string(const char *x)
{
	if (x == NULL) return NULL;

	char *y;

	size_t length = strlen(x);
	y = create_string((int)length);
	if (y == NULL) return NULL;

	memmove(y, x, length);
	return (y);
}

char *copy_append_string(const char *x, const char *y)
{
	if (x == NULL || y == NULL)
		return NULL;

	size_t x_length = strlen(x);
	size_t y_length = strlen(y);

	size_t length = (int)(x_length + y_length);
	char *z = create_string((int)length);
	if (z == NULL) return NULL;

	int index;
	for (index = 0; index < x_length; index++)
		z[index] = x[index];

	int offset = index;
	for (index = 0; index < y_length; index++)
		z[offset + index] = y[index];

	return z;
}

char *append_string_x(char *x, const char *y)
{
	if (x == NULL || y == NULL)
		return NULL;

	size_t x_length = strlen(x);
	size_t y_length = strlen(y);

	int length = (int)(x_length + y_length);
	char *z = create_string(length);
	if (z == NULL) return NULL;

	int index;
	for (index = 0; index < x_length; index++)
		z[index] = x[index];

	int offset = index;
	for (index = 0; index < y_length; index++)
		z[offset + index] = y[index];

	free(x);
	x = NULL;

	return z;
}

char *append_string_y(const char *x, char *y)
{
	if (x == NULL || y == NULL)
		return NULL;

	size_t x_length = strlen(x);
	size_t y_length = strlen(y);

	int length = (int)(x_length + y_length);
	char *z = create_string(length);

	int index;
	for (index = 0; index < x_length; index++)
		z[index] = x[index];

	int offset = index;
	for (index = 0; index < y_length; index++)
		z[offset + index] = y[index];

	free(y);
	y = NULL;

	return z;
}

char *append_string_xy(char *x, char *y)
{
	if (x == NULL || y == NULL)
		return NULL;

	size_t x_length = strlen(x);
	size_t y_length = strlen(y);

	int length = (int)(x_length + y_length);
	char *z = create_string(length);

	int index;
	for (index = 0; index < x_length; index++)
		z[index] = x[index];

	int offset = index;
	for (index = 0; index < y_length; index++)
		z[offset + index] = y[index];

	free(x);
	free(y);

	x = NULL;
	y = NULL;

	return z;
}

char *copy_append_nl(const char *v, const char *w)
{
	if (v == NULL) return NULL;

	char *x = NULL;

	if (w == NULL) x = copy_string(v);
	else x = copy_append_string(v, w);

	if (x == NULL) return NULL;

    x = append_string_x(x, "\n");
	if (x == NULL) return NULL;

	return x;
}

char *copy_append_nl_hex(const char *v, const char *w, int x)
{
	if (v == NULL) return NULL;

	char *y = NULL;
	
	if (w == NULL) y = copy_string(v);
	else y = copy_append_string(v, w);

	if (y == NULL) return NULL;

	char *z = hex_string(x);
	if (z == NULL) return NULL;

	y = append_string_xy(y, z);
	if (y == NULL) return NULL;

	y = append_string_x(y, "\n");
	if (y == NULL) return NULL;

	return y;
}

char *copy_append_nl_int(const char *v, const char *w, int x)
{
	if (v == NULL) return NULL;

	char *y = NULL;

	if (w == NULL) y = copy_string(v);
	else y = copy_append_string(v, w);

	if (y == NULL) return NULL;

	char *z = int_string(x);
	if (z == NULL) return NULL;

	y = append_string_xy(y, z);
	if (y == NULL) return NULL;

	y = append_string_x(y, "\n");
	if (y == NULL) return NULL;

	return y;
}

char *copy_append_nl_float(const char *v, const char *w, float x)
{
	if (v == NULL) return NULL;

	char *y = NULL;

	if (w == NULL) y = copy_string(v);
	else y = copy_append_string(v, w);

	if (y == NULL) return NULL;

	char *z = float_string(x);
	if (z == NULL) return NULL;

	y = append_string_xy(y, z);
	if (y == NULL) return NULL;

	y = append_string_x(y, "\n");
	if (y == NULL) return NULL;

	return y;
}

char *append_nl(char *w, const char *x)
{
	if (w == NULL) return NULL;
	char *z = copy_append_nl(w, x);

	free(w);
	return w;
}

char *append_nl_hex(char *w, const char *x, int y)
{
	if (w == NULL) return NULL;

	char *z = copy_append_nl_hex(w, x, y);
	if (z == NULL) return NULL;

	free(w);
	return z;
}

char *append_nl_int(char *w, const char *x, int y)
{
	if (w == NULL) return NULL;

	char *z = copy_append_nl_int(w, x, y);
	if (z == NULL) return NULL;

	free(w);
	return z;
}

char *append_nl_float(char *w, const char *x, float y)
{
	if (w == NULL) return NULL;

	char *z = copy_append_nl_float(w, x, y);
	if (z == NULL) return NULL;

	free(w);
	return z;
}

int append_log(const char *filename, const char *x)
{
	if (x == NULL) return -1;

	FILE *file = fopen(filename, "a");
	if (file == NULL) return -2;

	size_t size = strlen(x);
	size_t result = fwrite(x, sizeof(char), size, file);

	if (feof(file))
	{
		fclose(file);
		return -3;
	}

	if (ferror(file))
	{
		fclose(file);
		return -4;
	}

	if (NOT_EQUAL(result, size))
	{
		fclose(file);
		return -5;
	}

	fclose(file);
	return 0;
}

int append_log_nl(const char *filename, const char *x)
{
	char *z = copy_append_nl(x, NULL);
	if (z == NULL) return NULL;

	int result = append_log(filename, z);
	free(z);

	return result;
}

int append_log_nl_hex(const char *filename, const char *x, int y)
{
	char *z = copy_append_nl_hex(x, NULL, y);
	if (z == NULL) return NULL;

	int result = append_log(filename, z);
	free(z);

	return result;
}

int append_log_nl_int(const char *filename, const char *x, int y)
{
	char *z = copy_append_nl_int(x, NULL, y);
	if (z == NULL) return NULL;

	int result = append_log(filename, z);
	free(z);

	return result;
}

int append_log_nl_float(const char *filename, const char *x, float y)
{
	char *z = copy_append_nl_float(x, NULL, y);
	if (z == NULL) return NULL;

	int result = append_log(filename, z);
	free(z);

	return result;
}

int log_hex(const char *x, int y)
{
	char *z = copy_append_nl_hex(x, NULL, y);
	if (z == NULL) return NULL;

	int result = append_log(DIAGNOSTICS_LOG, z);
	free(z);

	return result;
}

int log_int(const char *x, int y)
{
	char *z = copy_append_nl_int(x, NULL, y);
	if (z == NULL) return NULL;

	int result = append_log(DIAGNOSTICS_LOG, z);
	free(z);

	return result;
}

int dump_memory(const t_byte *x, size_t size)
{
	if (x == NULL) return -1;

	FILE *file = fopen(MEMORY_DUMP, "wb");
	if (file == NULL) return -2;

	size_t result = fwrite(x, sizeof(t_byte), size, file);

	if (feof(file))
	{
		fclose(file);
		return -3;
	}

	if (ferror(file))
	{
		fclose(file);
		return -4;
	}

	if (NOT_EQUAL(result, size))
	{
		fclose(file);
		return -5;
	}

	fclose(file);
	return 0;
}