#ifndef _XmSearchBoxP_h
#define _XmSearchBoxP_h
#include <Xm/SearchBox.h>
#include <Xm/TextFP.h>
typedef struct _XmSearchBoxClassPart { XtPointer extension; } XmSearchBoxClassPart;
typedef struct _XmSearchBoxClassRec { CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class; XmTextFieldClassPart text_class;
    XmSearchBoxClassPart search_class; } XmSearchBoxClassRec;
typedef struct _XmSearchBoxPart { XmString placeholder; Boolean clear_button;
    XtCallbackList query_changed_callback, committed_callback, cleared_callback;
    Boolean clearing; } XmSearchBoxPart;
typedef struct _XmSearchBoxRec { CorePart core; XmPrimitivePart primitive;
    XmTextFieldPart text; XmSearchBoxPart search; } XmSearchBoxRec;
extern XmSearchBoxClassRec xmSearchBoxClassRec;
#endif
