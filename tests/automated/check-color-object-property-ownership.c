/*
 * A client that finds the pixel-set property of the color server on the
 * selection owner window reads it in ColorCachePropertyExists, which passes
 * the Xlib buffer to FetchPixelData. FetchPixelData owns the buffer from
 * there on and releases it on every path, so the caller must not release it
 * again. This program publishes such a property, as a session's color server
 * does, and creates the first Motif shell of a client. The color object of
 * that shell reads the property while the shell is initialized.
 *
 *   check-color-object-property-ownership
 *
 * Needs $DISPLAY. Build it with AddressSanitizer: a second release of the
 * buffer is then reported where it happens. Without the property the client
 * would ask the (silent) owner for the pixel set and wait for the selection
 * timeout, so a property that was not accepted ends the program through the
 * alarm instead of letting it hang.
 */

#define _GNU_SOURCE
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xatom.h>
#include <Xm/Xm.h>
#include <Xm/PushB.h>

/* colors in use, then background, foreground, top shadow, bottom shadow and
 * select of each of the eight color sets, every field closed by an
 * underscore; the last byte is the version of the property */
static char *make_property(size_t *length)
{
    static const char set[] = "ffffff_000000_cccccc_333333_999999_";
    char *blob = malloc(2 + 8 * (sizeof set - 1) + 2);
    char *p = blob;

    memcpy(p, "2_", 2);
    p += 2;
    for (int i = 0; i < 8; i++) {
	memcpy(p, set, sizeof set - 1);
	p += sizeof set - 1;
    }
    *p++ = '1';
    *length = (size_t)(p - blob);
    return blob;
}

static void on_alarm(int sig)
{
    static const char message[] =
	"FAIL: the client did not accept the pixel-set property\n";
    (void)sig;
    if (write(2, message, sizeof message - 1) < 0)
	_exit(3);
    _exit(2);
}

int main(int argc, char **argv)
{
    Display *server = XOpenDisplay(NULL);
    Window owner;
    char *blob;
    size_t length;
    XtAppContext app;
    Widget top, button;

    if (server == NULL) {
	fprintf(stderr, "FAIL: cannot open $DISPLAY\n");
	return 1;
    }
    owner = XCreateSimpleWindow(server, DefaultRootWindow(server),
				0, 0, 1, 1, 0, 0, 0);
    blob = make_property(&length);
    XChangeProperty(server, owner, XInternAtom(server, "SDT Pixel Set", False),
		    XA_STRING, 8, PropModeReplace, (unsigned char *)blob,
		    (int)length);
    XSetSelectionOwner(server, XInternAtom(server, "Customize Data:0", False),
		       owner, CurrentTime);
    XSync(server, False);
    free(blob);

    signal(SIGALRM, on_alarm);
    alarm(60);
    top = XtVaOpenApplication(&app, "ColorOwnership", NULL, 0, &argc, argv,
			      NULL, applicationShellWidgetClass, NULL);
    button = XtVaCreateManagedWidget("button", xmPushButtonWidgetClass, top,
				     NULL);
    XtRealizeWidget(top);
    XSync(XtDisplay(top), False);
    alarm(0);
    if (button == NULL) {
	fprintf(stderr, "FAIL: no widget\n");
	return 1;
    }
    XtDestroyWidget(top);
    XtDestroyApplicationContext(app);
    XCloseDisplay(server);
    puts("PASS: the pixel-set property was read and its buffer released once");
    return 0;
}
