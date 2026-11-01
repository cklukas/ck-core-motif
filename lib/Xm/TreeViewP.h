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
    XtPointer pressed_id;
    Cardinal top_row;
    Dimension indent;
    Dimension row_height;
    Boolean automatic_row_height;
    Boolean automatic_height;
    Cardinal visible_rows;
    Boolean rtl;
    XmRenderTable render_table;
    Boolean check_set_render_table;
    XtCallbackList selection_callback;
    XtCallbackList activate_callback;
    XtCallbackList expansion_callback;
    GC gc;
    Pixel select_color;
    Widget scroll_bar;
    Boolean updating_scrollbar;
} XmTreeViewPart;

typedef struct _XmTreeViewRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmTreeViewPart tree_view;
} XmTreeViewRec;

extern XmTreeViewClassRec xmTreeViewClassRec;

/* Shared keyboard/accessibility selection action. Hidden descendants and
 * insensitive widgets have no action. Invocation may destroy the widget. */
extern Boolean _XmTreeViewHasSelectionAction(Widget);
extern Boolean _XmTreeViewActivateSelection(Widget, XEvent *);

#endif /* _XmTreeViewP_h */
