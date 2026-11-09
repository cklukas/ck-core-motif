#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "PaintI.h"
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/TreeViewP.h>
#include <Xm/Appearance.h>
#include <string.h>
#include <Xm/XmP.h>
#include <limits.h>
#include <stdint.h>
#include "PrimitiveI.h"
#include "ColorI.h"
#include "TransltnsP.h"
#include "XmI.h"
#include "AppearanceDrawI.h"
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>

#define TREE_VIEW_MAX_NODES 4096

static int clamp_coordinate(long long value) {
    return value < INT_MIN ? INT_MIN :
           value > INT_MAX ? INT_MAX : (int)value;
}

static Position bounded_position(long long value) {
    if (value < SHRT_MIN) return (Position)SHRT_MIN;
    if (value > SHRT_MAX) return (Position)SHRT_MAX;
    return (Position)value;
}

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void BufferedRedisplay(Widget, XEvent *, Region);
static void Highlight(Widget);
static void Unhighlight(Widget);
static void Resize(Widget);
static Boolean BufferedSetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);
static void Events(Widget, XtPointer, XEvent *, Boolean *);

static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmTreeViewWidget view = (XmTreeViewWidget)widget;
    (void)offset;
    if (view->tree_view.check_set_render_table)
        value->addr = NULL;
    else {
        view->tree_view.check_set_render_table = True;
        value->addr = (char *)&view->tree_view.render_table;
    }
}

#define Offset(field) XtOffsetOf(XmTreeViewRec, field)
static XtResource resources[] = {
    {XmNselectColor, XmCSelectColor, XmRPixel,
     sizeof(Pixel), Offset(tree_view.select_color), XmRCallProc,
     (XtPointer)_XmSelectColorDefault},
    {XmNtreeViewIndent, XmCTreeViewIndent, XmRHorizontalDimension,
     sizeof(Dimension), Offset(tree_view.indent), XmRImmediate, (XtPointer)16},
    {XmNtreeViewRowHeight, XmCTreeViewRowHeight, XmRVerticalDimension,
     sizeof(Dimension), Offset(tree_view.row_height), XmRImmediate, (XtPointer)0},
    {XmNtreeViewVisibleRows, XmCTreeViewVisibleRows, XmRCardinal,
     sizeof(Cardinal), Offset(tree_view.visible_rows), XmRImmediate, (XtPointer)8},
    {XmNtreeViewTopRow, XmCTreeViewTopRow, XmRCardinal,
     sizeof(Cardinal), Offset(tree_view.top_row), XmRImmediate, (XtPointer)0},
    {XmNtreeViewRtl, XmCTreeViewRtl, XmRBoolean, sizeof(Boolean),
     Offset(tree_view.rtl), XmRImmediate, (XtPointer)False},
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmRenderTable),
     Offset(tree_view.render_table), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmRenderTable),
     Offset(tree_view.render_table), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNactivateCallback, XmCCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(tree_view.activate_callback), XmRPointer,
     (XtPointer)NULL},
    {XmNtreeViewSelectionCallback, XmCTreeViewSelectionCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(tree_view.selection_callback), XmRPointer,
     (XtPointer)NULL},
    {XmNtreeViewExpansionCallback, XmCTreeViewExpansionCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(tree_view.expansion_callback), XmRPointer,
     (XtPointer)NULL},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc,
     (XtPointer)_XmSetThickness}
};

externaldef(xmtreeviewclassrec) XmTreeViewClassRec xmTreeViewClassRec =
    {{(WidgetClass)&xmPrimitiveClassRec, "XmTreeView", sizeof(XmTreeViewRec),
      NULL, NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0,
      resources, XtNumber(resources), NULLQUARK, True,
      XtExposeCompressMaximal, True, False, Destroy, Resize, BufferedRedisplay,
      BufferedSetValues, NULL, XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
      (String)_XmPrimitive_defaultTranslations, QueryGeometry, NULL, NULL},
     {Highlight, Unhighlight,
      XtInheritTranslations, NULL, NULL, 0, NULL},
     {NULL}};

externaldef(xmtreeviewwidgetclass) WidgetClass xmTreeViewWidgetClass =
    (WidgetClass)&xmTreeViewClassRec;

static int find_id(XmTreeViewWidget view, XtPointer id) {
    Cardinal i;
    for (i = 0; i < view->tree_view.count; ++i)
        if (view->tree_view.items[i].id == id)
            return (int)i;
    return -1;
}

static int input_id(const XmTreeViewNode *nodes, Cardinal count,
                    XtPointer id) {
    Cardinal i;
    for (i = 0; i < count; ++i)
        if (nodes[i].id == id)
            return (int)i;
    return -1;
}

static Boolean valid_input(const XmTreeViewNode *nodes, Cardinal count) {
    Cardinal i, j, steps;
    if (count && !nodes)
        return False;
    for (i = 0; i < count; ++i) {
        if (!nodes[i].id || (nodes[i].expanded && !nodes[i].has_children))
            return False;
        for (j = i + 1; j < count; ++j)
            if (nodes[i].id == nodes[j].id)
                return False;
        if (nodes[i].parent_id && input_id(nodes, count,
                                           nodes[i].parent_id) < 0)
            return False;
    }
    for (i = 0; i < count; ++i) {
        XtPointer parent = nodes[i].parent_id;
        for (steps = 0; parent && steps <= count; ++steps) {
            int p = input_id(nodes, count, parent);
            if (p < 0 || parent == nodes[i].id)
                return False;
            parent = nodes[p].parent_id;
        }
        if (parent)
            return False;
    }
    for (i = 0; i < count; ++i)
        if (nodes[i].parent_id) {
            int parent = input_id(nodes, count, nodes[i].parent_id);
            if (!nodes[parent].has_children)
                return False;
        }
    return True;
}

