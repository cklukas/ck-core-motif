/* Xt must retain an executing callback list through immediate widget teardown. */
#include <Xm/DrawingA.h>
#include <X11/IntrinsicP.h>
#include <X11/ShellP.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "%s:%d: %s\n", \
    __FILE__, __LINE__, #x); exit(1); } ++checks; } while (0)
static unsigned checks, first_count, second_count, destroyed_count, mode;
static Boolean nested;
static Widget watched;

static void second(Widget w, XtPointer closure, XtPointer data)
{
    (void)closure;
    (void)data;
    CHECK(w == watched);
    ++second_count;
}

static void first(Widget w, XtPointer closure, XtPointer data)
{
    (void)closure;
    (void)data;
    ++first_count;
    if (mode == 2 && !nested) {
        nested = True;
        XtCallCallbacks(w, XtNpopupCallback, NULL);
        return;
    }
    if (mode == 3)
        XtRemoveCallback(w, XtNpopupCallback, second, NULL);
    XtDestroyWidget(w);
}

static void destroyed(Widget w, XtPointer closure, XtPointer data)
{
    (void)closure;
    (void)data;
    CHECK(w == watched);
    ++destroyed_count;
}

int main(int argc, char **argv)
{
    unsigned round;
    /* Keep the isolated server alive while independent Xt displays close;
     * otherwise Xvfb's automatic server reset races the next connection. */
    Display *anchor = XOpenDisplay(NULL);
    CHECK(anchor != NULL);
    for (round = 0; round < 20; ++round) {
        for (mode = 0; mode < 4; ++mode) {
            XtAppContext app;
            Widget shell;
            Display *display;
            shell = XtVaAppInitialize(&app, "CallbackLifetime", NULL, 0,
                                      &argc, argv, NULL, NULL);
            display = XtDisplay(shell);
            /* Force the same installed toolkit cohort as the other fixtures. */
            CHECK(XmCreateDrawingArea(shell, "child", NULL, 0) != NULL);
            watched = shell;
            first_count = second_count = destroyed_count = 0;
            nested = False;
            XtAddCallback(shell, XtNdestroyCallback, destroyed, NULL);
            XtAddCallback(shell, XtNpopupCallback, first, NULL);
            XtAddCallback(shell, XtNpopupCallback, second, NULL);
            if (mode == 1)
                XtCallCallbackList(shell, ((ShellWidget)shell)->shell.popup_callback, NULL);
            else
                XtCallCallbacks(shell, XtNpopupCallback, NULL);
            CHECK(first_count == (mode == 2 ? 2U : 1U));
            CHECK(second_count == (mode == 2 ? 2U : 1U));
            CHECK(destroyed_count == 1);
            XtCloseDisplay(display);
            XtDestroyApplicationContext(app);
        }
    }
    XCloseDisplay(anchor);
    printf("PASS direct, nested and mutating Xt callback teardown: %u checks\n", checks);
    return 0;
}
