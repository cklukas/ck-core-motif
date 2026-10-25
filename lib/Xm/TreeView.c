#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/TreeViewP.h>
#include <Xm/XmP.h>
#include <limits.h>
#include <stdint.h>
#include "PrimitiveI.h"
#include "XmI.h"

#define TREE_VIEW_MAX_NODES 4096

static int clamp_coordinate(long long value) {
    return value < INT_MIN ? INT_MIN :
           value > INT_MAX ? INT_MAX : (int)value;
}

static void text_metrics(XmTreeViewWidget view, int *ascent, int *descent,
                         int *width, const char *text, int length) {
    XGCValues values;
    XFontStruct *font = NULL;
    int measured = 0;
    *ascent = 0;
    *descent = 0;
    *width = 0;
    if (!view->tree_view.gc ||
        !XGetGCValues(XtDisplay((Widget)view), view->tree_view.gc, GCFont,
                      &values))
        return;
    font = XQueryFont(XtDisplay((Widget)view), values.font);
    if (!font)
        return;
    *ascent = font->ascent;
    *descent = font->descent;
    measured = (text && length > 0) ? XTextWidth(font, text, length) : 0;
    *width = measured < 0 ? 0 : measured;
    XFreeFontInfo(NULL, font, 1);
}

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);
static void Events(Widget, XtPointer, XEvent *, Boolean *);

#define Offset(field) XtOffsetOf(XmTreeViewRec, field)
static XtResource resources[] = {
    {XmNtreeViewIndent, XmCTreeViewIndent, XmRHorizontalDimension,
     sizeof(Dimension), Offset(tree_view.indent), XmRImmediate, (XtPointer)16},
    {XmNtreeViewRowHeight, XmCTreeViewRowHeight, XmRVerticalDimension,
     sizeof(Dimension), Offset(tree_view.row_height), XmRImmediate, (XtPointer)22},
    {XmNtreeViewVisibleRows, XmCTreeViewVisibleRows, XmRCardinal,
     sizeof(Cardinal), Offset(tree_view.visible_rows), XmRImmediate, (XtPointer)8},
    {XmNtreeViewTopRow, XmCTreeViewTopRow, XmRCardinal,
     sizeof(Cardinal), Offset(tree_view.top_row), XmRImmediate, (XtPointer)0},
    {XmNtreeViewRtl, XmCTreeViewRtl, XmRBoolean, sizeof(Boolean),
     Offset(tree_view.rtl), XmRImmediate, (XtPointer)False},
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
      XtExposeCompressMaximal, True, False, Destroy, NULL, Redisplay,
      SetValues, NULL, XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
      XtInheritTranslations, QueryGeometry, NULL, NULL},
     {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
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
    int index = find_id(view, view->tree_view.selected_id);
    if (!view->tree_view.selection_callback || index < 0)
        return;
    cb.reason = 1;
    cb.event = event;
    cb.id = view->tree_view.items[index].id;
    cb.parent_id = view->tree_view.items[index].parent_id;
    cb.index = (Cardinal)index;
    cb.expanded = view->tree_view.items[index].expanded;
    XtCallCallbacks((Widget)view, XmNtreeViewSelectionCallback, &cb);
}

static void notify_expansion(XmTreeViewWidget view, int index, XEvent *event) {
    XmTreeViewCallbackStruct cb;
    if (!view->tree_view.expansion_callback || index < 0)
        return;
    cb.reason = 2;
    cb.event = event;
    cb.id = view->tree_view.items[index].id;
    cb.parent_id = view->tree_view.items[index].parent_id;
    cb.index = (Cardinal)index;
    cb.expanded = view->tree_view.items[index].expanded;
    XtCallCallbacks((Widget)view, XmNtreeViewExpansionCallback, &cb);
}

static void redraw(XmTreeViewWidget view) {
    if (XtIsRealized((Widget)view))
        XClearArea(XtDisplay((Widget)view), XtWindow((Widget)view), 0, 0, 0, 0,
                  True);
}

