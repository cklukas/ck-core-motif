#ifndef _XmAccessibleI_h
#define _XmAccessibleI_h
#include "Accessible.h"
typedef struct _XmAccessibleNode {
  XmAccessibleId id, parent;
  Widget widget;
  XmAccessibleProvider provider;
  XtPointer data;
  struct _XmAccessibleNode *next;
} XmAccessibleNode;
struct _XmAccessibility {
  Widget root;
  XtAppContext app;
  unsigned int dispatch_depth;
  XtIntervalId deferred_destroy;
  XmAccessibleId focused_node;
  char *name;
  XmAccessibleNode *nodes;
  XmAccessibleId next_id;
  XmAccessibilityStatus status;
  XtPointer transport;
};
XmAccessibleNode *_XmAccessibilityNode(XmAccessibility *, XmAccessibleId);
void _XmAccessibilityTransportDestroy(XmAccessibility *);
XmAccessibilityStatus _XmAccessibilityTransportConnect(XmAccessibility *);
void _XmAccessibilityTransportChanged(XmAccessibility *, XmAccessibleId);
const char *_XmAccessibilityTransportBusName(XmAccessibility *);
#endif
