/* Additive segmented picker built from ordinary public ToggleButton children. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <Xm/SegmentedControlP.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBP.h>
#include <Xm/Appearance.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include "XmI.h"
#include "AppearanceDrawI.h"
#include "LabelI.h"
#include "CallbackI.h"

static void natural_size(Widget, Dimension *, Dimension *);

/* Internal ToggleButton children retain their input and accessibility behavior.
 * Only their presentation is supplied by the composite in modern profiles. */
Boolean _XmSegmentedControlUsesModern(Widget child) {
    XmAppearanceProfile profile;
    return XtParent(child) && XmIsSegmentedControl(XtParent(child)) &&
        XmGetAppearanceProfile(XtDisplay(child), &profile) &&
        (profile == XmAPPEARANCE_MODERN_LIGHT || profile == XmAPPEARANCE_MODERN_DARK);
}

/* Label expose invokes Primitive's border hook. Keep this operation limited
 * to the inset focus stroke so it cannot recursively redraw the label. */
void _XmSegmentedControlDrawFocus(Widget child) {
    XmToggleButtonWidget button = (XmToggleButtonWidget)child;
    int stroke = MAX(1, (int)_XmDpiDefaultDimension(child, 1));
    int inset = MAX(stroke+1, (int)_XmDpiDefaultDimension(child, 3));
    XGCValues values;
    GC gc;
    if (!XtIsRealized(child) || child->core.width <= 2*inset ||
        child->core.height <= 2*inset) return;
    values.foreground = button->primitive.highlighted && XtIsSensitive(child) ?
        button->primitive.highlight_color :
        (button->toggle.visual_set != XmUNSET ?
            button->toggle.select_color : button->toggle.unselect_color);
    values.line_width = stroke;
    gc = XCreateGC(XtDisplay(child), XtWindow(child), GCForeground|GCLineWidth, &values);
    if (!gc) return;
    XDrawRectangle(XtDisplay(child), XtWindow(child), gc, inset, inset,
        (unsigned int)child->core.width - 2U * (unsigned int)inset - 1U,
        (unsigned int)child->core.height - 2U * (unsigned int)inset - 1U);
    XFreeGC(XtDisplay(child), gc);
}

Boolean _XmSegmentedControlDraw(Widget child) {
    XmSegmentedControlWidget control;
    XmToggleButtonWidget button;
    Display *display;
    Drawable window;
    XGCValues values;
    GC gc;
    int x, width, height, stroke, radius;
    unsigned int child_width, child_height;
    Boolean separate;
    if (!_XmSegmentedControlUsesModern(child)) return False;
    if (!XtIsRealized(child)) return True;
    control = (XmSegmentedControlWidget)XtParent(child);
    button = (XmToggleButtonWidget)child;
    display = XtDisplay(child); window = XtWindow(child);
    /* Use the uncapped natural extent, as layout does: a preferred width
     * clamped to SHRT_MAX must not make overflowing gaps appear to fit. */
    {
        uint64_t natural = 0;
        Cardinal count = control->segmented.item_count;
        for (Cardinal i = 0; i < count; ++i) {
            Dimension w, h;
            natural_size(control->segmented.items[i].button, &w, &h);
            natural += w;
        }
        if (count > 1) natural += (uint64_t)control->segmented.spacing * (count-1);
        separate = control->segmented.spacing &&
            natural <= (unsigned)MIN(control->core.width, SHRT_MAX);
    }
    x = separate ? 0 : -(int)child->core.x;
    child_width = (unsigned int)child->core.width;
    child_height = (unsigned int)child->core.height;
    width = separate ? (int)child_width : (int)MIN(control->core.width, SHRT_MAX);
    height = (int)child_height;
    stroke = MAX(1, (int)_XmDpiDefaultDimension(child, 1));
    radius = MAX(stroke, (int)_XmDpiDefaultDimension(child, 6));
    values.foreground = control->core.background_pixel;
    gc = XCreateGC(display, window, GCForeground, &values);
    if (!gc) return True;
    XFillRectangle(display, window, gc, 0, 0, child_width, child_height);
    XSetForeground(display, gc, button->primitive.bottom_shadow_color);
    _XmAppearanceFillRoundedRectangle(display, window, gc, x, 0, width, height, radius);
    XSetForeground(display, gc, button->toggle.visual_set != XmUNSET ?
        button->toggle.select_color : button->toggle.unselect_color);
    if (width > 2 * stroke && height > 2 * stroke)
        _XmAppearanceFillRoundedRectangle(display, window, gc, x + stroke, stroke,
            width - 2 * stroke, height - 2 * stroke, MAX(0, radius - stroke));
    /* Each physical segment boundary is painted once, on its right side. */
    if (!separate && child->core.x > 0) {
        XSetForeground(display, gc, button->primitive.bottom_shadow_color);
        if (height > 2*stroke)
            XFillRectangle(display, window, gc, 0, stroke, (unsigned int)stroke,
                           (unsigned int)(height - 2 * stroke));
    }
    _XmRedisplayLabel(child, NULL, NULL, False);
    _XmSegmentedControlDrawFocus(child);
    XFreeGC(display, gc);
    return True;
}

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Resize(Widget);
static void ChangeManaged(Widget);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult GeometryManager(Widget, XtWidgetGeometry *,
                                       XtWidgetGeometry *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);
