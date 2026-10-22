/* Native collapsible sections. LGPL-2.0-or-later. */
#ifndef _XmDisclosureP_h
#define _XmDisclosureP_h
#include <Xm/Disclosure.h>
#include <Xm/ManagerP.h>
#define XmNdisclosureStateCallback "disclosureStateCallback"
typedef struct {
  XtPointer extension;
} XmDisclosureClassPart;
typedef struct _XmDisclosureClassRec {
  CoreClassPart core_class;
  CompositeClassPart composite_class;
  ConstraintClassPart constraint_class;
  XmManagerClassPart manager_class;
  XmDisclosureClassPart disclosure_class;
} XmDisclosureClassRec;
typedef struct {
  Boolean expanded;
  XmString label;
  XtCallbackList callbacks, state_callbacks;
  Dimension margin_width, margin_height, spacing;
  Widget arrow, header, content;
  Boolean laying_out, initializing;
} XmDisclosurePart;
typedef struct _XmDisclosureRec {
  CorePart core;
  CompositePart composite;
  ConstraintPart constraint;
  XmManagerPart manager;
  XmDisclosurePart disclosure;
} XmDisclosureRec;
extern XmDisclosureClassRec xmDisclosureClassRec;
#endif
