#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/LinkP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include "PrimitiveI.h"

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static void Events(Widget, XtPointer, XEvent *, Boolean *);
static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmLinkWidget link = (XmLinkWidget)widget;
    (void)offset;
    if (link->link.check_set_render_table)
        value->addr = NULL;
    else {
        link->link.check_set_render_table = True;
        value->addr = (char *)&link->link.render_table;
    }
}
#define Offset(field) XtOffsetOf(XmLinkRec, field)
static XtResource resources[] = {
    {XmNlinkLabel, XmCLinkLabel, XmRXmString, sizeof(XmString), Offset(link.label), XmRImmediate, NULL},
    {XmNlinkTarget, XmCLinkTarget, XmRString, sizeof(String), Offset(link.target), XmRImmediate, NULL},
    {XmNlinkVisited, XmCLinkVisited, XmRBoolean, sizeof(Boolean), Offset(link.visited), XmRImmediate, (XtPointer)False},
    {XmNlinkVisitOnActivate, XmCLinkVisitOnActivate, XmRBoolean, sizeof(Boolean), Offset(link.visit_on_activate), XmRImmediate, (XtPointer)True},
    {XmNlinkActivateCallback, XmCLinkActivateCallback, XmRCallback, sizeof(XtCallbackList), Offset(link.activate_callback), XmRPointer, NULL},
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmRenderTable),
     Offset(link.render_table), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmRenderTable),
     Offset(link.render_table), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean), Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True}
};
externaldef(xmlinkclassrec) XmLinkClassRec xmLinkClassRec = {
    {(WidgetClass)&xmPrimitiveClassRec, "XmLink", sizeof(XmLinkRec), NULL, NULL,
     False, Initialize, NULL, XtInheritRealize, NULL, 0, resources,
     XtNumber(resources), NULLQUARK, True, XtExposeCompressMaximal, True, False,
     Destroy, NULL, Redisplay, SetValues, NULL, XtInheritSetValuesAlmost, NULL,
     NULL, XtVersion, NULL, XtInheritTranslations, QueryGeometry, NULL, NULL},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight, XtInheritTranslations,
     NULL, NULL, 0, NULL}, NULL};
externaldef(xmlinkwidgetclass) WidgetClass xmLinkWidgetClass =
    (WidgetClass)&xmLinkClassRec;