static void child_changed(Widget, XtPointer, XtPointer);
static void child_key(Widget, XtPointer, XEvent *, Boolean *);

#define Offset(field) XtOffsetOf(XmSegmentedControlRec, field)
static XtResource resources[] = {
    {XmNSegmentedAllowClear, XmCSegmentedAllowClear, XmRBoolean,
     sizeof(Boolean), Offset(segmented.allow_clear), XmRImmediate,
     (XtPointer)False},
    {XmNSegmentedRtl, XmCSegmentedRtl, XmRBoolean, sizeof(Boolean),
     Offset(segmented.rtl), XmRImmediate, (XtPointer)False},
    {XmNSegmentedOverflow, XmCSegmentedOverflow, XmRUnsignedChar,
     sizeof(unsigned char), Offset(segmented.overflow), XmRImmediate,
     (XtPointer)XmSEGMENTED_OVERFLOW_CLIP},
    {XmNSegmentedSpacing, XmCSegmentedSpacing, XmRHorizontalDimension,
     sizeof(Dimension), Offset(segmented.spacing), XmRImmediate, (XtPointer)0},
    {XmNSegmentedSelectionCallback, XmCSegmentedSelectionCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(segmented.selection_changed_callback),
     XmRPointer, (XtPointer)NULL}
};

/* Preferred label geometry is independent of the last allocated segment size. */
static void natural_size(Widget child, Dimension *width, Dimension *height) {
    XtWidgetGeometry preferred = {0};
    *width = *height = 1;
    if (!child) return;
    XtQueryGeometry(child, NULL, &preferred);
    *width = (preferred.request_mode & CWWidth) ? preferred.width : child->core.width;
    *height = (preferred.request_mode & CWHeight) ? preferred.height : child->core.height;
    if (!*width) *width = 1;
    if (!*height) *height = 1;
}

/* The same pure allocation calculation serves layout and query-only requests.
 * Zero-share children remain managed but unmapped until space is available. */
