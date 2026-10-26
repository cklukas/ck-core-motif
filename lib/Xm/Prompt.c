#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <Xm/PromptP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <limits.h>
#include <string.h>

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static void CheckSetRenderTable(Widget widget,int offset,XrmValue*value){XmPromptWidget prompt=(XmPromptWidget)widget;(void)offset;if(prompt->prompt.check_set_render_table)value->addr=NULL;else{prompt->prompt.check_set_render_table=True;value->addr=(char*)&prompt->prompt.font;}}
#define Offset(field) XtOffsetOf(XmPromptRec, field)
static XtResource resources[] = {
    {XmNpromptText, XmCPromptText, XmRXmString, sizeof(XmString), Offset(prompt.text), XmRImmediate, NULL},
    {XmNpromptVisible, XmCPromptVisible, XmRBoolean, sizeof(Boolean), Offset(prompt.visible), XmRImmediate, (XtPointer)True},
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmRenderTable), Offset(prompt.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmRenderTable), Offset(prompt.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
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
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmPromptWidget prompt = (XmPromptWidget)widget; XGCValues values;
    (void)args; (void)count;
    prompt->prompt.text = copy_prompt(prompt->prompt.text);
    if (prompt->prompt.font == NULL)
        prompt->prompt.font = XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
    else
        prompt->prompt.font = XmFontListCopy(prompt->prompt.font);
    values.foreground = prompt->primitive.foreground;
    values.background = prompt->core.background_pixel;
    prompt->prompt.gc = XCreateGC(XtDisplay(widget), DefaultRootWindow(XtDisplay(widget)),
                                  GCForeground | GCBackground, &values);
    if (!request->core.width || !request->core.height) {
        Dimension width = 120, height = 24;
        if (prompt->prompt.font && prompt->prompt.text) {
            Dimension text_width = 0, text_height = 0;
            XmStringExtent(prompt->prompt.font, prompt->prompt.text,
                           &text_width, &text_height);
            width = text_width > (Dimension)-1 - 8 ? (Dimension)-1 : text_width + 8;
            height = text_height + 8;
        }
        if (!request->core.width) prompt->core.width = width;
        if (!request->core.height) prompt->core.height = height;
    }
}
static void Destroy(Widget widget) {
    XmPromptWidget prompt = (XmPromptWidget)widget;
    if (prompt->prompt.gc) XFreeGC(XtDisplay(widget), prompt->prompt.gc);
    if (prompt->prompt.font) XmRenderTableFree(prompt->prompt.font);
    /* Xt releases XmNpromptText resource storage after Destroy. */
}
static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmPromptWidget prompt = (XmPromptWidget)widget; Dimension width=0,height=0;
    (void)event; (void)region;
    if (!XtWindow(widget) || !prompt->prompt.gc) return;
    XClearWindow(XtDisplay(widget), XtWindow(widget));
    if (!prompt->prompt.visible) return;
    if (prompt->prompt.font && prompt->prompt.text) {
        XmStringExtent(prompt->prompt.font, prompt->prompt.text, &width, &height);
        XmStringDraw(XtDisplay(widget), XtWindow(widget), prompt->prompt.font,
                     prompt->prompt.text, prompt->prompt.gc, 4,
                     ((int)prompt->core.height - (int)height) / 2, width,
                     XmALIGNMENT_BEGINNING, XmPrim_layout_direction(prompt), NULL);
    }
}
static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmPromptWidget old = (XmPromptWidget)old_widget, prompt = (XmPromptWidget)widget;
    XmString replacement;
    Boolean font_changed = old->prompt.font != prompt->prompt.font;
    Boolean colors_changed =
        old->primitive.foreground != prompt->primitive.foreground ||
        old->core.background_pixel != prompt->core.background_pixel;
    (void)request; (void)args; (void)count;
    if (colors_changed && prompt->prompt.gc) {
        XSetForeground(XtDisplay(widget), prompt->prompt.gc,
                       prompt->primitive.foreground);
        XSetBackground(XtDisplay(widget), prompt->prompt.gc,
                       prompt->core.background_pixel);
    }
    if (font_changed) {
        XmRenderTable new_font = prompt->prompt.font ? XmFontListCopy(prompt->prompt.font) :
            XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
        if (new_font) {
            if (old->prompt.font) XmRenderTableFree(old->prompt.font);
            prompt->prompt.font = new_font;
        } else {
            prompt->prompt.font = old->prompt.font;
            font_changed = False;
        }
    }
    if (old->prompt.text != prompt->prompt.text) {
        replacement = copy_prompt(prompt->prompt.text);
        if (replacement) {
            XmStringFree(old->prompt.text);
            prompt->prompt.text = replacement;
        } else {
            prompt->prompt.text = old->prompt.text;
        }
    }
    return font_changed || colors_changed ||
           old->prompt.text != prompt->prompt.text ||
           old->prompt.visible != prompt->prompt.visible;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmPromptWidget prompt = (XmPromptWidget)widget;
    (void)request;
    reply->request_mode = CWWidth | CWHeight;
    if (prompt->prompt.font && prompt->prompt.text) {
        Dimension width = 0, height = 0;
        XmStringExtent(prompt->prompt.font, prompt->prompt.text, &width, &height);
        reply->width = width > (Dimension)-1 - 8 ? (Dimension)-1 : width + 8;
        reply->height = height > (Dimension)-1 - 8 ? (Dimension)-1 : height + 8;
    } else {
        reply->width = 8;
        reply->height = 24;
    }
    return (widget->core.width == reply->width && widget->core.height == reply->height)
               ? XtGeometryYes : XtGeometryAlmost;
}
void XmPromptSetVisible(Widget widget, Boolean visible) {
    if (widget == NULL || !XmIsPrompt(widget)) return;
    if (((XmPromptWidget)widget)->prompt.visible != visible) {
        ((XmPromptWidget)widget)->prompt.visible = visible;
        if (XtIsRealized(widget)) XClearArea(XtDisplay(widget), XtWindow(widget), 0, 0, 0, 0, True);
    }
}
Boolean XmPromptGetVisible(Widget widget) {
    return widget != NULL && XmIsPrompt(widget)
        ? ((XmPromptWidget)widget)->prompt.visible : False;
}
Widget XmCreatePrompt(Widget parent, char *name, ArgList args, Cardinal count) { return XtCreateWidget(name, xmPromptWidgetClass, parent, args, count); }
Widget XmVaCreatePrompt(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmPromptWidgetClass,parent,False,v,n); va_end(v); return w; }
Widget XmVaCreateManagedPrompt(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmPromptWidgetClass,parent,True,v,n); va_end(v); return w; }
