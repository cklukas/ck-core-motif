#include <Xm/Accessible.h>
#include <Xm/DrawingA.h>
#include <Xm/ScrollBar.h>
#include <Xm/Xm.h>
#include <assert.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

enum { HEADER_HEIGHT = 24, ROW_HEIGHT = 22, MAX_ROWS = 4096,
       MAX_COLUMNS = 256 };

typedef struct {
    uint64_t id;
    const char *label;
    Boolean editable;
} Column;

typedef struct {
    uint64_t id;
    const char *values[MAX_COLUMNS];
} Row;

typedef struct {
    const Column *columns;
    size_t column_count;
    const Row *rows;
    size_t row_count;
    size_t top_row;
    size_t selected_row;
    size_t selected_column;
    size_t viewport_rows;
    Boolean editing;
    char edit_buffer[128];
    size_t edit_row, edit_column;
    unsigned sort_requests;
    unsigned redraws;
    XmAccessibleId semantic_cells[6];
} Grid;

typedef struct {
    Grid *grid;
    size_t row;
    size_t column;
} Cell;

static size_t visible_rows(const Grid *grid, unsigned height) {
    size_t rows = height > HEADER_HEIGHT ?
        (height - HEADER_HEIGHT) / ROW_HEIGHT : 0;
    if (rows > MAX_ROWS) rows = MAX_ROWS;
    return rows;
}

static size_t max_top_row(const Grid *grid) {
    size_t bounded = grid->row_count > MAX_ROWS ? MAX_ROWS : grid->row_count;
    return bounded > grid->viewport_rows ? bounded - grid->viewport_rows : 0;
}

static void clamp_top(Grid *grid) {
    size_t limit = max_top_row(grid);
    if (grid->top_row > limit) grid->top_row = limit;
}

static Boolean layout(Grid *grid, unsigned width, unsigned height) {
    size_t before = grid->viewport_rows;
    (void)width;
    grid->viewport_rows = visible_rows(grid, height);
    clamp_top(grid);
    return before != grid->viewport_rows;
}

static Boolean select_cell(Grid *grid, size_t row, size_t column) {
    size_t bounded_rows = grid->row_count > MAX_ROWS ? MAX_ROWS : grid->row_count;
    if (row >= bounded_rows || column >= grid->column_count) return False;
    grid->selected_row = row;
    grid->selected_column = column;
    if (row < grid->top_row) grid->top_row = row;
    if (grid->viewport_rows && row >= grid->top_row + grid->viewport_rows)
        grid->top_row = row - grid->viewport_rows + 1;
    clamp_top(grid);
    ++grid->redraws;
    return True;
}

static Boolean request_sort(Grid *grid, uint64_t column_id, Boolean ascending) {
    size_t k;
    (void)ascending;
    for (k = 0; k < grid->column_count; ++k)
        if (grid->columns[k].id == column_id) {
            ++grid->sort_requests;
            return True;
        }
    return False;
}

static Boolean begin_edit(Grid *grid) {
    const char *value;
    if (grid->selected_row >= grid->row_count ||
        grid->selected_column >= grid->column_count ||
        !grid->columns[grid->selected_column].editable)
        return False;
    value = grid->rows[grid->selected_row].values[grid->selected_column];
    if (!value || strlen(value) >= sizeof(grid->edit_buffer)) return False;
    strcpy(grid->edit_buffer, value);
    grid->edit_row = grid->selected_row;
    grid->edit_column = grid->selected_column;
    grid->editing = True;
    return True;
}

static Boolean set_edit(Grid *grid, const char *text) {
    if (!grid->editing || !text || strlen(text) >= sizeof(grid->edit_buffer))
        return False;
    strcpy(grid->edit_buffer, text);
    return True;
}

static Boolean commit_edit(Grid *grid, const char **error_return) {
    (void)error_return;
    if (!grid->editing) return False;
    /* The prototype models commit/error ownership without mutating borrowed
       row storage; a release widget will call an application commit hook. */
    grid->editing = False;
    ++grid->redraws;
    return True;
}

static void cancel_edit(Grid *grid) { grid->editing = False; }

static void draw_body(Widget widget, XtPointer closure) {
    Grid *grid = (Grid *)closure;
    XGCValues values;
    GC gc;
    size_t row, column;
    Dimension widget_width;
    int width;
    XtVaGetValues(widget, XmNwidth, &widget_width, NULL);
    width = (int)widget_width;
    values.foreground = BlackPixelOfScreen(XtScreen(widget));
    gc = XCreateGC(XtDisplay(widget), XtWindow(widget), GCForeground, &values);
    for (column = 0; column < grid->column_count; ++column)
        XDrawString(XtDisplay(widget), XtWindow(widget), gc,
                    (int)(column * (size_t)width / grid->column_count) + 4,
                    16, grid->columns[column].label,
                    (int)strlen(grid->columns[column].label));
    for (row = 0; row < grid->viewport_rows &&
                  grid->top_row + row < grid->row_count; ++row)
        for (column = 0; column < grid->column_count; ++column)
            XDrawString(XtDisplay(widget), XtWindow(widget), gc,
                        (int)(column * (size_t)width / grid->column_count) + 4,
                        HEADER_HEIGHT + (int)(row * ROW_HEIGHT) + 16,
                        grid->rows[grid->top_row + row].values[column],
                        (int)strlen(grid->rows[grid->top_row + row].values[column]));
    XFreeGC(XtDisplay(widget), gc);
}

