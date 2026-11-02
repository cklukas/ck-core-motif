/* Exercise the actual demo with real pointer events and selection transfers. */
#include <assert.h>
#include <Xm/Xm.h>
#include <X11/extensions/XTest.h>
static Widget demo_shell;
static void remember_shell(Widget shell);
static void check_demo(XtAppContext app);
#define XtRealizeWidget remember_shell
#define XtAppMainLoop check_demo
#include "../../demos/programs/drag_and_drop/simpledrop.c"
#undef XtRealizeWidget
#undef XtAppMainLoop

static void remember_shell(Widget shell)
{
    demo_shell = shell;
    XtRealizeWidget(shell);
}

static void pump(XtAppContext app, unsigned ms)
{
    /* A timer bounds the wait without busy polling. */
    Boolean done = False;
    extern void test_tick(XtPointer, XtIntervalId *);
    XtAppAddTimeOut(app, ms, test_tick, &done);
    while (!done) XtAppProcessEvent(app, XtIMAll);
}
void test_tick(XtPointer data, XtIntervalId *id)
{
    (void)id;
    *(Boolean *)data = True;
}
static void expect_label(Widget widget, const char *expected)
{
    XmString label;
    char *text = NULL;
    XtVaGetValues(widget, XmNlabelString, &label, NULL);
    assert(XmStringGetLtoR(label, XmFONTLIST_DEFAULT_TAG, &text));
    assert(strcmp(text, expected) == 0);
    XtFree(text);
}
static Position target_x, target_y;
static unsigned drag_button;
static void move_pointer(XtPointer data, XtIntervalId *id)
{
    Display *display = data;
    (void)id;
    XTestFakeMotionEvent(display, -1, target_x, target_y, CurrentTime);
    XFlush(display);
}
static void release_pointer(XtPointer data, XtIntervalId *id)
{
    Display *display = data;
    (void)id;
    XTestFakeButtonEvent(display, drag_button, False, CurrentTime);
    XFlush(display);
}
static void check_demo(XtAppContext app)
{
    Widget scale = XtNameToWidget(demo_shell, "*scale");
    Widget source = XtNameToWidget(demo_shell, "*number_source");
    Widget area = XtNameToWidget(demo_shell, "*da");
    Widget title = XtNameToWidget(demo_shell, "*drop_here");
    Display *display = XtDisplay(demo_shell);
    WidgetList children;
    Cardinal count;
    Position sx, sy, dx, dy;
    unsigned button;
    assert(scale && source && area && title);
    expect_label(title, "drop here");
    pump(app, 100);
    for (button = Button1; button <= Button2; ++button) {
        XmScaleSetValue(scale, 216 + button);
        XtCallCallbacks(scale, XmNvalueChangedCallback, NULL);
        expect_label(source, button == Button1 ?
                     "Drag number: 217 (left mouse button)" :
                     "Drag number: 218 (left mouse button)");
        XtTranslateCoords(source, 20, 10, &sx, &sy);
        XtTranslateCoords(area, 40, 40 + 40 * button, &dx, &dy);
        target_x = dx;
        target_y = dy;
        drag_button = button;
        XtAppAddTimeOut(app, 100, move_pointer, display);
        XtAppAddTimeOut(app, 250, release_pointer, display);
        XTestFakeMotionEvent(display, -1, sx, sy, CurrentTime);
        XTestFakeButtonEvent(display, button, True, CurrentTime);
        XFlush(display);
        pump(app, 600);
        XtVaGetValues(area, XmNchildren, &children, XmNnumChildren, &count, NULL);
        assert(count == button);
        expect_label(children[count - 1], button == Button1 ? "217" : "218");
    }
    puts("PASS: simpledrop primary/middle drags transfer current numbers; labels correct");
}
