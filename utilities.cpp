#include "blue-dream.h"

int read_file(const char *filename, t_byte **stream)
{
	if (filename == NULL) return -1;

	FILE *file = fopen(filename, "rb");
	if (file == NULL) return -1;

	int result = fseek(file, 0L, SEEK_END);
	if (NOT_ZERO(result))
	{
		fclose(file);
		return -2;
	}

	size_t file_size = ftell(file);
	if (file_size < 0)
	{
		fclose(file);
		return -3;
	}

	*stream = (t_byte *)malloc(sizeof(t_byte) * file_size);
	if (*stream == NULL)
	{
		fclose(file);
		return -4;
	}

	rewind(file);
	size_t total_read = fread(*stream, 1, file_size, file);

	if (ferror(file))
	{
		fclose(file);
		return -5;
	}

	if (NOT_EQUAL(total_read, file_size))
	{
		fclose(file);
		return -6;
	}

	fclose(file);
	return file_size;
}