/* Normalize the accepted flat model to stable depth-first order. Iteration
 * keeps model depth off the C stack; each node is queued exactly once after
 * valid_input has established a forest with unique IDs. */
static Cardinal *input_preorder(const XmTreeViewNode *nodes, Cardinal count) {
    Cardinal *order, *stack, used = 0, pending = 0, i;
    size_t bytes;

    if (!count)
        return NULL;
#if SIZE_MAX < UINT_MAX
    if ((size_t)count > SIZE_MAX / sizeof(*order))
        return NULL;
#endif
    if (count > UINT_MAX / sizeof(*order))
        return NULL;
    bytes = (size_t)count * sizeof(*order);
    order = (Cardinal *)XtMalloc((Cardinal)bytes);
    stack = (Cardinal *)XtMalloc((Cardinal)bytes);
    if (!order || !stack) {
        XtFree((char *)order);
        XtFree((char *)stack);
        return NULL;
    }
    for (i = count; i != 0; --i)
        if (!nodes[i - 1].parent_id) stack[pending++] = i - 1;
    while (pending) {
        Cardinal index = stack[--pending];
        order[used++] = index;
        for (i = count; i != 0; --i)
            if (nodes[i - 1].parent_id == nodes[index].id)
                stack[pending++] = i - 1;
    }
    XtFree((char *)stack);
    return order;
}

static int depth_of(XmTreeViewWidget view, Cardinal index) {
    int depth = 0;
    XtPointer parent = view->tree_view.items[index].parent_id;
    Cardinal steps = 0;
    while (parent != NULL && steps++ < view->tree_view.count) {
        int p = find_id(view, parent);
        if (p < 0)
            break;
        ++depth;
        parent = view->tree_view.items[p].parent_id;
    }
    return depth;
}

static Boolean is_visible(XmTreeViewWidget view, Cardinal index) {
    XtPointer parent = view->tree_view.items[index].parent_id;
    Cardinal steps = 0;
    while (parent != NULL && steps++ < view->tree_view.count) {
        int p = find_id(view, parent);
        if (p < 0 || !view->tree_view.items[p].expanded)
            return False;
        parent = view->tree_view.items[p].parent_id;
    }
    return parent == NULL;
}

static Cardinal visible_count(XmTreeViewWidget view) {
    Cardinal i, count = 0;
    for (i = 0; i < view->tree_view.count; ++i)
        if (is_visible(view, i))
            ++count;
    return count;
}

static int visible_item(XmTreeViewWidget view, Cardinal row) {
    Cardinal i, seen = 0;
    for (i = 0; i < view->tree_view.count; ++i) {
        if (!is_visible(view, i))
            continue;
        if (seen++ == row)
            return (int)i;
    }
    return -1;
}

static int visible_row_for_id(XmTreeViewWidget view, XtPointer id) {
    Cardinal i, seen = 0;
    for (i = 0; i < view->tree_view.count; ++i) {
        if (!is_visible(view, i))
            continue;
        if (view->tree_view.items[i].id == id)
            return (int)seen;
        ++seen;
    }
    return -1;
}

static int first_child(XmTreeViewWidget view, XtPointer parent_id) {
    Cardinal i;
    for (i = 0; i < view->tree_view.count; ++i)
        if (view->tree_view.items[i].parent_id == parent_id &&
            is_visible(view, i))
            return (int)i;
    return -1;
}

static void notify_selection(XmTreeViewWidget view, XEvent *event) {
    XmTreeViewCallbackStruct cb;
    if (view == NULL || view->core.being_destroyed) return;
    int index = find_id(view, view->tree_view.selected_id);
    if (view == NULL || view->core.being_destroyed ||
        !view->tree_view.selection_callback || index < 0 ||
        (Cardinal)index >= view->tree_view.count)
        return;
    cb.reason = 1;
    cb.event = event;
    cb.id = view->tree_view.items[index].id;
    cb.parent_id = view->tree_view.items[index].parent_id;
    cb.index = (Cardinal)index;
    cb.expanded = view->tree_view.items[index].expanded;
    XtCallCallbacks((Widget)view, XmNtreeViewSelectionCallback, &cb);
}

static void notify_activation(XmTreeViewWidget view, int index, XEvent *event) {
    XmTreeViewCallbackStruct cb;
    if (!view->tree_view.activate_callback || index < 0 ||
        view->tree_view.items[index].has_children) return;
    cb.reason = XmCR_ACTIVATE;
    cb.event = event;
    cb.id = view->tree_view.items[index].id;
    cb.parent_id = view->tree_view.items[index].parent_id;
    cb.index = (Cardinal)index;
    cb.expanded = view->tree_view.items[index].expanded;
    XtCallCallbacks((Widget)view, XmNactivateCallback, &cb);
}

static void notify_expansion(XmTreeViewWidget view, int index, XEvent *event) {
    XmTreeViewCallbackStruct cb;
    if (view == NULL || view->core.being_destroyed ||
        !view->tree_view.expansion_callback || index < 0 ||
        (Cardinal)index >= view->tree_view.count)
        return;
    cb.reason = 2;
    cb.event = event;
    cb.id = view->tree_view.items[index].id;
    cb.parent_id = view->tree_view.items[index].parent_id;
    cb.index = (Cardinal)index;
    cb.expanded = view->tree_view.items[index].expanded;
    XtCallCallbacks((Widget)view, XmNtreeViewExpansionCallback, &cb);
}

static Cardinal viewport_rows(XmTreeViewWidget view);

/* Keep the standard ScrolledWindow navigator in row units, including changes
 * made by keyboard, wheel, expansion, resizing and programmatic selection. */