static XmString copy_label(XmString source) {
    char *text; XmString result;
    if (!source) return XmStringCreateLocalized("");
    text = XmStringUnparse(source, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
                           NULL, 0, XmOUTPUT_ALL);
    if (!text) return XmStringCreateLocalized("");
    result = XmStringCreateLocalized(text); XtFree(text); return result;
}
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmLinkWidget link = (XmLinkWidget)widget; XGCValues values;
    (void)request; (void)args; (void)count;
    link->link.label = copy_label(link->link.label);
    link->link.target = link->link.target ? XtNewString(link->link.target) : XtNewString("");
    values.foreground = link->primitive.foreground;
    values.background = link->core.background_pixel;
    if (link->link.render_table == NULL)
        link->link.render_table = XmFontListCopy(
            XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
    else
        link->link.render_table = XmFontListCopy(link->link.render_table);
    link->link.gc = XCreateGC(XtDisplay(widget), DefaultRootWindow(XtDisplay(widget)),
                              GCForeground | GCBackground, &values);
    if (!link->core.width) link->core.width = 80;
    if (!link->core.height) link->core.height = 24;
    XtAddEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events, NULL);
}
static void Destroy(Widget widget) {
    XmLinkWidget link = (XmLinkWidget)widget;
    XtRemoveEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events, NULL);
    if (link->link.gc) XFreeGC(XtDisplay(widget), link->link.gc);
    if (link->link.render_table)
        XmRenderTableFree(link->link.render_table);
    if (link->link.target) XtFree(link->link.target);
    /* Xt releases XmNlinkLabel resource storage after Destroy. */
}
static void text_value(XmLinkWidget link, char *buffer, size_t size) {
    char *text = XmStringUnparse(link->link.label, NULL, XmCHARSET_TEXT,
                                 XmCHARSET_TEXT, NULL, 0, XmOUTPUT_ALL);
    if (text) { snprintf(buffer, size, "%s", text); XtFree(text); }
    else buffer[0] = '\0';
}
static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmLinkWidget link = (XmLinkWidget)widget; Dimension text_width = 0, text_height = 0;
    XmString label;
    (void)event; (void)region;
    if (!XtWindow(widget) || !link->link.gc) return;
    XClearWindow(XtDisplay(widget), XtWindow(widget));
    label = link->link.label;
    if (link->link.render_table && label) {
        XmStringExtent(link->link.render_table, label, &text_width, &text_height);
        XmStringDraw(XtDisplay(widget), XtWindow(widget), link->link.render_table,
                     label, link->link.gc, 4,
                     ((int)link->core.height - (int)text_height) / 2,
                     text_width, XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(link), NULL);
    }
    XDrawLine(XtDisplay(widget), XtWindow(widget), link->link.gc, 4,
              (int)link->core.height - 5, 4 + text_width,
              (int)link->core.height - 5);
    /* Visited state is visible without depending on colour: a second rule
       distinguishes it in monochrome and high-contrast themes. */
    if (link->link.visited)
        XDrawLine(XtDisplay(widget), XtWindow(widget), link->link.gc, 4,
                  (int)link->core.height - 2, 4 + text_width,
                  (int)link->core.height - 2);
    if (link->primitive.highlighted && link->primitive.highlight_thickness)
        XDrawRectangle(XtDisplay(widget), XtWindow(widget), link->link.gc,
                       1, 1, link->core.width - 3, link->core.height - 3);
}
static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmLinkWidget old = (XmLinkWidget)old_widget, link = (XmLinkWidget)widget;
    Boolean redraw = False;
    XmString label_replacement;
    char *target_replacement;
    (void)request; (void)args; (void)count;
    if (old->link.render_table != link->link.render_table) {
        XmRenderTable new_table = link->link.render_table ?
            XmFontListCopy(link->link.render_table) :
            XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
        if (old->link.render_table)
            XmRenderTableFree(old->link.render_table);
        link->link.render_table = new_table;
        redraw = True;
    }
    if (old->link.label != link->link.label) {
        label_replacement = copy_label(link->link.label);
        if (label_replacement) {
            XmStringFree(old->link.label);
            link->link.label = label_replacement;
        } else {
            link->link.label = old->link.label;
        }
        redraw = True;
    }
    if (old->link.target != link->link.target) {
        target_replacement = XtNewString(link->link.target ? link->link.target : "");
        if (target_replacement) {
            if (old->link.target) XtFree(old->link.target);
            link->link.target = target_replacement;
            redraw = True;
        } else {
            link->link.target = old->link.target;
        }
    }
    return redraw || old->link.visited != link->link.visited ||
           old->link.visit_on_activate != link->link.visit_on_activate ||
           old->primitive.highlighted != link->primitive.highlighted;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmLinkWidget link = (XmLinkWidget)widget; char text[256];
    size_t length;
    (void)request; text_value(link, text, sizeof(text));
    reply->request_mode = CWWidth | CWHeight;
    length = strlen(text);
    {
        Dimension measured_width = 0, measured_height = 0;
        if (link->link.render_table && link->link.label)
            XmStringExtent(link->link.render_table, link->link.label,
                           &measured_width, &measured_height);
        unsigned long width = measured_width > 0 ? (unsigned long)measured_width :
                               length > ULONG_MAX / 8UL ? ULONG_MAX : length * 8UL;
        reply->width = width > (unsigned long)(Dimension)-1 - 8UL
                           ? (Dimension)-1 : (Dimension)(width + 8UL);
    }
    reply->height = 24;
    return (widget->core.width == reply->width && widget->core.height == reply->height)
               ? XtGeometryYes : XtGeometryAlmost;
}
static void Events(Widget widget, XtPointer data, XEvent *event, Boolean *dispatch) {
    KeySym key; char input[8];
    (void)data; (void)dispatch;
    if (!XtIsSensitive(widget)) return;
    if (event->type == ButtonPress) { XmLinkActivate(widget, event); return; }
    if (event->type != KeyPress) return;
    XLookupString(&event->xkey, input, sizeof(input), &key, NULL);
    if (key == XK_space || key == XK_Return || key == XK_KP_Enter)
        XmLinkActivate(widget, event);
}
void XmLinkActivate(Widget widget, XEvent *event) {
    XmLinkWidget link; XmLinkCallbackStruct callback;
    if (!XmIsLink(widget) || !XtIsSensitive(widget) || widget->core.being_destroyed) return;
    link = (XmLinkWidget)widget;
    callback.reason = XmLINK_REASON_ACTIVATE; callback.event = event;
    callback.target = link->link.target;
    if (link->link.visit_on_activate) link->link.visited = True;
    XtCallCallbacks(widget, XmNlinkActivateCallback, &callback);
}
Widget XmCreateLink(Widget parent, char *name, ArgList args, Cardinal count) { return XtCreateWidget(name, xmLinkWidgetClass, parent, args, count); }
Widget XmVaCreateLink(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count; Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name, xmLinkWidgetClass, parent, False, var, count); va_end(var); return widget;
}
Widget XmVaCreateManagedLink(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count; Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name, xmLinkWidgetClass, parent, True, var, count); va_end(var); return widget;
}
