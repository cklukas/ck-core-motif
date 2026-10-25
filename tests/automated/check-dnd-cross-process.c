/* Bounded cross-process Motif drag/drop transfer regression. */
#include <Xm/DragC.h>
#include <Xm/DropSMgr.h>
#include <Xm/DropTrans.h>
#include <Xm/Label.h>
#include <Xm/Xm.h>
#include <X11/extensions/XTest.h>
#include <errno.h>
#include <fcntl.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

static XtAppContext app;
static Display *display;
static Widget source;
static Widget destination;
static Atom target_atoms[2];
static XmDropTransferEntryRec transfer_entries[2];
static const char payload[] = "cross-process drag payload";
static const char transfer_cookie[] = "cross-process transfer cookie";
static const char multi_payloads[2][24] = {
    "cross-process target one", "cross-process target two"
};
static const char multi_cookies[2][24] = {
    "cross-process cookie one", "cross-process cookie two"
};
static unsigned transfer_count;
static Boolean transfer_seen[2];
static Boolean multiple_mode;
static unsigned checks;
static Boolean finished;
static int result = EXIT_SUCCESS;
static int control_fd = -1;
static Boolean parent_process;

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
        fprintf(stderr, "cross-process drag transfer timed out\n");
        result = EXIT_FAILURE;
    }
    XtAppSetExitFlag(app);
}

static void
send_control(char message)
{
    ssize_t sent;

    do {
        sent = write(control_fd, &message, 1);
    } while (sent < 0 && errno == EINTR);
    CHECK(sent == 1);
}

