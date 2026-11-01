#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <Xm/BadgeP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <string.h>
#include <stdio.h>
#include <limits.h>
#include "PrimitiveI.h"
#include "XmI.h"
#include "AppearanceDrawI.h"
#include <Xm/Appearance.h>

static void natural_size(XmBadgeWidget, Dimension *, Dimension *);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmBadgeWidget badge = (XmBadgeWidget)widget;
    (void)offset;
    if (badge->badge.check_set_render_table)
        value->addr = NULL;
    else {
        badge->badge.check_set_render_table = True;
        value->addr = (char *)&badge->badge.font;
    }
}
#define Offset(field) XtOffsetOf(XmBadgeRec, field)
static XtResource resources[] = {
    {XmNbadgeLabel, XmCBadgeLabel, XmRXmString, sizeof(XmString), Offset(badge.label), XmRImmediate, NULL},
    {XmNbadgeCount, XmCBadgeCount, XmRInt, sizeof(int), Offset(badge.count), XmRImmediate, (XtPointer)-1},
    {XmNbadgeMaximum, XmCBadgeMaximum, XmRInt, sizeof(int), Offset(badge.maximum), XmRImmediate, (XtPointer)99},
    {XmNbadgeSeverity, XmCBadgeSeverity, XmRUnsignedChar, sizeof(unsigned char), Offset(badge.severity), XmRImmediate, (XtPointer)XmBADGE_INFO},
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmRenderTable),
     Offset(badge.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmRenderTable),
     Offset(badge.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean), Offset(primitive.traversal_on), XmRImmediate, (XtPointer)False}
};
externaldef(xmbadgeclassrec) XmBadgeClassRec xmBadgeClassRec = {
    {(WidgetClass)&xmPrimitiveClassRec, "XmBadge", sizeof(XmBadgeRec), NULL, NULL,
     False, Initialize, NULL, XtInheritRealize, NULL, 0, resources,
     XtNumber(resources), NULLQUARK, True, XtExposeCompressMaximal, True, False,
     Destroy, NULL, Redisplay, SetValues, NULL, XtInheritSetValuesAlmost, NULL,
     NULL, XtVersion, NULL, XtInheritTranslations, QueryGeometry, NULL, NULL},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight, XtInheritTranslations,
     NULL, NULL, 0, NULL}, NULL};
externaldef(xmbadgewidgetclass) WidgetClass xmBadgeWidgetClass =
    (WidgetClass)&xmBadgeClassRec;

