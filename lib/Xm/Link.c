#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/LinkP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <Xm/Appearance.h>
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include "PrimitiveI.h"
#include "XmI.h"

static Position bounded_position(long long value) {
    if (value < SHRT_MIN) return (Position)SHRT_MIN;
    if (value > SHRT_MAX) return (Position)SHRT_MAX;
    return (Position)value;
}

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
    return source ? XmStringCopy(source) : XmStringCreateLocalized("");
}

static Dimension padding(XmLinkWidget link) {
    /* Reserve both underline rules even before visiting, so state changes do
     * not move adjacent controls. Rounded DPI gaps can exceed 4*scale at 1.5x. */
    unsigned long inset = MAX(_XmDpiDefaultDimension((Widget)link, 4),
                          3UL * _XmDpiDefaultDimension((Widget)link, 1) + 1);
    unsigned long frame = (unsigned long)link->primitive.highlight_thickness +
                          link->primitive.shadow_thickness;
    return (Dimension)MIN(USHRT_MAX, MAX((unsigned long)inset, frame));
}

static void natural_size(XmLinkWidget link, Dimension *width, Dimension *height) {
    Dimension text_width = 0, text_height = 0, inset = padding(link);
    XmAppearanceProfile profile;
    XmAppearanceDensity density;
    unsigned long minimum = 1;
    if (link->link.render_table && link->link.label)
        XmStringExtent(link->link.render_table, link->link.label, &text_width, &text_height);
    profile = _XmAppearanceGeometryProfile(XtDisplay((Widget)link));
    if (profile == XmAPPEARANCE_MODERN_LIGHT || profile == XmAPPEARANCE_MODERN_DARK) {
        density = _XmAppearanceGeometryDensity(XtDisplay((Widget)link));
        minimum = _XmDpiDefaultDimension((Widget)link,
            density == XmAPPEARANCE_DENSITY_COMPACT ? 28 : 34);
    }
    *width = (Dimension)MIN(USHRT_MAX, MAX(1UL, text_width + 2UL * inset));
    *height = (Dimension)MIN(USHRT_MAX, MAX(minimum, text_height + 2UL * inset));
}

