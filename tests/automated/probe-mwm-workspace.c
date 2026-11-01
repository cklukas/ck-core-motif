#define _POSIX_C_SOURCE 200809L
/* Query the real window manager's advertised protocol on a private test display.
 * Exit 0: compatible target set; 2: workspace targets absent; 1: probe failure. */
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <poll.h>
#include <stdio.h>
#include <string.h>
int main(void)
{
    Display *display = XOpenDisplay(NULL);
    if (!display) return 1;
    char selection_name[64];
    snprintf(selection_name, sizeof(selection_name), "WM_S%d", DefaultScreen(display));
    Atom selection = XInternAtom(display, selection_name, False);
    if (!XGetSelectionOwner(display, selection)) return 1;
    Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0, 1, 1, 0, 0, 0);
    Atom property = XInternAtom(display, "WORKSPACE_PROBE", False);
    XConvertSelection(display, selection, XInternAtom(display, "TARGETS", False), property, window, CurrentTime);
    XFlush(display);
    XEvent event;
    for (;;) {
        if (!XPending(display)) {
            struct pollfd descriptor = {ConnectionNumber(display), POLLIN, 0};
            if (poll(&descriptor, 1, 5000) <= 0) return 1;
        }
        XNextEvent(display, &event);
        if (event.type == SelectionNotify) break;
    }
    if (event.xselection.property == None) return 1;
    Atom type; int format; unsigned long count, remaining; unsigned char *data = NULL;
    if (XGetWindowProperty(display, window, property, 0, 4096, True, XA_ATOM,
                          &type, &format, &count, &remaining, &data) != Success ||
        type != XA_ATOM || format != 32 || remaining) return 1;
    const char *required[] = {"_MOTIF_WSM_CONNECT", "_MOTIF_WM_EXTENSIONS",
        "_MOTIF_WM_CONFIG_FORMAT", "_MOTIF_WSM_GET_STATE", "_MOTIF_WSM_SET_STATE", "_MOTIF_WSM_REGISTER_WINDOW"};
    unsigned missing = 0;
    for (unsigned i = 0; i < sizeof(required)/sizeof(required[0]); ++i) {
        Atom target = XInternAtom(display, required[i], False);
        unsigned long j;
        for (j = 0; j < count && ((Atom *)data)[j] != target; ++j) {}
        printf("%s %s\n", j == count ? "MISSING" : "PRESENT", required[i]);
        if (j == count) ++missing;
    }
    XFree(data); XDestroyWindow(display, window); XCloseDisplay(display);
    return missing ? 2 : 0;
}
