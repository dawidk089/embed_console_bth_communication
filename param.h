#include <inttypes.h>
#include <stdbool.h>

typedef struct {
	char * name;
	uint64_t address;
	uint32_t cod;
	bool verbose;
	bool silent;
	bool help;
	char * filename;
	bool is_device_specified;
}PARAM, *PPARAM;

PPARAM get_param(int argc, char **argv);
void clear_param();
uint8_t get_err_param();
void show_help();
