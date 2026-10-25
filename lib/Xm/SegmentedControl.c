/* Additive segmented picker built from ordinary public ToggleButton children. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <limits.h>
#include <stdio.h>
#include <stdint.h>
#include <Xm/SegmentedControlP.h>
#include <Xm/ToggleB.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include "XmI.h"

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

static void layout(XmSegmentedControlWidget control) {
    Dimension available, height, x, spacing;
    Cardinal i;
    unsigned long long pref_total = 0;
    if (!control->segmented.item_count) return;
    available = control->core.width > 2 * control->core.border_width
                    ? control->core.width - 2 * control->core.border_width : 1;
    height = control->core.height > 1 ? control->core.height : 1;
    spacing = control->segmented.spacing;
    for (i = 0; i < control->segmented.item_count; ++i) {
        Dimension width = 1;
        XtVaGetValues(control->segmented.items[i].button, XmNwidth, &width, NULL);
        pref_total += (unsigned long long)width;
    }
    if (control->segmented.item_count > 1)
        pref_total += (unsigned long long)spacing *
                      (unsigned long long)(control->segmented.item_count - 1);
    if (pref_total > (unsigned long long)available)
        spacing = 0;
    if (control->segmented.rtl) {
        x = available;
        for (i = 0; i < control->segmented.item_count; ++i) {
            Dimension width = pref_total <= (unsigned long long)available
                                  ? (Dimension)0 : available / control->segmented.item_count;
            if (!width) {
                Dimension natural = 1;
                XtVaGetValues(control->segmented.items[i].button, XmNwidth,
                              &natural, NULL);
                width = natural;
            }
            if (pref_total <= (unsigned long long)available) {
                XtVaGetValues(control->segmented.items[i].button, XmNwidth,
                              &width, NULL);
            }
            if (width > x) width = x;
            x -= width;
            XtConfigureWidget(control->segmented.items[i].button, x, 0, width,
                              height, 0);
            if (x >= spacing) x -= spacing;
        }
    } else {
        x = 0;
        for (i = 0; i < control->segmented.item_count; ++i) {
            Dimension width = pref_total <= (unsigned long long)available
                                  ? (Dimension)0 : available / control->segmented.item_count;
            if (!width) {
                Dimension natural = 1;
                XtVaGetValues(control->segmented.items[i].button, XmNwidth,
                              &natural, NULL);
                width = natural;
            }
            if (pref_total <= (unsigned long long)available)
                XtVaGetValues(control->segmented.items[i].button, XmNwidth,
                              &width, NULL);
	    if (x >= available)
		width = 0;
	    else if (width > available - x)
		width = available - x;
            XtConfigureWidget(control->segmented.items[i].button, x, 0, width,
                              height, 0);
	    if (x < available && width < available - x) {
		 x += width;
		 if (spacing < available - x) x += spacing;
		 else x = available;
	    } else {
		 x = available;
	    }
        }
    }
}

static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmSegmentedControlWidget control = (XmSegmentedControlWidget)widget;
    (void)request; (void)args; (void)count;
    control->segmented.items = NULL;
    control->segmented.item_count = 0;
    control->segmented.selected = -1;
    if (!control->core.width) control->core.width = 240;
    if (!control->core.height) control->core.height = 32;
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
static void ChangeManaged(Widget widget) { layout((XmSegmentedControlWidget)widget); }

static XtGeometryResult GeometryManager(Widget child, XtWidgetGeometry *request,
                                       XtWidgetGeometry *reply) {
    XmSegmentedControlWidget control = (XmSegmentedControlWidget)XtParent(child);
    (void)reply;
    if (request->request_mode & (CWWidth | CWHeight)) {
        XtConfigureWidget(child, child->core.x,
                          child->core.y,
                          (request->request_mode & CWWidth) ? request->width : child->core.width,
                          (request->request_mode & CWHeight) ? request->height : child->core.height,
                          child->core.border_width);
        layout(control);
    }
    return XtGeometryYes;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmSegmentedControlWidget control = (XmSegmentedControlWidget)widget;
    unsigned long width = 0;
    Dimension height = 1;
    Cardinal i;
    (void)request;
    for (i = 0; i < control->segmented.item_count; ++i) {
        Dimension child_width = 1, child_height = 1;
        XtVaGetValues(control->segmented.items[i].button, XmNwidth, &child_width,
                      XmNheight, &child_height, NULL);
        width += (unsigned long)child_width;
        if (child_height > height) height = child_height;
    }
    if (control->segmented.item_count > 1)
        width += (unsigned long)control->segmented.spacing *
                 (unsigned long)(control->segmented.item_count - 1);
    if (!width) width = 120;
    if (width > (unsigned long)(Dimension)-1)
        width = (unsigned long)(Dimension)-1;
    if (height < 24) height = 24;
    reply->request_mode = CWWidth | CWHeight;
    reply->width = (Dimension)width; reply->height = height;
    return (widget->core.width == width && widget->core.height == height)
               ? XtGeometryYes : XtGeometryAlmost;
}

static void emit_selection(XmSegmentedControlWidget control, XEvent *event) {
    XmSegmentedControlCallbackStruct callback;
    callback.reason = XmCR_VALUE_CHANGED;
    callback.event = event;
    callback.selected_index = control->segmented.selected;
    callback.selected_id = callback.selected_index >= 0
                               ? control->segmented.items[callback.selected_index].id
                               : NULL;
    XtCallCallbacks((Widget)control, XmNSegmentedSelectionCallback, &callback);
}

static void select_index(XmSegmentedControlWidget control, int index,
                         Boolean notify, XEvent *event) {
    Cardinal i;
    if (index < -1 || index >= (int)control->segmented.item_count) return;
    if (index >= 0 && !control->segmented.items[index].enabled) return;
    if (index == control->segmented.selected && !control->segmented.allow_clear)
        return;
    if (index == control->segmented.selected) index = -1;
    control->segmented.selected = index;
    for (i = 0; i < control->segmented.item_count; ++i)
        XmToggleButtonSetState(control->segmented.items[i].button,
                               (int)i == index, False);
    if (notify && !control->segmented.rebuilding && !control->core.being_destroyed)
        emit_selection(control, event);
}

static void child_changed(Widget child, XtPointer closure, XtPointer call_data) {
    XmSegmentedControlWidget control = (XmSegmentedControlWidget)closure;
    Cardinal i;
    Boolean set = False;
    XEvent *event = NULL;
    XmToggleButtonCallbackStruct *callback =
        (XmToggleButtonCallbackStruct *)call_data;
    if (control->segmented.rebuilding) return;
    if (callback) { set = callback->set; event = callback->event; }
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
    KeySym key; char buffer[8]; int index, next;
    (void)dispatch;
    if (event->type != KeyPress) return;
    XLookupString(&event->xkey, buffer, sizeof(buffer), &key, NULL);
    index = child_index(control, child); next = index;
    if (key == XK_Left || key == XK_Up) next += control->segmented.rtl ? 1 : -1;
    else if (key == XK_Right || key == XK_Down) next += control->segmented.rtl ? -1 : 1;
    else if (key == XK_Home) next = 0;
    else if (key == XK_End) next = (int)control->segmented.item_count - 1;
    else return;
    while (next >= 0 && next < (int)control->segmented.item_count &&
           !control->segmented.items[next].enabled)
        next += next >= index ? 1 : -1;
    if (next >= 0 && next < (int)control->segmented.item_count) {
        select_index(control, next, True, event);
        XmProcessTraversal(control->segmented.items[next].button, XmTRAVERSE_CURRENT);
    }
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmSegmentedControlWidget old = (XmSegmentedControlWidget)old_widget;
    XmSegmentedControlWidget control = (XmSegmentedControlWidget)widget;
    (void)request; (void)args; (void)count;
    if (old->segmented.allow_clear != control->segmented.allow_clear ||
        old->segmented.rtl != control->segmented.rtl ||
        old->segmented.spacing != control->segmented.spacing)
        layout(control);
    return False;
}

Boolean XmSegmentedControlSetItems(Widget widget,
                                   const XmSegmentedControlItem *items,
                                   Cardinal count) {
    XmSegmentedControlWidget control;
    XmSegmentedControlItemRec *copy;
    Cardinal i;
    if (!XmIsSegmentedControl(widget) || (count && !items)) return False;
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
    control->segmented.rebuilding = True;
    for (i = 0; i < control->segmented.item_count; ++i)
        XtDestroyWidget(control->segmented.items[i].button);
    for (i = 0; i < control->segmented.item_count; ++i) {
        XtFree(control->segmented.items[i].id);
        XmStringFree(control->segmented.items[i].label);
    }
    XtFree((char *)control->segmented.items);
    control->segmented.items = copy;
    control->segmented.item_count = count;
    control->segmented.selected = -1;
    for (i = 0; i < count; ++i) {
        char name[32];
        Arg args[8]; Cardinal n = 0;
        snprintf(name, sizeof(name), "segment%u", (unsigned)i);
        XtSetArg(args[n], XmNlabelString, copy[i].label); ++n;
        XtSetArg(args[n], XmNindicatorOn, False); ++n;
        XtSetArg(args[n], XmNfillOnSelect, True); ++n;
        XtSetArg(args[n], XmNshadowThickness, 1); ++n;
        XtSetArg(args[n], XmNhighlightThickness, 2); ++n;
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
            control->segmented.items = NULL;
            control->segmented.item_count = 0;
            control->segmented.selected = -1;
            control->segmented.rebuilding = False;
            return False;
        }
        XtAddCallback(copy[i].button, XmNvalueChangedCallback, child_changed,
                      (XtPointer)control);
        XtAddEventHandler(copy[i].button, KeyPressMask, False, child_key,
                          (XtPointer)control);
    }
    /* Manage only after every item has a valid button.  Managing inside the
     * creation loop invokes the composite change-managed hook early; layout
     * then walks later, still-uninitialized item slots and dereferences a
     * null child. */
    for (i = 0; i < count; ++i)
        XtManageChild(copy[i].button);
    control->segmented.rebuilding = False;
    layout(control);
    return True;
}

