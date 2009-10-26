#include"defs.h"


void fdTableInit();
void setup_IDT_entry (DESCR_INT *item, byte selector, dword offset, byte access, byte cero);

void *getTemporalStack();
