/* Additive delayed, non-modal teaching-tip presentation. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/IntrinsicP.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/keysym.h>
#include <Xm/TeachingTipP.h>
#include <Xm/Label.h>
#include <Xm/RowColumn.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);
static void owner_events(Widget, XtPointer, XEvent *, Boolean *);
static void popup_events(Widget, XtPointer, XEvent *, Boolean *);
static void owner_destroyed(Widget, XtPointer, XtPointer);
static void show_timeout(XtPointer, XtIntervalId *);

#define Offset(field) XtOffsetOf(XmTeachingTipRec, field)
static XtResource resources[] = {
    {XmNteachingTipTitle, XmCTeachingTipTitle, XmRXmString, sizeof(XmString),
     Offset(teaching_tip.title), XmRImmediate, (XtPointer)NULL},
    {XmNteachingTipText, XmCTeachingTipText, XmRXmString, sizeof(XmString),
     Offset(teaching_tip.text), XmRImmediate, (XtPointer)NULL},
    {XmNteachingTipOffsetX, XmCTeachingTipOffsetX, XmRPosition,
     sizeof(Position), Offset(teaching_tip.offset_x), XmRImmediate,
     (XtPointer)0},
    {XmNteachingTipOffsetY, XmCTeachingTipOffsetY, XmRPosition,
     sizeof(Position), Offset(teaching_tip.offset_y), XmRImmediate,
     (XtPointer)0},
    {XmNteachingTipDelay, XmCTeachingTipDelay, XmRCardinal,
     sizeof(unsigned int), Offset(teaching_tip.delay), XmRImmediate,
     (XtPointer)0},
    {XmNteachingTipDismissCallback, XmCTeachingTipDismissCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(teaching_tip.dismiss_callback), XmRPointer,
     (XtPointer)NULL},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)False}
};

externaldef(xmteachingtipclassrec) XmTeachingTipClassRec xmTeachingTipClassRec = {
    {(WidgetClass)&xmPrimitiveClassRec, "XmTeachingTip", sizeof(XmTeachingTipRec),
     NULL, NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0,
     resources, XtNumber(resources), NULLQUARK, True, XtExposeCompressMaximal,
     True, False, Destroy, NULL, NULL, SetValues, NULL,
     XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
     XtInheritTranslations, QueryGeometry, NULL, NULL},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
     XtInheritTranslations, NULL, NULL, 0, NULL},
    {NULL}};

externaldef(xmteachingtipwidgetclass) WidgetClass xmTeachingTipWidgetClass =
    (WidgetClass)&xmTeachingTipClassRec;

static XmString copy_string(XmString source) {
    char *text;
    XmString result;
    if (!source) return XmStringCreateLocalized("");
    text = XmStringUnparse(source, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
                           NULL, 0, XmOUTPUT_ALL);
    if (!text) return XmStringCreateLocalized("");
    result = XmStringCreateLocalized(text);
    XtFree(text);
    return result;
}

static Widget shell_ancestor(Widget widget) {
    while (widget && !XtIsShell(widget)) widget = XtParent(widget);
    return widget;
}

static void position_popup(XmTeachingTipWidget tip) {
    Position x, y;
    Dimension owner_width, owner_height, popup_width, popup_height;
    Screen *screen;
    int max_x, max_y;
    if (!tip->teaching_tip.owner || !tip->teaching_tip.popup_shell ||
        !XtIsRealized(tip->teaching_tip.owner)) return;
    XtTranslateCoords(tip->teaching_tip.owner, 0, 0, &x, &y);
    XtVaGetValues(tip->teaching_tip.owner, XmNwidth, &owner_width,
                  XmNheight, &owner_height, NULL);
    XtVaGetValues(tip->teaching_tip.popup_shell, XmNwidth, &popup_width,
                  XmNheight, &popup_height, NULL);
    x += (Position)((int)owner_width - (int)popup_width) / 2;
    y += (Position)owner_height;
    x += tip->teaching_tip.offset_x;
    y += tip->teaching_tip.offset_y;
    screen = XtScreen(tip->teaching_tip.popup_shell);
    max_x = WidthOfScreen(screen) - (int)popup_width;
    max_y = HeightOfScreen(screen) - (int)popup_height;
    if (x < 0) x = 0;
    if (y < 0) y = 0;
    if (max_x >= 0 && x > max_x) x = (Position)max_x;
    if (max_y >= 0 && y > max_y) y = (Position)max_y;
    XtVaSetValues(tip->teaching_tip.popup_shell, XtNx, x, XtNy, y, NULL);
}

static void create_popup(XmTeachingTipWidget tip) {
    Widget parent_shell, content;
    Arg args[4];
    Cardinal count = 0;
    if (tip->teaching_tip.popup_shell) return;
    parent_shell = shell_ancestor(tip->teaching_tip.owner);
    if (!parent_shell) return;
    XtSetArg(args[count], XmNoverrideRedirect, True); ++count;
    XtSetArg(args[count], XmNmappedWhenManaged, False); ++count;
    XtSetArg(args[count], XmNsaveUnder, True); ++count;
    XtSetArg(args[count], XmNtransientFor, XtWindow(parent_shell)); ++count;
    tip->teaching_tip.popup_shell = XtCreatePopupShell(
        "teachingTipShell", overrideShellWidgetClass, parent_shell, args, count);
    content = XmCreateRowColumn(tip->teaching_tip.popup_shell,
                                (char *)"teachingTipContent", NULL, 0);
    XtVaSetValues(content, XmNorientation, XmVERTICAL, XmNspacing, 4, NULL);
    tip->teaching_tip.title_label = XmCreateLabel(content, (char *)"title",
                                                   NULL, 0);
    tip->teaching_tip.text_label = XmCreateLabel(content, (char *)"text",
                                                  NULL, 0);
    XtVaSetValues(tip->teaching_tip.title_label,
                  XmNlabelString, tip->teaching_tip.title, NULL);
    XtVaSetValues(tip->teaching_tip.text_label,
                  XmNlabelString, tip->teaching_tip.text, NULL);
    XtManageChild(tip->teaching_tip.title_label);
    XtManageChild(tip->teaching_tip.text_label);
    XtManageChild(content);
    XtAddEventHandler(tip->teaching_tip.popup_shell,
                      KeyPressMask | ButtonPressMask, False, popup_events,
                      (XtPointer)tip);
    XtRealizeWidget(tip->teaching_tip.popup_shell);
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmTeachingTipWidget tip = (XmTeachingTipWidget)widget;
    (void)request; (void)args; (void)count;
    tip->teaching_tip.title = copy_string(tip->teaching_tip.title);
    tip->teaching_tip.text = copy_string(tip->teaching_tip.text);
    tip->teaching_tip.owner = XtParent(widget);
    tip->core.width = tip->core.width ? tip->core.width : 1;
    tip->core.height = tip->core.height ? tip->core.height : 1;
    if (tip->teaching_tip.owner) {
        XtAddCallback(tip->teaching_tip.owner, XmNdestroyCallback,
                      owner_destroyed, (XtPointer)tip);
        XtAddEventHandler(tip->teaching_tip.owner,
                          StructureNotifyMask | ButtonPressMask, False,
                          owner_events, (XtPointer)tip);
    }
}

static void owner_destroyed(Widget owner, XtPointer closure, XtPointer data) {
    XmTeachingTipWidget tip = (XmTeachingTipWidget)closure;
    (void)owner; (void)data;
    if (tip->teaching_tip.timeout_pending) {
        XtRemoveTimeOut(tip->teaching_tip.timeout_id);
        tip->teaching_tip.timeout_pending = False;
    }
    tip->teaching_tip.owner = NULL;
    if (tip->teaching_tip.popup_shell) {
        XtDestroyWidget(tip->teaching_tip.popup_shell);
        tip->teaching_tip.popup_shell = NULL;
        tip->teaching_tip.title_label = NULL;
        tip->teaching_tip.text_label = NULL;
    }
    tip->teaching_tip.visible = False;
}

static void Destroy(Widget widget) {
    XmTeachingTipWidget tip = (XmTeachingTipWidget)widget;
    if (tip->teaching_tip.timeout_pending)
        XtRemoveTimeOut(tip->teaching_tip.timeout_id);
    if (tip->teaching_tip.owner) {
        XtRemoveCallback(tip->teaching_tip.owner, XmNdestroyCallback,
                         owner_destroyed, (XtPointer)tip);
        XtRemoveEventHandler(tip->teaching_tip.owner,
                             StructureNotifyMask | ButtonPressMask, False,
                             owner_events, (XtPointer)tip);
    }
    if (tip->teaching_tip.popup_shell)
        XtDestroyWidget(tip->teaching_tip.popup_shell);
}

static void owner_events(Widget owner, XtPointer closure, XEvent *event,
                         Boolean *dispatch) {
    XmTeachingTipWidget tip = (XmTeachingTipWidget)closure;
    (void)owner; (void)dispatch;
    if (event->type == UnmapNotify || event->type == ButtonPress)
        XmTeachingTipHide((Widget)tip);
}

static void popup_events(Widget popup, XtPointer closure, XEvent *event,
                         Boolean *dispatch) {
    XmTeachingTipWidget tip = (XmTeachingTipWidget)closure;
    KeySym symbol;
    char input[2];
    (void)popup; (void)dispatch;
    if (event->type == ButtonPress) {
        XmTeachingTipHide((Widget)tip);
        return;
    }
    if (event->type == KeyPress &&
        XLookupString(&event->xkey, input, sizeof(input), &symbol, NULL) >= 0 &&
        (symbol == XK_Escape || symbol == XK_Return || symbol == XK_KP_Enter))
        XmTeachingTipHide((Widget)tip);
}

static void show_timeout(XtPointer data, XtIntervalId *id) {
    XmTeachingTipWidget tip = (XmTeachingTipWidget)data;
    unsigned int delay;
    (void)id;
    tip->teaching_tip.timeout_pending = False;
    delay = tip->teaching_tip.delay;
    tip->teaching_tip.delay = 0;
    XmTeachingTipShow((Widget)tip);
    tip->teaching_tip.delay = delay;
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmTeachingTipWidget old = (XmTeachingTipWidget)old_widget;
    XmTeachingTipWidget tip = (XmTeachingTipWidget)widget;
    (void)request; (void)args; (void)count;
    if (old->teaching_tip.title != tip->teaching_tip.title)
        tip->teaching_tip.title = copy_string(tip->teaching_tip.title);
    if (old->teaching_tip.text != tip->teaching_tip.text)
        tip->teaching_tip.text = copy_string(tip->teaching_tip.text);
    if (tip->teaching_tip.popup_shell &&
        (old->teaching_tip.title != tip->teaching_tip.title ||
         old->teaching_tip.text != tip->teaching_tip.text)) {
        XtVaSetValues(tip->teaching_tip.title_label,
                      XmNlabelString, tip->teaching_tip.title, NULL);
        XtVaSetValues(tip->teaching_tip.text_label,
                      XmNlabelString, tip->teaching_tip.text, NULL);
    }
    if (tip->teaching_tip.visible &&
        (old->teaching_tip.offset_x != tip->teaching_tip.offset_x ||
         old->teaching_tip.offset_y != tip->teaching_tip.offset_y))
        position_popup(tip);
    return False;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    (void)widget; (void)request;
    reply->request_mode = CWWidth | CWHeight;
    reply->width = 1; reply->height = 1;
    return XtGeometryAlmost;
}

void XmTeachingTipShow(Widget widget) {
    XmTeachingTipWidget tip;
    if (!XmIsTeachingTip(widget)) return;
    tip = (XmTeachingTipWidget)widget;
    if (!tip->teaching_tip.owner || !XtIsRealized(tip->teaching_tip.owner)) return;
    if (tip->teaching_tip.timeout_pending)
        XtRemoveTimeOut(tip->teaching_tip.timeout_id);
    tip->teaching_tip.timeout_pending = False;
    if (tip->teaching_tip.visible) return;
    if (tip->teaching_tip.delay) {
        tip->teaching_tip.timeout_id = XtAppAddTimeOut(
            XtWidgetToApplicationContext(widget), tip->teaching_tip.delay,
            show_timeout, (XtPointer)tip);
        tip->teaching_tip.timeout_pending = True;
        return;
    }
    create_popup(tip);
    if (!tip->teaching_tip.popup_shell) return;
    position_popup(tip);
    XtPopup(tip->teaching_tip.popup_shell, XtGrabNone);
    tip->teaching_tip.visible = True;
}

void XmTeachingTipHide(Widget widget) {
    XmTeachingTipWidget tip;
    XmAnyCallbackStruct callback;
    if (!XmIsTeachingTip(widget)) return;
    tip = (XmTeachingTipWidget)widget;
    if (tip->teaching_tip.timeout_pending) {
        XtRemoveTimeOut(tip->teaching_tip.timeout_id);
        tip->teaching_tip.timeout_pending = False;
    }
    if (!tip->teaching_tip.visible) return;
    tip->teaching_tip.visible = False;
    if (tip->teaching_tip.popup_shell)
        XtPopdown(tip->teaching_tip.popup_shell);
    callback.reason = XmCR_UNMAP; callback.event = NULL;
    XtCallCallbacks(widget, XmNteachingTipDismissCallback, &callback);
}

Boolean XmTeachingTipIsVisible(Widget widget) {
    return XmIsTeachingTip(widget)
               ? ((XmTeachingTipWidget)widget)->teaching_tip.visible : False;
}

Widget XmCreateTeachingTip(Widget parent, char *name, ArgList args,
                           Cardinal count) {
    return XtCreateWidget(name, xmTeachingTipWidgetClass, parent, args, count);
}
