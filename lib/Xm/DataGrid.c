#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <string.h>
#include <Xm/DataGridP.h>
#include <Xm/XmP.h>
#include "PrimitiveI.h"
#include "XmI.h"

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);
static void Events(Widget, XtPointer, XEvent *, Boolean *);

#define Offset(field) XtOffsetOf(XmDataGridRec, field)
static XtResource resources[] = {
    {XmNdataGridHeaderHeight, XmCDataGridHeaderHeight, XmRVerticalDimension,
     sizeof(Dimension), Offset(data_grid.header_height), XmRImmediate,
     (XtPointer)24},
    {XmNdataGridRowHeight, XmCDataGridRowHeight, XmRVerticalDimension,
     sizeof(Dimension), Offset(data_grid.row_height), XmRImmediate,
     (XtPointer)22},
    {XmNdataGridVisibleRows, XmCDataGridVisibleRows, XmRCardinal,
     sizeof(Cardinal), Offset(data_grid.visible_rows), XmRImmediate,
     (XtPointer)0},
    {XmNdataGridTopRow, XmCDataGridTopRow, XmRCardinal, sizeof(Cardinal),
     Offset(data_grid.top_row), XmRImmediate, (XtPointer)0},
    {XmNdataGridRtl, XmCDataGridRtl, XmRBoolean, sizeof(Boolean),
     Offset(data_grid.rtl), XmRImmediate, (XtPointer)False},
    {XmNdataGridSelectionCallback, XmCDataGridSelectionCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(data_grid.selection_callback), XmRPointer,
     (XtPointer)NULL},
    {XmNdataGridSortCallback, XmCDataGridSortCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(data_grid.sort_callback), XmRPointer,
     (XtPointer)NULL},
    {XmNdataGridEditCommittedCallback, XmCDataGridEditCommittedCallback,
     XmRCallback, sizeof(XtCallbackList),
     Offset(data_grid.edit_committed_callback), XmRPointer, (XtPointer)NULL},
    {XmNdataGridEditFailedCallback, XmCDataGridEditFailedCallback,
     XmRCallback, sizeof(XtCallbackList),
     Offset(data_grid.edit_failed_callback), XmRPointer, (XtPointer)NULL},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc,
     (XtPointer)_XmSetThickness}
};

externaldef(xmdatagridclassrec) XmDataGridClassRec xmDataGridClassRec =
    {{(WidgetClass)&xmPrimitiveClassRec, "XmDataGrid", sizeof(XmDataGridRec),
      NULL, NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0,
      resources, XtNumber(resources), NULLQUARK, True,
      XtExposeCompressMaximal, True, False, Destroy, NULL, Redisplay,
      SetValues, NULL, XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
      XtInheritTranslations, QueryGeometry, NULL, NULL},
     {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
      XtInheritTranslations, NULL, NULL, 0, NULL},
     {NULL}};

externaldef(xmdatagridwidgetclass) WidgetClass xmDataGridWidgetClass =
    (WidgetClass)&xmDataGridClassRec;

static int find_column(XmDataGridWidget grid, XtPointer id) {
    Cardinal k;
    for (k = 0; k < grid->data_grid.column_count; ++k)
        if (grid->data_grid.columns[k].id == id) return (int)k;
    return -1;
}

static int find_row(XmDataGridWidget grid, XtPointer id) {
    Cardinal k;
    for (k = 0; k < grid->data_grid.row_count; ++k)
        if (grid->data_grid.rows[k].id == id) return (int)k;
    return -1;
}

static void free_model(XmDataGridWidget grid) {
    Cardinal row, column;
    for (row = 0; row < grid->data_grid.row_count; ++row) {
        for (column = 0; column < grid->data_grid.column_count; ++column)
            XtFree(grid->data_grid.rows[row].values[column]);
        XtFree((char *)grid->data_grid.rows[row].values);
    }
    for (column = 0; column < grid->data_grid.column_count; ++column)
        XtFree(grid->data_grid.columns[column].title);
    XtFree((char *)grid->data_grid.rows);
    XtFree((char *)grid->data_grid.columns);
    grid->data_grid.rows = NULL;
    grid->data_grid.columns = NULL;
    grid->data_grid.row_count = grid->data_grid.column_count = 0;
}

