#ifndef _XmBreadcrumbBarP_h
#define _XmBreadcrumbBarP_h

#include <Xm/BreadcrumbBar.h>
#include <Xm/PrimitiveP.h>

typedef struct _XmBreadcrumbBarClassPart { XtPointer extension; }
    XmBreadcrumbBarClassPart;
typedef struct _XmBreadcrumbBarClassRec {
    CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class;
    XmBreadcrumbBarClassPart breadcrumb_class;
} XmBreadcrumbBarClassRec;

typedef struct {
    char *label;
    XtPointer id;
} XmBreadcrumbBarItem;

typedef struct _XmBreadcrumbBarPart {
    XmBreadcrumbBarItem *items;
    Cardinal count;
    int current;
    int max_visible;
    String separator;
    Boolean rtl;
    XtCallbackList activate_callback;
    XtCallbackList current_changed_callback;
    XmFontList font;
    Boolean check_set_render_table;
    GC gc;
} XmBreadcrumbBarPart;

typedef struct _XmBreadcrumbBarRec {
    CorePart core;
    XmPrimitivePart primitive;
    XmBreadcrumbBarPart breadcrumb;
} XmBreadcrumbBarRec;

extern XmBreadcrumbBarClassRec xmBreadcrumbBarClassRec;

#endif /* _XmBreadcrumbBarP_h */