static void allocate_segments(XmSegmentedControlWidget control, Widget target,
                              XtWidgetGeometry *result, Boolean apply) {
    Cardinal count = control->segmented.item_count;
    uint64_t natural = 0, gaps, usable, extra;
    /* Child origins are signed X Position values. Clip extreme allocations
     * to that addressable extent instead of wrapping origins negative. */
    unsigned available = MIN(control->core.width, SHRT_MAX);
    unsigned spacing = control->segmented.spacing;
    unsigned height = control->core.height ? control->core.height : 1;
    unsigned cursor = 0;
    if (!count || control->segmented.rebuilding) return;
    for (Cardinal i = 0; i < count; ++i) {
        Dimension width, child_height;
        natural_size(control->segmented.items[i].button, &width, &child_height);
        natural += width;
    }
    gaps = count > 1 ? (uint64_t)spacing * (count - 1) : 0;
    if (natural + gaps > available) spacing = 0;
    gaps = count > 1 ? (uint64_t)spacing * (count - 1) : 0;
    usable = available - gaps;
    extra = usable > natural ? usable - natural : 0;
    for (Cardinal i = 0; i < count; ++i) {
        Widget child = control->segmented.items[i].button;
        Dimension preferred, child_height;
        unsigned width, x;
        uint64_t allocated_width;
        natural_size(child, &preferred, &child_height);
        allocated_width = natural <= usable
            ? (uint64_t)preferred + extra / count + (i < extra % count)
            : usable / count + (i < usable % count);
        width = (unsigned)allocated_width;
        x = control->segmented.rtl ? available - cursor - width : cursor;
        if (child == target && result) {
            result->request_mode = CWX | CWY | CWWidth | CWHeight | CWBorderWidth;
            result->x = (Position)x; result->y = 0;
            result->width = (Dimension)(width ? width : 1U);
            result->height = (Dimension)height;
            result->border_width = 0;
        }
        if (apply && child && child != target) {
            XtSetMappedWhenManaged(child, width != 0);
            if (width)
                XtConfigureWidget(child, (Position)x, 0, (Dimension)width,
                                  (Dimension)height, 0);
        }
        cursor += width + (i + 1 < count ? spacing : 0);
    }
}

static void layout(XmSegmentedControlWidget control) {
    allocate_segments(control, control->segmented.geometry_child, NULL, True);
}

/* Request only dimensions owned by the toolkit. A parent remains free to
 * constrain allocation; QueryGeometry continues to report the natural size. */
static void request_natural_size(XmSegmentedControlWidget control) {
    XtWidgetGeometry preferred={0};
    Dimension width,height,reply_width,reply_height;
    if (control->segmented.rebuilding || control->segmented.sizing ||
        (!control->segmented.compute_width && !control->segmented.compute_height)) return;
    QueryGeometry((Widget)control,NULL,&preferred);
    width=control->segmented.compute_width ? preferred.width : control->core.width;
    height=control->segmented.compute_height ? preferred.height : control->core.height;
    if (width==control->core.width && height==control->core.height) return;
    control->segmented.sizing=True;
    if (XtMakeResizeRequest((Widget)control,width,height,&reply_width,&reply_height)==XtGeometryAlmost)
        (void)XtMakeResizeRequest((Widget)control,reply_width,reply_height,NULL,NULL);
    control->segmented.sizing=False;
}

void _XmSegmentedControlRefreshGeometry(Widget widget) {
    XmSegmentedControlWidget control=(XmSegmentedControlWidget)widget;
    request_natural_size(control);
    layout(control);
}

static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmSegmentedControlWidget control = (XmSegmentedControlWidget)widget;
    (void)args; (void)count;
    control->segmented.compute_width = request->core.width == 0;
    control->segmented.compute_height = request->core.height == 0;
    control->segmented.sizing = False;
    control->segmented.geometry_child = NULL;
    control->segmented.items = NULL;
    control->segmented.item_count = 0;
    control->segmented.selected = -1;
    if (!control->core.width) control->core.width = 120;
    if (!control->core.height) control->core.height = 24;
}

static void Destroy(Widget widget) {
    XmSegmentedControlWidget control = (XmSegmentedControlWidget)widget;
    Cardinal i;
    for (i = 0; i < control->segmented.item_count; ++i) {
        if (control->segmented.items[i].id) XtFree(control->segmented.items[i].id);
        if (control->segmented.items[i].label) XmStringFree(control->segmented.items[i].label);
    }
    if (control->segmented.items) XtFree((char *)control->segmented.items);
}

static void Resize(Widget widget) { layout((XmSegmentedControlWidget)widget); }
static void ChangeManaged(Widget widget) {
    request_natural_size((XmSegmentedControlWidget)widget);
    layout((XmSegmentedControlWidget)widget);
}