static Boolean duplicate_columns(const XmDataGridColumn *columns,
                                 Cardinal count, Cardinal index) {
    Cardinal k;
    (void)count;
    for (k = 0; k < index; ++k)
        if (columns[k].id == columns[index].id) return True;
    return False;
}

static Boolean duplicate_rows(const XmDataGridRow *rows, Cardinal count,
                              Cardinal index) {
    Cardinal k;
    (void)count;
    for (k = 0; k < index; ++k)
        if (rows[k].id == rows[index].id) return True;
    return False;
}

static Boolean copy_model(XmDataGridWidget grid,
                          const XmDataGridColumn *columns, Cardinal column_count,
                          const XmDataGridRow *rows, Cardinal row_count) {
    Cardinal row, column;
    XmDataGridColumnItem *new_columns = NULL;
    XmDataGridRowItem *new_rows = NULL;
    if (column_count > XmDATA_GRID_MAX_COLUMNS || row_count > XmDATA_GRID_MAX_ROWS)
        return False;
    if ((column_count && !columns) || (row_count && !rows)) return False;
    if (column_count) {
        new_columns = (XmDataGridColumnItem *)XtCalloc(column_count,
                                                       sizeof(*new_columns));
        if (!new_columns) return False;
    }
    for (column = 0; column < column_count; ++column) {
        if (!columns[column].id || !columns[column].title ||
            duplicate_columns(columns, column_count, column)) goto fail;
        new_columns[column].id = columns[column].id;
        new_columns[column].editable = columns[column].editable;
        new_columns[column].width = columns[column].width ? columns[column].width : 96;
        new_columns[column].title = XtNewString(columns[column].title);
        if (!new_columns[column].title) goto fail;
    }
    if (row_count) {
        new_rows = (XmDataGridRowItem *)XtCalloc(row_count,
                                                 sizeof(*new_rows));
        if (!new_rows) goto fail;
    }
    for (row = 0; row < row_count; ++row) {
        if (!rows[row].id || duplicate_rows(rows, row_count, row) ||
            (column_count && !rows[row].values)) goto fail;
        new_rows[row].id = rows[row].id;
        if (column_count) {
            new_rows[row].values = (char **)XtCalloc(
                column_count, sizeof(*new_rows[row].values));
            if (!new_rows[row].values) goto fail;
        }
        for (column = 0; column < column_count; ++column) {
            if (!rows[row].values[column]) goto fail;
            new_rows[row].values[column] = XtNewString(rows[row].values[column]);
            if (!new_rows[row].values[column]) goto fail;
        }
    }
    free_model(grid);
    grid->data_grid.columns = new_columns;
    grid->data_grid.rows = new_rows;
    grid->data_grid.column_count = column_count;
    grid->data_grid.row_count = row_count;
    grid->data_grid.selected_row = grid->data_grid.selected_column = -1;
    grid->data_grid.top_row = 0;
    grid->data_grid.editing = False;
    return True;
fail:
    for (row = 0; row < row_count; ++row) {
        if (new_rows && new_rows[row].values) {
            for (column = 0; column < column_count; ++column)
                XtFree(new_rows[row].values[column]);
            XtFree((char *)new_rows[row].values);
        }
    }
    for (column = 0; column < column_count; ++column)
        XtFree(new_columns ? new_columns[column].title : NULL);
    XtFree((char *)new_rows);
    XtFree((char *)new_columns);
    return False;
}

static Cardinal max_top_row(XmDataGridWidget grid) {
    Cardinal bounded = grid->data_grid.row_count;
    Cardinal visible = grid->data_grid.visible_rows;
    if (bounded > XmDATA_GRID_MAX_ROWS) bounded = XmDATA_GRID_MAX_ROWS;
    return bounded > visible ? bounded - visible : 0;
}

