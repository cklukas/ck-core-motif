/*
 * The source tab is extracted from this file.  Keep the example body real:
 * it is the same public-API construction path used by gallery.c.
 */
#include <Xm/Xm.h>
#include <Xm/Label.h>
#include <Xm/DrawingA.h>
#include <Xm/DragC.h>
#include <Xm/MenuShell.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>

/* GALLERY-EXAMPLE-BEGIN: shared-public-construction */
static void
gallery_attach_public_callback(Widget example, XtCallbackProc callback,
                               XtPointer client_data)
{
    /* Prefer the component's semantic action, then its value notification.
     * Every Motif widget/gadget still has the public help callback as a
     * keyboard-accessible interaction fallback. */
    if (XtHasCallbacks(example, XmNactivateCallback) == XtCallbackHasSome)
        XtAddCallback(example, XmNactivateCallback, callback, client_data);
    else if (XtHasCallbacks(example, XmNvalueChangedCallback) == XtCallbackHasSome)
        XtAddCallback(example, XmNvalueChangedCallback, callback, client_data);
    else if (XtHasCallbacks(example, XmNhelpCallback) == XtCallbackHasSome)
        XtAddCallback(example, XmNhelpCallback, callback, client_data);
}

Widget
gallery_create_component_example(Widget parent, WidgetClass class,
                                 const char *name, XtCallbackProc callback,
                                 XtPointer client_data)
{
    Widget example = XtVaCreateManagedWidget(name, class, parent, NULL);
    if (callback)
        gallery_attach_public_callback(example, callback, client_data);
    return example;
}
/* GALLERY-EXAMPLE-END: shared-public-construction */

/* GALLERY-EXAMPLE-BEGIN: public-teardown */
void
gallery_destroy_component_example(Widget *example)
{
    if (example && *example) {
        XtDestroyWidget(*example);
        *example = NULL;
    }
}
/* GALLERY-EXAMPLE-END: public-teardown */

/* GALLERY-EXAMPLE-BEGIN: drag-public-construction */
Widget
gallery_create_drag_source_example(Widget parent, XtCallbackProc callback,
                                   XtPointer client_data)
{
    Widget source = XtVaCreateManagedWidget(
        "dragSource", xmDrawingAreaWidgetClass, parent,
        XmNwidth, 180, XmNheight, 64, NULL);
    if (callback)
        XtAddCallback(source, XmNinputCallback, callback, client_data);
    return source;
}
/* GALLERY-EXAMPLE-END: drag-public-construction */

/* GALLERY-EXAMPLE-BEGIN: shell-public-construction */
Widget
gallery_create_shell_example(Widget parent, WidgetClass class,
                             const char *name)
{
    Arg args[2];
    XtSetArg(args[0], XmNwidth, 240);
    XtSetArg(args[1], XmNheight, 96);
    Widget shell = XtCreatePopupShell((char *)name, class, parent, args, 2);
    XmString label = XmStringCreateLocalized((char *)"Popup shell example");
    if (class == xmMenuShellWidgetClass) {
        Widget menu = XtVaCreateManagedWidget(
            "popupContent", xmRowColumnWidgetClass, shell,
            XmNrowColumnType, XmMENU_POPUP, NULL);
        XtVaCreateManagedWidget("popupAction", xmPushButtonWidgetClass, menu,
                                XmNlabelString, label, NULL);
    } else {
        XtVaCreateManagedWidget("popupContent", xmLabelWidgetClass, shell,
                                XmNlabelString, label, NULL);
    }
    XmStringFree(label);
    return shell;
}
/* GALLERY-EXAMPLE-END: shell-public-construction */
