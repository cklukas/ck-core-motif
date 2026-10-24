#ifndef _XmDataGridP_h
#define _XmDataGridP_h

#include <Xm/DataGrid.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmDataGridClassPart { XtPointer extension; }
    XmDataGridClassPart;
typedef struct _XmDataGridClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmDataGridClassPart data_grid_class;
} XmDataGridClassRec;

typedef struct {
    XtPointer id;
    char *title;
    Boolean editable;
    Dimension width;
} XmDataGridColumnItem;

typedef struct {
    XtPointer id;
    char **values;
} XmDataGridRowItem;

typedef struct _XmDataGridPart {
    XmDataGridColumnItem *columns;
    XmDataGridRowItem *rows;
    Cardinal column_count, row_count;
    int selected_row, selected_column;
    Cardinal top_row;
    Dimension header_height, row_height;
    Cardinal visible_rows;
    Boolean rtl;
    Boolean editing;
    char *edit_text;
    char *edit_error;
    int edit_row, edit_column;
    XtCallbackList selection_callback, sort_callback;
    XtCallbackList edit_committed_callback, edit_failed_callback;
    GC gc;
} XmDataGridPart;

typedef struct _XmDataGridRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmDataGridPart data_grid;
} XmDataGridRec;

extern XmDataGridClassRec xmDataGridClassRec;

#endif /* _XmDataGridP_h */
