/* Additive progress control.  The legacy Motif widget set is unchanged. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <Xm/ProgressBarP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include "PrimitiveI.h"
#include "TravActI.h"
#include "XmI.h"

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);

static char translations[] = "";

#define Offset(field) XtOffsetOf(XmProgressBarRec, field)
static XtResource resources[] = {
    {XmNprogressMinimum, XmCProgressMinimum, XmRInt, sizeof(int),
     Offset(progress.minimum), XmRImmediate, (XtPointer)0},
    {XmNprogressMaximum, XmCProgressMaximum, XmRInt, sizeof(int),
     Offset(progress.maximum), XmRImmediate, (XtPointer)100},
    {XmNprogressValue, XmCProgressValue, XmRInt, sizeof(int),
     Offset(progress.value), XmRImmediate, (XtPointer)0},
    {XmNprogressIndeterminate, XmCProgressIndeterminate, XmRBoolean,
     sizeof(Boolean), Offset(progress.indeterminate), XmRImmediate,
     (XtPointer)False},
    {XmNprogressLabelString, XmCProgressLabelString, XmRXmString,
     sizeof(XmString), Offset(progress.label), XmRImmediate, (XtPointer)NULL},
    {XmNprogressTextVisible, XmCProgressTextVisible, XmRBoolean,
     sizeof(Boolean), Offset(progress.text_visible), XmRImmediate,
     (XtPointer)True},
    {XmNprogressValueChangedCallback, XmCProgressValueChangedCallback,
     XmRCallback, sizeof(XtCallbackList),
     Offset(progress.value_changed_callback), XmRPointer, (XtPointer)NULL},
    {XmNspacing, XmCSpacing, XmRHorizontalDimension, sizeof(Dimension),
     Offset(progress.spacing), XmRImmediate, (XtPointer)8},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc,
     (XtPointer)_XmSetThickness}
};

externaldef(xmprogressbarclassrec) XmProgressBarClassRec
    xmProgressBarClassRec = {
        {
            (WidgetClass)&xmPrimitiveClassRec, "XmProgressBar",
            sizeof(XmProgressBarRec), NULL, NULL, False, Initialize, NULL,
            XtInheritRealize, NULL, 0, resources,
            XtNumber(resources), NULLQUARK, True, XtExposeCompressMaximal,
            True, False, Destroy, NULL, Redisplay, SetValues, NULL,
            XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
            translations, QueryGeometry, NULL, NULL
        },
        {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
         XtInheritTranslations, NULL, NULL, 0, NULL},
        {NULL}
    };
externaldef(xmprogressbarwidgetclass) WidgetClass xmProgressBarWidgetClass =
    (WidgetClass)&xmProgressBarClassRec;

static void Normalize(XmProgressBarWidget progress) {
    if (progress->progress.maximum < progress->progress.minimum)
        progress->progress.maximum = progress->progress.minimum;
    if (progress->progress.value < progress->progress.minimum)
        progress->progress.value = progress->progress.minimum;
    if (progress->progress.value > progress->progress.maximum)
        progress->progress.value = progress->progress.maximum;
}

static Dimension bounded_dimension(size_t value) {
    const size_t maximum = (size_t)(Dimension)~(Dimension)0;
    return (Dimension)(value > maximum ? maximum : value);
}

static void UpdateGCs(XmProgressBarWidget progress) {
    Display *display = XtDisplay((Widget)progress);
    if (progress->progress.track_gc)
        XSetForeground(display, progress->progress.track_gc,
                       progress->primitive.bottom_shadow_color);
    if (progress->progress.fill_gc)
        XSetForeground(display, progress->progress.fill_gc,
                       progress->primitive.highlight_color);
    if (progress->progress.text_gc)
        XSetForeground(display, progress->progress.text_gc,
                       progress->primitive.foreground);
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmProgressBarWidget progress = (XmProgressBarWidget)widget;
    Display *display = XtDisplay(widget);
    XGCValues values;
    XmFontList defaults;
    XmString replacement;
    Dimension label_width = 0, label_height = 0;
    unsigned long mask = GCForeground | GCBackground;
    (void)request; (void)args; (void)count;
    Normalize(progress);
    if (progress->progress.label)
    {
        replacement = XmStringCopy(progress->progress.label);
        progress->progress.label = replacement != NULL
            ? replacement : XmStringCreateLocalized("");
    }
    else
        progress->progress.label = XmStringCreateLocalized("");
    defaults = XmeGetDefaultRenderTable(widget, XmTEXT_FONTLIST);
    progress->progress.font = defaults ? XmFontListCopy(defaults) : NULL;
    if (progress->progress.font && progress->progress.label)
        XmStringExtent(progress->progress.font, progress->progress.label,
                       &label_width, &label_height);
    if (!progress->core.width)
        progress->core.width = bounded_dimension((size_t)label_width + 180 +
                                                 2 * (size_t)progress->progress.spacing);
    if (!progress->core.height)
        progress->core.height = bounded_dimension((size_t)label_height + 34 +
                                                  2 * (size_t)progress->progress.spacing);
    values.foreground = progress->primitive.bottom_shadow_color;
    values.background = progress->core.background_pixel;
    progress->progress.track_gc = XCreateGC(display, DefaultRootWindow(display),
                                            mask, &values);
    values.foreground = progress->primitive.highlight_color;
    progress->progress.fill_gc = XCreateGC(display, DefaultRootWindow(display),
                                           mask, &values);
    progress->progress.text_gc = XCreateGC(display, DefaultRootWindow(display),
                                           mask, &values);
}

static void Destroy(Widget widget) {
    XmProgressBarWidget progress = (XmProgressBarWidget)widget;
    Display *display = XtDisplay(widget);
    if (progress->progress.track_gc) XFreeGC(display, progress->progress.track_gc);
    if (progress->progress.fill_gc) XFreeGC(display, progress->progress.fill_gc);
    if (progress->progress.text_gc) XFreeGC(display, progress->progress.text_gc);
    if (progress->progress.label) XmStringFree(progress->progress.label);
    if (progress->progress.font) XmFontListFree(progress->progress.font);
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmProgressBarWidget progress = (XmProgressBarWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    Dimension label_width = 0, label_height = 0;
    Dimension bar_width, fill_width;
    Position bar_x, bar_y;
    char text[64];
    XmString text_string;
    unsigned long long numerator, denominator;
    GC text_gc;
    (void)event; (void)region;
    if (!window) return;
    UpdateGCs(progress);
    if (!progress->progress.track_gc || !progress->progress.fill_gc)
        return;
    XClearWindow(display, window);
    if (progress->progress.font)
        XmStringExtent(progress->progress.font, progress->progress.label,
                       &label_width, &label_height);
    text_gc = XtIsSensitive(widget) ? progress->progress.text_gc
                                    : progress->progress.track_gc;
    if (!text_gc) text_gc = progress->progress.track_gc;
    if (progress->progress.label && progress->progress.font)
        XmStringDraw(display, window, progress->progress.font,
                     progress->progress.label, text_gc,
                     progress->progress.spacing, progress->progress.spacing,
                     label_width, XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(progress), NULL);
    bar_x = (Position)progress->progress.spacing;
    bar_y = (Position)(progress->progress.spacing + label_height + 4);
    bar_width = progress->core.width > 2 * progress->progress.spacing
        ? progress->core.width - 2 * progress->progress.spacing : 1;
    XFillRectangle(display, window, progress->progress.track_gc, bar_x, bar_y,
                   bar_width, 16);
    if (progress->progress.indeterminate) {
        fill_width = bar_width / 3;
        if (!fill_width) fill_width = 1;
    } else if (progress->progress.maximum == progress->progress.minimum) {
        fill_width = progress->progress.value >= progress->progress.maximum
            ? bar_width : 0;
    } else {
        numerator = (unsigned long long)progress->progress.value -
                    (unsigned long long)progress->progress.minimum;
        denominator = (unsigned long long)progress->progress.maximum -
                     (unsigned long long)progress->progress.minimum;
        fill_width = (Dimension)((numerator * bar_width) / denominator);
    }
    if (fill_width)
        XFillRectangle(display, window, progress->progress.fill_gc, bar_x, bar_y,
                       fill_width, 16);
    if (progress->progress.text_visible) {
        if (progress->progress.indeterminate)
            snprintf(text, sizeof(text), "…");
        else if (progress->progress.maximum == progress->progress.minimum)
            snprintf(text, sizeof(text), "%s", progress->progress.value >=
                     progress->progress.maximum ? "100%" : "0%");
        else
            snprintf(text, sizeof(text), "%d%%", (int)((100LL *
                     ((long long)progress->progress.value -
                      (long long)progress->progress.minimum)) /
                     ((long long)progress->progress.maximum -
                      (long long)progress->progress.minimum)));
        text_string = XmStringCreateLocalized(text);
        if (text_string && progress->progress.font) {
            Dimension text_width = 0, text_height = 0;
            XmStringExtent(progress->progress.font, text_string, &text_width,
                           &text_height);
            XmStringDraw(display, window, progress->progress.font, text_string,
                         text_gc, (Position)(bar_x + (bar_width - text_width) / 2),
                         (Position)(bar_y + (16 - text_height) / 2), text_width,
                         XmALIGNMENT_BEGINNING,
                         XmPrim_layout_direction(progress), NULL);
        }
        if (text_string)
            XmStringFree(text_string);
    }
    if (progress->primitive.highlighted && progress->primitive.highlight_thickness &&
        progress->core.width > 2 * progress->primitive.highlight_thickness &&
        progress->core.height > 2 * progress->primitive.highlight_thickness)
        XDrawRectangle(display, window, progress->progress.fill_gc,
                       progress->primitive.highlight_thickness - 1,
                       progress->primitive.highlight_thickness - 1,
                       progress->core.width - 2 * progress->primitive.highlight_thickness,
                       progress->core.height - 2 * progress->primitive.highlight_thickness);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmProgressBarWidget old = (XmProgressBarWidget)old_widget;
    XmProgressBarWidget progress = (XmProgressBarWidget)widget;
    Boolean redraw = False;
    XmString replacement;
    (void)request; (void)args; (void)count;
    Normalize(progress);
    if (old->progress.label != progress->progress.label) {
        replacement = progress->progress.label != NULL
            ? XmStringCopy(progress->progress.label) : NULL;
        if (progress->progress.label == NULL || replacement != NULL) {
            if (old->progress.label) XmStringFree(old->progress.label);
            progress->progress.label = replacement;
        } else {
            progress->progress.label = old->progress.label;
        }
        redraw = True;
    }
    if (old->progress.minimum != progress->progress.minimum ||
        old->progress.maximum != progress->progress.maximum ||
        old->progress.value != progress->progress.value ||
        old->progress.indeterminate != progress->progress.indeterminate ||
        old->progress.text_visible != progress->progress.text_visible ||
        old->primitive.foreground != progress->primitive.foreground ||
        old->core.background_pixel != progress->core.background_pixel ||
        old->primitive.highlighted != progress->primitive.highlighted)
        redraw = True;
    return redraw;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmProgressBarWidget progress = (XmProgressBarWidget)widget;
    Dimension label_width = 0, label_height = 0;
    (void)request;
    if (progress->progress.font)
        XmStringExtent(progress->progress.font, progress->progress.label,
                       &label_width, &label_height);
    reply->request_mode = CWWidth | CWHeight;
    reply->width = bounded_dimension((size_t)label_width + 180 +
                                     2 * (size_t)progress->progress.spacing);
    reply->height = bounded_dimension((size_t)label_height + 34 +
                                      2 * (size_t)progress->progress.spacing);
    return (widget->core.width == reply->width && widget->core.height == reply->height)
               ? XtGeometryYes : XtGeometryAlmost;
}

static void Notify(XmProgressBarWidget progress, XEvent *event) {
    XmProgressBarCallbackStruct callback;
    callback.reason = XmCR_VALUE_CHANGED;
    callback.event = event;
    callback.minimum = progress->progress.minimum;
    callback.maximum = progress->progress.maximum;
    callback.value = progress->progress.value;
    callback.indeterminate = progress->progress.indeterminate;
    XtCallCallbacks((Widget)progress, XmNprogressValueChangedCallback,
                    &callback);
}

void XmProgressBarGetRange(Widget widget, int *minimum, int *maximum) {
    XmProgressBarWidget progress;
    if (minimum) *minimum = 0;
    if (maximum) *maximum = 0;
    if (widget == NULL || !XmIsProgressBar(widget)) return;
    progress = (XmProgressBarWidget)widget;
    if (minimum) *minimum = progress->progress.minimum;
    if (maximum) *maximum = progress->progress.maximum;
}
int XmProgressBarGetValue(Widget widget) {
    return widget != NULL && XmIsProgressBar(widget)
        ? ((XmProgressBarWidget)widget)->progress.value : 0;
}
void XmProgressBarSetRange(Widget widget, int minimum, int maximum,
                           Boolean notify) {
    XmProgressBarWidget progress;
    int old_min, old_max, old_value;
    if (widget == NULL || !XmIsProgressBar(widget)) return;
    progress = (XmProgressBarWidget)widget;
    old_min = progress->progress.minimum; old_max = progress->progress.maximum;
    old_value = progress->progress.value;
    progress->progress.minimum = minimum; progress->progress.maximum = maximum;
    Normalize(progress);
    if (XtIsRealized(widget)) XClearArea(XtDisplay(widget), XtWindow(widget),
                                         0, 0, 0, 0, True);
    if (notify && (old_min != progress->progress.minimum ||
                   old_max != progress->progress.maximum ||
                   old_value != progress->progress.value))
        Notify(progress, NULL);
}
void XmProgressBarSetValue(Widget widget, int value, Boolean notify) {
    XmProgressBarWidget progress;
    int old_value;
    if (widget == NULL || !XmIsProgressBar(widget)) return;
    progress = (XmProgressBarWidget)widget; old_value = progress->progress.value;
    progress->progress.value = value; Normalize(progress);
    if (XtIsRealized(widget)) XClearArea(XtDisplay(widget), XtWindow(widget),
                                         0, 0, 0, 0, True);
    if (notify && old_value != progress->progress.value) Notify(progress, NULL);
}
Boolean XmProgressBarGetIndeterminate(Widget widget) {
    return widget != NULL && XmIsProgressBar(widget)
        ? ((XmProgressBarWidget)widget)->progress.indeterminate : False;
}
void XmProgressBarSetIndeterminate(Widget widget, Boolean indeterminate,
                                   Boolean notify) {
    XmProgressBarWidget progress;
    Boolean old;
    if (widget == NULL || !XmIsProgressBar(widget)) return;
    progress = (XmProgressBarWidget)widget; old = progress->progress.indeterminate;
    progress->progress.indeterminate = indeterminate ? True : False;
    if (XtIsRealized(widget)) XClearArea(XtDisplay(widget), XtWindow(widget),
                                         0, 0, 0, 0, True);
    if (notify && old != progress->progress.indeterminate) Notify(progress, NULL);
}

Widget XmCreateProgressBar(Widget parent, char *name, ArgList args,
                           Cardinal arg_count) {
    return XtCreateWidget(name, xmProgressBarWidgetClass, parent, args,
                          arg_count);
}
Widget XmVaCreateProgressBar(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name,
        xmProgressBarWidgetClass, parent, False, var, count); va_end(var);
    return widget;
}
Widget XmVaCreateManagedProgressBar(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name,
        xmProgressBarWidgetClass, parent, True, var, count); va_end(var);
    return widget;
}
