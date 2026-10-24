/* Additive packed-RGBA color picker. */
#ifndef _XmColorPicker_h
#define _XmColorPicker_h

#include <Xm/Xm.h>

#ifdef __cplusplus
extern "C" {
#endif

externalref WidgetClass xmColorPickerWidgetClass;
typedef struct _XmColorPickerClassRec *XmColorPickerWidgetClass;
typedef struct _XmColorPickerRec *XmColorPickerWidget;
#ifndef XmIsColorPicker
#define XmIsColorPicker(w) XtIsSubclass((w), xmColorPickerWidgetClass)
#endif

typedef struct {
    int reason;
    XEvent *event;
    unsigned long value; /* 0xRRGGBBAA, each channel 8 bits */
} XmColorPickerCallbackStruct;

Widget XmCreateColorPicker(Widget parent, char *name, ArgList args,
                           Cardinal arg_count);
unsigned long XmColorPickerGetValue(Widget widget);
void XmColorPickerSetValue(Widget widget, unsigned long value,
                           Boolean notify);
void XmColorPickerPreviewValue(Widget widget, unsigned long value);

#ifdef __cplusplus
}
#endif

#endif /* _XmColorPicker_h */
