#ifndef _XmFormSectionP_h
#define _XmFormSectionP_h
#include <Xm/FormSection.h>
#include <Xm/ManagerP.h>
typedef struct _XmFormSectionClassPart { XtPointer extension; } XmFormSectionClassPart;
typedef struct _XmFormSectionClassRec { CoreClassPart core_class;
    CompositeClassPart composite_class; ConstraintClassPart constraint_class;
    XmManagerClassPart manager_class; XmFormSectionClassPart section_class; } XmFormSectionClassRec;
typedef struct _XmFormSectionPart { XmString header_text, footer_text;
    Dimension margin_width, margin_height, spacing; Widget header, content, footer;
    Boolean laying_out, initializing; } XmFormSectionPart;
typedef struct _XmFormSectionRec { CorePart core; CompositePart composite;
    ConstraintPart constraint; XmManagerPart manager; XmFormSectionPart section; } XmFormSectionRec;
extern XmFormSectionClassRec xmFormSectionClassRec;
#endif
