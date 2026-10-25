#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <Xm/PromptP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <string.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
#define Offset(field) XtOffsetOf(XmPromptRec, field)
static XtResource resources[] = {
    {XmNpromptText, XmCPromptText, XmRXmString, sizeof(XmString), Offset(prompt.text), XmRImmediate, NULL},
    {XmNpromptVisible, XmCPromptVisible, XmRBoolean, sizeof(Boolean), Offset(prompt.visible), XmRImmediate, (XtPointer)True},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean), Offset(primitive.traversal_on), XmRImmediate, (XtPointer)False}
};
externaldef(xmpromptclassrec) XmPromptClassRec xmPromptClassRec = {
    {(WidgetClass)&xmPrimitiveClassRec, "XmPrompt", sizeof(XmPromptRec), NULL, NULL,
     False, Initialize, NULL, XtInheritRealize, NULL, 0, resources,
     XtNumber(resources), NULLQUARK, True, XtExposeCompressMaximal, True, False,
     Destroy, NULL, Redisplay, SetValues, NULL, XtInheritSetValuesAlmost, NULL,
     NULL, XtVersion, NULL, XtInheritTranslations, QueryGeometry, NULL, NULL},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight, XtInheritTranslations,
     NULL, NULL, 0, NULL}, NULL};
externaldef(xmpromptwidgetclass) WidgetClass xmPromptWidgetClass =
    (WidgetClass)&xmPromptClassRec;

static XmString copy_prompt(XmString source) {
    char *text; XmString result;
    if (!source) return XmStringCreateLocalized("");
    text = XmStringUnparse(source, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
                           NULL, 0, XmOUTPUT_ALL);
    if (!text) return XmStringCreateLocalized("");
    result = XmStringCreateLocalized(text); XtFree(text); return result;
}
static char *prompt_text(XmPromptWidget prompt) {
    char *text = XmStringUnparse(prompt->prompt.text, NULL, XmCHARSET_TEXT,
                                 XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
    return text ? text : XtNewString("");
}
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmPromptWidget prompt = (XmPromptWidget)widget; XGCValues values;
    (void)request; (void)args; (void)count;
    prompt->prompt.text = copy_prompt(prompt->prompt.text);
    values.foreground = prompt->primitive.foreground;
    values.background = prompt->core.background_pixel;
    prompt->prompt.gc = XCreateGC(XtDisplay(widget), DefaultRootWindow(XtDisplay(widget)),
                                  GCForeground | GCBackground, &values);
    if (!prompt->core.width) prompt->core.width = 120;
    if (!prompt->core.height) prompt->core.height = 24;
}
static void Destroy(Widget widget) {
    XmPromptWidget prompt = (XmPromptWidget)widget;
    if (prompt->prompt.gc) XFreeGC(XtDisplay(widget), prompt->prompt.gc);
    /* Xt releases XmNpromptText resource storage after Destroy. */
}
static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmPromptWidget prompt = (XmPromptWidget)widget; char *text;
    (void)event; (void)region;
    if (!XtWindow(widget)) return;
    XClearWindow(XtDisplay(widget), XtWindow(widget));
    if (!prompt->prompt.visible) return;
    text = prompt_text(prompt);
    XDrawString(XtDisplay(widget), XtWindow(widget), prompt->prompt.gc, 4,
                (int)prompt->core.height - 8, text, (int)strlen(text));
    XtFree(text);
}
static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmPromptWidget old = (XmPromptWidget)old_widget, prompt = (XmPromptWidget)widget;
    (void)request; (void)args; (void)count;
    if (old->prompt.text != prompt->prompt.text)
        prompt->prompt.text = copy_prompt(prompt->prompt.text);
    return old->prompt.text != prompt->prompt.text ||
           old->prompt.visible != prompt->prompt.visible;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmPromptWidget prompt = (XmPromptWidget)widget; char *text;
    (void)request; text = prompt_text(prompt);
    reply->request_mode = CWWidth | CWHeight;
    reply->width = (Dimension)(strlen(text) * 8 + 8); reply->height = 24;
    XtFree(text);
    return (widget->core.width == reply->width && widget->core.height == reply->height)
               ? XtGeometryYes : XtGeometryAlmost;
}
void XmPromptSetVisible(Widget widget, Boolean visible) {
    if (!XmIsPrompt(widget)) return;
    if (((XmPromptWidget)widget)->prompt.visible != visible) {
        ((XmPromptWidget)widget)->prompt.visible = visible;
        if (XtIsRealized(widget)) XClearArea(XtDisplay(widget), XtWindow(widget), 0, 0, 0, 0, True);
    }
}
Boolean XmPromptGetVisible(Widget widget) {
    return XmIsPrompt(widget) ? ((XmPromptWidget)widget)->prompt.visible : False;
}
Widget XmCreatePrompt(Widget parent, char *name, ArgList args, Cardinal count) { return XtCreateWidget(name, xmPromptWidgetClass, parent, args, count); }
Widget XmVaCreatePrompt(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmPromptWidgetClass,parent,False,v,n); va_end(v); return w; }
Widget XmVaCreateManagedPrompt(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmPromptWidgetClass,parent,True,v,n); va_end(v); return w; }
