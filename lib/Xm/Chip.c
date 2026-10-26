/* Additive compact chip with explicit activation/removal callbacks. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/ChipP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <limits.h>
#include <string.h>
#include "PrimitiveI.h"
#include "XmI.h"

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);
static void Events(Widget, XtPointer, XEvent *, Boolean *);
static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmChipWidget chip = (XmChipWidget)widget;
    (void)offset;
    if (chip->chip.check_set_render_table)
        value->addr = NULL;
    else {
        chip->chip.check_set_render_table = True;
        value->addr = (char *)&chip->chip.font;
    }
}

#define Offset(field) XtOffsetOf(XmChipRec, field)
static XtResource resources[] = {
    {XmNchipLabel, XmCChipLabel, XmRXmString, sizeof(XmString),
     Offset(chip.label), XmRImmediate, (XtPointer)NULL},
    {XmNchipPixmap, XmCChipPixmap, XmRDynamicPixmap, sizeof(Pixmap),
     Offset(chip.pixmap), XmRImmediate, (XtPointer)XmUNSPECIFIED_PIXMAP},
    {XmNchipRemovable, XmCChipRemovable, XmRBoolean, sizeof(Boolean),
     Offset(chip.removable), XmRImmediate, (XtPointer)False},
    {XmNchipActivateCallback, XmCChipActivateCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(chip.activate_callback), XmRPointer,
     (XtPointer)NULL},
    {XmNchipRemoveCallback, XmCChipRemoveCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(chip.remove_callback), XmRPointer,
     (XtPointer)NULL},
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
     Offset(chip.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmRenderTable),
     Offset(chip.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc,
     (XtPointer)_XmSetThickness}
};

externaldef(xmchipclassrec) XmChipClassRec xmChipClassRec = {
    {(WidgetClass)&xmPrimitiveClassRec, "XmChip", sizeof(XmChipRec), NULL,
     NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0, resources,
     XtNumber(resources), NULLQUARK, True, XtExposeCompressMaximal, True,
     False, Destroy, NULL, Redisplay, SetValues, NULL,
     XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
     XtInheritTranslations, QueryGeometry, NULL, NULL},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
     XtInheritTranslations, NULL, NULL, 0, NULL},
    {NULL}};

externaldef(xmchipwidgetclass) WidgetClass xmChipWidgetClass =
    (WidgetClass)&xmChipClassRec;

static void create_gcs(XmChipWidget chip) {
    XGCValues values;
    Display *display = XtDisplay((Widget)chip);
    values.foreground = chip->primitive.foreground;
    values.background = chip->core.background_pixel;
    if (chip->chip.font == NULL)
        chip->chip.font = XmFontListCopy(
            XmeGetDefaultRenderTable((Widget)chip, XmLABEL_FONTLIST));
    else
        chip->chip.font = XmFontListCopy(chip->chip.font);
    chip->chip.foreground_gc = XCreateGC(display, DefaultRootWindow(display),
                                         GCForeground | GCBackground, &values);
    values.foreground = chip->core.background_pixel;
    chip->chip.background_gc = XCreateGC(display, DefaultRootWindow(display),
                                         GCForeground | GCBackground, &values);
}

static XmString copy_label(XmString source) {
    return source ? XmStringCopy(source) : XmStringCreateLocalized("");
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmChipWidget chip = (XmChipWidget)widget;
    (void)request; (void)args; (void)count;
    chip->chip.label = copy_label(chip->chip.label);
    if (!chip->core.width) chip->core.width = 96;
    if (!chip->core.height) chip->core.height = 28;
    create_gcs(chip);
    XtAddEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events,
                      NULL);
}

static void Destroy(Widget widget) {
    XmChipWidget chip = (XmChipWidget)widget;
    Display *display = XtDisplay(widget);
    XtRemoveEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events,
                         NULL);
    if (chip->chip.foreground_gc) XFreeGC(display, chip->chip.foreground_gc);
    if (chip->chip.background_gc) XFreeGC(display, chip->chip.background_gc);
    if (chip->chip.font) XmRenderTableFree(chip->chip.font);
    /* Xt owns XmNchipLabel resource storage and releases it after Destroy. */
    /* chip->font is borrowed from the parent/display default render table. */
}