static void clamp_top(XmDataGridWidget grid) {
    Cardinal limit = max_top_row(grid);
    if (grid->data_grid.top_row > limit) grid->data_grid.top_row = limit;
}

static void recompute_visible(XmDataGridWidget grid) {
    if (grid->data_grid.row_height == 0) grid->data_grid.row_height = 22;
    if (grid->data_grid.header_height >= grid->core.height)
        grid->data_grid.visible_rows = 0;
    else
        grid->data_grid.visible_rows =
            (grid->core.height - grid->data_grid.header_height) /
            grid->data_grid.row_height;
    if (grid->data_grid.visible_rows > XmDATA_GRID_MAX_ROWS)
        grid->data_grid.visible_rows = XmDATA_GRID_MAX_ROWS;
    clamp_top(grid);
}

static void redraw(XmDataGridWidget grid) {
    if (XtIsRealized((Widget)grid))
        XClearArea(XtDisplay((Widget)grid), XtWindow((Widget)grid), 0, 0, 0, 0,
                   True);
}

static void notify_selection(XmDataGridWidget grid, XEvent *event) {
    XmDataGridSelectionCallbackStruct callback;
    if (!grid->data_grid.selection_callback ||
        grid->data_grid.selected_row < 0 || grid->data_grid.selected_column < 0)
        return;
    callback.reason = 1;
    callback.event = event;
    callback.row_index = (Cardinal)grid->data_grid.selected_row;
    callback.column_index = (Cardinal)grid->data_grid.selected_column;
    callback.row_id = grid->data_grid.rows[callback.row_index].id;
    callback.column_id = grid->data_grid.columns[callback.column_index].id;
    XtCallCallbacks((Widget)grid, XmNdataGridSelectionCallback, &callback);
}

static void notify_sort(XmDataGridWidget grid, XEvent *event,
                        XtPointer column_id, Boolean ascending) {
    XmDataGridSortCallbackStruct callback;
    if (!grid->data_grid.sort_callback) return;
    callback.reason = 2;
    callback.event = event;
    callback.column_id = column_id;
    callback.ascending = ascending;
    XtCallCallbacks((Widget)grid, XmNdataGridSortCallback, &callback);
}

static void notify_edit(XmDataGridWidget grid, XEvent *event) {
    XmDataGridEditCallbackStruct callback;
    if (!grid->data_grid.edit_committed_callback) return;
    callback.reason = 3;
    callback.event = event;
    callback.row_id = grid->data_grid.rows[grid->data_grid.edit_row].id;
    callback.column_id = grid->data_grid.columns[grid->data_grid.edit_column].id;
    callback.text = grid->data_grid.rows[grid->data_grid.edit_row]
        .values[grid->data_grid.edit_column];
    XtCallCallbacks((Widget)grid, XmNdataGridEditCommittedCallback, &callback);
}

static void notify_edit_failed(XmDataGridWidget grid, XEvent *event) {
    XmDataGridEditFailedCallbackStruct callback;
    if (!grid->data_grid.edit_failed_callback) return;
    callback.reason = 4;
    callback.event = event;
    callback.row_id = grid->data_grid.rows[grid->data_grid.edit_row].id;
    callback.column_id = grid->data_grid.columns[grid->data_grid.edit_column].id;
    callback.error = grid->data_grid.edit_error;
    XtCallCallbacks((Widget)grid, XmNdataGridEditFailedCallback, &callback);
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmDataGridWidget grid = (XmDataGridWidget)widget;
    XGCValues values;
    (void)request; (void)args; (void)count;
    grid->data_grid.selected_row = grid->data_grid.selected_column = -1;
    grid->data_grid.row_height = grid->data_grid.row_height ?
        grid->data_grid.row_height : 22;
    grid->data_grid.header_height = grid->data_grid.header_height ?
        grid->data_grid.header_height : 24;
    values.foreground = grid->primitive.foreground;
    values.background = grid->core.background_pixel;
    grid->data_grid.gc = XCreateGC(XtDisplay(widget),
                                   DefaultRootWindow(XtDisplay(widget)),
                                   GCForeground | GCBackground, &values);
    if (!grid->core.width) grid->core.width = 480;
    if (!grid->core.height) grid->core.height = 240;
    recompute_visible(grid);
    XtAddEventHandler(widget, KeyPressMask | ButtonPressMask, False, Events,
                      NULL);
}

