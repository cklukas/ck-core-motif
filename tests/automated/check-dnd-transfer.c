/* Bounded same-process Motif drag/drop transfer regression. */
#include <Xm/DragC.h>
#include <Xm/DropSMgr.h>
#include <Xm/DropTrans.h>
#include <Xm/Label.h>
#include <Xm/Xm.h>
#include <X11/extensions/XTest.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static XtAppContext app;
static Display *display;
static Widget source;
static Widget destination;
static Atom target_atom;
static XmDropTransferEntryRec transfer_entry;
static const char payload[] = "drag payload";
static const char transfer_cookie[] = "transfer cookie";
static unsigned checks;
static Boolean finished;
static int result = EXIT_SUCCESS;

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        result = EXIT_FAILURE; \
    } else { \
        ++checks; \
    } \
} while (0)

static void
finish(XtPointer closure, XtIntervalId *id)
{
    (void)closure;
    (void)id;
    if (!finished) {
        fprintf(stderr, "drag transfer timed out\n");
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
    if (target == NULL || *target != target_atom || type_return == NULL ||
        value_return == NULL || length_return == NULL || format_return == NULL)
        return False;
    *type_return = target_atom;
    *value_return = XtNewString(payload);
    *length_return = strlen(payload);
    *format_return = 8;
    return True;
}

static void
transfer(Widget widget, XtPointer client_data, Atom *selection, Atom *type,
         XtPointer value, unsigned long *length, int *format)
{
    (void)widget;
    (void)selection;
    CHECK(client_data == (XtPointer)transfer_cookie);
    CHECK(type != NULL && *type == target_atom);
    CHECK(value != NULL && length != NULL && *length == strlen(payload));
    CHECK(format != NULL && *format == 8);
    CHECK(value != NULL && length != NULL &&
          memcmp(value, payload, *length) == 0);
    /* Xt owns the returned selection value and releases it after this
     * callback.  The consumer must not free it here. */
    finished = True;
    XtAppAddTimeOut(app, 50, finish, NULL);
}

static void
drop(Widget widget, XtPointer closure, XtPointer call_data)
{
    XmDropProcCallback drop_data = (XmDropProcCallback)call_data;
    Arg args[3];
    Cardinal count = 0;

    (void)widget;
    (void)closure;
    CHECK(drop_data != NULL && drop_data->dragContext != NULL);
    CHECK(drop_data != NULL &&
          drop_data->dropSiteStatus == XmVALID_DROP_SITE);
    CHECK(drop_data != NULL && (drop_data->operation & XmDROP_COPY) != 0);

    transfer_entry.client_data = (XtPointer)transfer_cookie;
    transfer_entry.target = target_atom;
    XtSetArg(args[count], XmNdropTransfers, &transfer_entry);
    ++count;
    XtSetArg(args[count], XmNnumDropTransfers, 1);
    ++count;
    XtSetArg(args[count], XmNtransferProc, transfer);
    ++count;
    if (drop_data == NULL || drop_data->dragContext == NULL ||
        XmDropTransferStart(drop_data->dragContext, args, count) == NULL)
        result = EXIT_FAILURE;
}

static void
start_drag(XtPointer closure, XtIntervalId *id)
{
    XEvent event;
    Arg args[4];
    Cardinal count = 0;
    Window root;
    Window child;
    int source_x;
    int source_y;
    int destination_x;
    int destination_y;
    Widget context;

    (void)closure;
    (void)id;
    root = RootWindowOfScreen(XtScreen(source));
    XTranslateCoordinates(display, XtWindow(source), root, 0, 0,
                          &source_x, &source_y, &child);
    XTranslateCoordinates(display, XtWindow(destination), root, 0, 0,
                          &destination_x, &destination_y, &child);
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

    XtSetArg(args[count], XmNexportTargets, &target_atom);
    ++count;
    XtSetArg(args[count], XmNnumExportTargets, 1);
    ++count;
    XtSetArg(args[count], XmNconvertProc, convert);
    ++count;
    XtSetArg(args[count], XmNdragOperations, XmDROP_COPY);
    ++count;
    context = XmDragStart(source, &event, args, count);
    CHECK(context != NULL);
    XTestFakeMotionEvent(display, -1, destination_x + 20,
                         destination_y + 20, CurrentTime);
    XTestFakeButtonEvent(display, Button1, False, CurrentTime);
    XFlush(display);
}

int
main(int argc, char **argv)
{
    Widget source_shell;
    Widget destination_shell;
    XmString label;
    Arg args[4];
    Cardinal count;
    int event_base;
    int error_base;
    int major;
    int minor;

    source_shell = XtVaAppInitialize(&app, "DndTransfer", NULL, 0,
                                     &argc, argv, NULL,
                                     XmNx, 20, XmNy, 20,
                                     XmNwidth, 180, XmNheight, 80, NULL);
    display = XtDisplay(source_shell);
    destination_shell = XtVaAppCreateShell(
        "destination", "DndTransfer", topLevelShellWidgetClass, display,
        XmNx, 320, XmNy, 20, XmNwidth, 180, XmNheight, 80, NULL);
    source = XtVaCreateManagedWidget("source", xmLabelWidgetClass,
                                     source_shell, NULL);
    label = XmStringCreateLocalized("Drag source");
    XtVaSetValues(source, XmNlabelString, label, NULL);
    XmStringFree(label);
    destination = XtVaCreateManagedWidget("destination", xmLabelWidgetClass,
                                          destination_shell, NULL);
    label = XmStringCreateLocalized("Drop target");
    XtVaSetValues(destination, XmNlabelString, label, NULL);
    XmStringFree(label);
    target_atom = XInternAtom(display, "UTF8_STRING", False);

    CHECK(XTestQueryExtension(display, &event_base, &error_base,
                              &major, &minor) != 0);
    (void)event_base;
    (void)error_base;
    (void)major;
    (void)minor;
    XtRealizeWidget(source_shell);
    XtRealizeWidget(destination_shell);

    count = 0;
    XtSetArg(args[count], XmNimportTargets, &target_atom);
    ++count;
    XtSetArg(args[count], XmNnumImportTargets, 1);
    ++count;
    XtSetArg(args[count], XmNdropSiteOperations, XmDROP_COPY);
    ++count;
    XtSetArg(args[count], XmNdropProc, drop);
    ++count;
    XmDropSiteRegister(destination, args, count);
    CHECK(XmDropSiteRegistered(destination));
    XtAppAddTimeOut(app, 20, start_drag, NULL);
    XtAppAddTimeOut(app, 3000, finish, NULL);
    XtAppMainLoop(app);

    XtDestroyWidget(destination_shell);
    XtDestroyWidget(source_shell);
    XtDestroyApplicationContext(app);
    if (!finished)
        result = EXIT_FAILURE;
    if (result == EXIT_SUCCESS)
        printf("PASS same-process Motif drag/drop transfer: %u checks\n",
               checks);
    return result;
}