static void select_id(XmTreeViewWidget view, XtPointer id, Boolean notify,
                      XEvent *event) {
    int row;
    if (find_id(view, id) < 0 || view->tree_view.selected_id == id)
        return;
    view->tree_view.selected_id = id;
    row = visible_row_for_id(view, id);
    if (row >= 0 && (Cardinal)row >= view->tree_view.top_row +
                    view->tree_view.visible_rows)
        view->tree_view.top_row = (Cardinal)row - view->tree_view.visible_rows + 1;
    else if (row >= 0 && (Cardinal)row < view->tree_view.top_row)
        view->tree_view.top_row = (Cardinal)row;
    redraw(view);
    if (notify)
        notify_selection(view, event);
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

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmTreeViewWidget view = (XmTreeViewWidget)widget;
    XGCValues values;
    (void)request; (void)args; (void)count;
    view->tree_view.items = NULL;
    view->tree_view.count = 0;
    view->tree_view.selected_id = NULL;
    if (!view->tree_view.row_height)
        view->tree_view.row_height = 1;
    if (!view->tree_view.visible_rows)
        view->tree_view.visible_rows = 1;
    values.foreground = view->primitive.foreground;
    values.background = view->core.background_pixel;
    view->tree_view.gc = XCreateGC(XtDisplay(widget),
                                   DefaultRootWindow(XtDisplay(widget)),
                                   GCForeground | GCBackground, &values);
    if (!view->core.width)
        view->core.width = 280;
    if (!view->core.height) {
        unsigned long long height =
            (unsigned long long)view->tree_view.row_height *
            (unsigned long long)view->tree_view.visible_rows;
        view->core.height = height > (unsigned long long)(Dimension)-1 ?
                            (Dimension)-1 : (Dimension)height;
    }
    XtAddEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events,
                      NULL);
}

static void Destroy(Widget widget) {
    XmTreeViewWidget view = (XmTreeViewWidget)widget;
    Cardinal i;
    XtRemoveEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events,
                         NULL);
    for (i = 0; i < view->tree_view.count; ++i)
        XtFree(view->tree_view.items[i].label);
    XtFree((char *)view->tree_view.items);
    if (view->tree_view.gc)
        XFreeGC(XtDisplay(widget), view->tree_view.gc);
}

static void draw_disclosure(XmTreeViewWidget view, int x, int y, Boolean open) {
    Display *display = XtDisplay((Widget)view);
    Window window = XtWindow((Widget)view);
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
    Window window = XtWindow(widget);
    Cardinal row, total = visible_count(view);
    int index;
    int font_ascent, font_descent, unused_width;
    const char *label;
    (void)event; (void)region;
    if (!window || !view->tree_view.gc)
        return;
    text_metrics(view, &font_ascent, &font_descent, &unused_width, NULL, 0);
    XClearWindow(display, window);
    for (row = 0; row < view->tree_view.visible_rows; ++row) {
        Cardinal absolute = view->tree_view.top_row + row;
        int x;
        int y = clamp_coordinate((long long)row *
                                 (long long)view->tree_view.row_height);
        if (absolute >= total)
            break;
        index = visible_item(view, absolute);
        if (index < 0)
            break;
        if (view->tree_view.items[index].id == view->tree_view.selected_id)
            XDrawRectangle(display, window, view->tree_view.gc, 1, y + 1,
                           view->core.width > 3 ? view->core.width - 3 : 1,
                           view->tree_view.row_height > 3 ?
                           view->tree_view.row_height - 3 : 1);
        {
            long long depth_offset = (long long)depth_of(view, (Cardinal)index) *
                                     view->tree_view.indent;
            long long origin = view->tree_view.rtl ?
                               (long long)view->core.width - 14 : 4;
            long long item_x = view->tree_view.rtl ? origin - depth_offset :
                               origin + depth_offset;
            x = item_x < INT_MIN ? INT_MIN :
                (item_x > INT_MAX ? INT_MAX : (int)item_x);
        }
        if (view->tree_view.items[index].has_children) {
            if (!view->tree_view.rtl)
                draw_disclosure(view, x, y, view->tree_view.items[index].expanded);
            else
                draw_disclosure(view, x - 10, y,
                                view->tree_view.items[index].expanded);
        }
        label = view->tree_view.items[index].label ?
                view->tree_view.items[index].label : "";
        {
            size_t label_length = strlen(label);
            int draw_length = label_length > (size_t)INT_MAX ? INT_MAX :
                              (int)label_length;
            int label_width = 0;
            int baseline;
            text_metrics(view, &font_ascent, &font_descent, &label_width,
                         label, draw_length);
            baseline = clamp_coordinate((long long)y +
                ((long long)view->tree_view.row_height - font_ascent - font_descent) / 2 +
                font_ascent);
        if (!view->tree_view.rtl)
            x += 14;
        else {
            long long rtl_x = (long long)x - label_width - 4;
            x = rtl_x < INT_MIN ? INT_MIN :
                (rtl_x > INT_MAX ? INT_MAX : (int)rtl_x);
        }
        XDrawString(display, window, view->tree_view.gc, x,
                    baseline,
                    label, draw_length);
        }
    }
    if (view->primitive.highlighted && view->primitive.highlight_thickness &&
        view->core.width > 2 * view->primitive.highlight_thickness &&
        view->core.height > 2 * view->primitive.highlight_thickness)
        XDrawRectangle(display, window, view->tree_view.gc,
                       view->primitive.highlight_thickness - 1,
                       view->primitive.highlight_thickness - 1,
                       view->core.width - 2 * view->primitive.highlight_thickness,
                       view->core.height - 2 * view->primitive.highlight_thickness);
}