static void Destroy(Widget widget) {
    XmDataGridWidget grid = (XmDataGridWidget)widget;
    XtRemoveEventHandler(widget, KeyPressMask | ButtonPressMask, False,
                         Events, NULL);
    free_model(grid);
    XtFree(grid->data_grid.edit_text);
    XtFree(grid->data_grid.edit_error);
    if (grid->data_grid.gc) XFreeGC(XtDisplay(widget), grid->data_grid.gc);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget new_widget,
                         ArgList args, Cardinal *count) {
    XmDataGridWidget old = (XmDataGridWidget)old_widget;
    XmDataGridWidget grid = (XmDataGridWidget)new_widget;
    Boolean redraw_needed;
    (void)request; (void)args; (void)count;
    if (!grid->data_grid.row_height) grid->data_grid.row_height = old->data_grid.row_height;
    if (!grid->data_grid.header_height) grid->data_grid.header_height = old->data_grid.header_height;
    recompute_visible(grid);
    redraw_needed = old->data_grid.row_height != grid->data_grid.row_height ||
                    old->data_grid.header_height != grid->data_grid.header_height ||
                    old->data_grid.top_row != grid->data_grid.top_row ||
                    old->data_grid.rtl != grid->data_grid.rtl;
    return redraw_needed;
}

static void draw_text(XmDataGridWidget grid, int x, int y, const char *text) {
    if (!grid->data_grid.gc) return;
    if (!text) text = "";
    XDrawString(XtDisplay((Widget)grid), XtWindow((Widget)grid),
                grid->data_grid.gc, x, y, text, (int)strlen(text));
}

static int column_width(XmDataGridWidget grid) {
    int width = grid->data_grid.column_count ?
        (int)grid->core.width / (int)grid->data_grid.column_count : 1;
    return width > 0 ? width : 1;
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmDataGridWidget grid = (XmDataGridWidget)widget;
    Cardinal row, column;
    int cell_width;
    (void)event; (void)region;
    if (!XtIsRealized(widget) || !grid->data_grid.gc) return;
    XClearWindow(XtDisplay(widget), XtWindow(widget));
    cell_width = grid->data_grid.column_count ?
        (int)grid->core.width / (int)grid->data_grid.column_count : 1;
    if (cell_width < 1) cell_width = 1;
    for (column = 0; column < grid->data_grid.column_count; ++column) {
        Cardinal visual = grid->data_grid.rtl ?
            grid->data_grid.column_count - column - 1 : column;
        draw_text(grid, (int)visual * cell_width + 4, 16,
                  grid->data_grid.columns[column].title);
    }
    for (row = 0; row < grid->data_grid.visible_rows &&
                  grid->data_grid.top_row + row < grid->data_grid.row_count; ++row) {
        Cardinal model_row = grid->data_grid.top_row + row;
        for (column = 0; column < grid->data_grid.column_count; ++column) {
            Cardinal visual = grid->data_grid.rtl ?
                grid->data_grid.column_count - column - 1 : column;
            int x = (int)visual * cell_width + 4;
            int y = (int)grid->data_grid.header_height +
                    (int)row * grid->data_grid.row_height + 16;
            if ((int)model_row == grid->data_grid.selected_row &&
                (int)column == grid->data_grid.selected_column)
                XDrawRectangle(XtDisplay(widget), XtWindow(widget),
                               grid->data_grid.gc, x - 2,
                               y - (int)grid->data_grid.row_height + 2,
                               (unsigned)(cell_width > 5 ? cell_width - 5 : 1),
                               (unsigned)(grid->data_grid.row_height > 3 ?
                                          grid->data_grid.row_height - 3 : 1));
            draw_text(grid, x, y,
                      grid->data_grid.rows[model_row].values[column]);
        }
    }
}

