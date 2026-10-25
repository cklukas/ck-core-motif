/* Bounded Motif drop-site registration, retrieval and teardown regression. */
#include <Xm/DropSMgr.h>
#include <Xm/Label.h>
#include <Xm/Xm.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned checks;

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return EXIT_FAILURE; \
    } \
    ++checks; \
} while (0)

static void
DragProc(Widget widget, XtPointer closure, XtPointer call_data)
{
    (void)widget;
    (void)closure;
    (void)call_data;
}

static void
DropProc(Widget widget, XtPointer closure, XtPointer call_data)
{
    (void)widget;
    (void)closure;
    (void)call_data;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Widget target;
    Widget retired;
    Display *display;
    Atom target_atom;
    Atom *import_targets = NULL;
    Cardinal import_count = 0;
    unsigned char operations = 0;
    XtCallbackProc drag_proc = NULL;
    XtCallbackProc drop_proc = NULL;
    Arg args[6];
    Cardinal count;
    XmString label;

    shell = XtVaAppInitialize(&app, "DropSiteLifecycle", NULL, 0,
                              &argc, argv, NULL,
                              XmNwidth, 260, XmNheight, 120, NULL);
    display = XtDisplay(shell);
    label = XmStringCreateLocalized("Drop target");
    target = XtVaCreateManagedWidget("target", xmLabelWidgetClass, shell,
                                     XmNlabelString, label, NULL);
    XmStringFree(label);
    XtRealizeWidget(shell);
    target_atom = XInternAtom(display, "UTF8_STRING", False);

    count = 0;
    XtSetArg(args[count], XmNdropSiteOperations, XmDROP_COPY | XmDROP_MOVE);
    ++count;
    XtSetArg(args[count], XmNimportTargets, &target_atom);
    ++count;
    XtSetArg(args[count], XmNnumImportTargets, 1);
    ++count;
    XtSetArg(args[count], XmNdragProc, DragProc);
    ++count;
    XtSetArg(args[count], XmNdropProc, DropProc);
    ++count;
    XmDropSiteRegister(target, args, count);
    CHECK(XmDropSiteRegistered(target));

    count = 0;
    XtSetArg(args[count], XmNdropSiteOperations, &operations);
    ++count;
    XtSetArg(args[count], XmNimportTargets, &import_targets);
    ++count;
    XtSetArg(args[count], XmNnumImportTargets, &import_count);
    ++count;
    XtSetArg(args[count], XmNdragProc, &drag_proc);
    ++count;
    XtSetArg(args[count], XmNdropProc, &drop_proc);
    ++count;
    XmDropSiteRetrieve(target, args, count);
    CHECK((operations & (XmDROP_COPY | XmDROP_MOVE)) ==
          (XmDROP_COPY | XmDROP_MOVE));
    CHECK(import_targets != NULL && import_count == 1 &&
          import_targets[0] == target_atom);
    CHECK(drag_proc == DragProc && drop_proc == DropProc);

    XmDropSiteStartUpdate(target);
    XmDropSiteEndUpdate(target);
    XmDropSiteUnregister(target);
    CHECK(!XmDropSiteRegistered(target));

    /* Re-registration and implicit widget teardown must not retain the old
     * drop-site record or callback closure. */
    count = 0;
    XtSetArg(args[count], XmNdropSiteOperations, XmDROP_COPY);
    ++count;
    XtSetArg(args[count], XmNimportTargets, &target_atom);
    ++count;
    XtSetArg(args[count], XmNnumImportTargets, 1);
    ++count;
    XtSetArg(args[count], XmNdragProc, DragProc);
    ++count;
    XtSetArg(args[count], XmNdropProc, DropProc);
    ++count;
    XmDropSiteRegister(target, args, count);
    CHECK(XmDropSiteRegistered(target));

    retired = XtVaCreateManagedWidget("retired", xmLabelWidgetClass, shell,
                                      NULL);
    XmDropSiteRegister(retired, args, count);
    CHECK(XmDropSiteRegistered(retired));
    XtDestroyWidget(retired);
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    printf("PASS drop-site registration/retrieval/teardown: %u checks\n",
           checks);
    return EXIT_SUCCESS;
}
