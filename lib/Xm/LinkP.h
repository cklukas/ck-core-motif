#ifndef _XmLinkP_h
#define _XmLinkP_h
#include <Xm/Link.h>
#include <Xm/PrimitiveP.h>
typedef struct _XmLinkClassRec { CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class; XtPointer extension; } XmLinkClassRec;
typedef struct _XmLinkPart { XmString label; char *target; Boolean visited;
    Boolean visit_on_activate; XtCallbackList activate_callback; GC gc;
    XmRenderTable render_table; Boolean check_set_render_table; } XmLinkPart;
typedef struct _XmLinkRec { CorePart core; XmPrimitivePart primitive;
    XmLinkPart link; } XmLinkRec;
extern XmLinkClassRec xmLinkClassRec;
#endif
