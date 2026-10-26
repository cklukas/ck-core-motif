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
    char message[128];
    XGetErrorText(display, event->error_code, message, sizeof(message));
    fprintf(stderr, "X error: %s request=%u minor=%u resource=%lu\n",
            message, event->request_code, event->minor_code, event->resourceid);
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
    /* SetValues must retain a private handle for either public font alias.
     * The caller may release its table immediately after the assignment. */
    for (unsigned int i = 0; i < 4; ++i) {
        XmRenderTable current = NULL, supplied, retained = NULL;
        XmString label;
        Dimension width = 0, height = 0;
        XtVaGetValues(tab_box, XmNrenderTable, &current, NULL);
        supplied = XmRenderTableCopy(current, NULL, 0);
        if (supplied == NULL) return fail("could not copy font table");
        XtVaSetValues(tab_box, i % 2 ? XmNfontList : XmNrenderTable, supplied, NULL);
        XtVaGetValues(tab_box, XmNrenderTable, &retained, NULL);
        if (retained == NULL || retained == supplied)
            return fail("SetValues retained a borrowed font-table handle");
        XmRenderTableFree(supplied);
        label = XmStringCreateLocalized("Tab font ownership");
        XmStringExtent(retained, label, &width, &height);
        XmStringFree(label);
        if (width == 0 || height == 0) return fail("retained table lost its font");
        XtVaSetValues(tab_box, XmNrenderTable, NULL, NULL);
        XtVaGetValues(tab_box, XmNrenderTable, &retained, NULL);
        if (retained == NULL) return fail("default table was not restored");
        XSync(display, False);
        drain_events(app);
        if (x_errors != 0) return fail("font update emitted an X error");
    }
    XtDestroyWidget(shell);
    XSync(display, False);
    if (x_errors != 0) return fail("destruction emitted an X error");
    XtDestroyApplicationContext(app);
    puts("PASS XmTabBox live appearance redisplay");
    return EXIT_SUCCESS;
}
