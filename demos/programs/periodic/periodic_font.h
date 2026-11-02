#ifndef PERIODIC_FONT_H
#define PERIODIC_FONT_H
#include <X11/Intrinsic.h>
typedef void (*PeriodicFontLayoutProc)(XtPointer, double);
Boolean PeriodicInstallFontMenu(Widget root, PeriodicFontLayoutProc layout, XtPointer closure);
#endif
