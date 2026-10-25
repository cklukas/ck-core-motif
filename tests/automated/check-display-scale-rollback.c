/* Verify display-scale override rollback when transaction staging fails. */
#include <Xm/Display.h>
#include <Xm/ToggleB.h>
#include <X11/Intrinsic.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>

static int
fail(const char *message)
{
    fprintf(stderr, "display-scale rollback: %s\n", message);
    return 1;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, toggle;
    Display *display;
    double scale;

    shell = XtVaAppInitialize(&app, "DisplayScaleRollback", NULL, 0,
                              &argc, argv, NULL, NULL);
    display = XtDisplay(shell);
    toggle = XmCreateToggleButton(shell, "toggle", NULL, 0);
    XtManageChild(toggle);
    XtRealizeWidget(shell);

    if (!XmSetDisplayScaleOverride(display, 1.0) ||
        !XmGetDisplayScaleOverride(display, &scale) || scale != 1.0)
        return fail("initial override was not retained");

    if (raise(SIGUSR1) != 0)
        return fail("could not enable allocation fault");
    if (XmSetDisplayScaleOverride(display, 2.0))
        return fail("failed staging was reported as successful");
    if (raise(SIGUSR2) != 0)
        return fail("could not disable allocation fault");
    if (!XmGetDisplayScaleOverride(display, &scale) || scale != 1.0)
        return fail("set failure did not restore the previous override");

    if (!XmSetDisplayScaleOverride(display, 2.0))
        return fail("second override was not retained");
    if (raise(SIGUSR1) != 0)
        return fail("could not enable clear fault");
    if (XmClearDisplayScaleOverride(display))
        return fail("failed clear staging was reported as successful");
    if (raise(SIGUSR2) != 0)
        return fail("could not disable clear fault");
    if (!XmGetDisplayScaleOverride(display, &scale) || scale != 2.0)
        return fail("clear failure did not restore the previous override");

    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    puts("PASS WP-05.2 display-scale rollback");
    return 0;
}
