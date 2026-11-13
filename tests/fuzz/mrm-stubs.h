/* Shared by the stand-ins and the harnesses of the resource manager targets. */
#ifndef MRM_STUBS_H
#define MRM_STUBS_H
#include <X11/Intrinsic.h>

Display *fuzz_display(void);
Screen *fuzz_screen(void);
Widget fuzz_new_widget(Widget parent);
/* The arguments of the last XtSetValues call (at most FUZZ_SET_VALUES_MAX). */
#define FUZZ_SET_VALUES_MAX 16
extern Cardinal fuzz_set_values_count;
extern Arg fuzz_set_values_args[FUZZ_SET_VALUES_MAX];

/* Frees every widget record made since the last call. */
void fuzz_free_widgets(void);

#endif