static void update_scrollbar(XmTreeViewWidget view) {
    Widget bar=view->tree_view.scroll_bar;
    Cardinal total, rows, last;
    if (!bar || view->tree_view.updating_scrollbar || view->core.being_destroyed) return;
    view->tree_view.updating_scrollbar=True;
    total=visible_count(view); rows=viewport_rows(view);
    last=total>rows?total-rows:0;
    if (view->tree_view.top_row>last) view->tree_view.top_row=last;
    XtVaSetValues(bar, XmNminimum,0, XmNmaximum,MAX(1,total),
        XmNsliderSize,MIN(MAX(1,total),rows), XmNvalue,view->tree_view.top_row,
        XmNincrement,1, XmNpageIncrement,MAX(1,rows-1), NULL);
    if (XtIsManaged((Widget)view) && total>rows) {
        if (!XtIsManaged(bar)) XtManageChild(bar);
    } else if (XtIsManaged(bar)) XtUnmanageChild(bar);
    view->tree_view.updating_scrollbar=False;
}

static void slider_move(Widget bar, XtPointer closure, XtPointer call) {
    XmTreeViewWidget view=(XmTreeViewWidget)closure;
    XmScrollBarCallbackStruct *data=(XmScrollBarCallbackStruct *)call;
    (void)bar;
    if (view && !view->core.being_destroyed && data)
        XmTreeViewSetTopRow((Widget)view,
                            data->value > 0 ? (Cardinal)data->value : 0);
}

static void redraw(XmTreeViewWidget view) {
    update_scrollbar(view);
    if (XtIsRealized((Widget)view))
        _XmPaintInvalidate((Widget)view);
}

/* visible_rows is a preferred-size resource, not the allocated viewport. */
static Cardinal viewport_rows(XmTreeViewWidget view) {
    return MAX(1, view->core.height / MAX(1, view->tree_view.row_height));
}

static void ensure_selected_visible(XmTreeViewWidget view) {
    int row=visible_row_for_id(view,view->tree_view.selected_id);
    if (row >= 0 && (Cardinal)row >= view->tree_view.top_row +
                    viewport_rows(view))
        view->tree_view.top_row = (Cardinal)row - viewport_rows(view) + 1;
    else if (row >= 0 && (Cardinal)row < view->tree_view.top_row)
        view->tree_view.top_row = (Cardinal)row;
}

static void Resize(Widget widget) {
    XmTreeViewWidget view = (XmTreeViewWidget)widget;
    Cardinal total = visible_count(view), rows = viewport_rows(view);
    Cardinal last_top = total > rows ? total - rows : 0;
    if (view->tree_view.top_row > last_top)
        view->tree_view.top_row = last_top;
    ensure_selected_visible(view);
    redraw(view);
}

static void select_id(XmTreeViewWidget view, XtPointer id, Boolean notify,
                      XEvent *event) {
    Boolean changed=view->tree_view.selected_id != id;
    if (find_id(view,id)<0) return;
    view->tree_view.selected_id=id;
    ensure_selected_visible(view);
    redraw(view);
    if (notify && changed) notify_selection(view,event);
}

static void toggle_item(XmTreeViewWidget view, int index, Boolean expanded,
                        Boolean notify, XEvent *event) {
    if (index < 0 || !view->tree_view.items[index].has_children ||
        view->tree_view.items[index].expanded == expanded)
        return;
    view->tree_view.items[index].expanded = expanded;
    redraw(view);
    if (notify)
        notify_expansion(view, index, event);
}

Boolean _XmTreeViewHasSelectionAction(Widget widget) {
    XmTreeViewWidget view;
    int index;
    if (!widget || !XmIsTreeView(widget) || !XtIsSensitive(widget) ||
        widget->core.being_destroyed) return False;
    view = (XmTreeViewWidget)widget;
    index = find_id(view, view->tree_view.selected_id);
    return index >= 0 && is_visible(view, (Cardinal)index);
}

Boolean _XmTreeViewActivateSelection(Widget widget, XEvent *event) {
    XmTreeViewWidget view;
    int index;
    if (!_XmTreeViewHasSelectionAction(widget)) return False;
    view = (XmTreeViewWidget)widget;
    index = find_id(view, view->tree_view.selected_id);
    view->tree_view.pressed_id = NULL;
    if (view->tree_view.items[index].has_children)
        toggle_item(view, index, !view->tree_view.items[index].expanded,
                    True, event);
    else
        notify_activation(view, index, event);
    /* The callback can retire the widget or its accessibility bridge. */
    return True;
}

/* The same modern disclosure slot drives measurement, drawing and hits. */
static Boolean disclosure_metrics(XmTreeViewWidget view, int *inset,
                                   int *size, int *gap) {
    XmAppearanceProfile profile;
    if (!XmGetAppearanceProfile(XtDisplay((Widget)view), &profile) ||
        (profile != XmAPPEARANCE_MODERN_LIGHT &&
         profile != XmAPPEARANCE_MODERN_DARK)) return False;
    *inset = _XmDpiDefaultDimension((Widget)view, 8);
    *size = _XmDpiDefaultDimension((Widget)view, 12);
    *gap = _XmDpiDefaultDimension((Widget)view, 6);
    return True;
}

/* Include every node so expanding a branch does not change preferred width.
 * Text extent, indentation and disclosure space share one measurement path. */
