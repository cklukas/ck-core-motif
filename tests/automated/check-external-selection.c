/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Foreign X11 selection-owner lifecycle regression for Motif transfer helpers. */
#include <Xm/Xm.h>
#include <Xm/TransferP.h>
#include <X11/Xatom.h>
#include <stdio.h>

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); return 1; } \
    ++checks; \
} while (0)

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, owner;
    Display *display;
    Window foreign;
    Atom selection;
    unsigned long checks = 0;

    shell = XtVaAppInitialize(&app, "ExternalSelectionRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    XtRealizeWidget(shell);
    display = XtDisplay(shell);
    selection = XInternAtom(display, "XM_EXTERNAL_OWNER_REGRESSION", False);
    foreign = XCreateSimpleWindow(display, XtWindow(shell), 0, 0, 1, 1, 0, 0, 0);
    XSetSelectionOwner(display, selection, foreign, CurrentTime);
    XFlush(display);
    CHECK(XGetSelectionOwner(display, selection) == foreign);

    owner = XtVaCreateManagedWidget("owner", xmPrimitiveWidgetClass, shell, NULL);
    CHECK(XmeNamedSource(owner, selection, CurrentTime));
    XFlush(display);
    CHECK(XGetSelectionOwner(display, selection) == XtWindow(owner));

    XtDestroyWidget(owner);
    XSync(display, False);
    CHECK(XGetSelectionOwner(display, selection) == None);

    XDestroyWindow(display, foreign);
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    printf("PASS external selection owner lifecycle: %lu checks\n", checks);
    return 0;
}
