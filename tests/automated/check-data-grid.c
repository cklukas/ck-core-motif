#include <Xm/Accessible.h>
#include <Xm/DataGrid.h>
#include <assert.h>
#include <stdio.h>

static int selections, sorts, commits, failures;
static char committed_text[64];

static void selected(Widget w, XtPointer c, XtPointer data) {
    XmDataGridSelectionCallbackStruct *cb =
        (XmDataGridSelectionCallbackStruct *)data;
    (void)w; (void)c; assert(cb && cb->row_id && cb->column_id);
    ++selections;
}
static void sorted(Widget w, XtPointer c, XtPointer data) {
    XmDataGridSortCallbackStruct *cb = (XmDataGridSortCallbackStruct *)data;
    (void)w; (void)c; assert(cb && cb->column_id); ++sorts;
}
static void committed(Widget w, XtPointer c, XtPointer data) {
    XmDataGridEditCallbackStruct *cb = (XmDataGridEditCallbackStruct *)data;
    (void)w; (void)c; assert(cb && cb->text);
    snprintf(committed_text, sizeof(committed_text), "%s", cb->text);
    ++commits;
}
static void failed(Widget w, XtPointer c, XtPointer data) {
    XmDataGridEditFailedCallbackStruct *cb =
        (XmDataGridEditFailedCallbackStruct *)data;
    (void)w; (void)c; assert(cb && cb->error); ++failures;
}

int main(int argc, char **argv) {
    XtAppContext app;
    Widget shell, grid;
    XmDataGridColumn columns[3] = {{(XtPointer)1, "Name", True, 120},
                                   {(XtPointer)2, "Kind", False, 100},
                                   {(XtPointer)3, "State", True, 100}};
    const char *values0[] = {"alpha", "file", "ready"};
    const char *values1[] = {"beta", "file", "busy"};
    const char *values2[] = {"gamma", "directory", "ready"};
    XmDataGridRow rows[] = {{(XtPointer)10, values0}, {(XtPointer)20, values1},
                            {(XtPointer)30, values2}};
    XmDataGridRow duplicate[] = {{(XtPointer)10, values0},
                                 {(XtPointer)10, values1}};
    XtPointer row_id, column_id;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId accessible_id;

    shell = XtVaAppInitialize(&app, "DataGridFixture", NULL, 0,
                              &argc, argv, NULL, XmNwidth, 360,
                              XmNheight, 160, NULL);
    grid = XmCreateDataGrid(shell, (char *)"grid", NULL, 0);
    XtVaSetValues(grid, XmNwidth, 320, XmNheight, 140, NULL);
    XtAddCallback(grid, XmNdataGridSelectionCallback, selected, NULL);
    XtAddCallback(grid, XmNdataGridSortCallback, sorted, NULL);
    XtAddCallback(grid, XmNdataGridEditCommittedCallback, committed, NULL);
    XtAddCallback(grid, XmNdataGridEditFailedCallback, failed, NULL);
    XtManageChild(grid);
    assert(XmDataGridSetModel(grid, columns, 3, rows, 3));
    assert(XmDataGridGetColumnCount(grid) == 3 &&
           XmDataGridGetRowCount(grid) == 3);
    values0[0] = "mutated-source";
    assert(XmDataGridSetModel(grid, columns, 3, duplicate, 2) == False);
    assert(XmDataGridGetRowCount(grid) == 3);
    assert(XmDataGridSetSelectedCell(grid, (XtPointer)20, (XtPointer)1, True));
    assert(selections == 1);
    assert(XmDataGridGetSelectedCell(grid, &row_id, &column_id));
    assert(row_id == (XtPointer)20 && column_id == (XtPointer)1);
    assert(XmDataGridRequestSort(grid, (XtPointer)2, False, True));
    assert(sorts == 1);
    assert(XmDataGridSetTopRow(grid, 99));
    assert(XmDataGridGetTopRow(grid) == 0);
    assert(XmDataGridSetSelectedCell(grid, (XtPointer)20, (XtPointer)2, False));
    assert(!XmDataGridBeginEdit(grid));
    assert(XmDataGridSetSelectedCell(grid, (XtPointer)20, (XtPointer)3, False));
    assert(XmDataGridBeginEdit(grid));
    assert(XmDataGridSetEditText(grid, "complete"));
    assert(XmDataGridCommitEdit(grid, True));
    assert(commits == 1 && strcmp(committed_text, "complete") == 0);
    assert(XmDataGridBeginEdit(grid));
    assert(XmDataGridSetEditError(grid, "validation failed"));
    assert(!XmDataGridCommitEdit(grid, True));
    assert(failures == 1 && XmDataGridIsEditing(grid));
    XmDataGridCancelEdit(grid);
    assert(!XmDataGridIsEditing(grid));
    XtRealizeWidget(shell);
    accessibility = XmAccessibilityCreate(grid, "DataGridFixture");
    assert(accessibility != NULL);
    accessible_id = XmAccessibilityWidget(accessibility, grid);
    assert(accessible_id != XmACCESSIBLE_NONE);
    assert(XmAccessibilityQuery(accessibility, accessible_id, &info));
    assert(info.role == XmACCESSIBLE_TABLE && info.rows == 3 &&
           info.columns == 3);
    XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);
    XtDestroyWidget(shell);
    puts("PASS data grid: 20 checks");
    return 0;
}
