#include "param.h"
#include "stringcheck.h"

#include "gc.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define HELP_ON_PARAM_ERROR
#define PRINT_FROM_INSIDE

#define IS_NOT_INIT (!param && !err_code)

static uint8_t err_code = 0;
PPARAM param;

PPARAM get_param(int argc, char **argv) {
	int i;
	bool is_critical = false;

	if(!(param = calloc(1, sizeof(PARAM)))){ err_code = 0x11; return NULL;}

	for (i = 1; i < argc; i++) {
        if (is_critical) break;
		if (strlen(argv[i]) != 2 || argv[i][0] != '-'){ //switch: ^-.$
            err_code = 0x21;
            continue; //next
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
			if (i + 1 > argc - 1) {err_code = 0x61; break;} //brak nastepnego argumentu
			if (strlen(argv[i+1]) == 0) {err_code = 0x62; break;} //pusty napis ""
			if(argv[i+1][0] == '-' && strlen(argv[i+1]) == 2){ err_code = 0x34; break;} //nastepny argument jest switchem
													  //napis nie jest poprawna sciezka do pliku
        */
		default:
			err_code = 0x20;
		}
	}

	if (param->help) err_code = 0x01;

	return param;
}

void clear_param(){
#if defined (HELP_ON_PARAM_ERROR) && defined (PRINT_FROM_INSIDE)
    if(!param->silent && err_code) show_help();
#endif
    if(param){
        free(param);
        param = NULL;
    }
    err_code = 0;
}

void console_out(bool verbose, const char * format, ...){
    va_list args;

    if(IS_NOT_INIT){
        err_code = 0x10;
        return;
    }
    if(param->silent) return;
    if((!param->verbose && !verbose) || param->verbose){
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}

uint8_t get_err_param(){
    return (IS_NOT_INIT ? 0x10 : err_code);
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
