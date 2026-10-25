/* Verify that a live display-scale callback cannot start a nested transaction. */
#include <Xm/Display.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <Xm/ToggleB.h>

#include <stdio.h>
#include <stdlib.h>

typedef void (*XmScaleChangeProc)(Widget widget,
                                  double old_scale,
                                  double new_scale,
                                  XtPointer closure);
extern Boolean _XmRegisterScaleChange(Widget widget,
                                      XmScaleChangeProc proc,
                                      XtPointer closure);

static Display *callback_display;
static Boolean callback_called;
static Boolean nested_result;

static void
attempt_nested_scale(Widget widget, double old_scale, double new_scale,
                     XtPointer closure)
{
    (void)widget;
    (void)old_scale;
    (void)new_scale;
    (void)closure;
    if (callback_called)
        return;
    callback_called = True;
    nested_result = XmSetDisplayScaleOverride(callback_display, 3.0);
}

static int
fail(const char *message)
{
    fprintf(stderr, "display-scale reentrancy: %s\n", message);
    return EXIT_FAILURE;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Widget toggle;
    Display *display;
    double scale;

    shell = XtVaAppInitialize(&app, (String)"DisplayScaleReentrant", NULL, 0,
                              &argc, argv, NULL, NULL);
    display = XtDisplay(shell);
    callback_display = display;
    toggle = XmCreateToggleButton(shell, (String)"toggle", NULL, 0);
    XtManageChild(toggle);
    XtRealizeWidget(shell);
    if (!_XmRegisterScaleChange(toggle, attempt_nested_scale, NULL))
        return fail("could not register callback");
    if (!XmSetDisplayScaleOverride(display, 2.0))
        return fail("outer scale transaction failed");
    if (!callback_called)
        return fail("scale callback was not called");
    if (nested_result)
        return fail("nested scale transaction was accepted");
    if (!XmGetDisplayScaleOverride(display, &scale) || scale != 2.0)
        return fail("outer scale override was not authoritative");
    if (!XmClearDisplayScaleOverride(display))
        return fail("outer override did not clear");
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    puts("PASS WP-05.2 display-scale reentrancy");
    return EXIT_SUCCESS;
}
