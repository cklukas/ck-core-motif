/* Additive, application-dispatched command discovery composite. */
#ifndef _XmCommandPalette_h
#define _XmCommandPalette_h
#include <Xm/Xm.h>
#ifdef __cplusplus
extern "C" {
#endif
externalref WidgetClass xmCommandPaletteWidgetClass;
typedef struct _XmCommandPaletteClassRec *XmCommandPaletteWidgetClass;
typedef struct _XmCommandPaletteRec *XmCommandPaletteWidget;
#ifndef XmIsCommandPalette
#define XmIsCommandPalette(w) XtIsSubclass((w), xmCommandPaletteWidgetClass)
#endif
typedef struct { const char *id; XmString label; XmString shortcut; XtPointer user_data; } XmCommandPaletteItem;
typedef struct { int reason; XEvent *event; const char *id; XtPointer user_data; } XmCommandPaletteCallbackStruct;
enum { XmCOMMAND_PALETTE_REASON_INVOKED = 1 };
Widget XmCreateCommandPalette(Widget parent, char *name, ArgList args, Cardinal count);
Widget XmVaCreateCommandPalette(Widget parent, char *name, ...);
Widget XmVaCreateManagedCommandPalette(Widget parent, char *name, ...);
void XmCommandPaletteSetItems(Widget widget, const XmCommandPaletteItem *items, Cardinal count);
Cardinal XmCommandPaletteGetItemCount(Widget widget);
Cardinal XmCommandPaletteGetVisibleCount(Widget widget);
void XmCommandPaletteSetFilter(Widget widget, const char *filter);
char *XmCommandPaletteGetFilter(Widget widget);
void XmCommandPaletteShow(Widget widget);
void XmCommandPaletteDismiss(Widget widget, XEvent *event);
void XmCommandPaletteInvokeSelection(Widget widget, XEvent *event);
Boolean XmCommandPaletteIsShown(Widget widget);
#ifdef __cplusplus
}
#endif
#endif /* _XmCommandPalette_h */
