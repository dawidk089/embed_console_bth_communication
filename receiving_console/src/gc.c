#include "gc.h"
#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>

//gc
void ** gc_pointers_collector = NULL;
uint32_t gc_n_pointer = 0;

//add new allocated pointer
bool gc_add(void ** pointer) {
	gc_pointers_collector = (void**)realloc(gc_pointers_collector, ++gc_n_pointer * sizeof(void*));
	if (gc_pointers_collector == NULL) return false;
	gc_pointers_collector[gc_n_pointer - 1] = *pointer;
	return true;
}

//change pointer when change destination (realloc)
bool gc_update(void ** old_pointer, void ** new_pointer) {
	uint32_t i;
	for (i = 0; i<gc_n_pointer; i++)
		if (gc_pointers_collector[i] == *old_pointer) {
			gc_pointers_collector[i] = *new_pointer;
			return true;
		}
	return false;
}

//free all allocate pointers
void gc_free() {
	uint32_t i;

	for (i = 0; i<gc_n_pointer; i++) {
		free(gc_pointers_collector[i]);
		gc_pointers_collector[i] = NULL;
	}
	gc_pointers_collector = NULL;
	gc_n_pointer = 0;
}
//end gc