static void move_selection(XmDataGridWidget grid, int row_delta,
                           int column_delta, XEvent *event) {
    int row = grid->data_grid.selected_row;
    int column = grid->data_grid.selected_column;
    Cardinal bounded = grid->data_grid.row_count > XmDATA_GRID_MAX_ROWS ?
        XmDATA_GRID_MAX_ROWS : grid->data_grid.row_count;
    if (!bounded || !grid->data_grid.column_count) return;
    if (row < 0) row = 0;
    if (column < 0) column = 0;
    row += row_delta; column += column_delta;
    if (row < 0) row = 0;
    if (column < 0) column = 0;
    if ((Cardinal)row >= bounded) row = (int)bounded - 1;
    if ((Cardinal)column >= grid->data_grid.column_count)
        column = (int)grid->data_grid.column_count - 1;
    grid->data_grid.selected_row = row;
    grid->data_grid.selected_column = column;
    if ((Cardinal)row < grid->data_grid.top_row)
        grid->data_grid.top_row = (Cardinal)row;
    if (grid->data_grid.visible_rows &&
        (Cardinal)row >= grid->data_grid.top_row + grid->data_grid.visible_rows)
        grid->data_grid.top_row = (Cardinal)row - grid->data_grid.visible_rows + 1;
    clamp_top(grid);
    redraw(grid);
    notify_selection(grid, event);
}

static void Events(Widget widget, XtPointer closure, XEvent *event,
                   Boolean *continue_dispatch) {
    XmDataGridWidget grid = (XmDataGridWidget)widget;
    KeySym key;
    char input[2];
    int x, y, column, row;
    (void)closure; (void)continue_dispatch;
    if (event->type == KeyPress) {
        if (XLookupString(&event->xkey, input, sizeof(input), &key, NULL) == 0)
            return;
        if (key == XK_Left) { move_selection(grid, 0, grid->data_grid.rtl ? 1 : -1, event); return; }
        if (key == XK_Right) { move_selection(grid, 0, grid->data_grid.rtl ? -1 : 1, event); return; }
        if (key == XK_Up) { move_selection(grid, -1, 0, event); return; }
        if (key == XK_Down) { move_selection(grid, 1, 0, event); return; }
        if (key == XK_Home) { grid->data_grid.top_row = 0; move_selection(grid, -grid->data_grid.selected_row, 0, event); return; }
        if (key == XK_End) { move_selection(grid, (int)max_top_row(grid) - grid->data_grid.selected_row, 0, event); return; }
        if (key == XK_Return || key == XK_KP_Enter) {
            if (grid->data_grid.editing) XmDataGridCommitEdit(widget, True);
            else XmDataGridBeginEdit(widget);
            return;
        }
        if (key == XK_Escape && grid->data_grid.editing) {
            XmDataGridCancelEdit(widget); return;
        }
        if (key == XK_Page_Up) { XmDataGridSetTopRow(widget, grid->data_grid.top_row > grid->data_grid.visible_rows ? grid->data_grid.top_row - grid->data_grid.visible_rows : 0); return; }
        if (key == XK_Page_Down) { XmDataGridSetTopRow(widget, grid->data_grid.top_row + grid->data_grid.visible_rows); return; }
        return;
    }
    if (event->type != ButtonPress) return;
    if (event->xbutton.button == Button4) { XmDataGridSetTopRow(widget, grid->data_grid.top_row ? grid->data_grid.top_row - 1 : 0); return; }
    if (event->xbutton.button == Button5) { XmDataGridSetTopRow(widget, grid->data_grid.top_row + 1); return; }
    x = event->xbutton.x; y = event->xbutton.y;
    if (!grid->data_grid.column_count || y < (int)grid->data_grid.header_height) {
        if (grid->data_grid.column_count) {
            column = x / column_width(grid);
            if (grid->data_grid.rtl) column = (int)grid->data_grid.column_count - column - 1;
            if (column >= 0 && (Cardinal)column < grid->data_grid.column_count)
                XmDataGridRequestSort(widget, grid->data_grid.columns[column].id,
                                      True, True);
        }
        return;
    }
    row = (y - (int)grid->data_grid.header_height) /
          (int)grid->data_grid.row_height;
    column = x / column_width(grid);
    if (grid->data_grid.rtl) column = (int)grid->data_grid.column_count - column - 1;
    if (row >= 0 && (Cardinal)row < grid->data_grid.visible_rows &&
        grid->data_grid.top_row + (Cardinal)row < grid->data_grid.row_count &&
        column >= 0 && (Cardinal)column < grid->data_grid.column_count)
        XmDataGridSetSelectedCell(widget,
            grid->data_grid.rows[grid->data_grid.top_row + row].id,
            grid->data_grid.columns[column].id, True);
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    (void)widget; (void)request;
    reply->request_mode = CWWidth | CWHeight;
    reply->width = 480; reply->height = 240;
    return XtGeometryAlmost;
}