static Boolean
convert(Widget widget, Atom *selection, Atom *target, Atom *type_return,
        XtPointer *value_return, unsigned long *length_return,
        int *format_return)
{
    unsigned index;

    (void)widget;
    (void)selection;
    if (target == NULL || type_return == NULL ||
        value_return == NULL || length_return == NULL || format_return == NULL)
        return False;
    for (index = 0; index < (multiple_mode ? 2U : 1U); ++index)
        if (*target == target_atoms[index])
            break;
    if (index == (multiple_mode ? 2U : 1U))
        return False;
    *type_return = target_atoms[index];
    if (multiple_mode) {
        *value_return = XtNewString(multi_payloads[index]);
        *length_return = strlen(multi_payloads[index]);
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
    unsigned index;

    (void)widget;
    (void)selection;
    CHECK(type != NULL && value != NULL && length != NULL && format != NULL);
    if (type == NULL || value == NULL || length == NULL || format == NULL)
        return;
    if (!multiple_mode) {
        CHECK(client_data == (XtPointer)transfer_cookie);
        CHECK(*type == target_atoms[0]);
        CHECK(*length == strlen(payload));
        CHECK(*format == 8);
        CHECK(memcmp(value, payload, *length) == 0);
    } else {
        for (index = 0; index < 2; ++index)
            if (*type == target_atoms[index])
                break;
        CHECK(index < 2);
        if (index >= 2)
            return;
        CHECK(client_data == (XtPointer)multi_cookies[index]);
        CHECK(*length == strlen(multi_payloads[index]));
        CHECK(*format == 8);
        CHECK(memcmp(value, multi_payloads[index], *length) == 0);
        CHECK(!transfer_seen[index]);
        transfer_seen[index] = True;
    }
    ++transfer_count;
    /* Xt owns the returned selection value and releases it after this
     * callback.  The consumer must not free it here. */
    if (transfer_count == (multiple_mode ? 2U : 1U)) {
        finished = True;
        send_control('D');
        XtAppSetExitFlag(app);
    }
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

    if (multiple_mode) {
        transfer_entries[0].client_data = (XtPointer)multi_cookies[0];
        transfer_entries[0].target = target_atoms[0];
        transfer_entries[1].client_data = (XtPointer)multi_cookies[1];
        transfer_entries[1].target = target_atoms[1];
    } else {
        transfer_entries[0].client_data = (XtPointer)transfer_cookie;
        transfer_entries[0].target = target_atoms[0];
    }
    XtSetArg(args[count], XmNdropTransfers, transfer_entries);
    ++count;
    XtSetArg(args[count], XmNnumDropTransfers, multiple_mode ? 2 : 1);
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
    /* The two clients use fixed root coordinates so the source client does
     * not need to dereference a widget owned by the destination process. */
    destination_x = 320;
    destination_y = 20;
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

    XtSetArg(args[count], XmNexportTargets, target_atoms);
    ++count;
    XtSetArg(args[count], XmNnumExportTargets, multiple_mode ? 2 : 1);
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

static void
control_input(XtPointer closure, int *source_fd, XtInputId *id)
{
    char message[16];
    ssize_t count;
    ssize_t index;

    (void)closure;
    (void)id;
    do {
        count = read(*source_fd, message, sizeof(message));
    } while (count < 0 && errno == EINTR);
    if (count <= 0) {
        result = EXIT_FAILURE;
        XtAppSetExitFlag(app);
        return;
    }
    for (index = 0; index < count; ++index) {
        if (!parent_process && message[index] == 'R') {
            XtAppAddTimeOut(app, 20, start_drag, NULL);
        } else if (parent_process && message[index] == 'S') {
            send_control('R');
        } else if (!parent_process && message[index] == 'D') {
            finished = True;
            XtAppSetExitFlag(app);
        }
    }
}

static int
run_parent(int fd, int argc, char **argv)
{
    Widget shell;
    XmString label;
    Arg args[4];
    Cardinal count = 0;
    int event_base;
    int error_base;
    int major;
    int minor;

    control_fd = fd;
    parent_process = True;
    shell = XtVaAppInitialize(&app, "DndCrossProcess", NULL, 0,
                              &argc, argv, NULL,
                              XmNx, 320, XmNy, 20,
                              XmNwidth, 180, XmNheight, 80, NULL);
    display = XtDisplay(shell);
    destination = XtVaCreateManagedWidget("destination", xmLabelWidgetClass,
                                          shell, NULL);
    label = XmStringCreateLocalized("Cross-process drop target");
    XtVaSetValues(destination, XmNlabelString, label, NULL);
    XmStringFree(label);
    if (multiple_mode) {
        target_atoms[0] = XInternAtom(display, "CK_CROSS_TARGET_ONE", False);
        target_atoms[1] = XInternAtom(display, "CK_CROSS_TARGET_TWO", False);
    } else {
        target_atoms[0] = XInternAtom(display, "UTF8_STRING", False);
    }
    CHECK(XTestQueryExtension(display, &event_base, &error_base,
                              &major, &minor) != 0);
    (void)event_base;
    (void)error_base;
    (void)major;
    (void)minor;
    XtRealizeWidget(shell);
    XtSetArg(args[count], XmNimportTargets, target_atoms);
    ++count;
    XtSetArg(args[count], XmNnumImportTargets, multiple_mode ? 2 : 1);
    ++count;
    XtSetArg(args[count], XmNdropSiteOperations, XmDROP_COPY);
    ++count;
    XtSetArg(args[count], XmNdropProc, drop);
    ++count;
    XmDropSiteRegister(destination, args, count);
    CHECK(XmDropSiteRegistered(destination));
    XtAppAddInput(app, control_fd, (XtPointer)XtInputReadMask,
                  control_input, NULL);
    XtAppAddTimeOut(app, 10000, finish, NULL);
    XtAppMainLoop(app);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    close(control_fd);
    if (!finished)
        result = EXIT_FAILURE;
    return result;
}

static int
run_child(int fd, int argc, char **argv)
{
    Widget shell;
    XmString label;
    int event_base;
    int error_base;
    int major;
    int minor;

    control_fd = fd;
    parent_process = False;
    shell = XtVaAppInitialize(&app, "DndCrossProcess", NULL, 0,
                              &argc, argv, NULL,
                              XmNx, 20, XmNy, 20,
                              XmNwidth, 180, XmNheight, 80, NULL);
    display = XtDisplay(shell);
    source = XtVaCreateManagedWidget("source", xmLabelWidgetClass,
                                     shell, NULL);
    label = XmStringCreateLocalized("Cross-process drag source");
    XtVaSetValues(source, XmNlabelString, label, NULL);
    XmStringFree(label);
    if (multiple_mode) {
        target_atoms[0] = XInternAtom(display, "CK_CROSS_TARGET_ONE", False);
        target_atoms[1] = XInternAtom(display, "CK_CROSS_TARGET_TWO", False);
    } else {
        target_atoms[0] = XInternAtom(display, "UTF8_STRING", False);
    }
    CHECK(XTestQueryExtension(display, &event_base, &error_base,
                              &major, &minor) != 0);
    (void)event_base;
    (void)error_base;
    (void)major;
    (void)minor;
    XtRealizeWidget(shell);
    XtAppAddInput(app, control_fd, (XtPointer)XtInputReadMask,
                  control_input, NULL);
    XtAppAddTimeOut(app, 10000, finish, NULL);
    send_control('S');
    XtAppMainLoop(app);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    close(control_fd);
    if (!finished)
        result = EXIT_FAILURE;
    return result;
}

int
main(int argc, char **argv)
{
    int sockets[2];
    pid_t child;
    int status;
    int parent_result;

    multiple_mode = getenv("CK_DND_MULTIPLE") != NULL;

    if (socketpair(AF_UNIX, SOCK_STREAM, 0, sockets) != 0) {
        perror("socketpair");
        return EXIT_FAILURE;
    }
    child = fork();
    if (child < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }
    if (child == 0) {
        close(sockets[0]);
        return run_child(sockets[1], argc, argv);
    }
    close(sockets[1]);
    parent_result = run_parent(sockets[0], argc, argv);
    if (waitpid(child, &status, 0) < 0) {
        perror("waitpid");
        parent_result = EXIT_FAILURE;
    }
    if (!WIFEXITED(status) || WEXITSTATUS(status) != EXIT_SUCCESS)
        parent_result = EXIT_FAILURE;
    if (parent_result == EXIT_SUCCESS)
        printf("PASS cross-process Motif drag/drop transfer (%s): %u checks\n",
               multiple_mode ? "multiple-target" : "single-target", checks);
    return parent_result;
}
