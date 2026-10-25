#ifndef _XmTreeViewP_h
#define _XmTreeViewP_h

#include <Xm/TreeView.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmTreeViewClassPart { XtPointer extension; }
    XmTreeViewClassPart;
typedef struct _XmTreeViewClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmTreeViewClassPart tree_view_class;
} XmTreeViewClassRec;

typedef struct {
    XtPointer id;
    XtPointer parent_id;
    char *label;
    Boolean has_children;
    Boolean expanded;
} XmTreeViewItem;

typedef struct _XmTreeViewPart {
    XmTreeViewItem *items;
    Cardinal count;
    XtPointer selected_id;
    Cardinal top_row;
    Dimension indent;
    Dimension row_height;
    Cardinal visible_rows;
    Boolean rtl;
    XmRenderTable render_table;
    Boolean check_set_render_table;
    XtCallbackList selection_callback;
    XtCallbackList expansion_callback;
    GC gc;
} XmTreeViewPart;

typedef struct _XmTreeViewRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmTreeViewPart tree_view;
} XmTreeViewRec;

extern XmTreeViewClassRec xmTreeViewClassRec;

#endif /* _XmTreeViewP_h */
