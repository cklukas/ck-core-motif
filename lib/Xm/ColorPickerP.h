#ifndef _XmColorPickerP_h
#define _XmColorPickerP_h
#include <Xm/ColorPicker.h>
#include <Xm/ManagerP.h>
#include <stdint.h>
typedef struct _XmColorPickerClassPart { XtPointer extension; } XmColorPickerClassPart;
typedef struct _XmColorPickerClassRec {
    CoreClassPart core_class;
    CompositeClassPart composite_class;
    ConstraintClassPart constraint_class;
    XmManagerClassPart manager_class;
    XmColorPickerClassPart color_picker_class;
} XmColorPickerClassRec;
typedef struct _XmColorPickerPart {
    uint32_t value, preview;
    XtCallbackList preview_callback, value_changed_callback;
    XmRenderTable render_table;
    Boolean check_set_render_table;
    Widget swatch, editor, hex_label, error_label, apply, revert;
    Widget labels[4], channels[4];
    Widget modes, plane, mode_hint, mode_labels[3], mode_channels[3], pencils[24];
    XmColorPickerMode mode;
    double hsv[3], lab[3];
    int model_source;
    Boolean gamut, dragging;
    Pixmap plane_cache;
    unsigned plane_width, plane_height;
    double plane_parameter;
    int plane_mode;
    Boolean plane_sensitive;
    Colormap plane_colormap;
    Pixel palette[216];
    Boolean palette_allocated[216], palette_ready;
    Colormap palette_colormap;
    Pixel palette_background;
    Dimension spacing;
    Boolean initializing, updating, automatic_width, automatic_height, invalid;
    XtWorkProcId layout_work;
    GC swatch_gc;
    Pixel swatch_pixels[2];
    Boolean swatch_allocated[2];
    Colormap swatch_colormap;
} XmColorPickerPart;
typedef struct _XmColorPickerRec {
    CorePart core;
    CompositePart composite;
    ConstraintPart constraint;
    XmManagerPart manager;
    XmColorPickerPart color_picker;
} XmColorPickerRec;
extern XmColorPickerClassRec xmColorPickerClassRec;
#endif /* _XmColorPickerP_h */
