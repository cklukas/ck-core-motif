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
#include <limits.h>
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
static void popup_destroyed(Widget, XtPointer, XtPointer);

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
    Position anchor_x, anchor_y;
    Dimension owner_width, owner_height, popup_width, popup_height;
    Screen *screen;
    long long x_pos, y_pos, max_x, max_y;
    if (!owner || !shell || !XtIsRealized(owner)) return;
    XtTranslateCoords(owner, 0, 0, &anchor_x, &anchor_y);
    XtVaGetValues(owner, XmNwidth, &owner_width, XmNheight, &owner_height,
                  NULL);
    XtVaGetValues(shell, XmNwidth, &popup_width, XmNheight, &popup_height,
                  NULL);
    x_pos = (long long)anchor_x +
            ((long long)owner_width - (long long)popup_width) / 2 +
            (long long)flyout->flyout.offset_x;
    y_pos = (long long)anchor_y + (long long)owner_height +
            (long long)flyout->flyout.offset_y;
    screen = XtScreen(shell);
    max_x = (long long)WidthOfScreen(screen) - (long long)popup_width;
    max_y = (long long)HeightOfScreen(screen) - (long long)popup_height;
    if (max_x < 0 || x_pos < 0) x_pos = 0;
    else if (x_pos > max_x) x_pos = max_x;
    if (max_y < 0 || y_pos < 0) y_pos = 0;
    else if (y_pos > max_y) y_pos = max_y;
    if (x_pos > SHRT_MAX) x_pos = SHRT_MAX;
    if (y_pos > SHRT_MAX) y_pos = SHRT_MAX;
    XtVaSetValues(shell, XtNx, (Position)x_pos, XtNy, (Position)y_pos, NULL);
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
    if (!flyout->flyout.popup_shell) return;
    XtAddCallback(flyout->flyout.popup_shell, XtNdestroyCallback,
                  popup_destroyed, (XtPointer)flyout);
    label_string = XmStringCreateLocalized(flyout->flyout.text ?
                                           flyout->flyout.text : "");
    if (!label_string) {
        XtDestroyWidget(flyout->flyout.popup_shell);
        flyout->flyout.popup_shell = NULL;
        return;
    }
    flyout->flyout.label = XmCreateLabel(flyout->flyout.popup_shell,
                                         (char *)"flyoutLabel", NULL, 0);
    if (!flyout->flyout.label) {
        XmStringFree(label_string);
        XtDestroyWidget(flyout->flyout.popup_shell);
        flyout->flyout.popup_shell = NULL;
        return;
    }
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
    if (flyout == NULL || !XmIsFlyout((Widget)flyout) ||
        flyout->core.being_destroyed)
        return;
    flyout->flyout.owner = NULL;
    if (flyout->flyout.popup_shell &&
        !((CoreWidget)flyout->flyout.popup_shell)->core.being_destroyed) {
        XtDestroyWidget(flyout->flyout.popup_shell);
        flyout->flyout.popup_shell = NULL;
        flyout->flyout.label = NULL;
    }
    flyout->flyout.visible = False;
}

static void popup_destroyed(Widget shell, XtPointer closure, XtPointer data) {
    XmFlyoutWidget flyout = (XmFlyoutWidget)closure;
    (void)shell; (void)data;
    if (flyout == NULL || !XmIsFlyout((Widget)flyout) ||
        flyout->core.being_destroyed)
        return;
    flyout->flyout.popup_shell = NULL;
    flyout->flyout.label = NULL;
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
    if (flyout->flyout.popup_shell &&
        !((CoreWidget)flyout->flyout.popup_shell)->core.being_destroyed)
        XtDestroyWidget(flyout->flyout.popup_shell);
}

static void owner_events(Widget owner, XtPointer closure, XEvent *event,
                         Boolean *dispatch) {
    XmFlyoutWidget flyout = (XmFlyoutWidget)closure;
    (void)owner; (void)dispatch;
    if (flyout == NULL || event == NULL || !XmIsFlyout((Widget)flyout)) return;
    if (event->type == UnmapNotify || event->type == ButtonPress)
        XmFlyoutHide((Widget)flyout);
}

static void popup_events(Widget popup, XtPointer closure, XEvent *event,
                         Boolean *dispatch) {
    XmFlyoutWidget flyout = (XmFlyoutWidget)closure;
    KeySym symbol;
    char input[2];
    (void)popup; (void)dispatch;
    if (flyout == NULL || event == NULL || !XmIsFlyout((Widget)flyout)) return;
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
    if (flyout->flyout.label &&
        !((CoreWidget)flyout->flyout.label)->core.being_destroyed &&
        old->flyout.text != flyout->flyout.text) {
        label_string = XmStringCreateLocalized(flyout->flyout.text ?
                                               flyout->flyout.text : "");
        if (!label_string) return False;
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
    if (widget == NULL || !XmIsFlyout(widget) || widget->core.being_destroyed) return;
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
    if (widget == NULL || !XmIsFlyout(widget)) return;
    flyout = (XmFlyoutWidget)widget;
    if (!flyout->flyout.visible) return;
    flyout->flyout.visible = False;
    if (flyout->flyout.popup_shell) XtPopdown(flyout->flyout.popup_shell);
    callback.reason = XmCR_UNMAP; callback.event = NULL;
    XtCallCallbacks(widget, XmNflyoutDismissCallback, &callback);
}

Boolean XmFlyoutIsVisible(Widget widget) {
    return widget != NULL && XmIsFlyout(widget)
        ? ((XmFlyoutWidget)widget)->flyout.visible : False;
}

Widget XmCreateFlyout(Widget parent, char *name, ArgList args, Cardinal count) {
    return XtCreateWidget(name, xmFlyoutWidgetClass, parent, args, count);
}