static int row_at(XmTreeViewWidget view, int y) {
    Cardinal total = visible_count(view);
    Cardinal row;
    if (y < 0 || view->tree_view.row_height == 0)
        return -1;
    row = view->tree_view.top_row + (Cardinal)(y / view->tree_view.row_height);
    if (row >= total)
        return -1;
    return visible_item(view, row);
}

static void Events(Widget widget, XtPointer data, XEvent *event,
                   Boolean *dispatch) {
    XmTreeViewWidget view = (XmTreeViewWidget)widget;
    KeySym key;
    char buffer[2];
    int length, index, row, next;
    (void)data; (void)dispatch;
    if (event->type == ButtonPress) {
        if (event->xbutton.button == Button4) {
            if (view->tree_view.top_row)
                --view->tree_view.top_row;
            redraw(view);
            return;
        }
        if (event->xbutton.button == Button5) {
            if (view->tree_view.top_row + view->tree_view.visible_rows <
                visible_count(view))
                ++view->tree_view.top_row;
            redraw(view);
            return;
        }
        index = row_at(view, event->xbutton.y);
        if (index < 0)
            return;
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
    length = XLookupString(&event->xkey, buffer, sizeof(buffer), &key, NULL);
    (void)length;
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
            row = visible_row_for_id(view, view->tree_view.items[next].id);
            if (row >= 0 && (Cardinal)row >= view->tree_view.top_row +
                view->tree_view.visible_rows)
                view->tree_view.top_row = (Cardinal)row -
                                           view->tree_view.visible_rows + 1;
            else if (row >= 0 && (Cardinal)row < view->tree_view.top_row)
                view->tree_view.top_row = (Cardinal)row;
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
        if (index >= 0 && view->tree_view.items[index].has_children)
            toggle_item(view, index, !view->tree_view.items[index].expanded,
                        True, event);
        return;
    }
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmTreeViewWidget old = (XmTreeViewWidget)old_widget;
    XmTreeViewWidget view = (XmTreeViewWidget)widget;
    (void)request; (void)args; (void)count;
    if (!view->tree_view.row_height)
        view->tree_view.row_height = 1;
    if (view->tree_view.visible_rows == 0)
        view->tree_view.visible_rows = 1;
    return old->tree_view.indent != view->tree_view.indent ||
           old->tree_view.row_height != view->tree_view.row_height ||
           old->tree_view.visible_rows != view->tree_view.visible_rows ||
           old->tree_view.top_row != view->tree_view.top_row ||
           old->tree_view.rtl != view->tree_view.rtl;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmTreeViewWidget view = (XmTreeViewWidget)widget;
    unsigned long height;
    (void)request;
    reply->request_mode = CWWidth | CWHeight;
    reply->width = 280;
    height = (unsigned long)view->tree_view.row_height *
             (unsigned long)view->tree_view.visible_rows;
    reply->height = height > (unsigned long)(Dimension)-1
                        ? (Dimension)-1 : (Dimension)height;
    return XtGeometryAlmost;
}