static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmLinkWidget link = (XmLinkWidget)widget; XGCValues values;
    (void)args; (void)count;
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
    link->link.automatic_width = request->core.width == 0;
    link->link.automatic_height = request->core.height == 0;
    {
        Dimension width, height;
        natural_size(link, &width, &height);
        if (link->link.automatic_width) link->core.width = width;
        if (link->link.automatic_height) link->core.height = height;
    }
    XtAddEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events, NULL);
}
static void Destroy(Widget widget) {
    XmLinkWidget link = (XmLinkWidget)widget;
    XtRemoveEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events, NULL);
    if (link->link.gc) XFreeGC(XtDisplay(widget), link->link.gc);
    if (link->link.render_table)
        XmRenderTableFree(link->link.render_table);
    if (link->link.target) XtFree(link->link.target);
    if (link->link.label) XmStringFree(link->link.label);
}
static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmLinkWidget link = (XmLinkWidget)widget; Dimension text_width = 0, text_height = 0;
    XmString label;
    Dimension inset = padding(link);
    (void)event; (void)region;
    if (!XtWindow(widget) || !link->link.gc) return;
    XSetForeground(XtDisplay(widget), link->link.gc,
        XtIsSensitive(widget) ? link->primitive.foreground : link->primitive.bottom_shadow_color);
    XSetBackground(XtDisplay(widget), link->link.gc, link->core.background_pixel);
    XClearWindow(XtDisplay(widget), XtWindow(widget));
    label = link->link.label;
    if (link->link.render_table && label) {
        XmStringExtent(link->link.render_table, label, &text_width, &text_height);
        XmStringDraw(XtDisplay(widget), XtWindow(widget), link->link.render_table,
                     label, link->link.gc, bounded_position(inset),
                     bounded_position(((long long)link->core.height - (long long)text_height) / 2),
                     text_width, XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(link), NULL);
    }
    if (text_width && link->core.width > 2 * inset) {
        int underline_end = MIN((int)inset + text_width - 1,
                                (int)link->core.width - inset - 1);
        int gap = _XmDpiDefaultDimension(widget, 1);
        int underline_y = ((int)link->core.height + text_height) / 2 + gap;
        if (underline_y < (int)link->core.height)
            XDrawLine(XtDisplay(widget), XtWindow(widget), link->link.gc,
                      inset, underline_y, underline_end, underline_y);
        /* A second rule makes visited state distinguishable without color. */
        if (link->link.visited && underline_y + 2 * gap < (int)link->core.height)
            XDrawLine(XtDisplay(widget), XtWindow(widget), link->link.gc,
                      inset, underline_y + 2 * gap, underline_end, underline_y + 2 * gap);
    }
    if (link->primitive.highlighted && link->primitive.highlight_thickness &&
        link->core.width > 2 && link->core.height > 2)
        XDrawRectangle(XtDisplay(widget), XtWindow(widget), link->link.gc,
                       1, 1, link->core.width - 3, link->core.height - 3);
}
static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmLinkWidget old = (XmLinkWidget)old_widget, link = (XmLinkWidget)widget;
    Boolean redraw = False;
    Boolean colors_changed = old->primitive.foreground != link->primitive.foreground ||
                             old->core.background_pixel != link->core.background_pixel;
    XmString label_replacement;
    char *target_replacement;
    (void)request; (void)args; (void)count;
    if (colors_changed && link->link.gc) {
        XSetForeground(XtDisplay(widget), link->link.gc, link->primitive.foreground);
        XSetBackground(XtDisplay(widget), link->link.gc, link->core.background_pixel);
    }
    if (old->link.render_table != link->link.render_table) {
        XmRenderTable new_table = link->link.render_table ?
            XmFontListCopy(link->link.render_table) :
            XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
        if (new_table) {
            if (old->link.render_table)
                XmRenderTableFree(old->link.render_table);
            link->link.render_table = new_table;
            redraw = True;
        } else {
            link->link.render_table = old->link.render_table;
        }
    }
    if (old->link.label != link->link.label) {
        label_replacement = copy_label(link->link.label);
        if (label_replacement) {
            XmStringFree(old->link.label);
            link->link.label = label_replacement;
            redraw = True;
        } else {
            link->link.label = old->link.label;
        }
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
    for (Cardinal i = 0; i < *count; ++i) {
        if (!strcmp(args[i].name, XmNwidth))
            link->link.automatic_width = (Dimension)args[i].value == 0;
        if (!strcmp(args[i].name, XmNheight))
            link->link.automatic_height = (Dimension)args[i].value == 0;
    }
    {
        Dimension width, height;
        natural_size(link, &width, &height);
        if (link->link.automatic_width) link->core.width = width;
        if (link->link.automatic_height) link->core.height = height;
    }
    return redraw || old->primitive.foreground != link->primitive.foreground ||
           old->core.background_pixel != link->core.background_pixel ||
           old->primitive.bottom_shadow_color != link->primitive.bottom_shadow_color ||
           XtIsSensitive(old_widget) != XtIsSensitive(widget) ||
           old->link.visited != link->link.visited ||
           old->link.visit_on_activate != link->link.visit_on_activate ||
           old->primitive.highlighted != link->primitive.highlighted;
}
static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmLinkWidget link = (XmLinkWidget)widget;
    reply->request_mode = CWWidth | CWHeight;
    natural_size(link, &reply->width, &reply->height);
    if (!link->link.automatic_width) reply->width = link->core.width;
    if (!link->link.automatic_height) reply->height = link->core.height;
    return XmeReplyToQueryGeometry(widget, request, reply);
}
static void Events(Widget widget, XtPointer data, XEvent *event, Boolean *dispatch) {
    KeySym key; char input[8];
    (void)data; (void)dispatch;
    if (widget == NULL || event == NULL || !XmIsLink(widget)) return;
    if (!XtIsSensitive(widget)) return;
    if (event->type == ButtonPress) { XmLinkActivate(widget, event); return; }
    if (event->type != KeyPress) return;
    XLookupString(&event->xkey, input, sizeof(input), &key, NULL);
    if (key == XK_space || key == XK_Return || key == XK_KP_Enter)
        XmLinkActivate(widget, event);
}
void XmLinkActivate(Widget widget, XEvent *event) {
    XmLinkWidget link; XmLinkCallbackStruct callback;
    if (widget == NULL || !XmIsLink(widget) || !XtIsSensitive(widget) ||
        widget->core.being_destroyed) return;
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