Cardinal XmSegmentedControlGetItemCount(Widget widget) {
    return XmIsSegmentedControl(widget)
               ? ((XmSegmentedControlWidget)widget)->segmented.item_count : 0;
}
int XmSegmentedControlGetSelectedIndex(Widget widget) {
    return XmIsSegmentedControl(widget)
               ? ((XmSegmentedControlWidget)widget)->segmented.selected : -1;
}
const char *XmSegmentedControlGetItemId(Widget widget, Cardinal index) {
    XmSegmentedControlWidget control = (XmSegmentedControlWidget)widget;
    if (!XmIsSegmentedControl(widget) || index >= control->segmented.item_count)
        return NULL;
    return control->segmented.items[index].id;
}
Boolean XmSegmentedControlSetSelectedIndex(Widget widget, int index,
                                           Boolean notify) {
    if (!XmIsSegmentedControl(widget)) return False;
    select_index((XmSegmentedControlWidget)widget, index, notify, NULL);
    return index == -1 || index == ((XmSegmentedControlWidget)widget)->segmented.selected;
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
    { NULL, 0, 0, NULL, NULL, NULL },
    { XtInheritTranslations, NULL, 0, NULL, 0, XmInheritParentProcess, NULL },
    { NULL }
};
WidgetClass xmSegmentedControlWidgetClass =
    (WidgetClass)&xmSegmentedControlClassRec;
