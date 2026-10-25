/* Additive presentation composite for labelled form content. */
#ifndef _XmFormSection_h
#define _XmFormSection_h
#include <Xm/Xm.h>
#ifdef __cplusplus
extern "C" {
#endif
externalref WidgetClass xmFormSectionWidgetClass;
typedef struct _XmFormSectionClassRec *XmFormSectionWidgetClass;
typedef struct _XmFormSectionRec *XmFormSectionWidget;
#ifndef XmIsFormSection
#define XmIsFormSection(w) XtIsSubclass((w), xmFormSectionWidgetClass)
#endif
Widget XmCreateFormSection(Widget parent, char *name, ArgList args, Cardinal count);
Widget XmVaCreateFormSection(Widget parent, char *name, ...);
Widget XmVaCreateManagedFormSection(Widget parent, char *name, ...);
Widget XmFormSectionGetContent(Widget widget);
Widget XmFormSectionGetHeader(Widget widget);
Widget XmFormSectionGetFooter(Widget widget);
#ifdef __cplusplus
}
#endif
#endif /* _XmFormSection_h */