static void draw_event(Widget widget, XtPointer closure, XEvent *event,
                       Boolean *dispatch) {
    (void)dispatch;
    if (event->type == Expose && event->xexpose.count == 0)
        draw_body(widget, closure);
}

static Boolean table_query(XtPointer data, XmAccessibleInfo *info) {
    Grid *grid = (Grid *)data;
    info->role = XmACCESSIBLE_TABLE;
    info->rows = (int)(grid->row_count > MAX_ROWS ? MAX_ROWS : grid->row_count);
    info->columns = (int)grid->column_count;
    info->has_value = True;
    info->minimum_value = 0;
    info->maximum_value = info->rows;
    info->current_value = grid->selected_row;
    info->minimum_increment = 1;
    return True;
}

static XmAccessibleId cell_id(XtPointer data, int row, int column) {
    Grid *grid = (Grid *)data;
    if (row < 0 || row >= 2 || column < 0 || column >= 3)
        return XmACCESSIBLE_NONE;
    return grid->semantic_cells[row * 3 + column];
}

static Boolean cell_query(XtPointer data, XmAccessibleInfo *info) {
    Cell *cell = (Cell *)data;
    const char *value = cell->grid->rows[cell->row].values[cell->column];
    info->role = XmACCESSIBLE_TABLE_CELL;
    info->name = XtNewString(value ? value : "");
    info->has_text = True;
    info->text_length = (int)strlen(value ? value : "");
    return info->name != NULL;
}

static char *cell_text(XtPointer data, int start, int end) {
    Cell *cell = (Cell *)data;
    const char *value = cell->grid->rows[cell->row].values[cell->column];
    size_t length = strlen(value ? value : "");
    if (start < 0 || end < start || (size_t)end > length) return NULL;
    {
        char *result = XtMalloc((size_t)(end - start) + 1);
        memcpy(result, (value ? value : "") + start, (size_t)(end - start));
        result[end - start] = '\0';
        return result;
    }
}

int main(int argc, char **argv) {
    XtAppContext app;
    Widget shell, drawing, scrollbar;
    XmAccessibility *accessibility;
    XmAccessibleProvider table_provider = {0}, cell_provider = {0};
    XmAccessibleInfo info;
    XmAccessibleId table_id, cell_ids[6];
    Column columns[3] = {{10, "Name", True}, {20, "Kind", False},
                         {30, "State", True}};
    Row rows[4] = {{100, {"alpha", "file", "ready"}},
                   {200, {"beta", "file", "busy"}},
                   {300, {"gamma", "directory", "ready"}},
                   {400, {"delta", "file", "offline"}}};
    Grid grid = {columns, 3, rows, 4, 0, 0, 0, 0, False, {0},
                 0, 0, 0, 0, {0}};
    Cell cells[6];
    size_t i, row, column;

    shell = XtVaAppInitialize(&app, "DataGridPrototype", NULL, 0,
                              &argc, argv, NULL, NULL);
    drawing = XmCreateDrawingArea(shell, (char *)"body", NULL, 0);
    scrollbar = XmCreateScrollBar(shell, (char *)"scroll", NULL, 0);
    XtVaSetValues(drawing, XmNwidth, 320, XmNheight, 120, NULL);
    XtAddEventHandler(drawing, ExposureMask, False, draw_event, &grid);
    XtManageChild(drawing);
    XtManageChild(scrollbar);
    assert(layout(&grid, 320, 120));
    assert(grid.viewport_rows == 4);
    assert(select_cell(&grid, 3, 2));
    assert(grid.top_row == 0);
    assert(request_sort(&grid, 20, True));
    assert(!request_sort(&grid, 999, True));
    assert(begin_edit(&grid));
    assert(set_edit(&grid, "done"));
    assert(commit_edit(&grid, NULL));
    assert(!grid.editing);
    assert(begin_edit(&grid));
    cancel_edit(&grid);
    assert(!grid.editing);

    XtRealizeWidget(shell);
    /* Scope the prototype provider to the public drawing-area owner so the
       unrelated scroll bar does not become part of this semantic snapshot. */
    accessibility = XmAccessibilityCreate(drawing, "DataGridPrototype");
    assert(accessibility != NULL);
    table_provider.query = table_query;
    table_provider.cell = cell_id;
    table_id = XmAccessibilityAdd(accessibility,
                                   XmAccessibilityWidget(accessibility, drawing),
                                   &table_provider, &grid);
    assert(table_id != XmACCESSIBLE_NONE);
    for (row = 0; row < 2; ++row)
        for (column = 0; column < 3; ++column) {
            cells[row * 3 + column].grid = &grid;
            cells[row * 3 + column].row = row;
            cells[row * 3 + column].column = column;
            cell_provider.query = cell_query;
            cell_provider.text = cell_text;
            cell_ids[row * 3 + column] = XmAccessibilityAdd(
                accessibility, table_id, &cell_provider,
                &cells[row * 3 + column]);
            assert(cell_ids[row * 3 + column] != XmACCESSIBLE_NONE);
            grid.semantic_cells[row * 3 + column] =
                cell_ids[row * 3 + column];
        }
    assert(XmAccessibilityQuery(accessibility, table_id, &info));
    assert(info.role == XmACCESSIBLE_TABLE && info.rows == 4 &&
           info.columns == 3);
    XmAccessibleInfoClear(&info);
    assert(XmAccessibilityQuery(accessibility, cell_ids[0], &info));
    assert(info.role == XmACCESSIBLE_TABLE_CELL && info.has_text);
    XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);
    XtDestroyWidget(shell);
    printf("PASS data-grid prototype: 20 checks\n");
    return 0;
}
