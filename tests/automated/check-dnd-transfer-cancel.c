/* Regression for destroying an XmDropTransfer before its start timer runs. */
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
static Boolean finished;
static Boolean transfer_called;
static Boolean active_mode;
static Boolean destroy_scheduled;
static Boolean transfer_destroyed;
static Widget transfer_object;
static unsigned transfer_calls;
static int result = EXIT_SUCCESS;
static unsigned checks;
static const char payload[] = "cancelled transfer";
#define ACTIVE_PAYLOAD_SIZE (2U * 1024U * 1024U)

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
    if (active_mode)
        CHECK(transfer_destroyed && transfer_calls > 0);
    else
        CHECK(!transfer_called);
    finished = True;
    XtAppSetExitFlag(app);
}

static void
destroy_transfer(XtPointer closure, XtIntervalId *id)
{
    (void)closure;
    (void)id;
    CHECK(transfer_object != NULL);
    if (transfer_object != NULL) {
        transfer_destroyed = True;
        XtDestroyWidget(transfer_object);
        transfer_object = NULL;
    }
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
    if (active_mode) {
        unsigned char *value = (unsigned char *)XtMalloc(ACTIVE_PAYLOAD_SIZE);
        memset(value, 'x', ACTIVE_PAYLOAD_SIZE);
        *value_return = (XtPointer)value;
        *length_return = ACTIVE_PAYLOAD_SIZE;
    } else {
        *value_return = XtNewString(payload);
        *length_return = strlen(payload);
    }
    *format_return = 8;
    return True;
}

static void
transfer(Widget widget, XtPointer client_data, Atom *selection, Atom *type,
         XtPointer value, unsigned long *length, int *format)
{
    (void)widget;
    (void)client_data;
    (void)selection;
    (void)type;
    (void)value;
    (void)length;
    (void)format;
    transfer_called = True;
    ++transfer_calls;
    if (active_mode && !destroy_scheduled) {
        destroy_scheduled = True;
        XtAppAddTimeOut(app, 0, destroy_transfer, NULL);
    }
}

static void
drop(Widget widget, XtPointer closure, XtPointer call_data)
{
    XmDropProcCallback drop_data = (XmDropProcCallback)call_data;
    Arg args[4];
    Cardinal count = 0;
    (void)widget;
    (void)closure;
    CHECK(drop_data != NULL && drop_data->dragContext != NULL);
    CHECK(drop_data != NULL &&
          drop_data->dropSiteStatus == XmVALID_DROP_SITE);
    transfer_entry.client_data = NULL;
    transfer_entry.target = target_atom;
    XtSetArg(args[count], XmNdropTransfers, &transfer_entry);
    ++count;
    XtSetArg(args[count], XmNnumDropTransfers, 1);
    ++count;
    XtSetArg(args[count], XmNtransferProc, transfer);
    ++count;
    if (active_mode) {
        XtSetArg(args[count], XmNincremental, True);
        ++count;
    }
    transfer_object = XmDropTransferStart(drop_data->dragContext,
                                          args, count);
    CHECK(transfer_object != NULL);
    if (!active_mode && transfer_object != NULL) {
        XtDestroyWidget(transfer_object);
        transfer_object = NULL;
        transfer_destroyed = True;
    }
    XtAppAddTimeOut(app, 100, finish, NULL);
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
    XtSetArg(args[count], XmNexportTargets, &target_atom); ++count;
    XtSetArg(args[count], XmNnumExportTargets, 1); ++count;
    XtSetArg(args[count], XmNconvertProc, convert); ++count;
    XtSetArg(args[count], XmNdragOperations, XmDROP_COPY); ++count;
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
    Cardinal count = 0;
    int event_base;
    int error_base;
    int major;
    int minor;

    active_mode = argc > 1 && strcmp(argv[1], "active") == 0;

    source_shell = XtVaAppInitialize(&app, "DndTransferCancel", NULL, 0,
                                     &argc, argv, NULL,
                                     XmNx, 20, XmNy, 20,
                                     XmNwidth, 180, XmNheight, 80, NULL);
    display = XtDisplay(source_shell);
    destination_shell = XtVaAppCreateShell(
        "destination", "DndTransferCancel", topLevelShellWidgetClass,
        display, XmNx, 320, XmNy, 20, XmNwidth, 180, XmNheight, 80, NULL);
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
    XtSetArg(args[count], XmNimportTargets, &target_atom); ++count;
    XtSetArg(args[count], XmNnumImportTargets, 1); ++count;
    XtSetArg(args[count], XmNdropSiteOperations, XmDROP_COPY); ++count;
    XtSetArg(args[count], XmNdropProc, drop); ++count;
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
        printf("PASS XmDropTransfer cancellation lifecycle: %u checks\n",
               checks);
    return result;
}
