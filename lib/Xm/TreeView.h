#ifndef _XmTreeView_h
#define _XmTreeView_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmTreeViewWidgetClass;
typedef struct _XmTreeViewClassRec *XmTreeViewWidgetClass;
typedef struct _XmTreeViewRec *XmTreeViewWidget;

#ifndef XmIsTreeView
#define XmIsTreeView(w) XtIsSubclass((w), xmTreeViewWidgetClass)
#endif

typedef struct {
    XtPointer id;
    XtPointer parent_id;
    const char *label;
    Boolean has_children;
    Boolean expanded;
} XmTreeViewNode;

typedef struct {
    int reason;
    XEvent *event;
    XtPointer id;
    XtPointer parent_id;
    Cardinal index;
    Boolean expanded;
} XmTreeViewCallbackStruct;

Widget XmCreateTreeView(Widget parent, char *name, ArgList args,
                        Cardinal count);
void XmTreeViewSetNodes(Widget widget, const XmTreeViewNode *nodes,
                        Cardinal count);
Cardinal XmTreeViewGetNodeCount(Widget widget);
XtPointer XmTreeViewGetSelectedId(Widget widget);
void XmTreeViewSetSelectedId(Widget widget, XtPointer id,
                             Boolean notify_change);
Boolean XmTreeViewSetExpanded(Widget widget, XtPointer id, Boolean expanded,
                               Boolean notify_change);
Boolean XmTreeViewGetExpanded(Widget widget, XtPointer id);
void XmTreeViewSetTopRow(Widget widget, Cardinal top_row);
Cardinal XmTreeViewGetTopRow(Widget widget);

#ifdef __cplusplus
}
#endif

#endif /* _XmTreeView_h */
