/* Verify that the optional shell observer is inert on ordinary X servers. */
#include <stdio.h>
#include <stdlib.h>

#include <X11/Intrinsic.h>
#include <X11/StringDefs.h>
#include <X11/Shell.h>

#include "XlibreDpiI.h"

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;

    shell = XtVaAppInitialize(&app, (String)"XlibreObserver", NULL, 0,
                              &argc, argv, NULL, NULL);
    XtRealizeWidget(shell);
    if (_XmXlibreDpiInstallShell(shell, NULL, NULL)) {
        fprintf(stderr, "ordinary X server was treated as XLibre\n");
        XtDestroyWidget(shell);
        XtDestroyApplicationContext(app);
        return EXIT_FAILURE;
    }
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    puts("PASS XLibre shell observer fallback");
    return EXIT_SUCCESS;
}
