/* Bounded Motif drag-initiator cancellation regression. */
#include <Xm/DragC.h>
#include <Xm/Label.h>
#include <Xm/Xm.h>
#include <X11/extensions/XTest.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static XtAppContext app;
static Display *display;
static Widget source;
static Widget context;
static Boolean finished;
static int result = EXIT_SUCCESS;
static unsigned checks;

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        result = EXIT_FAILURE; \
    } else { \
        ++checks; \
    } \
} while (0)

static void
drag_finished(Widget widget, XtPointer closure, XtPointer call_data)
{
    XmDragDropFinishCallback callback =
        (XmDragDropFinishCallback)call_data;

    (void)widget;
    (void)closure;
    CHECK(callback != NULL && callback->reason == XmCR_DRAG_DROP_FINISH);
    finished = True;
    XtAppSetExitFlag(app);
}

static void
cancel_drag(XtPointer closure, XtIntervalId *id)
{
    (void)closure;
    (void)id;
    CHECK(context != NULL);
    if (context != NULL)
        XmDragCancel(context);
}

static void
cancel_timeout(XtPointer closure, XtIntervalId *id)
{
    (void)closure;
    (void)id;
    if (!finished) {
        fprintf(stderr, "drag cancellation timed out\n");
        result = EXIT_FAILURE;
    }
    XtAppSetExitFlag(app);
}

static Boolean
convert(Widget widget, Atom *selection, Atom *target, Atom *type_return,
        XtPointer *value_return, unsigned long *length_return,
        int *format_return)
{
    (void)widget;
    (void)selection;
    (void)target;
    (void)type_return;
    (void)value_return;
    (void)length_return;
    (void)format_return;
    return False;
}

static void
start_drag(XtPointer closure, XtIntervalId *id)
{
    XEvent event;
    Arg args[3];
    Cardinal count = 0;
    Window root;
    Window child;
    int source_x;
    int source_y;

    (void)closure;
    (void)id;
    root = RootWindowOfScreen(XtScreen(source));
    XTranslateCoordinates(display, XtWindow(source), root, 0, 0,
                          &source_x, &source_y, &child);
    XWarpPointer(display, None, root, 0, 0, 0, 0,
                 source_x + 10, source_y + 10);
    XTestFakeButtonEvent(display, Button1, True, CurrentTime);
    XSync(display, False);

    memset(&event, 0, sizeof(event));
    event.type = ButtonPress;
    event.xbutton.display = display;
    event.xbutton.window = XtWindow(source);
    event.xbutton.root = root;
    event.xbutton.time = CurrentTime;
    event.xbutton.x = 10;
    event.xbutton.y = 10;
    event.xbutton.x_root = source_x + 10;
    event.xbutton.y_root = source_y + 10;
    event.xbutton.button = Button1;
    event.xbutton.same_screen = True;

    XtSetArg(args[count], XmNnumExportTargets, 0);
    ++count;
    XtSetArg(args[count], XmNconvertProc, convert);
    ++count;
    XtSetArg(args[count], XmNdragOperations, XmDROP_COPY);
    ++count;
    context = XmDragStart(source, &event, args, count);
    CHECK(context != NULL);
    if (context != NULL)
        XtAddCallback(context, XmNdragDropFinishCallback, drag_finished, NULL);
    XtAppAddTimeOut(app, 50, cancel_drag, NULL);
    XTestFakeButtonEvent(display, Button1, False, CurrentTime);
    XFlush(display);
}

int
main(int argc, char **argv)
{
    Widget shell;
    XmString label;
    int event_base;
    int error_base;
    int major;
    int minor;

    shell = XtVaAppInitialize(&app, "DndCancel", NULL, 0,
                              &argc, argv, NULL,
                              XmNx, 20, XmNy, 20,
                              XmNwidth, 180, XmNheight, 80, NULL);
    display = XtDisplay(shell);
    source = XtVaCreateManagedWidget("source", xmLabelWidgetClass,
                                     shell, NULL);
    label = XmStringCreateLocalized("Drag source");
    XtVaSetValues(source, XmNlabelString, label, NULL);
    XmStringFree(label);
    CHECK(XTestQueryExtension(display, &event_base, &error_base,
                              &major, &minor) != 0);
    (void)event_base;
    (void)error_base;
    (void)major;
    (void)minor;
    XtRealizeWidget(shell);
    XtAppAddTimeOut(app, 20, start_drag, NULL);
    XtAppAddTimeOut(app, 3000, cancel_timeout, NULL);
    XtAppMainLoop(app);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    if (!finished)
        result = EXIT_FAILURE;
    if (result == EXIT_SUCCESS)
        printf("PASS Motif drag cancellation: %u checks\n", checks);
    return result;
}
