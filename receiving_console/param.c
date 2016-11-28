#include "param.h"
#include "stringcheck.h"

#include "gc.h"

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdarg.h>

#define HELP_ON_PARAM_ERROR
#define PRINT_FROM_INSIDE
#define NAME_LENGTH (16)
#define LETTER_NO (52) //must be in uint8_t
#define GET_NO_ERR (UCHAR_MAX)

static bool is_init = false;
static uint8_t err_code = 0;

#define S_NO (18)
#define V_NO (21)
#define H_NO (7)

//casesensitive
//{false, false, 'a', NULL}

static PARAM param[LETTER_NO]; //#TODO czy to bedzie wyzerowane?!

static uint8_t get_no(char dash_switch){
    if(dash_switch<'A') return GET_NO_ERR;
    else if(dash_switch<='Z') return dash_switch-'A'+26;
    else if(dash_switch<'a') return GET_NO_ERR;
    else if(dash_switch<='z') return dash_switch-'a';
    else return GET_NO_ERR;
}

//#TODO can be overwrite
bool param_add(char dash_switch, VAL_TYPE var_type, char name[NAME_LENGTH]){
    uint8_t no;
    size_t var_size;

    if(is_init){ err_code = 0x12; return false; }

    //error due to embedded switch
    if(dash_switch=='s' || dash_switch=='v' || dash_switch=='h'){ err_code = 0x23; return false; }
    if((no = get_no(dash_switch))==GET_NO_ERR){ err_code = 0x22; return false; }

    switch(var_type){
    case VAL_TYPE_BOOL: var_size= sizeof(bool); break;
    case VAL_TYPE_STRING: var_size = 0; break;
    case VAL_TYPE_UINT8: var_size = sizeof(uint8_t); break;
    case VAL_TYPE_UINT16: var_size = sizeof(uint16_t); break;
    case VAL_TYPE_UINT32: var_size = sizeof(uint32_t); break;
    case VAL_TYPE_UINT64: var_size = sizeof(uint64_t); break;
    }

    if(strlen(name)>=NAME_LENGTH){ err_code = 0x13; return false; }

    strcpy(param[no].label, name);

    param[no].var_type = var_type;
    param[no].is_defined = true;
    param[no].is_specified = false;
    param[no].value = NULL;
    param[no].var_size = var_size;

    return true;
}

