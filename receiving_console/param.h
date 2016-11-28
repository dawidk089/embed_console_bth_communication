#include <inttypes.h>
#include <stdbool.h>

typedef enum{
    VAL_TYPE_BOOL,
    VAL_TYPE_STRING,
    VAL_TYPE_UINT8,
    VAL_TYPE_UINT16,
    VAL_TYPE_UINT32,
    VAL_TYPE_UINT64
}VAL_TYPE;

typedef struct{
    bool is_defined;
    bool is_specified;
    void * value;
    VAL_TYPE var_type;
    size_t var_size;
    char label[16];
}PARAM, *PPARAM;



bool param_add(char dash_switch, VAL_TYPE var_type, char name[16]);
bool param_set(int argc, char **argv);
//#TODO niezainicjowane moga miec losowe wartosci
void * param_get(char name[16]);
bool param_is_silent();
bool param_is_verbose();
bool param_is_help();
void param_clear();
void param_cout(bool verbose, const char * format, ...);
uint8_t param_get_err();
void param_show_help();
