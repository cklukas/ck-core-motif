/* Additive, presentation-only prompt text. */
#ifndef _XmPrompt_h
#define _XmPrompt_h
#include <Xm/Xm.h>
#ifdef __cplusplus
extern "C" {
#endif
externalref WidgetClass xmPromptWidgetClass;
typedef struct _XmPromptClassRec *XmPromptWidgetClass;
typedef struct _XmPromptRec *XmPromptWidget;
#ifndef XmIsPrompt
#define XmIsPrompt(w) XtIsSubclass((w), xmPromptWidgetClass)
#endif
Widget XmCreatePrompt(Widget parent, char *name, ArgList args, Cardinal count);
Widget XmVaCreatePrompt(Widget parent, char *name, ...);
Widget XmVaCreateManagedPrompt(Widget parent, char *name, ...);
void XmPromptSetVisible(Widget widget, Boolean visible);
Boolean XmPromptGetVisible(Widget widget);
#ifdef __cplusplus
}
#endif
#endif /* _XmPrompt_h */
