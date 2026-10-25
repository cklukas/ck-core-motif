/* Installed-consumer regression for XmTabBox's live appearance redisplay. */
#include <Xm/Appearance.h>
#include <Xm/TabBox.h>
#include <Xm/Xm.h>
#include <X11/Xlib.h>

#include <stdio.h>
#include <stdlib.h>

static unsigned x_errors;

static int
x_error(Display *display, XErrorEvent *event)
{
    (void)display;
    (void)event;
    ++x_errors;
    return 0;
}

static void
drain_events(XtAppContext app)
{
    while (XtAppPending(app) & XtIMXEvent)
        XtAppProcessEvent(app, XtIMXEvent);
}

static int
fail(const char *message)
{
    fprintf(stderr, "tab-box redisplay: %s\n", message);
    return EXIT_FAILURE;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, tab_box;
    Display *display;

    shell = XtVaAppInitialize(&app, "TabBoxRedisplay", NULL, 0,
                              &argc, argv, NULL,
                              XmNwidth, 320, XmNheight, 120, NULL);
    display = XtDisplay(shell);
    tab_box = XmCreateTabBox(shell, (char *)"tabBox", NULL, 0);
    XtManageChild(tab_box);
    XtRealizeWidget(shell);
    XSetErrorHandler(x_error);
    XClearArea(display, XtWindow(tab_box), 0, 0, 0, 0, True);
    XSync(display, False);
    drain_events(app);
    if (x_errors != 0)
        return fail("classic redisplay emitted an X error");
    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_MODERN_LIGHT))
        return fail("modern-light transaction failed");
    XSync(display, False);
    drain_events(app);
    if (x_errors != 0)
        return fail("modern-light redisplay emitted an X error");
    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_MODERN_DARK))
        return fail("modern-dark transaction failed");
    XSync(display, False);
    drain_events(app);
    if (x_errors != 0)
        return fail("modern-dark redisplay emitted an X error");
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    puts("PASS XmTabBox live appearance redisplay");
    return EXIT_SUCCESS;
}