Widget XmCreateDataGrid(Widget parent, char *name, ArgList args,
                        Cardinal count) {
    return XtCreateWidget(name, xmDataGridWidgetClass, parent, args, count);
}

Boolean XmDataGridSetModel(Widget widget, const XmDataGridColumn *columns,
                           Cardinal column_count, const XmDataGridRow *rows,
                           Cardinal row_count) {
    Boolean result;
    if (!widget || !XmIsDataGrid(widget)) return False;
    result = copy_model((XmDataGridWidget)widget, columns, column_count, rows,
                        row_count);
    if (result) redraw((XmDataGridWidget)widget);
    return result;
}

Cardinal XmDataGridGetColumnCount(Widget widget) {
    return widget && XmIsDataGrid(widget) ?
        ((XmDataGridWidget)widget)->data_grid.column_count : 0;
}

Cardinal XmDataGridGetRowCount(Widget widget) {
    return widget && XmIsDataGrid(widget) ?
        ((XmDataGridWidget)widget)->data_grid.row_count : 0;
}

Boolean XmDataGridSetSelectedCell(Widget widget, XtPointer row_id,
                                  XtPointer column_id, Boolean notify) {
    XmDataGridWidget grid;
    int row, column;
    if (!widget || !XmIsDataGrid(widget)) return False;
    grid = (XmDataGridWidget)widget;
    row = find_row(grid, row_id); column = find_column(grid, column_id);
    if (row < 0 || column < 0 || (Cardinal)row >= XmDATA_GRID_MAX_ROWS)
        return False;
    grid->data_grid.selected_row = row;
    grid->data_grid.selected_column = column;
    clamp_top(grid);
    redraw(grid);
    if (notify) notify_selection(grid, NULL);
    return True;
}

Boolean XmDataGridGetSelectedCell(Widget widget, XtPointer *row_id_return,
                                  XtPointer *column_id_return) {
    XmDataGridWidget grid;
    if (!widget || !XmIsDataGrid(widget) || !row_id_return ||
        !column_id_return) return False;
    grid = (XmDataGridWidget)widget;
    if (grid->data_grid.selected_row < 0 || grid->data_grid.selected_column < 0)
        return False;
    *row_id_return = grid->data_grid.rows[grid->data_grid.selected_row].id;
    *column_id_return = grid->data_grid.columns[grid->data_grid.selected_column].id;
    return True;
}

Boolean XmDataGridSetTopRow(Widget widget, Cardinal top_row) {
    XmDataGridWidget grid;
    if (!widget || !XmIsDataGrid(widget)) return False;
    grid = (XmDataGridWidget)widget;
    grid->data_grid.top_row = top_row;
    clamp_top(grid);
    redraw(grid);
    return True;
}

Cardinal XmDataGridGetTopRow(Widget widget) {
    return widget && XmIsDataGrid(widget) ?
        ((XmDataGridWidget)widget)->data_grid.top_row : 0;
}

