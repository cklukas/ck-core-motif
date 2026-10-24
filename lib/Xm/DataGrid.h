/* Additive bounded DataGrid/TableView control. */
#ifndef _XmDataGrid_h
#define _XmDataGrid_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmDataGridWidgetClass;
typedef struct _XmDataGridClassRec *XmDataGridWidgetClass;
typedef struct _XmDataGridRec *XmDataGridWidget;

#ifndef XmIsDataGrid
#define XmIsDataGrid(w) XtIsSubclass((w), xmDataGridWidgetClass)
#endif

enum { XmDATA_GRID_MAX_ROWS = 4096, XmDATA_GRID_MAX_COLUMNS = 256,
       XmDATA_GRID_MAX_EDIT_TEXT = 4096 };

typedef struct {
    XtPointer id;             /* stable application-owned identity */
    const char *title;        /* copied by XmDataGridSetModel */
    Boolean editable;
    Dimension width;
} XmDataGridColumn;

typedef struct {
    XtPointer id;             /* stable application-owned identity */
    const char *const *values; /* one value per column; copied by SetModel */
} XmDataGridRow;

typedef struct {
    int reason;
    XEvent *event;
    XtPointer row_id;
    XtPointer column_id;
    Cardinal row_index;
    Cardinal column_index;
} XmDataGridSelectionCallbackStruct;

typedef struct {
    int reason;
    XEvent *event;
    XtPointer column_id;
    Boolean ascending;
} XmDataGridSortCallbackStruct;

typedef struct {
    int reason;
    XEvent *event;
    XtPointer row_id;
    XtPointer column_id;
    String text;
} XmDataGridEditCallbackStruct;

typedef struct {
    int reason;
    XEvent *event;
    XtPointer row_id;
    XtPointer column_id;
    String error;
} XmDataGridEditFailedCallbackStruct;

Widget XmCreateDataGrid(Widget parent, char *name, ArgList args,
                        Cardinal count);
Boolean XmDataGridSetModel(Widget widget, const XmDataGridColumn *columns,
                           Cardinal column_count, const XmDataGridRow *rows,
                           Cardinal row_count);
Cardinal XmDataGridGetColumnCount(Widget widget);
Cardinal XmDataGridGetRowCount(Widget widget);
Boolean XmDataGridSetSelectedCell(Widget widget, XtPointer row_id,
                                  XtPointer column_id, Boolean notify);
Boolean XmDataGridGetSelectedCell(Widget widget, XtPointer *row_id_return,
                                  XtPointer *column_id_return);
Boolean XmDataGridSetTopRow(Widget widget, Cardinal top_row);
Cardinal XmDataGridGetTopRow(Widget widget);
Boolean XmDataGridBeginEdit(Widget widget);
Boolean XmDataGridSetEditText(Widget widget, const char *text);
Boolean XmDataGridSetEditError(Widget widget, const char *error);
const char *XmDataGridGetEditError(Widget widget);
Boolean XmDataGridCommitEdit(Widget widget, Boolean notify);
void XmDataGridCancelEdit(Widget widget);
Boolean XmDataGridIsEditing(Widget widget);
Boolean XmDataGridRequestSort(Widget widget, XtPointer column_id,
                              Boolean ascending, Boolean notify);

#ifdef __cplusplus
}
#endif

#endif /* _XmDataGrid_h */
