#include "blue-dream.h"
/*
const char *get_language_record(const char **language, int id)
{
	int index; for (index = 0; index < LNG_RECORD_TOTAL; index++)
		if (id == index) return language[index];

	return NULL;
}

char **create_language_table(void)
{
	int total = LNG_RECORD_TOTAL;
	char **language = (char **)malloc(sizeof(char *) * total);
	if (language == NULL) return NULL;

	int index; for (index = 0; index < total; index++)
		language[index] = NULL;

	return language;
}

void release_language_table(char **language)
{
	if (language == NULL) return; 

	int index; for (index = 0; index < LNG_RECORD_TOTAL; index++)
	{
		free(language[index]);
		language[index] = NULL;
	}

	free(language);
}

void get_eng_language(char **language)
{
	language[LNG_UNKNOWN_E] = copy_append_nl("An unknown error has occured.", NULL);
	language[LNG_COM_INITIALIZE_F] = copy_append_nl("Com initialization failed.", NULL);
	language[LNG_MEMm_alloc_F] = copy_append_nl("Memory allocation failed.", NULL);
	language[LNG_WIN_CREATE_F] = copy_append_nl("Window creation failed.", NULL);

	language[LNG_D2D_CREATE_F] = copy_append_nl("Direct2D creation failed.", NULL);
	language[LNG_D2D_RTARGET_F] = copy_append_nl("Direct2D render target creation failed.", NULL);
	language[LNG_D2D_BITMAP_F] = copy_append_nl("Direct2D bitmap creation failed.", NULL);

	language[LNG_FILE_EOF] = copy_append_nl("File operation failed: End of file.", NULL);
	language[LNG_FILE_UNKNOWN_E] = copy_append_nl("File operation failed: Unknown error.", NULL);
	language[LNG_FILE_OPEN_F] = copy_append_nl("Opening file failed.", NULL);
	language[LNG_FILE_READ_F] = copy_append_nl("Reading file failed.", NULL);
	language[LNG_FILE_WRITE_F] = copy_append_nl("Writing file failed.", NULL);
}*/