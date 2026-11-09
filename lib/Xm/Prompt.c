#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "PaintI.h"
#include <X11/IntrinsicP.h>
#include <Xm/PromptP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <limits.h>
#include <string.h>
#include "XmI.h"

static Position bounded_position(long long value) {
    if (value < SHRT_MIN) return (Position)SHRT_MIN;
    if (value > SHRT_MAX) return (Position)SHRT_MAX;
    return (Position)value;
}
static Dimension padded_extent(Dimension value, Dimension padding) {
    return value > (Dimension)USHRT_MAX - padding
               ? (Dimension)USHRT_MAX : (Dimension)(value + padding);
}

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void BufferedRedisplay(Widget, XEvent *, Region);
static Boolean BufferedSetValues(Widget, Widget, Widget, ArgList, Cardinal *);
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
     Destroy, NULL, BufferedRedisplay, BufferedSetValues, NULL, XtInheritSetValuesAlmost, NULL,
     NULL, XtVersion, NULL, XtInheritTranslations, QueryGeometry, NULL, NULL},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight, XtInheritTranslations,
     NULL, NULL, 0, NULL}, NULL};
externaldef(xmpromptwidgetclass) WidgetClass xmPromptWidgetClass =
    (WidgetClass)&xmPromptClassRec;

static XmString copy_prompt(XmString source) {
    return source ? XmStringCopy(source) : XmStringCreateLocalized("");
}
static Dimension padding(XmPromptWidget prompt) {
    unsigned long frame = (unsigned long)prompt->primitive.highlight_thickness +
                          prompt->primitive.shadow_thickness;
    return (Dimension)MIN(USHRT_MAX, MAX(frame,
        (unsigned long)_XmDpiDefaultDimension((Widget)prompt, 4)));
}
static void natural_size(XmPromptWidget prompt, Dimension *width, Dimension *height) {
    Dimension tw = 0, th = 0, inset = padding(prompt);
    if (prompt->prompt.font && prompt->prompt.text)
        XmStringExtent(prompt->prompt.font, prompt->prompt.text, &tw, &th);
    *width = (Dimension)MIN(USHRT_MAX, MAX(1, (unsigned long)tw + 2UL*inset));
    *height = (Dimension)MIN(USHRT_MAX, MAX(1, (unsigned long)th + 2UL*inset));
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
    prompt->prompt.automatic_width = request->core.width == 0;
    prompt->prompt.automatic_height = request->core.height == 0;
    {
        Dimension width, height;
        natural_size(prompt, &width, &height);
        if (prompt->prompt.automatic_width) prompt->core.width = width;
        if (prompt->prompt.automatic_height) prompt->core.height = height;
    }
}
static void Destroy(Widget widget) {
    XmPromptWidget prompt = (XmPromptWidget)widget;
    if (prompt->prompt.gc) XFreeGC(XtDisplay(widget), prompt->prompt.gc);
    if (prompt->prompt.font) XmRenderTableFree(prompt->prompt.font);
    if (prompt->prompt.text) XmStringFree(prompt->prompt.text);
}
static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmPromptWidget prompt = (XmPromptWidget)widget; Dimension width=0,height=0;
    (void)event; (void)region;
    if (!_XmPaintDrawable(widget) || !prompt->prompt.gc) return;
    XSetForeground(XtDisplay(widget), prompt->prompt.gc,
        XtIsSensitive(widget) ? prompt->primitive.foreground : prompt->primitive.bottom_shadow_color);
    XSetBackground(XtDisplay(widget), prompt->prompt.gc, prompt->core.background_pixel);

    if (!prompt->prompt.visible) return;
    if (prompt->prompt.font && prompt->prompt.text) {
        Dimension inset = padding(prompt);
        XRectangle clip;
        if ((unsigned long)prompt->core.width <= 2UL * inset ||
            (unsigned long)prompt->core.height <= 2UL * inset) return;
        clip.x = clip.y = (short)inset;
        clip.width = prompt->core.width - 2 * inset;
        clip.height = prompt->core.height - 2 * inset;
        XmStringExtent(prompt->prompt.font, prompt->prompt.text, &width, &height);
        XmStringDraw(XtDisplay(widget), _XmPaintDrawable(widget), prompt->prompt.font,
                     prompt->prompt.text, prompt->prompt.gc, inset,
                     bounded_position(((long long)prompt->core.height - (long long)height) / 2), clip.width,
                     XmALIGNMENT_BEGINNING, XmPrim_layout_direction(prompt), &clip);
    }
}

static void BufferedRedisplay(Widget widget, XEvent *event, Region region)
{
    _XmBufferedPaint(widget, Redisplay, event, region);
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
    for (Cardinal i=0; i<*count; ++i) {
        if (!strcmp(args[i].name, XmNwidth))
            prompt->prompt.automatic_width = (Dimension)args[i].value == 0;
        if (!strcmp(args[i].name, XmNheight))
            prompt->prompt.automatic_height = (Dimension)args[i].value == 0;
    }
    {
        Dimension width, height;
        natural_size(prompt, &width, &height);
        if (prompt->prompt.automatic_width) prompt->core.width = width;
        if (prompt->prompt.automatic_height) prompt->core.height = height;
    }
    return font_changed || colors_changed ||
           old->primitive.bottom_shadow_color != prompt->primitive.bottom_shadow_color ||
           XtIsSensitive(old_widget) != XtIsSensitive(widget) ||
           old->prompt.text != prompt->prompt.text ||
           old->prompt.visible != prompt->prompt.visible;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    if (widget == NULL || reply == NULL || !XmIsPrompt(widget))
        return XtGeometryNo;
    XmPromptWidget prompt = (XmPromptWidget)widget;
    reply->request_mode = CWWidth | CWHeight;
    natural_size(prompt, &reply->width, &reply->height);
    if (!prompt->prompt.automatic_width) reply->width = prompt->core.width;
    if (!prompt->prompt.automatic_height) reply->height = prompt->core.height;
    return XmeReplyToQueryGeometry(widget, request, reply);
}
void XmPromptSetVisible(Widget widget, Boolean visible) {
    if (widget == NULL || !XmIsPrompt(widget) || widget->core.being_destroyed)
        return;
    if (((XmPromptWidget)widget)->prompt.visible != visible) {
        ((XmPromptWidget)widget)->prompt.visible = visible;
        if (XtIsRealized(widget)) _XmPaintInvalidate(widget);
    }
}
Boolean XmPromptGetVisible(Widget widget) {
    return widget != NULL && XmIsPrompt(widget) && !widget->core.being_destroyed
        ? ((XmPromptWidget)widget)->prompt.visible : False;
}
Widget XmCreatePrompt(Widget parent, char *name, ArgList args, Cardinal count) { return XtCreateWidget(name, xmPromptWidgetClass, parent, args, count); }
Widget XmVaCreatePrompt(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmPromptWidgetClass,parent,False,v,n); va_end(v); return w; }
Widget XmVaCreateManagedPrompt(Widget parent, char *name, ...) { Widget w; va_list v; int n; Va_start(v,name); n=XmeCountVaListSimple(v); va_end(v); Va_start(v,name); w=XmeVLCreateWidget(name,xmPromptWidgetClass,parent,True,v,n); va_end(v); return w; }

/* Xt's default redisplay request clears the front window. Queue our complete
 * frame instead, after resource and geometry negotiation has finished. */
static Boolean BufferedSetValues(Widget old, Widget request, Widget widget,
                                ArgList args, Cardinal *count)
{
    if (SetValues(old, request, widget, args, count))
        _XmPaintInvalidate(widget);
    return False;
}
