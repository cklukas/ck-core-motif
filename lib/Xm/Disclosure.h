/* Native collapsible sections. LGPL-2.0-or-later. */
#ifndef _XmDisclosure_h
#define _XmDisclosure_h
#include <Xm/Xm.h>
#ifdef __cplusplus
extern "C" {
#endif
#define XmNexpanded "expanded"
#define XmCExpanded "Expanded"
#define XmNexpandedCallback "expandedCallback"
extern WidgetClass xmDisclosureWidgetClass;
typedef struct _XmDisclosureRec *XmDisclosureWidget;
typedef struct _XmDisclosureClassRec *XmDisclosureWidgetClass;
#define XmIsDisclosure(w) XtIsSubclass((w), xmDisclosureWidgetClass)
typedef struct {
  int reason;    /* XmCR_VALUE_CHANGED */
  XEvent *event; /* NULL for programmatic changes */
  Boolean expanded;
} XmDisclosureCallbackStruct;
Widget XmCreateDisclosure(Widget parent, char *name, ArgList args,
                          Cardinal count);
/* Borrowed children owned by the disclosure. Create application children under
   GetContent(), never directly under the disclosure. Do not destroy/reparent
   these internal children or change their managed/mapped state. GetHeader()
   is the keyboard focus target; its presentation resources are internal. */
Widget XmDisclosureGetContent(Widget);
Widget XmDisclosureGetHeader(Widget);
Boolean XmDisclosureGetExpanded(Widget);
/* notify sends exactly one expandedCallback on an actual state transition.
   XtSetValues(XmNexpanded) also notifies. Initial creation does not notify. */
void XmDisclosureSetExpanded(Widget, Boolean expanded, Boolean notify);
#ifdef __cplusplus
}
#endif
#endif
