/* Bounded Motif drag/drop regression for more than one transfer target. */
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
static Atom targets[2];
static XmDropTransferEntryRec transfer_entries[2];
static unsigned transfer_count;
static Boolean transfer_seen[2];
static Boolean finished;
static int result = EXIT_SUCCESS;
static unsigned checks;
static const char payloads[2][16] = { "first target", "second target" };
static const char cookies[2][16] = { "first cookie", "second cookie" };

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
    CHECK(transfer_count == 2);
    finished = True;
    XtAppSetExitFlag(app);
}

static Boolean
convert(Widget widget, Atom *selection, Atom *target, Atom *type_return,
        XtPointer *value_return, unsigned long *length_return,
        int *format_return)
{
    unsigned index;

    (void)widget;
    (void)selection;
    if (target == NULL || type_return == NULL || value_return == NULL ||
        length_return == NULL || format_return == NULL)
        return False;
    for (index = 0; index < 2; ++index)
        if (*target == targets[index])
            break;
    if (index == 2)
        return False;
    *type_return = targets[index];
    *value_return = XtNewString(payloads[index]);
    *length_return = strlen(payloads[index]);
    *format_return = 8;
    return True;
}

static void
transfer(Widget widget, XtPointer client_data, Atom *selection, Atom *type,
         XtPointer value, unsigned long *length, int *format)
{
    unsigned index;

    (void)widget;
    (void)selection;
    CHECK(type != NULL && value != NULL && length != NULL && format != NULL);
    if (type == NULL || value == NULL || length == NULL || format == NULL)
        return;
    for (index = 0; index < 2; ++index)
        if (*type == targets[index])
            break;
    CHECK(index < 2);
    if (index >= 2)
        return;
    CHECK(client_data == (XtPointer)cookies[index]);
    CHECK(*format == 8 && *length == strlen(payloads[index]));
    CHECK(memcmp(value, payloads[index], *length) == 0);
    CHECK(!transfer_seen[index]);
    transfer_seen[index] = True;
    CHECK(transfer_count < 2);
    if (transfer_count < 2) {
        ++transfer_count;
        if (transfer_count == 2)
            XtAppAddTimeOut(app, 50, finish, NULL);
    }
}

static void
drop(Widget widget, XtPointer closure, XtPointer call_data)
{
    XmDropProcCallback drop_data = (XmDropProcCallback)call_data;
    Arg args[3];
    Cardinal count = 0;
    Widget transfer_object;

    (void)widget;
    (void)closure;
    CHECK(drop_data != NULL && drop_data->dragContext != NULL);
    CHECK(drop_data != NULL &&
          drop_data->dropSiteStatus == XmVALID_DROP_SITE);
    transfer_entries[0].target = targets[0];
    transfer_entries[0].client_data = (XtPointer)cookies[0];
    transfer_entries[1].target = targets[1];
    transfer_entries[1].client_data = (XtPointer)cookies[1];
    XtSetArg(args[count], XmNdropTransfers, transfer_entries); ++count;
    XtSetArg(args[count], XmNnumDropTransfers, 2); ++count;
    XtSetArg(args[count], XmNtransferProc, transfer); ++count;
    transfer_object = NULL;
    if (drop_data != NULL && drop_data->dragContext != NULL)
        transfer_object = XmDropTransferStart(drop_data->dragContext,
                                              args, count);
    CHECK(transfer_object != NULL);
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
    XtSetArg(args[count], XmNexportTargets, targets); ++count;
    XtSetArg(args[count], XmNnumExportTargets, 2); ++count;
    XtSetArg(args[count], XmNconvertProc, convert); ++count;
    XtSetArg(args[count], XmNdragOperations, XmDROP_COPY); ++count;
    context = XmDragStart(source, &event, args, count);
    CHECK(context != NULL);
    XTestFakeMotionEvent(display, -1, destination_x + 20,
                         destination_y + 20, CurrentTime);
    XTestFakeButtonEvent(display, Button1, False, CurrentTime);
    XFlush(display);
}

static void
timeout(XtPointer closure, XtIntervalId *id)
{
    (void)closure;
    (void)id;
    if (!finished) {
        fprintf(stderr, "multiple-target transfer timed out\n");
        result = EXIT_FAILURE;
    }
    XtAppSetExitFlag(app);
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

    source_shell = XtVaAppInitialize(&app, "DndMultipleTargets", NULL, 0,
                                     &argc, argv, NULL,
                                     XmNx, 20, XmNy, 20,
                                     XmNwidth, 180, XmNheight, 80, NULL);
    display = XtDisplay(source_shell);
    destination_shell = XtVaAppCreateShell(
        "destination", "DndMultipleTargets", topLevelShellWidgetClass,
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
    targets[0] = XInternAtom(display, "CK_DND_TARGET_ONE", False);
    targets[1] = XInternAtom(display, "CK_DND_TARGET_TWO", False);
    CHECK(XTestQueryExtension(display, &event_base, &error_base,
                              &major, &minor) != 0);
    (void)event_base;
    (void)error_base;
    (void)major;
    (void)minor;
    XtRealizeWidget(source_shell);
    XtRealizeWidget(destination_shell);
    XtSetArg(args[count], XmNimportTargets, targets); ++count;
    XtSetArg(args[count], XmNnumImportTargets, 2); ++count;
    XtSetArg(args[count], XmNdropSiteOperations, XmDROP_COPY); ++count;
    XtSetArg(args[count], XmNdropProc, drop); ++count;
    XmDropSiteRegister(destination, args, count);
    CHECK(XmDropSiteRegistered(destination));
    XtAppAddTimeOut(app, 20, start_drag, NULL);
    XtAppAddTimeOut(app, 3000, timeout, NULL);
    XtAppMainLoop(app);
    XtDestroyWidget(destination_shell);
    XtDestroyWidget(source_shell);
    XtDestroyApplicationContext(app);
    if (!finished)
        result = EXIT_FAILURE;
    if (result == EXIT_SUCCESS)
        printf("PASS multiple-target Motif drag/drop transfer: %u checks\n",
               checks);
    return result;
}