static XtGeometryResult GeometryManager(Widget child, XtWidgetGeometry *request,
                                       XtWidgetGeometry *reply) {
    XmSegmentedControlWidget control = (XmSegmentedControlWidget)XtParent(child);
    XtWidgetGeometry allocation = {0};
    Boolean matches;
    if (control->segmented.rebuilding ||
        (request->request_mode & (CWSibling | CWStackMode))) return XtGeometryNo;
    if (!(request->request_mode & XtCWQueryOnly) &&
        (request->request_mode & (CWWidth | CWHeight))) {
        control->segmented.geometry_child = child;
        request_natural_size(control);
        control->segmented.geometry_child = NULL;
    }
    allocate_segments(control, child, &allocation, False);
    if (!allocation.request_mode) return XtGeometryNo;
    matches = (!(request->request_mode & CWX) || request->x == allocation.x) &&
              (!(request->request_mode & CWY) || request->y == allocation.y) &&
              (!(request->request_mode & CWWidth) || request->width == allocation.width) &&
              (!(request->request_mode & CWHeight) || request->height == allocation.height) &&
              (!(request->request_mode & CWBorderWidth) || request->border_width == 0);
    if (!matches) {
        *reply = allocation;
        return XtGeometryAlmost;
    }
    if (!(request->request_mode & XtCWQueryOnly))
        allocate_segments(control, child, NULL, True);
    return XtGeometryYes;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmSegmentedControlWidget control = (XmSegmentedControlWidget)widget;
    uint64_t width = 0;
    Dimension height = 1;
    Cardinal i;
    for (i = 0; i < control->segmented.item_count; ++i) {
        Dimension child_width = 1, child_height = 1;
        natural_size(control->segmented.items[i].button, &child_width, &child_height);
        width += (unsigned long)child_width;
        if (child_height > height) height = child_height;
    }
    if (control->segmented.item_count > 1)
        width += (uint64_t)control->segmented.spacing *
                 (uint64_t)(control->segmented.item_count - 1);
    if (!width) width = 120;
    if (width > SHRT_MAX)
        width = SHRT_MAX;
    if (height < 24) height = 24;
    reply->request_mode = CWWidth | CWHeight;
    reply->width = (Dimension)width; reply->height = height;
    return XmeReplyToQueryGeometry(widget, request, reply);
}

/* The notification owns its ID snapshot across callback model replacement. */
static Boolean emit_selection(XmSegmentedControlWidget control, XEvent *event) {
    XmSegmentedControlCallbackStruct callback;
    char *id = control->segmented.selected >= 0
        ? XtNewString(control->segmented.items[control->segmented.selected].id)
        : NULL;
    callback.reason = XmCR_VALUE_CHANGED;
    callback.event = event;
    callback.selected_index = control->segmented.selected;
    callback.selected_id = id;
    Boolean alive = _XmCallCallbacksUntilDestroyed((Widget)control,
                         XmNSegmentedSelectionCallback, &callback);
    XtFree(id);
    return alive;
}

typedef enum { SelectionRejected, SelectionAlive, SelectionDestroyed } SelectionResult;
static SelectionResult select_index(XmSegmentedControlWidget control, int index,
                                     Boolean notify, XEvent *event) {
    Cardinal i;
    if (index < -1 || index >= (int)control->segmented.item_count)
        return SelectionRejected;
    if (index >= 0 && !control->segmented.items[index].enabled)
        return SelectionRejected;
    if (index == control->segmented.selected) {
        /* Assignment is idempotent. A native deselection gesture arrives here
         * after clearing its toggle; restore it when selection is required. */
        if (index >= 0)
            XmToggleButtonSetState(control->segmented.items[index].button,
                                   True, False);
        return SelectionAlive;
    }
    control->segmented.selected = index;
    for (i = 0; i < control->segmented.item_count; ++i)
        XmToggleButtonSetState(control->segmented.items[i].button,
                               (int)i == index, False);
    if (notify && !control->segmented.rebuilding && !control->core.being_destroyed)
        return emit_selection(control, event) ? SelectionAlive : SelectionDestroyed;
    return SelectionAlive;
}

