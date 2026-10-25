#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <Xm/BadgeP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <string.h>
#include <stdio.h>
#include "PrimitiveI.h"

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
#define Offset(field) XtOffsetOf(XmBadgeRec, field)
static XtResource resources[] = {
    {XmNbadgeLabel, XmCBadgeLabel, XmRXmString, sizeof(XmString), Offset(badge.label), XmRImmediate, NULL},
    {XmNbadgeCount, XmCBadgeCount, XmRInt, sizeof(int), Offset(badge.count), XmRImmediate, (XtPointer)-1},
    {XmNbadgeMaximum, XmCBadgeMaximum, XmRInt, sizeof(int), Offset(badge.maximum), XmRImmediate, (XtPointer)99},
    {XmNbadgeSeverity, XmCBadgeSeverity, XmRUnsignedChar, sizeof(unsigned char), Offset(badge.severity), XmRImmediate, (XtPointer)XmBADGE_INFO},
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
    char *text; XmString result;
    if (!source) return XmStringCreateLocalized("");
    text = XmStringUnparse(source, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
                           NULL, 0, XmOUTPUT_ALL);
    if (!text) return XmStringCreateLocalized("");
    result = XmStringCreateLocalized(text); XtFree(text); return result;
}
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmBadgeWidget badge = (XmBadgeWidget)widget; XGCValues values;
    (void)request; (void)args; (void)count;
    badge->badge.label = copy_label(badge->badge.label);
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
    (void)event; (void)region; text_value(badge, text, sizeof(text));
    if (!XtWindow(widget) || !badge->badge.foreground_gc) return;
    XClearWindow(XtDisplay(widget), XtWindow(widget));
    XDrawRectangle(XtDisplay(widget), XtWindow(widget), badge->badge.foreground_gc,
                   0, 0, badge->core.width - 1, badge->core.height - 1);
    XDrawString(XtDisplay(widget), XtWindow(widget), badge->badge.foreground_gc,
                6, (int)badge->core.height - 7, text, (int)strlen(text));
}
static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmBadgeWidget old = (XmBadgeWidget)old_widget, badge = (XmBadgeWidget)widget;
    XmString replacement;
    (void)request; (void)args; (void)count;
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
           old->badge.maximum != badge->badge.maximum || old->badge.severity != badge->badge.severity;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                       XtWidgetGeometry *reply) {
    XmBadgeWidget badge = (XmBadgeWidget)widget; char text[64];
    (void)request; text_value(badge, text, sizeof(text));
    reply->request_mode = CWWidth | CWHeight;
    reply->width = (Dimension)(strlen(text) * 8 + 12); reply->height = 22;
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