static void label_metrics(XmChipWidget chip, Dimension *width,
                          Dimension *height, Dimension *icon_width) {
    Dimension label_width = 0, label_height = 16;
    Dimension iw = 0, ih = 0;
    if (chip->chip.label) {
        XmStringExtent(chip->chip.font, chip->chip.label,
                       &label_width, &label_height);
    }
    if (chip->chip.pixmap != XmUNSPECIFIED_PIXMAP &&
        chip->chip.pixmap != None) {
        Window root;
        int px, py;
        unsigned int pixmap_width, pixmap_height, border, depth;
        XGetGeometry(XtDisplay((Widget)chip), chip->chip.pixmap, &root, &px,
                     &py, &pixmap_width, &pixmap_height, &border, &depth);
        iw = (Dimension)pixmap_width;
        ih = (Dimension)pixmap_height;
    }
    *icon_width = iw;
    {
        unsigned long total_width = label_width + 20UL;
        if (iw) total_width += (unsigned long)iw + 6;
        if (chip->chip.removable) total_width += 18;
        *width = (Dimension)(total_width > USHRT_MAX ? USHRT_MAX :
                             total_width);
    }
    *height = label_height > ih ? label_height : ih;
    if (*height < 16) *height = 16;
    *height = *height > USHRT_MAX - 10 ? USHRT_MAX : *height + 10;
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmChipWidget chip = (XmChipWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    Dimension width, height, icon_width;
    Dimension label_width = 0, label_height = 0;
    int x, y, radius;
    (void)event; (void)region;
    if (!window) return;
    label_metrics(chip, &width, &height, &icon_width);
    XClearWindow(display, window);
    radius = (int)chip->core.height / 2;
    XDrawArc(display, window, chip->chip.foreground_gc, 0, 0,
             chip->core.height - 1, chip->core.height - 1, 90 * 64, 180 * 64);
    XDrawArc(display, window, chip->chip.foreground_gc,
             (int)chip->core.width - (int)chip->core.height, 0,
             chip->core.height - 1, chip->core.height - 1, 270 * 64, 180 * 64);
    XDrawLine(display, window, chip->chip.foreground_gc, radius, 0,
              (int)chip->core.width - radius, 0);
    XDrawLine(display, window, chip->chip.foreground_gc, radius,
              chip->core.height - 1, (int)chip->core.width - radius,
              chip->core.height - 1);
    x = 10;
    if (icon_width) {
        Window root;
        int px, py;
        unsigned int pw, ph, border, depth;
        if (XGetGeometry(display, chip->chip.pixmap, &root, &px, &py, &pw,
                         &ph, &border, &depth))
            XCopyArea(display, chip->chip.pixmap, window,
                      chip->chip.foreground_gc, 0, 0, pw, ph, x,
                      ((int)chip->core.height - (int)ph) / 2);
        x += (int)icon_width + 6;
    }
    if (chip->chip.label && chip->chip.font)
        XmStringExtent(chip->chip.font, chip->chip.label,
                       &label_width, &label_height);
    y = ((int)chip->core.height - (int)label_height) / 2;
    if (chip->chip.label) {
        if (chip->chip.font)
            XmStringDraw(display, window, chip->chip.font, chip->chip.label,
                         chip->chip.foreground_gc, x, y, label_width,
                         XmALIGNMENT_BEGINNING, XmPrim_layout_direction(chip),
                         NULL);
    }
    if (chip->chip.removable) {
        int cx = (int)chip->core.width - 12;
        int cy = (int)chip->core.height / 2;
        XDrawLine(display, window, chip->chip.foreground_gc, cx - 4, cy - 4,
                  cx + 4, cy + 4);
        XDrawLine(display, window, chip->chip.foreground_gc, cx + 4, cy - 4,
                  cx - 4, cy + 4);
    }
    if (chip->primitive.highlighted && chip->primitive.highlight_thickness)
        XDrawRectangle(display, window, chip->chip.foreground_gc,
                       chip->primitive.highlight_thickness - 1,
                       chip->primitive.highlight_thickness - 1,
                       chip->core.width - 2 * chip->primitive.highlight_thickness,
                       chip->core.height - 2 * chip->primitive.highlight_thickness);
}

static void emit(XmChipWidget chip, String resource, int reason, XEvent *event) {
    XmChipCallbackStruct callback;
    callback.reason = reason;
    callback.event = event;
    XtCallCallbacks((Widget)chip, resource, &callback);
}

void XmChipInvoke(Widget widget, XEvent *event) {
    if (XmIsChip(widget) && XtIsSensitive(widget) &&
        !widget->core.being_destroyed)
        emit((XmChipWidget)widget, XmNchipActivateCallback, XmCR_ACTIVATE,
             event);
}

void XmChipRemove(Widget widget, XEvent *event) {
    if (XmIsChip(widget) && XtIsSensitive(widget) &&
        !widget->core.being_destroyed && ((XmChipWidget)widget)->chip.removable)
        emit((XmChipWidget)widget, XmNchipRemoveCallback,
             XmCHIP_REASON_REMOVE, event);
}

static void Events(Widget widget, XtPointer data, XEvent *event,
                   Boolean *dispatch) {
    XmChipWidget chip = (XmChipWidget)widget;
    KeySym key;
    char input[8];
    (void)data; (void)dispatch;
    if (event->type == ButtonPress) {
        if (chip->chip.removable && event->xbutton.x >= (int)chip->core.width - 28)
            XmChipRemove(widget, event);
        else
            XmChipInvoke(widget, event);
        return;
    }
    if (event->type != KeyPress) return;
    XLookupString(&event->xkey, input, sizeof(input), &key, NULL);
    if (key == XK_space || key == XK_Return || key == XK_KP_Enter)
        XmChipInvoke(widget, event);
    else if (chip->chip.removable && (key == XK_Delete || key == XK_BackSpace))
        XmChipRemove(widget, event);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmChipWidget old = (XmChipWidget)old_widget;
    XmChipWidget chip = (XmChipWidget)widget;
    Boolean redraw = False;
    (void)request; (void)args; (void)count;
    if (old->chip.font != chip->chip.font) {
        XmFontList new_font = chip->chip.font ? XmFontListCopy(chip->chip.font) :
            XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
        if (old->chip.font) XmRenderTableFree(old->chip.font);
        chip->chip.font = new_font;
        redraw = True;
    }
    if (old->chip.label != chip->chip.label) {
        XmString replacement = copy_label(chip->chip.label);
        chip->chip.label = replacement;
        redraw = True;
    }
    if (old->chip.pixmap != chip->chip.pixmap ||
        old->chip.removable != chip->chip.removable ||
        old->primitive.highlighted != chip->primitive.highlighted)
        redraw = True;
    return redraw;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmChipWidget chip = (XmChipWidget)widget;
    Dimension width, height, icon_width;
    (void)request;
    label_metrics(chip, &width, &height, &icon_width);
    reply->request_mode = CWWidth | CWHeight;
    reply->width = width;
    reply->height = height;
    return (widget->core.width == width && widget->core.height == height)
               ? XtGeometryYes : XtGeometryAlmost;
}

Widget XmCreateChip(Widget parent, char *name, ArgList args, Cardinal count) {
    return XtCreateWidget(name, xmChipWidgetClass, parent, args, count);
}

Widget XmVaCreateChip(Widget parent, char *name, ...) {
    Widget widget;
    va_list var;
    int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name);
    widget = XmeVLCreateWidget(name, xmChipWidgetClass, parent, False, var,
                               count);
    va_end(var);
    return widget;
}

Widget XmVaCreateManagedChip(Widget parent, char *name, ...) {
    Widget widget;
    va_list var;
    int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name);
    widget = XmeVLCreateWidget(name, xmChipWidgetClass, parent, True, var,
                               count);
    va_end(var);
    return widget;
}
