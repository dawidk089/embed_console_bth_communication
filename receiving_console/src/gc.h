#include "stdbool.h"

bool gc_add(void ** pointer);
bool gc_update(void ** old_pointer, void ** new_pointer);
void gc_free();
