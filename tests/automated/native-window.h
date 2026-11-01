/* Window managers can process MapRequest after XSync returns (e.g. XQuartz).
 * Pixel tests must wait for viewability before using XGetImage on a window. */
#ifndef CKMOTIF_TEST_NATIVE_WINDOW_H
#define CKMOTIF_TEST_NATIVE_WINDOW_H
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <sys/select.h>
#include <stdio.h>
#include <stdlib.h>
static void NativeWaitForViewable(Widget widget)
{
    Display *display = XtDisplay(widget);
    XtAppContext app = XtWidgetToApplicationContext(widget);
    for (unsigned i = 0; i < 200; ++i) {
        XWindowAttributes attributes;
        XSync(display, False);
        while (XtAppPending(app)) XtAppProcessEvent(app, XtIMAll);
        if (XGetWindowAttributes(display, XtWindow(widget), &attributes) &&
            attributes.map_state == IsViewable) return;
        struct timeval delay = {0, 10000};
        select(0, NULL, NULL, NULL, &delay);
    }
    fprintf(stderr, "Window did not become viewable: %s\n", XtName(widget));
    exit(1);
}
#endif