static void natural_metrics(XmTreeViewWidget view, Dimension *width, Dimension *row_height) {
    uint64_t widest=0, tallest=0;
    Dimension padding=0;
    int inset=4, indicator=9, gap=5;
    Boolean modern=disclosure_metrics(view,&inset,&indicator,&gap);
    XmString sample=XmStringCreateLocalized("Mg");
    Dimension w=0,h=0;
    if (view->tree_view.render_table)
        XmStringExtent(view->tree_view.render_table,sample,&w,&h);
    XmStringFree(sample);tallest=h;
    (void)XmGetAppearanceSpacing(XtDisplay((Widget)view),XmAPPEARANCE_SPACING_XS,&padding);
    for (Cardinal i=0;i<view->tree_view.count;++i) {
        XmString label=XmStringCreateLocalized(view->tree_view.items[i].label);
        uint64_t extent;
        w=h=0;
        if (view->tree_view.render_table)
            XmStringExtent(view->tree_view.render_table,label,&w,&h);
        XmStringFree(label);
        extent=(uint64_t)w+(uint64_t)depth_of(view,i)*view->tree_view.indent+
            (modern ? 2ULL*(uint64_t)inset+(uint64_t)indicator+(uint64_t)gap
                    : 22ULL);
        if (extent>widest) widest=extent;
        if (h>tallest) tallest=h;
    }
    if (!widest) widest=_XmDpiDefaultDimension((Widget)view,120);
    tallest=MAX(tallest,(uint64_t)indicator)+2*(uint64_t)padding;
    *width=(Dimension)MIN(widest,SHRT_MAX);
    *row_height=(Dimension)MIN(MAX(tallest,1),USHRT_MAX);
}

