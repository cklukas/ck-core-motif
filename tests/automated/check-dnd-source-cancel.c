/* Regression for source-side drag selection loss and owner replacement. */
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
static Widget replacement;
static Widget context;
static Atom target_atom;
static Atom drag_atom;
static XmDropTransferEntryRec transfer_entry;
static Boolean post_drop;
static Boolean owner_replaced;
static Boolean dropped;
static Boolean finished;
static unsigned transfer_calls;
static unsigned checks;
static int result = EXIT_SUCCESS;
static const char payload[] = "selection owner replacement";
#define POST_PAYLOAD_SIZE (4U * 1024U * 1024U)

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        result = EXIT_FAILURE; \
    } else { \
        ++checks; \
    } \
} while (0)

static void
finish_timeout(XtPointer closure, XtIntervalId *id)
{
    (void)closure;
    (void)id;
    if (!finished) {
        fprintf(stderr, "source-side drag cancellation timed out\n");
        result = EXIT_FAILURE;
    }
    XtAppSetExitFlag(app);
}

static Boolean
replacement_convert(Widget widget, Atom *selection, Atom *target,
                    Atom *type_return, XtPointer *value_return,
                    unsigned long *length_return, int *format_return)
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
drag_finished(Widget widget, XtPointer closure, XtPointer call_data)
{
    XmDragDropFinishCallback callback =
        (XmDragDropFinishCallback)call_data;

    (void)widget;
    (void)closure;
    CHECK(callback != NULL && callback->reason == XmCR_DRAG_DROP_FINISH);
    CHECK(owner_replaced);
    finished = True;
    context = NULL;
    XtAppSetExitFlag(app);
}

static void
release_drag(XtPointer closure, XtIntervalId *id)
{
    Window root;

    (void)closure;
    (void)id;
    root = RootWindowOfScreen(XtScreen(source));
    XTestFakeMotionEvent(display, -1, 340, 40, CurrentTime);
    XTestFakeButtonEvent(display, Button1, False, CurrentTime);
    XFlush(display);
    (void)root;
}

static void
replace_owner(XtPointer closure, XtIntervalId *id)
{
    Boolean owned;

    (void)closure;
    (void)id;
    CHECK(drag_atom != None);
    if (drag_atom == None)
        return;
    /* Xt may dispatch the old owner's loss callback synchronously while
     * installing the replacement owner.  Mark the expected transition before
     * entering XtOwnSelection so the finish callback can validate it. */
    owner_replaced = True;
    owned = XtOwnSelection(replacement, drag_atom, CurrentTime,
                           replacement_convert, NULL, NULL);
    CHECK(owned);
    XSync(display, False);
    CHECK(XGetSelectionOwner(display, drag_atom) == XtWindow(replacement));
    if (!post_drop)
        XtAppAddTimeOut(app, 80, release_drag, NULL);
}

static void
top_level_enter(Widget widget, XtPointer closure, XtPointer call_data)
{
    XmTopLevelEnterCallback callback =
        (XmTopLevelEnterCallback)call_data;

    (void)widget;
    (void)closure;
    CHECK(callback != NULL && callback->iccHandle != None);
    if (callback != NULL)
        drag_atom = callback->iccHandle;
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
    *format_return = 8;
    if (post_drop) {
        unsigned char *value = (unsigned char *)XtMalloc(POST_PAYLOAD_SIZE);
        memset(value, 's', POST_PAYLOAD_SIZE);
        *value_return = (XtPointer)value;
        *length_return = POST_PAYLOAD_SIZE;
    } else {
        *value_return = XtNewString(payload);
        *length_return = strlen(payload);
    }
    return True;
}

static void
transfer(Widget widget, XtPointer closure, Atom *selection, Atom *type,
         XtPointer value, unsigned long *length, int *format)
{
    (void)widget;
    (void)closure;
    (void)selection;
    (void)type;
    (void)value;
    (void)length;
    (void)format;
    ++transfer_calls;
}