Boolean XmDataGridBeginEdit(Widget widget) {
    XmDataGridWidget grid;
    const char *value;
    if (!widget || !XmIsDataGrid(widget)) return False;
    grid = (XmDataGridWidget)widget;
    if (grid->data_grid.selected_row < 0 || grid->data_grid.selected_column < 0 ||
        !grid->data_grid.columns[grid->data_grid.selected_column].editable)
        return False;
    value = grid->data_grid.rows[grid->data_grid.selected_row]
        .values[grid->data_grid.selected_column];
    XtFree(grid->data_grid.edit_text);
    XtFree(grid->data_grid.edit_error);
    grid->data_grid.edit_text = XtNewString(value ? value : "");
    grid->data_grid.edit_error = NULL;
    if (!grid->data_grid.edit_text) return False;
    grid->data_grid.edit_row = grid->data_grid.selected_row;
    grid->data_grid.edit_column = grid->data_grid.selected_column;
    grid->data_grid.editing = True;
    return True;
}

Boolean XmDataGridSetEditText(Widget widget, const char *text) {
    XmDataGridWidget grid;
    char *copy;
    if (!widget || !XmIsDataGrid(widget) || !text || strlen(text) > XmDATA_GRID_MAX_EDIT_TEXT)
        return False;
    grid = (XmDataGridWidget)widget;
    if (!grid->data_grid.editing) return False;
    copy = XtNewString(text);
    if (!copy) return False;
    XtFree(grid->data_grid.edit_text);
    grid->data_grid.edit_text = copy;
    return True;
}

Boolean XmDataGridSetEditError(Widget widget, const char *error) {
    XmDataGridWidget grid;
    char *copy = NULL;
    if (!widget || !XmIsDataGrid(widget) || !error) return False;
    grid = (XmDataGridWidget)widget;
    if (!grid->data_grid.editing) return False;
    copy = XtNewString(error);
    if (!copy) return False;
    XtFree(grid->data_grid.edit_error);
    grid->data_grid.edit_error = copy;
    return True;
}

const char *XmDataGridGetEditError(Widget widget) {
    if (!widget || !XmIsDataGrid(widget)) return NULL;
    return ((XmDataGridWidget)widget)->data_grid.edit_error;
}

Boolean XmDataGridCommitEdit(Widget widget, Boolean notify) {
    XmDataGridWidget grid;
    char *copy;
    if (!widget || !XmIsDataGrid(widget)) return False;
    grid = (XmDataGridWidget)widget;
    if (!grid->data_grid.editing) return False;
    if (grid->data_grid.edit_error) {
        notify_edit_failed(grid, NULL);
        return False;
    }
    copy = XtNewString(grid->data_grid.edit_text ? grid->data_grid.edit_text : "");
    if (!copy) return False;
    XtFree(grid->data_grid.rows[grid->data_grid.edit_row]
           .values[grid->data_grid.edit_column]);
    grid->data_grid.rows[grid->data_grid.edit_row]
        .values[grid->data_grid.edit_column] = copy;
    grid->data_grid.editing = False;
    XtFree(grid->data_grid.edit_text); grid->data_grid.edit_text = NULL;
    redraw(grid);
    if (notify) notify_edit(grid, NULL);
    return True;
}

void XmDataGridCancelEdit(Widget widget) {
    XmDataGridWidget grid;
    if (!widget || !XmIsDataGrid(widget)) return;
    grid = (XmDataGridWidget)widget;
    if (!grid->data_grid.editing) return;
    grid->data_grid.editing = False;
    XtFree(grid->data_grid.edit_text); grid->data_grid.edit_text = NULL;
    XtFree(grid->data_grid.edit_error); grid->data_grid.edit_error = NULL;
}

Boolean XmDataGridIsEditing(Widget widget) {
    return widget && XmIsDataGrid(widget) &&
           ((XmDataGridWidget)widget)->data_grid.editing;
}

Boolean XmDataGridRequestSort(Widget widget, XtPointer column_id,
                              Boolean ascending, Boolean notify) {
    XmDataGridWidget grid;
    if (!widget || !XmIsDataGrid(widget) ||
        find_column((XmDataGridWidget)widget, column_id) < 0) return False;
    grid = (XmDataGridWidget)widget;
    if (notify) notify_sort(grid, NULL, column_id, ascending);
    return True;
}