static void clear_bar(Widget bar, XtPointer closure, XtPointer call) {
    XmTreeViewWidget view=(XmTreeViewWidget)closure;
    (void)bar; (void)call;
    view->tree_view.scroll_bar=NULL;
}
static void focus_destroyed(Widget widget, XtPointer closure, XtPointer call) {
    (void)widget; (void)call; *(Boolean *)closure=True;
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmTreeViewWidget view = (XmTreeViewWidget)widget;
    XGCValues values;
    (void)request; (void)args; (void)count;
    view->tree_view.automatic_width = ((XmTreeViewWidget)request)->core.width == 0;
    view->tree_view.automatic_height = ((XmTreeViewWidget)request)->core.height == 0;
    view->tree_view.items = NULL;
    view->tree_view.count = 0;
    view->tree_view.selected_id = NULL;
    view->tree_view.pressed_id = NULL;
    view->tree_view.scroll_bar = NULL;
    view->tree_view.updating_scrollbar = False;
    if (view->tree_view.render_table == NULL)
        view->tree_view.render_table = XmFontListCopy(
            XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
    else
        view->tree_view.render_table = XmFontListCopy(
            view->tree_view.render_table);
    view->tree_view.automatic_row_height = view->tree_view.row_height == 0;
    if (view->tree_view.automatic_row_height) {
        Dimension width;
        natural_metrics(view,&width,&view->tree_view.row_height);
    }
    if (!view->tree_view.visible_rows)
        view->tree_view.visible_rows = 1;
    values.foreground = view->primitive.foreground;
    values.background = view->core.background_pixel;
    view->tree_view.gc = XCreateGC(XtDisplay(widget),
                                   DefaultRootWindow(XtDisplay(widget)),
                                   GCForeground | GCBackground, &values);
    if (!((XmTreeViewWidget)request)->core.width) {
        Dimension height;
        natural_metrics(view,&view->core.width,&height);
    }
    if (!((XmTreeViewWidget)request)->core.height) {
        unsigned long long height =
            (unsigned long long)view->tree_view.row_height *
            (unsigned long long)view->tree_view.visible_rows;
        view->core.height = height > (unsigned long long)(Dimension)-1 ?
                            (Dimension)-1 : (Dimension)height;
    }
    if (XmIsScrolledWindow(XtParent(widget))) {
        Widget existing=NULL;
        unsigned char policy;
        XtVaGetValues(XtParent(widget),XmNverticalScrollBar,&existing,
                      XmNscrollingPolicy,&policy,NULL);
        if (policy==XmAPPLICATION_DEFINED && !existing) {
            Arg a[3]; Cardinal n=0;
            XtSetArg(a[n],XmNorientation,XmVERTICAL); ++n;
            XtSetArg(a[n],XmNtraversalOn,False); ++n;
            view->tree_view.scroll_bar=XmCreateScrollBar(XtParent(widget),"treeScrollBar",a,n);
            if (view->tree_view.scroll_bar) {
                XtAddCallback(view->tree_view.scroll_bar,XmNdestroyCallback,clear_bar,view);
                XtAddCallback(view->tree_view.scroll_bar,XmNvalueChangedCallback,slider_move,view);
                XtAddCallback(view->tree_view.scroll_bar,XmNdragCallback,slider_move,view);
            }
        }
    }
    XtAddEventHandler(widget, KeyPressMask | ButtonPressMask | ButtonReleaseMask, False, Events,
                      NULL);
}

static void Destroy(Widget widget) {
    XmTreeViewWidget view = (XmTreeViewWidget)widget;
    Widget bar=view->tree_view.scroll_bar;
    if (bar) {
        XtRemoveCallback(bar,XmNdestroyCallback,clear_bar,view);
        view->tree_view.scroll_bar=NULL;
        if (!bar->core.being_destroyed) XtDestroyWidget(bar);
    }
    Cardinal i;
    XtRemoveEventHandler(widget, KeyPressMask | ButtonPressMask | ButtonReleaseMask, False, Events,
                         NULL);
    for (i = 0; i < view->tree_view.count; ++i)
        XtFree(view->tree_view.items[i].label);
    XtFree((char *)view->tree_view.items);
    if (view->tree_view.render_table)
        XmRenderTableFree(view->tree_view.render_table);
    if (view->tree_view.gc)
        XFreeGC(XtDisplay(widget), view->tree_view.gc);
}

static void draw_disclosure(XmTreeViewWidget view, int x, int y, Boolean open) {
    Display *display = XtDisplay((Widget)view);
    Drawable window = _XmPaintDrawable((Widget)view);
    int inset, size, gap;
    if (disclosure_metrics(view, &inset, &size, &gap)) {
        int extent = MIN(size, (int)view->tree_view.row_height - 2);
        int left = x + (size - extent) / 2;
        int top = y + ((int)view->tree_view.row_height - extent) / 2;
        XRectangle clip;
        if (extent < 3 || left < 0 || left + extent > view->core.width ||
            top < 0 || top + extent > view->core.height ||
            left + extent > SHRT_MAX || top + extent > SHRT_MAX) return;
        clip=(XRectangle){(short)left,(short)top,
                           (unsigned short)extent,(unsigned short)extent};
        XSetClipRectangles(display,view->tree_view.gc,0,0,&clip,1,Unsorted);
        int glyph = MIN(extent, (int)_XmDpiDefaultDimension((Widget)view,8));
        _XmAppearanceDrawChevron(display, window, view->tree_view.gc,
            left+(extent-glyph)/2, top+(extent-glyph)/2, glyph, glyph,
            _XmDpiDefaultDimension((Widget)view,2),
            open ? XmARROW_DOWN : (view->tree_view.rtl ? XmARROW_LEFT : XmARROW_RIGHT));
        XSetClipMask(display,view->tree_view.gc,None);
        return;
    }
    /* Use a nine-pixel raster box (an eight-pixel X11 extent), whose centre
     * is an actual pixel.  A one-pixel stroke then lands on the same centre
     * pixel horizontally and vertically instead of straddling an even-sized
     * box and looking offset. */
    /* The indicator is a nine-pixel raster box (an eight-pixel X11
     * extent).  Center it in the actual row instead of assuming the
     * historical 21-pixel row height; custom row heights must not make the
     * disclosure control appear too high or too low. */
    const int box_size = 9;
    const int top = y + (view->tree_view.row_height >= box_size ?
                         ((int)view->tree_view.row_height - box_size) / 2 : 0);
    const int center_x = x + 4;
    const int center_y = top + 4;
    XDrawRectangle(display, window, view->tree_view.gc, x, top, 8, 8);
    XDrawLine(display, window, view->tree_view.gc, x + 2, center_y, x + 6,
              center_y);
    if (!open)
        XDrawLine(display, window, view->tree_view.gc, center_x, top + 2,
                  center_x, top + 6);
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmTreeViewWidget view = (XmTreeViewWidget)widget;
    Display *display = XtDisplay(widget);
    Drawable window = _XmPaintDrawable(widget);
    Cardinal row, total = visible_count(view);
    int index, inset=4, indicator=9, gap=5;
    Boolean modern=disclosure_metrics(view,&inset,&indicator,&gap);
    const char *label;
    (void)event; (void)region;
    if (!window || !view->tree_view.gc)
        return;
    update_scrollbar(view);
    for (row = 0; row < viewport_rows(view); ++row) {
        Cardinal absolute = view->tree_view.top_row + row;
        int x;
        int y = clamp_coordinate((long long)row *
                                 (long long)view->tree_view.row_height);
        if (absolute >= total)
            break;
        index = visible_item(view, absolute);
        if (index < 0)
            break;
        XSetBackground(display,view->tree_view.gc,view->core.background_pixel);
        if (modern && view->tree_view.items[index].id == view->tree_view.selected_id) {
            int height=MIN((int)view->tree_view.row_height,(int)view->core.height-y);
            if (height > 2 && view->core.width > 4 && y < SHRT_MAX) {
                XSetForeground(display,view->tree_view.gc,view->tree_view.select_color);
                XFillRectangle(display,window,view->tree_view.gc,2,y+1,
                    (unsigned int)view->core.width-4U,
                    (unsigned int)height-2U);
                XSetForeground(display,view->tree_view.gc,view->primitive.foreground);
                XSetBackground(display,view->tree_view.gc,view->tree_view.select_color);
            }
        } else if (view->tree_view.items[index].id == view->tree_view.selected_id)
            XDrawRectangle(display, window, view->tree_view.gc, 1, y + 1,
                           view->core.width > 3 ? view->core.width - 3 : 1,
                           view->tree_view.row_height > 3 ?
                           view->tree_view.row_height - 3 : 1);
        {
            long long depth_offset = (long long)depth_of(view, (Cardinal)index) *
                                     view->tree_view.indent;
            long long origin = view->tree_view.rtl ?
                               (long long)view->core.width - (modern ? inset+indicator : 14) :
                               (modern ? inset : 4);
            long long item_x = view->tree_view.rtl ? origin - depth_offset :
                               origin + depth_offset;
            x = item_x < INT_MIN ? INT_MIN :
                (item_x > INT_MAX ? INT_MAX : (int)item_x);
        }
        if (view->tree_view.items[index].has_children) {
            if (!view->tree_view.rtl)
                draw_disclosure(view, x, y, view->tree_view.items[index].expanded);
            else
                draw_disclosure(view, x - (modern ? 0 : 10), y,
                                view->tree_view.items[index].expanded);
        }
        label = view->tree_view.items[index].label ?
                view->tree_view.items[index].label : "";
        {
            Dimension label_width = 0, label_height = 0;
            XmString label_string = XmStringCreateLocalized((char *)label);
            if (view->tree_view.render_table && label_string)
                XmStringExtent(view->tree_view.render_table, label_string,
                               &label_width, &label_height);
        if (!view->tree_view.rtl)
            x += modern ? indicator+gap : 14;
        else {
            long long rtl_x = (long long)x - (long long)label_width - (modern ? gap : 4);
            x = rtl_x < INT_MIN ? INT_MIN :
                (rtl_x > INT_MAX ? INT_MAX : (int)rtl_x);
        }
        if (view->tree_view.render_table && label_string) {
            int label_y = y + ((int)view->tree_view.row_height -
                               (int)label_height) / 2;
            XmStringDraw(display, window, view->tree_view.render_table,
                         label_string, view->tree_view.gc,
                         bounded_position((long long)x),
                         bounded_position((long long)label_y),
                         label_width, XmALIGNMENT_BEGINNING,
                         XmPrim_layout_direction(view), NULL);
        }
        if (label_string)
            XmStringFree(label_string);
        }
    }
    XSetBackground(display,view->tree_view.gc,view->core.background_pixel);
    if (modern) XSetForeground(display,view->tree_view.gc,view->primitive.highlight_color);
    if (view->primitive.highlighted && view->primitive.highlight_thickness &&
        view->core.width > 2 * view->primitive.highlight_thickness &&
        view->core.height > 2 * view->primitive.highlight_thickness)
        XDrawRectangle(display, window, view->tree_view.gc,
                       view->primitive.highlight_thickness - 1,
                       view->primitive.highlight_thickness - 1,
                       view->core.width - 2 * view->primitive.highlight_thickness,
                       view->core.height - 2 * view->primitive.highlight_thickness);
    XSetForeground(display,view->tree_view.gc,view->primitive.foreground);
}

static void BufferedRedisplay(Widget widget, XEvent *event, Region region)
{
    _XmBufferedPaint(widget, Redisplay, event, region);
}

/* Keep focus transitions in the same frame as selection and disclosure marks.
 * Primitive's border clearing would erase the selected row's top outline. */
static void Highlight(Widget widget)
{
    XmTreeViewWidget view = (XmTreeViewWidget)widget;
    view->primitive.highlighted = view->primitive.highlight_drawn = True;
    BufferedRedisplay(widget, NULL, NULL);
}
static void Unhighlight(Widget widget)
{
    XmTreeViewWidget view = (XmTreeViewWidget)widget;
    view->primitive.highlighted = view->primitive.highlight_drawn = False;
    BufferedRedisplay(widget, NULL, NULL);
}

static int row_at(XmTreeViewWidget view, int y) {
    Cardinal total = visible_count(view);
    Cardinal row;
    if (y < 0 || y >= view->core.height || view->tree_view.row_height == 0 ||
        (Cardinal)(y / view->tree_view.row_height) >= viewport_rows(view))
        return -1;
    row = view->tree_view.top_row + (Cardinal)(y / view->tree_view.row_height);
    if (row >= total)
        return -1;
    return visible_item(view, row);
}

static void Events(Widget widget, XtPointer data, XEvent *event,
                   Boolean *dispatch) {
    if (widget == NULL || event == NULL || !XmIsTreeView(widget))
        return;
    XmTreeViewWidget view = (XmTreeViewWidget)widget;
    KeySym key;
    char buffer[2];
    int length, index, row, next;
    (void)data; (void)dispatch;
    if (!XtIsSensitive(widget)) {
        view->tree_view.pressed_id = NULL;
        return;
    }
    if (event->type == ButtonRelease) {
        XtPointer pressed = view->tree_view.pressed_id;
        view->tree_view.pressed_id = NULL;
        index = row_at(view,event->xbutton.y);
        if (event->xbutton.button == Button1 && pressed && index >= 0 &&
            event->xbutton.x >= 0 && event->xbutton.x < view->core.width &&
            event->xbutton.y < view->core.height &&
            pressed == view->tree_view.items[index].id &&
            pressed == view->tree_view.selected_id)
            notify_activation(view,index,event);
        return;
    }
    if (event->type == ButtonPress) {
        if (event->xbutton.button == Button1 && view->primitive.traversal_on) {
            Boolean destroyed=False;
            XtAddCallback(widget,XmNdestroyCallback,focus_destroyed,&destroyed);
            XmProcessTraversal(widget, XmTRAVERSE_CURRENT);
            if (destroyed) return;
            if (widget->core.being_destroyed) {
                XtRemoveCallback(widget,XmNdestroyCallback,focus_destroyed,&destroyed);
                return;
            }
            XtRemoveCallback(widget,XmNdestroyCallback,focus_destroyed,&destroyed);
        }
        view->tree_view.pressed_id = NULL;
        if (event->xbutton.button == Button4) {
            if (view->tree_view.top_row)
                --view->tree_view.top_row;
            redraw(view);
            return;
        }
        if (event->xbutton.button == Button5) {
            if (view->tree_view.top_row + viewport_rows(view) <
                visible_count(view))
                ++view->tree_view.top_row;
            redraw(view);
            return;
        }
        index = row_at(view, event->xbutton.y);
        if (index < 0)
            return;
        {
            int inset, size, gap;
            if (view->tree_view.items[index].has_children &&
                disclosure_metrics(view,&inset,&size,&gap)) {
                long long offset=(long long)depth_of(view,(Cardinal)index)*view->tree_view.indent;
                long long left=view->tree_view.rtl ?
                    (long long)view->core.width-inset-size-offset : inset+offset;
                if (event->xbutton.button == Button1 &&
                    event->xbutton.x >= left-gap/2 &&
                    event->xbutton.x < left+size+gap/2) {
                    toggle_item(view,index,!view->tree_view.items[index].expanded,True,event);
                    return;
                }
                select_id(view,view->tree_view.items[index].id,True,event);
                return;
            }
        }
        if (event->xbutton.button == Button1 && event->xbutton.x >= 0 &&
            event->xbutton.x < view->core.width && event->xbutton.y < view->core.height)
            view->tree_view.pressed_id = view->tree_view.items[index].id;
        if (view->tree_view.selected_id != view->tree_view.items[index].id) {
            select_id(view, view->tree_view.items[index].id, True, event);
            return;
        }
        if ((!view->tree_view.rtl && event->xbutton.x <
             8 + depth_of(view, (Cardinal)index) * view->tree_view.indent + 12) ||
            (view->tree_view.rtl && event->xbutton.x >
             (int)view->core.width - 20 -
             depth_of(view, (Cardinal)index) * view->tree_view.indent))
            toggle_item(view, index, !view->tree_view.items[index].expanded,
                        True, event);
        return;
    }
    if (event->type != KeyPress)
        return;
    view->tree_view.pressed_id = NULL;
    length = XLookupString(&event->xkey, buffer, sizeof(buffer), &key, NULL);
    (void)length;
    /* Tree navigation consumes these keys; inherited Primitive traversal and
     * parent activation must not also run for the same event. Tab and focus
     * actions remain owned by the standard Primitive translation table. */
    if (key == XK_Up || key == XK_Down || key == XK_Left || key == XK_Right ||
        key == XK_Return || key == XK_KP_Enter)
        *dispatch = False;
    index = find_id(view, view->tree_view.selected_id);
    row = index < 0 ? -1 : visible_row_for_id(view, view->tree_view.selected_id);
    if (key == XK_Up || key == XK_Down) {
        if (row < 0)
            next = visible_item(view, 0);
        else {
            next = visible_item(view, (Cardinal)(row + (key == XK_Down ? 1 : -1)));
            if (next < 0)
                return;
        }
        if (next >= 0) {
            select_id(view, view->tree_view.items[next].id, True, event);
            return;
        }
    } else if (key == XK_Left || key == XK_Right) {
        Boolean forward = (key == XK_Right) != view->tree_view.rtl;
        if (index < 0)
            return;
        if (forward && view->tree_view.items[index].has_children &&
            !view->tree_view.items[index].expanded) {
            toggle_item(view, index, True, True, event);
            return;
        } else if (forward && view->tree_view.items[index].expanded) {
            next = first_child(view, view->tree_view.items[index].id);
            if (next >= 0) {
                select_id(view, view->tree_view.items[next].id, True, event);
                return;
            }
        } else if (!forward && view->tree_view.items[index].expanded) {
            toggle_item(view, index, False, True, event);
            return;
        } else {
            next = find_id(view, view->tree_view.items[index].parent_id);
            if (next >= 0) {
                select_id(view, view->tree_view.items[next].id, True, event);
                return;
            }
        }
    } else if (key == XK_Return || key == XK_KP_Enter) {
        (void)_XmTreeViewActivateSelection(widget, event);
        return;
    }
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmTreeViewWidget old = (XmTreeViewWidget)old_widget;
    XmTreeViewWidget view = (XmTreeViewWidget)widget;
    Boolean colors_changed = old->primitive.foreground != view->primitive.foreground ||
                             old->core.background_pixel != view->core.background_pixel;
    (void)request; (void)args; (void)count;
    if (!XtIsSensitive(widget)) view->tree_view.pressed_id = NULL;
    if (colors_changed && view->tree_view.gc) {
        XGCValues values;
        values.foreground = view->primitive.foreground;
        values.background = view->core.background_pixel;
        XChangeGC(XtDisplay(widget), view->tree_view.gc,
                  GCForeground | GCBackground, &values);
    }
    for (Cardinal i=0;i<*count;++i)
        if (!strcmp(args[i].name,XmNtreeViewRowHeight))
            view->tree_view.automatic_row_height = view->tree_view.row_height == 0;
    if (view->tree_view.visible_rows == 0)
        view->tree_view.visible_rows = 1;
    if (old->tree_view.render_table != view->tree_view.render_table) {
        XmRenderTable new_table = view->tree_view.render_table ?
            XmFontListCopy(view->tree_view.render_table) :
            XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
        if (new_table) {
            if (old->tree_view.render_table)
                XmRenderTableFree(old->tree_view.render_table);
            view->tree_view.render_table = new_table;
        } else {
            /* Keep the last valid table if a replacement cannot be copied. */
            view->tree_view.render_table = old->tree_view.render_table;
        }
    }
    if (view->tree_view.automatic_row_height) {
        Dimension width;
        natural_metrics(view,&width,&view->tree_view.row_height);
    }
    for (Cardinal i=0;i<*count;++i)
        if (!strcmp(args[i].name,XmNwidth))
            view->tree_view.automatic_width = (Dimension)args[i].value == 0;
    for (Cardinal i=0;i<*count;++i)
        if (!strcmp(args[i].name,XmNheight))
            view->tree_view.automatic_height = (Dimension)args[i].value == 0;
    if (view->tree_view.automatic_width) {
        Dimension height;
        natural_metrics(view, &view->core.width, &height);
    }
    if (view->tree_view.automatic_height) {
        uint64_t height = (uint64_t)view->tree_view.row_height * view->tree_view.visible_rows;
        view->core.height = (Dimension)MIN(MAX(height,1),USHRT_MAX);
    }
    if (old->tree_view.visible_rows != view->tree_view.visible_rows)
        ensure_selected_visible(view);
    return colors_changed ||
           old->tree_view.select_color != view->tree_view.select_color ||
           old->primitive.highlight_color != view->primitive.highlight_color ||
           old->tree_view.indent != view->tree_view.indent ||
           old->tree_view.row_height != view->tree_view.row_height ||
           old->tree_view.visible_rows != view->tree_view.visible_rows ||
           old->tree_view.top_row != view->tree_view.top_row ||
           old->tree_view.rtl != view->tree_view.rtl ||
           old->tree_view.render_table != view->tree_view.render_table || colors_changed;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmTreeViewWidget view = (XmTreeViewWidget)widget;
    unsigned long height;
    Dimension row_height;
    reply->request_mode = CWWidth | CWHeight;
    natural_metrics(view,&reply->width,&row_height);
    if (!view->tree_view.automatic_width)
        reply->width = view->core.width;
    height = (unsigned long)view->tree_view.row_height *
             (unsigned long)view->tree_view.visible_rows;
    reply->height = height > (unsigned long)(Dimension)-1
                        ? (Dimension)-1 : (Dimension)height;
    if (!view->tree_view.automatic_height)
        reply->height = view->core.height;
    return XmeReplyToQueryGeometry(widget,request,reply);
}

Widget XmCreateTreeView(Widget parent, char *name, ArgList args,
                        Cardinal count) {
    return XtCreateWidget(name, xmTreeViewWidgetClass, parent, args, count);
}

void XmTreeViewSetNodes(Widget widget, const XmTreeViewNode *nodes,
                        Cardinal count) {
    XmTreeViewWidget view;
    XmTreeViewItem *items;
    Cardinal i, *order;
    XtPointer selected;
    if (widget == NULL || !XmIsTreeView(widget) || widget->core.being_destroyed ||
        count > TREE_VIEW_MAX_NODES ||
        !valid_input(nodes, count))
        return;
    view = (XmTreeViewWidget)widget;
    selected = view->tree_view.selected_id;
    items = count ? (XmTreeViewItem *)XtCalloc(count, sizeof(*items)) : NULL;
    if (count != 0 && items == NULL)
        return;
    order = input_preorder(nodes, count);
    if (count && !order) {
        XtFree((char *)items);
        return;
    }
    for (i = 0; i < count; ++i) {
        const XmTreeViewNode *node = &nodes[order[i]];
        items[i].id = node->id;
        items[i].parent_id = node->parent_id;
        items[i].label = XtNewString(node->label ? node->label : "");
        if (items[i].label == NULL) {
            while (i != 0)
                XtFree(items[--i].label);
            XtFree((char *)items);
            XtFree((char *)order);
            return;
        }
        items[i].has_children = node->has_children;
        items[i].expanded = node->expanded;
    }
    XtFree((char *)order);
    for (i = 0; i < view->tree_view.count; ++i)
        XtFree(view->tree_view.items[i].label);
    XtFree((char *)view->tree_view.items);
    view->tree_view.pressed_id = NULL;
    view->tree_view.items = items;
    view->tree_view.count = count;
    view->tree_view.selected_id = find_id(view, selected) >= 0 ? selected : NULL;
    view->tree_view.top_row = 0;
    if (view->tree_view.automatic_row_height) {
        Dimension width;
        natural_metrics(view,&width,&view->tree_view.row_height);
    }
    redraw(view);
}

Cardinal XmTreeViewGetNodeCount(Widget widget) {
    return widget != NULL && XmIsTreeView(widget) && !widget->core.being_destroyed
        ? ((XmTreeViewWidget)widget)->tree_view.count : 0;
}

XtPointer XmTreeViewGetSelectedId(Widget widget) {
    return widget != NULL && XmIsTreeView(widget) && !widget->core.being_destroyed ?
        ((XmTreeViewWidget)widget)->tree_view.selected_id : NULL;
}

void XmTreeViewSetSelectedId(Widget widget, XtPointer id,
                             Boolean notify_change) {
    if (widget != NULL && XmIsTreeView(widget) && !widget->core.being_destroyed)
        select_id((XmTreeViewWidget)widget, id, notify_change, NULL);
}

Boolean XmTreeViewSetExpanded(Widget widget, XtPointer id, Boolean expanded,
                              Boolean notify_change) {
    XmTreeViewWidget view;
    int index;
    if (widget == NULL || !XmIsTreeView(widget) || widget->core.being_destroyed)
        return False;
    view = (XmTreeViewWidget)widget;
    index = find_id(view, id);
    if (index < 0 || !view->tree_view.items[index].has_children)
        return False;
    toggle_item(view, index, expanded, notify_change, NULL);
    return True;
}

Boolean XmTreeViewGetExpanded(Widget widget, XtPointer id) {
    XmTreeViewWidget view;
    int index;
    if (widget == NULL || !XmIsTreeView(widget) || widget->core.being_destroyed)
        return False;
    view = (XmTreeViewWidget)widget;
    index = find_id(view, id);
    return index >= 0 && view->tree_view.items[index].expanded;
}

void XmTreeViewSetTopRow(Widget widget, Cardinal top_row) {
    XmTreeViewWidget view;
    Cardinal total;
    if (widget == NULL || !XmIsTreeView(widget) || widget->core.being_destroyed)
        return;
    view = (XmTreeViewWidget)widget;
    total = visible_count(view);
    if (top_row > total)
        top_row = total;
    view->tree_view.top_row = top_row;
    redraw(view);
}

Cardinal XmTreeViewGetTopRow(Widget widget) {
    return widget != NULL && XmIsTreeView(widget) && !widget->core.being_destroyed
        ? ((XmTreeViewWidget)widget)->tree_view.top_row : 0;
}

/* Xt's default redisplay request clears the front window. Queue our complete
 * frame instead, after resource and geometry negotiation has finished. */
static Boolean BufferedSetValues(Widget old, Widget request, Widget widget,
                                ArgList args, Cardinal *count)
{
    if (SetValues(old, request, widget, args, count))
        _XmPaintInvalidate(widget);
    return False;
}