static void child_changed(Widget child, XtPointer closure, XtPointer call_data) {
    XmSegmentedControlWidget control = (XmSegmentedControlWidget)closure;
    Cardinal i;
    Boolean set = False;
    XEvent *event = NULL;
    XmToggleButtonCallbackStruct *callback =
        (XmToggleButtonCallbackStruct *)call_data;
    if (control == NULL || child == NULL ||
        !XmIsSegmentedControl((Widget)control))
	return;
    if (control->core.being_destroyed || control->segmented.rebuilding) return;
    if (callback) { set = callback->set ? True : False; event = callback->event; }
    for (i = 0; i < control->segmented.item_count; ++i)
        if (control->segmented.items[i].button == child) break;
    if (i == control->segmented.item_count) return;
    if (!control->segmented.items[i].enabled) {
        XmToggleButtonSetState(child, False, False);
        return;
    }
    if (set) select_index(control, (int)i, True, event);
    else if ((int)i == control->segmented.selected)
        select_index(control, control->segmented.allow_clear ? -1 : (int)i,
                     True, event);
}

static int child_index(XmSegmentedControlWidget control, Widget child) {
    Cardinal i;
    for (i = 0; i < control->segmented.item_count; ++i)
        if (control->segmented.items[i].button == child) return (int)i;
    return -1;
}

static void child_key(Widget child, XtPointer closure, XEvent *event,
                      Boolean *dispatch) {
    XmSegmentedControlWidget control = (XmSegmentedControlWidget)closure;
    KeySym key; char buffer[8]; int index, next, step;
    if (control == NULL || child == NULL || event == NULL || dispatch == NULL ||
        !XmIsSegmentedControl((Widget)control) || control->core.being_destroyed) return;
    if (event->type != KeyPress || !XtIsSensitive((Widget)control) ||
        !XtIsSensitive(child)) return;
    XLookupString(&event->xkey, buffer, sizeof(buffer), &key, NULL);
    index = child_index(control, child);
    if (index < 0) return;
    next = index;
    if (key == XK_Left || key == XK_Up) {
        step = control->segmented.rtl ? 1 : -1; next += step;
    } else if (key == XK_Right || key == XK_Down) {
        step = control->segmented.rtl ? -1 : 1; next += step;
    } else if (key == XK_Home) { next = 0; step = 1; }
    else if (key == XK_End) {
        next = (int)control->segmented.item_count - 1; step = -1;
    } else return;
    *dispatch = False;
    while (next >= 0 && next < (int)control->segmented.item_count &&
           !control->segmented.items[next].enabled) next += step;
    if (next < 0 || next >= (int)control->segmented.item_count) return;
    if (select_index(control, next, True, event) != SelectionAlive) return;
    /* Reentrant callbacks may replace the model or choose another item. Focus
     * only the surviving current selection, never an old item/index. */
    next = control->segmented.selected;
    if (next >= 0 && next < (int)control->segmented.item_count)
        XmProcessTraversal(control->segmented.items[next].button, XmTRAVERSE_CURRENT);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmSegmentedControlWidget old = (XmSegmentedControlWidget)old_widget;
    XmSegmentedControlWidget control = (XmSegmentedControlWidget)widget;
    Boolean redraw = False;
    for (Cardinal i=0;i<*count;++i) {
        if (!strcmp(args[i].name,XmNwidth))
            control->segmented.compute_width=request->core.width==0;
        if (!strcmp(args[i].name,XmNheight))
            control->segmented.compute_height=request->core.height==0;
    }
    if (control->segmented.compute_width || control->segmented.compute_height) {
        XtWidgetGeometry preferred={0};
        QueryGeometry(widget,NULL,&preferred);
        if (control->segmented.compute_width) control->core.width=preferred.width;
        if (control->segmented.compute_height) control->core.height=preferred.height;
    }
    if (old->segmented.allow_clear != control->segmented.allow_clear ||
        old->segmented.rtl != control->segmented.rtl ||
        old->segmented.spacing != control->segmented.spacing) {
        layout(control);
        redraw = True;
    }
    return redraw;
}