static void
drop(Widget widget, XtPointer closure, XtPointer call_data)
{
    XmDropProcCallback drop_data = (XmDropProcCallback)call_data;
    Arg args[4];
    Cardinal count = 0;

    (void)widget;
    (void)closure;
    CHECK(post_drop);
    CHECK(drop_data != NULL && drop_data->dragContext != NULL);
    CHECK(drop_data != NULL &&
          drop_data->dropSiteStatus == XmVALID_DROP_SITE);
    transfer_entry.client_data = NULL;
    transfer_entry.target = target_atom;
    XtSetArg(args[count], XmNdropTransfers, &transfer_entry); ++count;
    XtSetArg(args[count], XmNnumDropTransfers, 1); ++count;
    XtSetArg(args[count], XmNtransferProc, transfer); ++count;
    XtSetArg(args[count], XmNincremental, True); ++count;
    if (drop_data != NULL && drop_data->dragContext != NULL)
        CHECK(XmDropTransferStart(drop_data->dragContext, args, count) != NULL);
    XtAppAddTimeOut(app, 0, replace_owner, NULL);
    dropped = True;
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
    XtSetArg(args[count], XmNexportTargets, &target_atom); ++count;
    XtSetArg(args[count], XmNnumExportTargets, 1); ++count;
    XtSetArg(args[count], XmNconvertProc, convert); ++count;
    XtSetArg(args[count], XmNdragOperations, XmDROP_COPY); ++count;
    context = XmDragStart(source, &event, args, count);
    CHECK(context != NULL);
    if (context != NULL) {
        XtAddCallback(context, XmNtopLevelEnterCallback, top_level_enter,
                      NULL);
        XtAddCallback(context, XmNdragDropFinishCallback, drag_finished, NULL);
    }
    if (post_drop) {
        XTestFakeMotionEvent(display, -1, 340, 40, CurrentTime);
        XTestFakeButtonEvent(display, Button1, False, CurrentTime);
        XFlush(display);
    } else {
        XtAppAddTimeOut(app, 40, replace_owner, NULL);
    }
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

    post_drop = argc > 1 && strcmp(argv[1], "post") == 0;
    source_shell = XtVaAppInitialize(&app, "DndSourceCancel", NULL, 0,
                                     &argc, argv, NULL,
                                     XmNx, 20, XmNy, 20,
                                     XmNwidth, 180, XmNheight, 80, NULL);
    display = XtDisplay(source_shell);
    destination_shell = XtVaAppCreateShell(
        "destination", "DndSourceCancel", topLevelShellWidgetClass,
        display, XmNx, 320, XmNy, 20, XmNwidth, 180, XmNheight, 80, NULL);
    source = XtVaCreateManagedWidget("source", xmLabelWidgetClass,
                                     source_shell, NULL);
    label = XmStringCreateLocalized("Drag source");
    XtVaSetValues(source, XmNlabelString, label, NULL);
    XmStringFree(label);
    replacement = XtVaCreateManagedWidget("replacement", xmLabelWidgetClass,
                                          source_shell, NULL);
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
    if (post_drop) {
        XtSetArg(args[count], XmNimportTargets, &target_atom); ++count;
        XtSetArg(args[count], XmNnumImportTargets, 1); ++count;
        XtSetArg(args[count], XmNdropSiteOperations, XmDROP_COPY); ++count;
        XtSetArg(args[count], XmNdropProc, drop); ++count;
        XmDropSiteRegister(destination, args, count);
        CHECK(XmDropSiteRegistered(destination));
    }
    XtAppAddTimeOut(app, 20, start_drag, NULL);
    XtAppAddTimeOut(app, 3000, finish_timeout, NULL);
    XtAppMainLoop(app);
    XtDestroyWidget(destination_shell);
    XtDestroyWidget(source_shell);
    XtDestroyApplicationContext(app);
    if (!finished || !owner_replaced || (post_drop && !dropped))
        result = EXIT_FAILURE;
    if (result == EXIT_SUCCESS)
        printf("PASS source-side DnD cancellation (%s): %u checks, %u transfer callbacks\n",
               post_drop ? "post-drop" : "owner replacement", checks,
               transfer_calls);
    return result;
}
