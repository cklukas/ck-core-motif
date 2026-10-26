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
    if (!badge->core.width) badge->core.width = 32;
    if (!badge->core.height) badge->core.height = 22;
}
static void Destroy(Widget widget) {
    XmBadgeWidget badge = (XmBadgeWidget)widget;
    if (badge->badge.foreground_gc) XFreeGC(XtDisplay(widget), badge->badge.foreground_gc);
    if (badge->badge.font) XmRenderTableFree(badge->badge.font);
    /* Xt releases XmNbadgeLabel resource storage after Destroy. */
}
static void text_value(XmBadgeWidget badge, char *buffer, size_t size) {
    if (badge->badge.count >= 0) {
        if (badge->badge.count > badge->badge.maximum)
            snprintf(buffer, size, "%d+", badge->badge.maximum);
        else snprintf(buffer, size, "%d", badge->badge.count);
    } else {
        char *text = XmStringUnparse(badge->badge.label, NULL, XmCHARSET_TEXT,
                                     XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
        if (text) { snprintf(buffer, size, "%s", text); XtFree(text); }
        else buffer[0] = '\0';
    }
}
static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmBadgeWidget badge = (XmBadgeWidget)widget; char text[64];
    XmString label; Dimension text_width = 0, text_height = 0;
    (void)event; (void)region; text_value(badge, text, sizeof(text));
    if (!XtWindow(widget) || !badge->badge.foreground_gc) return;
    XClearWindow(XtDisplay(widget), XtWindow(widget));
    XDrawRectangle(XtDisplay(widget), XtWindow(widget), badge->badge.foreground_gc,
                   0, 0, badge->core.width - 1, badge->core.height - 1);
    label = (badge->badge.count >= 0) ? XmStringCreateLocalized(text) :
        badge->badge.label;
    if (badge->badge.font && label) {
        XmStringExtent(badge->badge.font, label, &text_width, &text_height);
        XmStringDraw(XtDisplay(widget), XtWindow(widget), badge->badge.font,
                     label, badge->badge.foreground_gc, 6,
                     ((int)badge->core.height - (int)text_height) / 2,
                     text_width, XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(badge), NULL);
    }
    if (badge->badge.count >= 0 && label) XmStringFree(label);
}
static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmBadgeWidget old = (XmBadgeWidget)old_widget, badge = (XmBadgeWidget)widget;
    XmString replacement;
    Boolean font_changed = old->badge.font != badge->badge.font;
    (void)request; (void)args; (void)count;
    if (font_changed) {
        XmRenderTable new_font = badge->badge.font ? XmFontListCopy(badge->badge.font) :
            XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
        if (old->badge.font) XmRenderTableFree(old->badge.font);
        badge->badge.font = new_font;
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
    if (badge->badge.maximum < 0) badge->badge.maximum = 0;
    if (badge->badge.count < -1) badge->badge.count = -1;
    return old->badge.label != badge->badge.label || old->badge.count != badge->badge.count ||
           old->badge.maximum != badge->badge.maximum || old->badge.severity != badge->badge.severity ||
           font_changed;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                       XtWidgetGeometry *reply) {
    XmBadgeWidget badge = (XmBadgeWidget)widget; char text[64];
    size_t length;
    (void)request; text_value(badge, text, sizeof(text));
    reply->request_mode = CWWidth | CWHeight;
    length = strlen(text);
    {
        Dimension measured_width = 0, measured_height = 0;
        XmString query_label = (badge->badge.count >= 0) ?
            XmStringCreateLocalized(text) : badge->badge.label;
        if (badge->badge.font && query_label)
            XmStringExtent(badge->badge.font, query_label,
                           &measured_width, &measured_height);
        if (badge->badge.count >= 0 && query_label) XmStringFree(query_label);
        unsigned long width = measured_width > 0 ? (unsigned long)measured_width :
                               length > ULONG_MAX / 8UL ? ULONG_MAX : length * 8UL;
        reply->width = width > (unsigned long)(Dimension)-1 - 12UL
                           ? (Dimension)-1 : (Dimension)(width + 12UL);
    }
    reply->height = 22;
    return (widget->core.width == reply->width && widget->core.height == reply->height)
               ? XtGeometryYes : XtGeometryAlmost;
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
    XmBadgeWidget badge = (XmBadgeWidget)widget;
    if (!XmIsBadge(widget) || badge->badge.count < 0) return -1;
    return badge->badge.count > badge->badge.maximum ? badge->badge.maximum : badge->badge.count;
}
