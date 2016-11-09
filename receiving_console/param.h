#include <inttypes.h>
#include <stdbool.h>

typedef enum{
    VAR_TYPE_BOOL,
    VAR_TYPE_STRING,
    VAR_TYPE_UINT8,
    VAR_TYPE_UINT16,
    VAR_TYPE_UINT32,
    VAR_TYPE_UINT64
}VAR_TYPE;

typedef struct{
    bool is_active;
    bool is_specified;
    char dash_switch;
    void * value;
    VAR_TYPE var_type;
    size_t var_size;
    char name[16];
}PARAM, *PPARAM;

bool param_add(char dash_switch, VAR_TYPE var_type, char name[16]);
bool param_set(int argc, char **argv);
void * param_get(char name[16]);
bool param_is_silent();
bool param_is_verbose();
bool param_is_help();
void param_clear();
void param_cout(bool verbose, const char * format, ...);
uint8_t param_get_err();
void param_show_help();
