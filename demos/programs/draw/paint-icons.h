/* SPDX-License-Identifier: LGPL-2.0-or-later */
#ifndef PAINT_ICONS_H
#define PAINT_ICONS_H
#include <Xm/Xm.h>
enum {
    ICON_PENCIL, ICON_BRUSH, ICON_ERASER, ICON_LINE, ICON_RECTANGLE,
    ICON_ELLIPSE, ICON_FILL, ICON_PICKER, ICON_TEXT, ICON_SELECT,
    ICON_NEW, ICON_OPEN, ICON_SAVE, ICON_UNDO, ICON_REDO, ICON_COPY,
    ICON_CUT, ICON_PASTE, ICON_ZOOM_OUT, ICON_FIT, ICON_ZOOM_IN, ICON_RESIZE
};
/* Creates classic colored vector artwork at the widget's display scale.
 * Each button owns its pixmaps and releases them on destruction. */
void paint_button_icon(Widget button, int icon, Boolean toggle);
#endif
