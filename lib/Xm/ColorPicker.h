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

typedef enum { XmCOLOR_PICKER_RGB, XmCOLOR_PICKER_HSV,
               XmCOLOR_PICKER_LAB, XmCOLOR_PICKER_PENCILS } XmColorPickerMode;
/* Switching modes never changes the selected color. Default: HSV. */
Boolean XmColorPickerSetMode(Widget widget, XmColorPickerMode mode);
XmColorPickerMode XmColorPickerGetMode(Widget widget);
/* HSV: hue [0,360], saturation/value [0,1]. Lab: D50, L [0,100],
 * a/b [-128,127]. Preview APIs preserve alpha and do not commit.
 * Lab outside sRGB is explicitly clipped, with a visible gamut notice. */
Boolean XmColorPickerPreviewHSV(Widget widget, double hue, double saturation, double value);
Boolean XmColorPickerPreviewLab(Widget widget, double lightness, double a, double b);
Widget XmCreateColorPicker(Widget parent, char *name, ArgList args,
                           Cardinal arg_count);
unsigned long XmColorPickerGetValue(Widget widget);
unsigned long XmColorPickerGetPreviewValue(Widget widget);
/* Borrowed editor; owned and destroyed by the ColorPicker. */
Widget XmColorPickerGetTextField(Widget widget);
/* Commit valid editor text; False leaves the committed value unchanged. */
Boolean XmColorPickerCommit(Widget widget, Boolean notify);
void XmColorPickerRevert(Widget widget);
void XmColorPickerSetValue(Widget widget, unsigned long value,
                           Boolean notify);
void XmColorPickerPreviewValue(Widget widget, unsigned long value);

#ifdef __cplusplus
}
#endif

#endif /* _XmColorPicker_h */
