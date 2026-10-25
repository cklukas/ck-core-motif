#ifndef _XmPromptP_h
#define _XmPromptP_h
#include <Xm/Prompt.h>
#include <Xm/PrimitiveP.h>
typedef struct _XmPromptClassRec { CoreClassPart core_class;
    XmPrimitiveClassPart primitive_class; XtPointer extension; } XmPromptClassRec;
typedef struct _XmPromptPart { XmString text; Boolean visible; GC gc; } XmPromptPart;
typedef struct _XmPromptRec { CorePart core; XmPrimitivePart primitive;
    XmPromptPart prompt; } XmPromptRec;
extern XmPromptClassRec xmPromptClassRec;
#endif
