/* Additive non-modal anchored flyout.  It deliberately uses XtGrabNone. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/IntrinsicP.h>
#include <X11/Shell.h>
#include <X11/keysym.h>
#include <Xm/FlyoutP.h>
#include <Xm/Label.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include "PrimitiveI.h"
#include "XmI.h"

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);
static void owner_events(Widget, XtPointer, XEvent *, Boolean *);
static void popup_events(Widget, XtPointer, XEvent *, Boolean *);
static void owner_destroyed(Widget, XtPointer, XtPointer);

#define Offset(field) XtOffsetOf(XmFlyoutRec, field)
static XtResource resources[] = {
    {XmNflyoutText, XmCFlyoutText, XmRString, sizeof(String),
     Offset(flyout.text), XmRImmediate, (XtPointer)NULL},
    {XmNflyoutOffsetX, XmCFlyoutOffsetX, XmRPosition, sizeof(Position),
     Offset(flyout.offset_x), XmRImmediate, (XtPointer)0},
    {XmNflyoutOffsetY, XmCFlyoutOffsetY, XmRPosition, sizeof(Position),
     Offset(flyout.offset_y), XmRImmediate, (XtPointer)0},
    {XmNflyoutDismissCallback, XmCFlyoutDismissCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(flyout.dismiss_callback), XmRPointer,
     (XtPointer)NULL},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)False}
};

externaldef(xmflyoutclassrec) XmFlyoutClassRec xmFlyoutClassRec = {
    {
        (WidgetClass)&xmPrimitiveClassRec, "XmFlyout", sizeof(XmFlyoutRec),
        NULL, NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0,
        resources, XtNumber(resources), NULLQUARK, True,
        XtExposeCompressMaximal, True, False, Destroy, NULL, NULL,
        SetValues, NULL, XtInheritSetValuesAlmost, NULL, NULL, XtVersion,
        NULL, XtInheritTranslations, QueryGeometry, NULL, NULL
    },
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
     XtInheritTranslations, NULL, NULL, 0, NULL},
    {NULL}
};
externaldef(xmflyoutwidgetclass) WidgetClass xmFlyoutWidgetClass =
    (WidgetClass)&xmFlyoutClassRec;

static Widget shell_ancestor(Widget widget) {
    while (widget && !XtIsShell(widget)) widget = XtParent(widget);
    return widget;
}

static void position_popup(XmFlyoutWidget flyout) {
    Widget owner = flyout->flyout.owner;
    Widget shell = flyout->flyout.popup_shell;
    Position x, y;
    Dimension owner_width, owner_height, popup_width, popup_height;
    Screen *screen;
    int max_x, max_y;
    if (!owner || !shell || !XtIsRealized(owner)) return;
    XtTranslateCoords(owner, 0, 0, &x, &y);
    XtVaGetValues(owner, XmNwidth, &owner_width, XmNheight, &owner_height,
                  NULL);
    XtVaGetValues(shell, XmNwidth, &popup_width, XmNheight, &popup_height,
                  NULL);
    x += (Position)((int)owner_width - (int)popup_width) / 2;
    y += (Position)owner_height;
    x += flyout->flyout.offset_x;
    y += flyout->flyout.offset_y;
    screen = XtScreen(shell);
    max_x = WidthOfScreen(screen) - (int)popup_width;
    max_y = HeightOfScreen(screen) - (int)popup_height;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (max_x >= 0 && x > max_x) x = (Position)max_x;
    if (max_y >= 0 && y > max_y) y = (Position)max_y;
    XtVaSetValues(shell, XtNx, x, XtNy, y, NULL);
}

static void create_popup(XmFlyoutWidget flyout) {
    Widget parent_shell;
    XmString label_string;
    Arg args[6];
    Cardinal count = 0;
    if (flyout->flyout.popup_shell) return;
    parent_shell = shell_ancestor(flyout->flyout.owner);
    if (!parent_shell) return;
    XtSetArg(args[count], XmNoverrideRedirect, True); ++count;
    XtSetArg(args[count], XmNmappedWhenManaged, False); ++count;
    XtSetArg(args[count], XmNsaveUnder, True); ++count;
    XtSetArg(args[count], XmNtransientFor, XtWindow(parent_shell)); ++count;
    flyout->flyout.popup_shell = XtCreatePopupShell(
        "flyoutShell", overrideShellWidgetClass, parent_shell, args, count);
    label_string = XmStringCreateLocalized(flyout->flyout.text ?
                                           flyout->flyout.text : "");
    flyout->flyout.label = XmCreateLabel(flyout->flyout.popup_shell,
                                         (char *)"flyoutLabel", NULL, 0);
    XtVaSetValues(flyout->flyout.label, XmNlabelString, label_string, NULL);
    XmStringFree(label_string);
    XtManageChild(flyout->flyout.label);
    XtAddEventHandler(flyout->flyout.popup_shell,
                      KeyPressMask | ButtonPressMask, False, popup_events,
                      (XtPointer)flyout);
    XtRealizeWidget(flyout->flyout.popup_shell);
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmFlyoutWidget flyout = (XmFlyoutWidget)widget;
    (void)request; (void)args; (void)count;
    flyout->flyout.owner = XtParent(widget);
    flyout->core.width = flyout->core.width ? flyout->core.width : 1;
    flyout->core.height = flyout->core.height ? flyout->core.height : 1;
    if (flyout->flyout.owner) {
        XtAddCallback(flyout->flyout.owner, XmNdestroyCallback,
                      owner_destroyed, (XtPointer)flyout);
        XtAddEventHandler(flyout->flyout.owner,
                          StructureNotifyMask | ButtonPressMask, False,
                          owner_events, (XtPointer)flyout);
    }
}

static void owner_destroyed(Widget owner, XtPointer closure, XtPointer data) {
    XmFlyoutWidget flyout = (XmFlyoutWidget)closure;
    (void)owner; (void)data;
    flyout->flyout.owner = NULL;
    if (flyout->flyout.popup_shell) {
        XtDestroyWidget(flyout->flyout.popup_shell);
        flyout->flyout.popup_shell = NULL;
        flyout->flyout.label = NULL;
    }
    flyout->flyout.visible = False;
}

static void Destroy(Widget widget) {
    XmFlyoutWidget flyout = (XmFlyoutWidget)widget;
    if (flyout->flyout.owner) {
        XtRemoveCallback(flyout->flyout.owner, XmNdestroyCallback,
                         owner_destroyed, (XtPointer)flyout);
        XtRemoveEventHandler(flyout->flyout.owner,
                             StructureNotifyMask | ButtonPressMask, False,
                             owner_events, (XtPointer)flyout);
    }
    if (flyout->flyout.popup_shell)
        XtDestroyWidget(flyout->flyout.popup_shell);
}

static void owner_events(Widget owner, XtPointer closure, XEvent *event,
                         Boolean *dispatch) {
    XmFlyoutWidget flyout = (XmFlyoutWidget)closure;
    (void)owner; (void)dispatch;
    if (event->type == UnmapNotify || event->type == ButtonPress)
        XmFlyoutHide((Widget)flyout);
}

static void popup_events(Widget popup, XtPointer closure, XEvent *event,
                         Boolean *dispatch) {
    XmFlyoutWidget flyout = (XmFlyoutWidget)closure;
    KeySym symbol;
    char input[2];
    (void)popup; (void)dispatch;
    if (event->type == ButtonPress) {
        XmFlyoutHide((Widget)flyout);
        return;
    }
    if (event->type == KeyPress &&
        XLookupString(&event->xkey, input, sizeof(input), &symbol, NULL) >= 0 &&
        (symbol == XK_Escape || symbol == XK_Return || symbol == XK_KP_Enter))
        XmFlyoutHide((Widget)flyout);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmFlyoutWidget old = (XmFlyoutWidget)old_widget;
    XmFlyoutWidget flyout = (XmFlyoutWidget)widget;
    XmString label_string;
    (void)request; (void)args; (void)count;
    if (flyout->flyout.label && old->flyout.text != flyout->flyout.text) {
        label_string = XmStringCreateLocalized(flyout->flyout.text ?
                                               flyout->flyout.text : "");
        XtVaSetValues(flyout->flyout.label, XmNlabelString, label_string, NULL);
        XmStringFree(label_string);
    }
    if (flyout->flyout.visible &&
        (old->flyout.offset_x != flyout->flyout.offset_x ||
         old->flyout.offset_y != flyout->flyout.offset_y))
        position_popup(flyout);
    return False;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    (void)widget; (void)request;
    reply->request_mode = CWWidth | CWHeight;
    reply->width = 1; reply->height = 1;
    return XtGeometryAlmost;
}

void XmFlyoutShow(Widget widget) {
    XmFlyoutWidget flyout;
    if (!XmIsFlyout(widget)) return;
    flyout = (XmFlyoutWidget)widget;
    if (!flyout->flyout.owner || !XtIsRealized(flyout->flyout.owner)) return;
    create_popup(flyout);
    if (!flyout->flyout.popup_shell) return;
    position_popup(flyout);
    XtPopup(flyout->flyout.popup_shell, XtGrabNone);
    flyout->flyout.visible = True;
}

void XmFlyoutHide(Widget widget) {
    XmFlyoutWidget flyout;
    XmAnyCallbackStruct callback;
    if (!XmIsFlyout(widget)) return;
    flyout = (XmFlyoutWidget)widget;
    if (!flyout->flyout.visible) return;
    flyout->flyout.visible = False;
    if (flyout->flyout.popup_shell) XtPopdown(flyout->flyout.popup_shell);
    callback.reason = XmCR_UNMAP; callback.event = NULL;
    XtCallCallbacks(widget, XmNflyoutDismissCallback, &callback);
}

Boolean XmFlyoutIsVisible(Widget widget) {
    return XmIsFlyout(widget) ? ((XmFlyoutWidget)widget)->flyout.visible : False;
}

Widget XmCreateFlyout(Widget parent, char *name, ArgList args, Cardinal count) {
    return XtCreateWidget(name, xmFlyoutWidgetClass, parent, args, count);
}