Boolean XmSegmentedControlSetItems(Widget widget,
                                   const XmSegmentedControlItem *items,
                                   Cardinal count) {
    XmSegmentedControlWidget control;
    XmSegmentedControlItemRec *copy;
    XmSegmentedControlItemRec *old_items;
    Cardinal old_count, selected_index = 0;
    const char *selected_id = NULL;
    int preserved_selected = -1;
    Cardinal i;
    if (widget == NULL || !XmIsSegmentedControl(widget) ||
        widget->core.being_destroyed ||
        (count && !items) || count > (Cardinal)INT_MAX) return False;
    control = (XmSegmentedControlWidget)widget;
    if (count) {
        if ((size_t)count > SIZE_MAX / sizeof(*copy) ||
            (size_t)count > UINT_MAX / sizeof(*copy))
            return False;
        copy = (XmSegmentedControlItemRec *)XtCalloc(
            (Cardinal)count, sizeof(*copy));
        if (!copy) return False;
        for (i = 0; i < count; ++i) {
            copy[i].id = XtNewString(items[i].id ? items[i].id : "");
            copy[i].label = items[i].label ? XmStringCopy(items[i].label)
                                           : XmStringCreateLocalized("");
            if (!copy[i].id || !copy[i].label) {
                Cardinal j;
                for (j = 0; j <= i; ++j) {
                    XtFree(copy[j].id);
                    XmStringFree(copy[j].label);
                }
                XtFree((char *)copy);
                return False;
            }
            copy[i].pixmap = items[i].pixmap;
            copy[i].enabled = items[i].enabled;
        }
    } else copy = NULL;
    old_items = control->segmented.items;
    old_count = control->segmented.item_count;
    if (control->segmented.selected >= 0 &&
        (Cardinal)control->segmented.selected < old_count)
        selected_id = old_items[control->segmented.selected].id;
    control->segmented.rebuilding = True;
    for (i = 0; i < count; ++i) {
        char name[32];
        Arg args[8]; Cardinal n = 0;
        snprintf(name, sizeof(name), "segment%u", (unsigned)i);
        XtSetArg(args[n], XmNlabelString, copy[i].label); ++n;
        XtSetArg(args[n], XmNindicatorOn, False); ++n;
        XtSetArg(args[n], XmNfillOnSelect, True); ++n;
        XtSetArg(args[n], XtNsensitive, copy[i].enabled); ++n;
        if (copy[i].pixmap != XmUNSPECIFIED_PIXMAP) {
            XtSetArg(args[n], XmNlabelType, XmPIXMAP_AND_STRING); ++n;
            XtSetArg(args[n], XmNlabelPixmap, copy[i].pixmap); ++n;
        }
        copy[i].button = XmCreateToggleButton(widget, name, args, n);
        if (!copy[i].button) {
            Cardinal j;
            XtWarning("XmSegmentedControlSetItems: unable to create segment button");
            for (j = 0; j <= i; ++j) {
                if (copy[j].button) XtDestroyWidget(copy[j].button);
                XtFree(copy[j].id);
                XmStringFree(copy[j].label);
            }
            XtFree((char *)copy);
            control->segmented.rebuilding = False;
            return False;
        }
        XtAddCallback(copy[i].button, XmNvalueChangedCallback, child_changed,
                      (XtPointer)control);
        XtAddEventHandler(copy[i].button, KeyPressMask, False, child_key,
                          (XtPointer)control);
    }
    if (selected_id != NULL) {
        for (selected_index = 0; selected_index < count; ++selected_index)
            if (strcmp(copy[selected_index].id, selected_id) == 0 &&
                copy[selected_index].enabled) {
                preserved_selected = (int)selected_index;
                break;
            }
    }
    for (i = 0; i < old_count; ++i)
        XtDestroyWidget(old_items[i].button);
    for (i = 0; i < old_count; ++i) {
        XtFree(old_items[i].id);
        XmStringFree(old_items[i].label);
    }
    XtFree((char *)old_items);
    control->segmented.items = copy;
    control->segmented.item_count = count;
    control->segmented.selected = preserved_selected;
    /* Manage only after every item has a valid button.  Managing inside the
     * creation loop invokes the composite change-managed hook early; layout
     * then walks later, still-uninitialized item slots and dereferences a
     * null child. */
    for (i = 0; i < count; ++i)
        XtManageChild(copy[i].button);
    for (i = 0; i < count; ++i)
        XmToggleButtonSetState(copy[i].button,
                               (int)i == control->segmented.selected, False);
    control->segmented.rebuilding = False;
    request_natural_size(control);
    layout(control);
    return True;
}

