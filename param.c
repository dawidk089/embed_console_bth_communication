#include "param.h"

#include "gc.h"

#include <string.h>
#include <stdlib.h>

uint8_t get_param(int argc, char **argv, PPARAM param) {
	int i;
	bool is_critical = false;
	uint8_t err_code = 0;

	if (param->name != NULL || param->address != 0 || param->cod != 0 || param->filename != NULL) err_code = 0x10;
	param->is_device_specified = false;
	param->help = false;
	param->verbose = false;
	param->silent = false;

	for (i = 1; i < argc; i++) {
        if (is_critical) break;
		if (strlen(argv[i]) != 2 || argv[i][0] != '-'){
            err_code = 0x21;
            continue;
		}
        switch (argv[i][1]){
		case 's':
			param->silent = true;
			break;
		case 'v':
			param->verbose = true;
			break;
		case 'h':
			param->help = true;
			break;
		case 'n':
			if (i + 1 > argc - 1){ err_code = 0x31; break;}//brak nastepnego argumentu
			if (strlen(argv[i+1]) == 0){ err_code = 0x32; break;} //pusty napis ""
			if(argv[i+1][0] == '-' && strlen(argv[i+1]) == 2){ err_code = 0x34; break;} //nastepny argument jest switchem
            ++i;
            param->name = (char*)calloc(strlen(argv[i]) + 1, sizeof(char));
            if (param->name == NULL){ err_code = 0x11; is_critical = true; break;}
            gc_add((void**) &(param->name));
            strcpy(param->name, argv[i]);

			break;
		case 'a':
			if (i + 1 > argc - 1) {err_code = 0x41; break;} //brak nastepnego argumentu
			if (strlen(argv[i+1]) == 0) {err_code = 0x42; break;} //pusty napis ""
			if (!is_hex_number(argv[i+1])) {err_code = 0x43; break;} //napis nie jest liczba
			++i;
			param->address = strtoll(argv[i], NULL, 16);
			break;
		case 'c':
			if (i + 1 > argc - 1) {err_code = 0x51; break;} //brak nastepnego argumentu
			if (strlen(argv[i+1]) == 0) {err_code = 0x52; break;} //pusty napis ""
			if (!is_hex_number(argv[i+1])) {err_code = 0x53; break;} //napis nie jest liczba
			++i;
			param->cod = strtol(argv[i], NULL, 16);
			break;
		/*case 'f':
			if (i + 1 > argc - 1) return false; //brak nastepnego argumentu
			if (strlen(argv[++i]) == 0) return false; //pusty napis ""
													  //napis nie jest poprawna sciezka do pliku
		*/
		default:
			err_code = 0x20;
		}
	}
	if (param->address != 0 || param->name != NULL || param->cod != 0) param->is_device_specified = true;
	return err_code;
}

void show_help() {
	printf(
		"\n"
		"Weather station by Dawid Karminski v1.0 10/2016\n"
		"interface to downloading data over bluetooth\n"
		"\n"
		"weatherstationDK.exe [ -v | -s | -h | -a <address> | -c <COD> | -n <name> | -f <filepath> ]\n"
		"\n"
		"to be continue...\n" //#TODO
		"\n"
	);
}
