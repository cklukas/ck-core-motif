/* Verify optional shell-observer fallback and supported-server registration. */
#include <stdio.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>
#include <Xm/Label.h>

#include "XlibreDpiI.h"

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return EXIT_FAILURE; \
    } \
} while (0)

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Widget child;
    double scale;
    Boolean installed;

    shell = XtVaAppInitialize(&app, (String)"XlibreObserver", NULL, 0,
                              &argc, argv, NULL, NULL);
    CHECK(!_XmXlibreDpiInstallShell(shell, NULL, NULL));
    child = XtVaCreateManagedWidget("child", xmLabelWidgetClass, shell, NULL);
    XtRealizeWidget(shell);
    installed = _XmXlibreDpiInstallShell(shell, NULL, NULL);
    if (installed) {
        if (!_XmXlibreDpiGetWidgetScale(shell, &scale) || scale <= 0.0 ||
            !_XmXlibreDpiGetWidgetScale(child, &scale) || scale <= 0.0 ||
            !_XmXlibreDpiInstallShell(shell, NULL, NULL)) {
            fprintf(stderr, "XLibre shell observer registration failed\n");
            XtDestroyWidget(shell);
            XtDestroyApplicationContext(app);
            return EXIT_FAILURE;
        }
        puts("PASS XLibre shell observer registration");
    } else {
        CHECK(!_XmXlibreDpiGetWidgetScale(shell, &scale));
        CHECK(!_XmXlibreDpiGetWidgetScale(child, &scale));
        puts("PASS XLibre shell observer fallback");
    }
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    return EXIT_SUCCESS;
}