//#TODO jaki wplyw ma niezainicjalizowana tablica globalna param
bool param_set(int argc, char **argv) {
	uint8_t i;
	uint8_t no = GET_NO_ERR;

	if(is_init){ err_code = 0x12; return false; }

	//init embedded switch
	no = get_no('s');
	param[no].is_defined = true;
	param[no].var_type = VAL_TYPE_BOOL;
	strcpy(param[no].label, "silent");

	no = get_no('v');
	param[no].is_defined = true;
	param[no].var_type = VAL_TYPE_BOOL;
	strcpy(param[no].label, "verbose");

	no = get_no('h');
	param[no].is_defined = true;
	param[no].var_type = VAL_TYPE_BOOL;
	strcpy(param[no].label, "help");

	no = GET_NO_ERR;

	for (i = 1; i < argc; i++) {
        uint32_t ulnumber;
        uint64_t ullnumber;

		if (strlen(argv[i]) != 2 || argv[i][0] != '-'){ //switch: ^-.$
            err_code = 0x21;
            continue; //next
		}

		if((no = get_no(argv[i][1]))==GET_NO_ERR){ err_code = 0x22; continue; }
		if(!param[no].is_defined){ err_code = 0x20; continue; }
		if(param[no].var_type != VAL_TYPE_BOOL){
            if (i + 1 > argc - 1){ err_code = 0x31; continue; } //brak nastepnego argumentu
            if (strlen(argv[i+1]) == 0){ err_code = 0x32; continue; } //pusty napis ""
            if(argv[i+1][0] == '-' && strlen(argv[i+1]) == 2){ err_code = 0x34; continue; } //nastepny argument jest switchem
            ++i;
		}

        if(param[no].var_type == VAL_TYPE_STRING)
            param[no].var_size = sizeof(char) * (strlen(argv[i]) + 1);

        if(!(param[no].value = calloc(1, param[no].var_size))){ err_code = 0x11; return false; }

        switch(param[no].var_type){
        case VAL_TYPE_STRING:
            memcpy(param[no].value, argv[i], param[no].var_size);
            break;
        case VAL_TYPE_BOOL:
            *(bool*)param[no].value = true;
            break;
        //#TODO specified to need to use param_clear to clear alloc to string values
        case VAL_TYPE_UINT8:
            if(!is_pfixnumber(argv[i])) { err_code = 0x40; continue; }
            ulnumber = strtoul(argv[i], NULL, 0);
            if(ulnumber == 0 || ulnumber >= UCHAR_MAX){ err_code = 0x51; continue; }
            *(uint8_t*)param[no].value = ulnumber;
            break;
        case VAL_TYPE_UINT16:
            if(!is_pfixnumber(argv[i])) { err_code = 0x40; continue; }
            ulnumber = strtoul(argv[i], NULL, 0);
            if(ulnumber == 0 || ulnumber >= USHRT_MAX){ err_code = 0x51; continue; }
            *(uint16_t*)param[no].value = ulnumber;
            break;
        case VAL_TYPE_UINT32:
            if(!is_pfixnumber(argv[i])) { err_code = 0x40; continue; }
            ulnumber = strtoul(argv[i], NULL, 0);
            if(ulnumber == 0 || ulnumber == ULONG_MAX){ err_code = 0x51; continue; }
            *(uint32_t*)param[no].value = ulnumber;
            break;
        case VAL_TYPE_UINT64:
            if(!is_pfixnumber(argv[i])) { err_code = 0x40; continue; }
            ullnumber = strtoull(argv[i], NULL, 0);
            if(ullnumber == 0 || ullnumber == ULLONG_MAX){ err_code = 0x51; continue; }
            *(uint64_t*)param[no].value = ullnumber;
            break;
        }
        param[no].is_specified = true;
	}

	for(i=0; i<LETTER_NO; i++){
        if(param[i].is_defined
        &&param[i].var_type == VAL_TYPE_BOOL
        &&!param[i].is_specified){
            if(!(param[i].value = calloc(1, param[i].var_size))){ err_code = 0x11; return false; }
            *(bool*)param[i].value = false;
            param[i].is_specified = true;
        }
	}

	is_init = true;

	return (!err_code ? true : false);
}

void * param_get(char name[16]){
    uint8_t i;

    if(!is_init){ err_code = 0x10; return NULL; }
    for(i=0; i<LETTER_NO; i++){
        if(!strcmp(name, param[i].label)){
            if(!param[i].is_specified) return NULL;
            else return param[i].value;
        }
    }
    err_code = 0x24;
    return NULL;
}

bool param_is_silent(){
    return *(bool*)param[S_NO].value;
}

bool param_is_verbose(){
    return *(bool*)param[V_NO].value;
}

bool param_is_help(){
    return *(bool*)param[H_NO].value;
}

void param_clear(){
    uint8_t i;

    for(i=0; i<LETTER_NO; i++)
        if(param[i].value){
            free(param[i].value);
            param[i].value = NULL;
            param[i].is_specified = false;
        }

    is_init = false;
    err_code = 0;
}

void param_cout(bool verbose, const char * format, ...){
    va_list args;

    if(!is_init){ err_code = 0x10; return; }

    if(param_is_silent()) return;
    if((!param_is_verbose() && !verbose) || param_is_verbose()){
        va_start(args, format);
        vprintf(format, args);
        va_end(args);
    }
}

uint8_t param_get_err(){
    return (!is_init ? 0x10 : err_code);
}

void param_show_help() {
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
