/* Opt-in drag/drop target feedback; transfer ownership remains application-owned. */
#ifndef _XmDropHighlight_h
#define _XmDropHighlight_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmDropHighlightWidgetClass;
typedef struct _XmDropHighlightClassRec *XmDropHighlightWidgetClass;
typedef struct _XmDropHighlightRec *XmDropHighlightWidget;

#ifndef XmIsDropHighlight
#define XmIsDropHighlight(w) XtIsSubclass((w), xmDropHighlightWidgetClass)
#endif

enum {
    XmDROP_HIGHLIGHT_NONE = 0,
    XmDROP_HIGHLIGHT_VALID = 1,
    XmDROP_HIGHLIGHT_INVALID = 2
};

Widget XmCreateDropHighlight(Widget parent, char *name, ArgList args,
                             Cardinal count);
Widget XmVaCreateDropHighlight(Widget parent, char *name, ...);
Widget XmVaCreateManagedDropHighlight(Widget parent, char *name, ...);
Boolean XmDropHighlightSetState(Widget widget, unsigned char state);
unsigned char XmDropHighlightGetState(Widget widget);

#ifdef __cplusplus
}
#endif

#endif /* _XmDropHighlight_h */
