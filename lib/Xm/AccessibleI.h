#ifndef _XmAccessibleI_h
#define _XmAccessibleI_h
#include "Accessible.h"
#include <stddef.h>
typedef struct _XmAccessibleNode {
  XmAccessibleId id, parent;
  Widget widget;
  XmAccessibleProvider provider;
  XtPointer data;
  struct _XmAccessibleNode *next, *previous, *id_next, *widget_next;
  struct _XmAccessibleNode **children;
  size_t child_count, child_capacity, child_index;
  Boolean removed, text_snapshot_busy;
  uint64_t change_serial;
  struct _XmAccessibleNode *retired_next;
} XmAccessibleNode;
typedef struct _XmAccessibleDisplayHook {
  Widget hooks;
  struct _XmAccessibleDisplayHook *next;
} XmAccessibleDisplayHook;
struct _XmAccessibility {
  Widget root;
  XmAccessibleDisplayHook *display_hooks;
  XtAppContext app;
  unsigned int dispatch_depth;
  XtIntervalId deferred_destroy;
  Boolean destroy_requested;
  XmAccessibleId focused_node, active_shell_node;
  char *name;
  XmAccessibleNode *nodes, *retired_nodes;
  XmAccessibleNode **id_buckets, **widget_buckets;
  size_t id_capacity, widget_capacity, node_count, widget_count;
  XmAccessibleId next_id;
  XmAccessibilityStatus status;
  XtPointer transport;
};
XmAccessibleNode *_XmAccessibilityNode(XmAccessibility *, XmAccessibleId);
size_t _XmAccessibilityChildCount(XmAccessibility *, XmAccessibleId);
XmAccessibleId _XmAccessibilityChildAt(XmAccessibility *, XmAccessibleId, size_t);
int _XmAccessibilityIndexInParent(XmAccessibility *, XmAccessibleId);
void _XmAccessibilityDispatchDone(XmAccessibility *);
Boolean _XmAccessibilityNativeTextSelection(XmAccessibility *, XmAccessibleId, Boolean *, int *, int *);
void _XmAccessibilityTransportRemoved(XmAccessibility *, XmAccessibleId, XmAccessibleId, int);
void _XmAccessibilityTransportDestroy(XmAccessibility *);
XmAccessibilityStatus _XmAccessibilityTransportConnect(XmAccessibility *);
void _XmAccessibilityTransportChanged(XmAccessibility *, XmAccessibleId);
const char *_XmAccessibilityTransportBusName(XmAccessibility *);
#endif
