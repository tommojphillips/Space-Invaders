/* file.c 
 * Github: https:\\github.com\tommojphillips
 */
#include <stdint.h>
#include <stdio.h>

int read_file_into_buffer(const char* filename, void* buff, const uint32_t expected_size) {
	FILE* file = NULL;
	uint32_t size = 0;
	if (filename == NULL)
		return 1;

	fopen_s(&file, filename, "rb");
	if (file == NULL) {
		printf("Error: could not open file: %s\n", filename);
		return 1;
	}

	fseek(file, 0, SEEK_END);
	size = ftell(file);
	fseek(file, 0, SEEK_SET);

	if (expected_size != 0 && size != expected_size) {
		printf("Error: invalid file size. Expected %u bytes. Got %u bytes\n", expected_size, size);
		fclose(file);
		return 1;
	}

	fread(buff, 1, size, file);
	fclose(file);
	printf("Loaded %s\n", filename);
	return 0;
}