Cardinal XmSegmentedControlGetItemCount(Widget widget) {
    return widget != NULL && XmIsSegmentedControl(widget) && !widget->core.being_destroyed
               ? ((XmSegmentedControlWidget)widget)->segmented.item_count : 0;
}
int XmSegmentedControlGetSelectedIndex(Widget widget) {
    return widget != NULL && XmIsSegmentedControl(widget) && !widget->core.being_destroyed
               ? ((XmSegmentedControlWidget)widget)->segmented.selected : -1;
}
const char *XmSegmentedControlGetItemId(Widget widget, Cardinal index) {
    XmSegmentedControlWidget control;
    if (widget == NULL || !XmIsSegmentedControl(widget) ||
        widget->core.being_destroyed) return NULL;
    control = (XmSegmentedControlWidget)widget;
    if (index >= control->segmented.item_count)
        return NULL;
    return control->segmented.items[index].id;
}
Boolean XmSegmentedControlSetSelectedIndex(Widget widget, int index,
                                           Boolean notify) {
    if (widget == NULL || !XmIsSegmentedControl(widget)) return False;
    /* Report admission, without dereferencing a callback-destroyed control. */
    return select_index((XmSegmentedControlWidget)widget, index, notify, NULL)
           != SelectionRejected;
}
void XmSegmentedControlClearSelection(Widget widget, Boolean notify) {
    (void)XmSegmentedControlSetSelectedIndex(widget, -1, notify);
}

Widget XmCreateSegmentedControl(Widget parent, char *name, ArgList args,
                                Cardinal arg_count) {
    return XtCreateWidget(name, xmSegmentedControlWidgetClass, parent, args,
                          arg_count);
}
Widget XmVaCreateSegmentedControl(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name);
    widget = XmeVLCreateWidget(name, xmSegmentedControlWidgetClass, parent,
                               False, var, count);
    va_end(var); return widget;
}
Widget XmVaCreateManagedSegmentedControl(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name);
    widget = XmeVLCreateWidget(name, xmSegmentedControlWidgetClass, parent,
                               True, var, count);
    va_end(var); return widget;
}

XmSegmentedControlClassRec xmSegmentedControlClassRec = {
    { (WidgetClass)&xmManagerClassRec, "XmSegmentedControl",
      sizeof(XmSegmentedControlRec), NULL, NULL, False, Initialize, NULL,
      XtInheritRealize, NULL, 0, resources, XtNumber(resources), NULLQUARK,
      True, XtExposeCompressMaximal, True, False, Destroy, Resize, NULL,
      SetValues, NULL, XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
      XtInheritTranslations, QueryGeometry, NULL, NULL },
    { GeometryManager, ChangeManaged, XtInheritInsertChild,
      XtInheritDeleteChild, NULL },
    { NULL, 0, 0, NULL, NULL, NULL, NULL },
    { XtInheritTranslations, NULL, 0, NULL, 0, XmInheritParentProcess, NULL },
    { NULL }
};
WidgetClass xmSegmentedControlWidgetClass =
    (WidgetClass)&xmSegmentedControlClassRec;