Widget XmCreateTreeView(Widget parent, char *name, ArgList args,
                        Cardinal count) {
    return XtCreateWidget(name, xmTreeViewWidgetClass, parent, args, count);
}

void XmTreeViewSetNodes(Widget widget, const XmTreeViewNode *nodes,
                        Cardinal count) {
    XmTreeViewWidget view;
    XmTreeViewItem *items;
    Cardinal i;
    XtPointer selected;
    if (!XmIsTreeView(widget) || count > TREE_VIEW_MAX_NODES ||
        !valid_input(nodes, count))
        return;
    view = (XmTreeViewWidget)widget;
    selected = view->tree_view.selected_id;
    items = count ? (XmTreeViewItem *)XtCalloc(count, sizeof(*items)) : NULL;
    if (count != 0 && items == NULL)
        return;
    for (i = 0; i < count; ++i) {
        items[i].id = nodes ? nodes[i].id : NULL;
        items[i].parent_id = nodes ? nodes[i].parent_id : NULL;
        items[i].label = XtNewString(nodes && nodes[i].label ?
                                     nodes[i].label : "");
        if (items[i].label == NULL) {
            while (i != 0)
                XtFree(items[--i].label);
            XtFree((char *)items);
            return;
        }
        items[i].has_children = nodes ? nodes[i].has_children : False;
        items[i].expanded = nodes ? nodes[i].expanded : False;
    }
    for (i = 0; i < view->tree_view.count; ++i)
        XtFree(view->tree_view.items[i].label);
    XtFree((char *)view->tree_view.items);
    view->tree_view.items = items;
    view->tree_view.count = count;
    view->tree_view.selected_id = find_id(view, selected) >= 0 ? selected : NULL;
    view->tree_view.top_row = 0;
    redraw(view);
}

Cardinal XmTreeViewGetNodeCount(Widget widget) {
    return XmIsTreeView(widget) ? ((XmTreeViewWidget)widget)->tree_view.count : 0;
}

XtPointer XmTreeViewGetSelectedId(Widget widget) {
    return XmIsTreeView(widget) ?
        ((XmTreeViewWidget)widget)->tree_view.selected_id : NULL;
}

void XmTreeViewSetSelectedId(Widget widget, XtPointer id,
                             Boolean notify_change) {
    if (XmIsTreeView(widget))
        select_id((XmTreeViewWidget)widget, id, notify_change, NULL);
}

Boolean XmTreeViewSetExpanded(Widget widget, XtPointer id, Boolean expanded,
                              Boolean notify_change) {
    XmTreeViewWidget view;
    int index;
    if (!XmIsTreeView(widget))
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
    if (!XmIsTreeView(widget))
        return False;
    view = (XmTreeViewWidget)widget;
    index = find_id(view, id);
    return index >= 0 && view->tree_view.items[index].expanded;
}

void XmTreeViewSetTopRow(Widget widget, Cardinal top_row) {
    XmTreeViewWidget view;
    Cardinal total;
    if (!XmIsTreeView(widget))
        return;
    view = (XmTreeViewWidget)widget;
    total = visible_count(view);
    if (top_row > total)
        top_row = total;
    view->tree_view.top_row = top_row;
    redraw(view);
}

Cardinal XmTreeViewGetTopRow(Widget widget) {
    return XmIsTreeView(widget) ? ((XmTreeViewWidget)widget)->tree_view.top_row : 0;
}
