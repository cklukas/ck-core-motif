/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Foreign X11 selection-owner lifecycle regression for Motif transfer helpers. */
#define _GNU_SOURCE
#include <Xm/Xm.h>
#include <Xm/TransferP.h>
#include <X11/Xatom.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } \
    ++checks; \
} while (0)

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, owner;
    Display *display, *foreign_display;
    Window foreign;
    Atom selection;
    unsigned long checks = 0;
    Dl_info loaded;
    const char *expected = getenv("CKMOTIF_EXPECT_LIBRARY");
    char *actual_path, *expected_path;

    CHECK(expected != NULL);
    CHECK(dladdr((void *)XmeNamedSource, &loaded) != 0);
    actual_path = realpath(loaded.dli_fname, NULL);
    expected_path = realpath(expected, NULL);
    CHECK(actual_path && expected_path && strcmp(actual_path, expected_path) == 0);
    printf("Loaded library: %s\n", actual_path);
    free(actual_path); free(expected_path);

    shell = XtVaAppInitialize(&app, "ExternalSelectionRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    owner = XtVaCreateManagedWidget("owner", xmPrimitiveWidgetClass, shell,
                                   XmNwidth, 64, XmNheight, 64, NULL);
    XtRealizeWidget(shell);
    display = XtDisplay(shell);
    foreign_display = XOpenDisplay(DisplayString(display));
    CHECK(foreign_display != NULL && foreign_display != display);
    selection = XInternAtom(display, "XM_EXTERNAL_OWNER_REGRESSION", False);
    CHECK(!XmeNamedSource(NULL, selection, CurrentTime));
    foreign = XCreateSimpleWindow(foreign_display, DefaultRootWindow(foreign_display),
                                  0, 0, 1, 1, 0, 0, 0);
    XSetSelectionOwner(foreign_display, selection, foreign, CurrentTime);
    XSync(foreign_display, False);
    CHECK(XGetSelectionOwner(display, selection) == foreign);

    CHECK(!XmeNamedSource(owner, None, CurrentTime));
    CHECK(XmeNamedSource(owner, selection, CurrentTime));
    XFlush(display);
    CHECK(XGetSelectionOwner(display, selection) == XtWindow(owner));

    XSetSelectionOwner(foreign_display, selection, foreign, CurrentTime);
    XSync(foreign_display, False);
    CHECK(XGetSelectionOwner(display, selection) == foreign);
    XSync(display, False);
    unsigned int processed = 0;
    while (XtAppPending(app) && processed < 1000) {
        XtAppProcessEvent(app, XtIMAll);
        ++processed;
    }
    CHECK(processed < 1000);
    XtDestroyWidget(owner);
    XSync(display, False);
    CHECK(XGetSelectionOwner(display, selection) == foreign);
    XDestroyWindow(foreign_display, foreign);
    XSync(foreign_display, False);
    CHECK(XGetSelectionOwner(display, selection) == None);

    owner = XtVaCreateManagedWidget("replacement-owner", xmPrimitiveWidgetClass,
                                   shell, XmNwidth, 64, XmNheight, 64, NULL);
    CHECK(XmeNamedSource(owner, selection, CurrentTime));
    XSync(display, False);
    CHECK(XGetSelectionOwner(foreign_display, selection) == XtWindow(owner));
    XtDestroyWidget(owner);
    XSync(display, False);
    CHECK(XGetSelectionOwner(display, selection) == None);

    XCloseDisplay(foreign_display);
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    printf("PASS independent-client selection ownership/replacement/destruction: %lu checks\n", checks);
    return 0;
}
