/* Additive text entry with application-provided, generation-bound suggestions. */
#ifndef _XmAutoSuggestBox_h
#define _XmAutoSuggestBox_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmAutoSuggestBoxWidgetClass;
typedef struct _XmAutoSuggestBoxClassRec *XmAutoSuggestBoxWidgetClass;
typedef struct _XmAutoSuggestBoxRec *XmAutoSuggestBoxWidget;

#ifndef XmIsAutoSuggestBox
#define XmIsAutoSuggestBox(w) XtIsSubclass((w), xmAutoSuggestBoxWidgetClass)
#endif

typedef struct {
    const char *id;
    XmString label;
    XtPointer user_data;
} XmAutoSuggestItem;

typedef struct {
    int reason;
    XEvent *event;
    const char *query;
    unsigned long generation;
    const char *id;
    XtPointer user_data;
} XmAutoSuggestCallbackStruct;

enum {
    XmAUTO_SUGGEST_REASON_QUERY_CHANGED = 1,
    XmAUTO_SUGGEST_REASON_COMMITTED = 2,
    XmAUTO_SUGGEST_REASON_DISMISSED = 3
};

Widget XmCreateAutoSuggestBox(Widget parent, char *name, ArgList args,
                              Cardinal count);
Widget XmVaCreateAutoSuggestBox(Widget parent, char *name, ...);
Widget XmVaCreateManagedAutoSuggestBox(Widget parent, char *name, ...);
Boolean XmAutoSuggestBoxSetSuggestions(Widget widget,
                                       const XmAutoSuggestItem *items,
                                       Cardinal count,
                                       unsigned long generation);
Cardinal XmAutoSuggestBoxGetSuggestionCount(Widget widget);
unsigned long XmAutoSuggestBoxGetGeneration(Widget widget);
void XmAutoSuggestBoxShow(Widget widget);
void XmAutoSuggestBoxDismiss(Widget widget, XEvent *event);
void XmAutoSuggestBoxCommit(Widget widget, Cardinal index, XEvent *event);
Widget XmAutoSuggestBoxGetSearchField(Widget widget);

#ifdef __cplusplus
}
#endif

#endif /* _XmAutoSuggestBox_h */