static XmString copy_label(XmString source) {
    return source ? XmStringCopy(source) : XmStringCreateLocalized("");
}
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmBadgeWidget badge = (XmBadgeWidget)widget; XGCValues values;
    (void)request; (void)args; (void)count;
    if (widget == NULL || !XtIsSubclass(widget, xmBadgeWidgetClass))
        return;
    badge->badge.label = copy_label(badge->badge.label);
    if (badge->badge.font == NULL)
        badge->badge.font = XmFontListCopy(
            XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
    else
        badge->badge.font = XmFontListCopy(badge->badge.font);
    if (badge->badge.maximum < 0) badge->badge.maximum = 0;
    if (badge->badge.count < -1) badge->badge.count = -1;
    values.foreground = badge->primitive.foreground;
    values.background = badge->core.background_pixel;
    badge->badge.foreground_gc = XCreateGC(XtDisplay(widget),
                                           DefaultRootWindow(XtDisplay(widget)),
                                           GCForeground | GCBackground, &values);
    badge->badge.automatic_width = request->core.width == 0;
    badge->badge.automatic_height = request->core.height == 0;
    Dimension width, height;
    natural_size(badge, &width, &height);
    if (badge->badge.automatic_width) badge->core.width = width;
    if (badge->badge.automatic_height) badge->core.height = height;
}
static void Destroy(Widget widget) {
    XmBadgeWidget badge = (XmBadgeWidget)widget;
    if (widget == NULL || !XtIsSubclass(widget, xmBadgeWidgetClass))
        return;
    if (badge->badge.foreground_gc) XFreeGC(XtDisplay(widget), badge->badge.foreground_gc);
    if (badge->badge.font) XmRenderTableFree(badge->badge.font);
    if (badge->badge.label) XmStringFree(badge->badge.label);
}
/* Label mode keeps the complete compound string, including rendition tags. */
static XmString display_label(XmBadgeWidget badge) {
    if (badge->badge.count < 0) return XmStringCopy(badge->badge.label);
    char text[32];
    if (badge->badge.count > badge->badge.maximum)
        snprintf(text, sizeof(text), "%d+", badge->badge.maximum);
    else snprintf(text, sizeof(text), "%d", badge->badge.count);
    return XmStringCreateLocalized(text);
}
static void natural_size(XmBadgeWidget badge, Dimension *width, Dimension *height) {
    Dimension tw=0, th=0;
    XmString label=display_label(badge);
    if (label && badge->badge.font) XmStringExtent(badge->badge.font,label,&tw,&th);
    if (label) XmStringFree(label);
    unsigned long horizontal=_XmDpiDefaultDimension((Widget)badge,6);
    unsigned long vertical=_XmDpiDefaultDimension((Widget)badge,4);
    *height=(Dimension)MIN(USHRT_MAX,MAX((unsigned long)_XmDpiDefaultDimension((Widget)badge,22),
                                       (unsigned long)th+2*vertical));
    *width=(Dimension)MIN(USHRT_MAX,MAX((unsigned long)*height,(unsigned long)tw+2*horizontal));
}
static Position bounded_position(long long value) {
    if (value < SHRT_MIN) return (Position)SHRT_MIN;
    if (value > SHRT_MAX) return (Position)SHRT_MAX;
    return (Position)value;
}
static void Redisplay(Widget widget, XEvent *event, Region region) {
    if (widget == NULL || !XtIsSubclass(widget, xmBadgeWidgetClass)) return;
    XmBadgeWidget badge=(XmBadgeWidget)widget;
    Display *display=XtDisplay(widget); Drawable window=XtWindow(widget);
    GC gc=badge->badge.foreground_gc;
    XmAppearanceProfile profile=XmAPPEARANCE_CLASSIC;
    (void)event; (void)region;
    if (!window || !gc) return;
    XSetClipMask(display,gc,None);
    XClearWindow(display,window);
    XSetBackground(display,gc,badge->core.background_pixel);
    (void)XmGetAppearanceProfile(display,&profile);
    int width=badge->core.width, height=badge->core.height;
    if (profile==XmAPPEARANCE_MODERN_LIGHT || profile==XmAPPEARANCE_MODERN_DARK) {
        int radius=MIN(width,height)/2;
        XSetForeground(display,gc,badge->primitive.bottom_shadow_color);
        _XmAppearanceFillRoundedRectangle(display,window,gc,0,0,width,height,radius);
        XSetForeground(display,gc,badge->core.background_pixel);
        _XmAppearanceFillRoundedRectangle(display,window,gc,1,1,width-2,height-2,MAX(0,radius-1));
    } else if (width>1 && height>1) {
        XSetForeground(display,gc,badge->primitive.foreground);
        XDrawRectangle(display,window,gc,0,0,width-1,height-1);
    }
    int horizontal=_XmDpiDefaultDimension(widget,6), vertical=_XmDpiDefaultDimension(widget,4);
    int inner_width=width-2*horizontal, inner_height=height-2*vertical;
    if (inner_width<=0 || inner_height<=0) return;
    XmString label=display_label(badge);
    if (label && badge->badge.font) {
        Dimension tw=0, th=0;
        XmStringExtent(badge->badge.font,label,&tw,&th);
        XRectangle clip={horizontal,vertical,inner_width,inner_height};
        XSetForeground(display,gc,XtIsSensitive(widget)?badge->primitive.foreground:
                       badge->primitive.bottom_shadow_color);
        XmStringDraw(display,window,badge->badge.font,label,gc,
            bounded_position(horizontal),
            bounded_position(((long long)height-(long long)th)/2),inner_width,XmALIGNMENT_CENTER,
            XmPrim_layout_direction(badge),&clip);
        XSetClipMask(display,gc,None);
    }
    if (label) XmStringFree(label);
}
static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmBadgeWidget old = (XmBadgeWidget)old_widget, badge = (XmBadgeWidget)widget;
    XmString replacement;
    Boolean font_changed;
    Boolean colors_changed = old->primitive.foreground != badge->primitive.foreground ||
                             old->core.background_pixel != badge->core.background_pixel;
    (void)request; (void)args; (void)count;
    if (old_widget == NULL || widget == NULL ||
        !XtIsSubclass(old_widget, xmBadgeWidgetClass) ||
        !XtIsSubclass(widget, xmBadgeWidgetClass))
        return False;
    font_changed = old->badge.font != badge->badge.font;
    if (font_changed) {
        XmRenderTable new_font = badge->badge.font ? XmFontListCopy(badge->badge.font) :
            XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
        if (new_font) {
            if (old->badge.font) XmRenderTableFree(old->badge.font);
            badge->badge.font = new_font;
        } else {
            badge->badge.font = old->badge.font;
            font_changed = False;
        }
    }
    if (old->badge.label != badge->badge.label) {
        replacement = copy_label(badge->badge.label);
        if (replacement) {
            XmStringFree(old->badge.label);
            badge->badge.label = replacement;
        } else {
            badge->badge.label = old->badge.label;
        }
    }
    if (colors_changed && badge->badge.foreground_gc) {
        XSetForeground(XtDisplay(widget), badge->badge.foreground_gc,
                       badge->primitive.foreground);
        XSetBackground(XtDisplay(widget), badge->badge.foreground_gc,
                       badge->core.background_pixel);
    }
    if (badge->badge.maximum < 0) badge->badge.maximum = 0;
    if (badge->badge.count < -1) badge->badge.count = -1;
    for (Cardinal i=0;i<*count;++i) {
        if (!strcmp(args[i].name,XmNwidth)) badge->badge.automatic_width=(Dimension)args[i].value==0;
        if (!strcmp(args[i].name,XmNheight)) badge->badge.automatic_height=(Dimension)args[i].value==0;
    }
    Dimension width,height;
    natural_size(badge,&width,&height);
    if (badge->badge.automatic_width) badge->core.width=width;
    if (badge->badge.automatic_height) badge->core.height=height;
    return old->primitive.foreground != badge->primitive.foreground ||
           old->core.background_pixel != badge->core.background_pixel ||
           old->primitive.bottom_shadow_color != badge->primitive.bottom_shadow_color ||
           XtIsSensitive(old_widget) != XtIsSensitive(widget) ||
           old->badge.label != badge->badge.label || old->badge.count != badge->badge.count ||
           old->badge.maximum != badge->badge.maximum || old->badge.severity != badge->badge.severity ||
           font_changed || colors_changed;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                       XtWidgetGeometry *reply) {
    if (widget == NULL || reply == NULL ||
        !XtIsSubclass(widget, xmBadgeWidgetClass))
        return XtGeometryNo;
    XmBadgeWidget badge=(XmBadgeWidget)widget;
    reply->request_mode=CWWidth|CWHeight;
    natural_size(badge,&reply->width,&reply->height);
    if (!badge->badge.automatic_width) reply->width=badge->core.width;
    if (!badge->badge.automatic_height) reply->height=badge->core.height;
    return XmeReplyToQueryGeometry(widget,request,reply);
}
Widget XmCreateBadge(Widget parent, char *name, ArgList args, Cardinal count) {
    return XtCreateWidget(name, xmBadgeWidgetClass, parent, args, count);
}
Widget XmVaCreateBadge(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name, xmBadgeWidgetClass, parent, False, var, count); va_end(var); return widget;
}
Widget XmVaCreateManagedBadge(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name, xmBadgeWidgetClass, parent, True, var, count); va_end(var); return widget;
}
int XmBadgeGetDisplayedCount(Widget widget) {
    XmBadgeWidget badge;
    if (widget == NULL || !XmIsBadge(widget)) return -1;
    badge = (XmBadgeWidget)widget;
    if (badge->badge.count < 0) return -1;
    return badge->badge.count > badge->badge.maximum ? badge->badge.maximum : badge->badge.count;
}
