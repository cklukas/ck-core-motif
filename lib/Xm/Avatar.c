/* Additive non-interactive avatar with pixmap/initials fallback. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "PaintI.h"

#include <X11/IntrinsicP.h>
#include <Xm/AvatarP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <limits.h>
#include <string.h>
#include "XmI.h"
#include "AppearanceDrawI.h"

static Position bounded_position(long long value) {
    if (value < SHRT_MIN) return (Position)SHRT_MIN;
    if (value > SHRT_MAX) return (Position)SHRT_MAX;
    return (Position)value;
}

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void BufferedRedisplay(Widget, XEvent *, Region);
static Boolean BufferedSetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *, XtWidgetGeometry *);

#define Offset(field) XtOffsetOf(XmAvatarRec, field)
static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmAvatarWidget avatar = (XmAvatarWidget)widget;
    (void)offset;
    if (avatar->avatar.check_set_render_table)
        value->addr = NULL;
    else {
        avatar->avatar.check_set_render_table = True;
        value->addr = (char *)&avatar->avatar.font;
    }
}
static XtResource resources[] = {
    {XmNavatarInitials, XmCAvatarInitials, XmRXmString, sizeof(XmString),
     Offset(avatar.initials), XmRImmediate, (XtPointer)NULL},
    {XmNavatarPixmap, XmCAvatarPixmap, XmRDynamicPixmap, sizeof(Pixmap),
     Offset(avatar.pixmap), XmRImmediate, (XtPointer)XmUNSPECIFIED_PIXMAP},
    {XmNavatarSize, XmCAvatarSize, XmRHorizontalDimension, sizeof(Dimension),
     Offset(avatar.size), XmRImmediate, (XtPointer)32},
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
     Offset(avatar.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmFontList),
     Offset(avatar.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)False}
};

externaldef(xmavatarclassrec) XmAvatarClassRec xmAvatarClassRec = {
    {(WidgetClass)&xmPrimitiveClassRec, "XmAvatar", sizeof(XmAvatarRec),
     NULL, NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0,
     resources, XtNumber(resources), NULLQUARK, True, XtExposeCompressMaximal,
     True, False, Destroy, NULL, BufferedRedisplay, BufferedSetValues, NULL,
     XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
     XtInheritTranslations, QueryGeometry, NULL, NULL},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
     XtInheritTranslations, NULL, NULL, 0, NULL},
    {NULL}};

externaldef(xmavatarwidgetclass) WidgetClass xmAvatarWidgetClass =
    (WidgetClass)&xmAvatarClassRec;

static XmString copy_initials(XmString source) {
    return source ? XmStringCopy(source) : XmStringCreateLocalized("");
}

static Boolean pixmap_size(XmAvatarWidget avatar, unsigned int *width,
                           unsigned int *height, unsigned int *depth) {
    Window root;
    int x, y;
    unsigned int border;
    if (avatar->avatar.pixmap == XmUNSPECIFIED_PIXMAP ||
        avatar->avatar.pixmap == None)
        return False;
    return XGetGeometry(XtDisplay((Widget)avatar), avatar->avatar.pixmap,
                        &root, &x, &y, width, height, &border, depth);
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmAvatarWidget avatar = (XmAvatarWidget)widget;
    XGCValues values;
    (void)args; (void)count;
    if (widget == NULL || !XtIsSubclass(widget, xmAvatarWidgetClass)) return;
    avatar->avatar.initials = copy_initials(avatar->avatar.initials);
    if (!avatar->avatar.font)
        avatar->avatar.font = XmFontListCopy(
            XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
    else
        avatar->avatar.font = XmFontListCopy(avatar->avatar.font);
    if (avatar->avatar.size == 0) avatar->avatar.size = 1;
    values.foreground = avatar->primitive.foreground;
    values.background = avatar->core.background_pixel;
    avatar->avatar.foreground_gc = XCreateGC(
        XtDisplay(widget), DefaultRootWindow(XtDisplay(widget)),
        GCForeground | GCBackground, &values);
    avatar->avatar.automatic_width = request->core.width == 0;
    avatar->avatar.automatic_height = request->core.height == 0;
    if (avatar->avatar.automatic_width) avatar->core.width = avatar->avatar.size;
    if (avatar->avatar.automatic_height) avatar->core.height = avatar->avatar.size;
}

static void Destroy(Widget widget) {
    XmAvatarWidget avatar = (XmAvatarWidget)widget;
    if (widget == NULL || !XtIsSubclass(widget, xmAvatarWidgetClass))
        return;
    if (avatar->avatar.foreground_gc)
        XFreeGC(XtDisplay(widget), avatar->avatar.foreground_gc);
    if (avatar->avatar.initials)
        XmStringFree(avatar->avatar.initials);
    if (avatar->avatar.font)
        XmRenderTableFree(avatar->avatar.font);
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    if (widget == NULL || !XtIsSubclass(widget, xmAvatarWidgetClass)) return;
    XmAvatarWidget avatar = (XmAvatarWidget)widget;
    Display *display = XtDisplay(widget);
    Drawable window = _XmPaintDrawable(widget);
    GC gc = avatar->avatar.foreground_gc;
    unsigned int pixmap_width = 0, pixmap_height = 0, depth = 0;
    int diameter, x, y, inner;
    Dimension text_width = 0, text_height = 0;
    Region clip;
    Pixel foreground = XtIsSensitive(widget) ? avatar->primitive.foreground
                                            : avatar->primitive.bottom_shadow_color;
    (void)event; (void)region;
    if (!window || !gc) return;
    XSetClipMask(display, gc, None);
    XSetForeground(display, gc, foreground);
    XSetBackground(display, gc, avatar->core.background_pixel);

    diameter = MIN(avatar->core.width, avatar->core.height);
    if (diameter <= 0) return;
    x = ((int)avatar->core.width - diameter) / 2;
    y = ((int)avatar->core.height - diameter) / 2;
    _XmAppearanceFillRoundedRectangle(display, window, gc, x, y,
                                      diameter, diameter, diameter / 2);
    inner = diameter - 2;
    if (inner <= 0) return;
    XSetForeground(display, gc, avatar->core.background_pixel);
    _XmAppearanceFillRoundedRectangle(display, window, gc, x + 1, y + 1,
                                      inner, inner, inner / 2);
    XSetForeground(display, gc, foreground);
    clip = _XmAppearanceRoundedRectangleRegion(x + 1, y + 1,
                                                inner, inner, inner / 2);
    if (pixmap_size(avatar, &pixmap_width, &pixmap_height, &depth) &&
        (depth == 1 || depth == avatar->core.depth)) {
        unsigned int copy_width = MIN(pixmap_width, (unsigned int)diameter);
        unsigned int copy_height = MIN(pixmap_height, (unsigned int)diameter);
        int target_x = ((int)avatar->core.width - (int)copy_width) / 2;
        int target_y = ((int)avatar->core.height - (int)copy_height) / 2;
        int source_x = (pixmap_width - copy_width) / 2;
        int source_y = (pixmap_height - copy_height) / 2;
        XSetRegion(display, gc, clip);
        if (depth == 1)
            XCopyPlane(display, avatar->avatar.pixmap, window, gc,
                       source_x, source_y, copy_width, copy_height,
                       target_x, target_y, 1);
        else
            XCopyArea(display, avatar->avatar.pixmap, window, gc,
                      source_x, source_y, copy_width, copy_height,
                      target_x, target_y);
    } else {
        if (avatar->avatar.initials && avatar->avatar.font)
            XmStringExtent(avatar->avatar.font, avatar->avatar.initials,
                           &text_width, &text_height);
        if (text_width && text_height) {
            /* Xft and core fonts share the same final circular clip. Drawing
             * text offscreen prevents a font backend's own clip from replacing
             * the circle and leaking long initials into the corner pixels. */
            Pixmap text = XCreatePixmap(display, window, diameter, diameter,
                                        avatar->core.depth);
            XSetForeground(display, gc, avatar->core.background_pixel);
            XFillRectangle(display, text, gc, 0, 0, diameter, diameter);
            XSetForeground(display, gc, foreground);
            XmStringDraw(display, text, avatar->avatar.font,
                         avatar->avatar.initials, gc,
                         (diameter - (int)text_width) / 2,
                         (diameter - (int)text_height) / 2,
                         text_width, XmALIGNMENT_BEGINNING,
                         XmPrim_layout_direction(avatar), NULL);
            XSetRegion(display, gc, clip);
            XCopyArea(display, text, window, gc, 0, 0, diameter, diameter, x, y);
            XFreePixmap(display, text);
        }
    }
    XSetClipMask(display, gc, None);
    XDestroyRegion(clip);
}

static void BufferedRedisplay(Widget widget, XEvent *event, Region region)
{
    _XmBufferedPaint(widget, Redisplay, event, region);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmAvatarWidget old = (XmAvatarWidget)old_widget;
    XmAvatarWidget avatar = (XmAvatarWidget)widget;
    if (old_widget == NULL || widget == NULL ||
        !XtIsSubclass(old_widget, xmAvatarWidgetClass) ||
        !XtIsSubclass(widget, xmAvatarWidgetClass)) return False;
    Boolean font_changed = old->avatar.font != avatar->avatar.font;
    Boolean initials_changed = old->avatar.initials != avatar->avatar.initials;
    (void)request;
    if (initials_changed) {
        XmString replacement = copy_initials(avatar->avatar.initials);
        if (replacement) {
            if (old->avatar.initials) XmStringFree(old->avatar.initials);
            avatar->avatar.initials = replacement;
        }
        else
            avatar->avatar.initials = old->avatar.initials;
    }
    if (!avatar->avatar.size) avatar->avatar.size = 1;
    if (font_changed) {
        XmFontList replacement = avatar->avatar.font
            ? XmFontListCopy(avatar->avatar.font)
            : XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
        if (replacement) {
            if (old->avatar.font) XmRenderTableFree(old->avatar.font);
            avatar->avatar.font = replacement;
        } else {
            avatar->avatar.font = old->avatar.font;
            font_changed = False;
        }
    }
    for (Cardinal i = 0; i < *count; ++i) {
        if (!strcmp(args[i].name, XmNwidth))
            avatar->avatar.automatic_width = (Dimension)args[i].value == 0;
        if (!strcmp(args[i].name, XmNheight))
            avatar->avatar.automatic_height = (Dimension)args[i].value == 0;
    }
    if (avatar->avatar.automatic_width) avatar->core.width = avatar->avatar.size;
    if (avatar->avatar.automatic_height) avatar->core.height = avatar->avatar.size;
    return (initials_changed &&
            avatar->avatar.initials != old->avatar.initials) ||
           old->avatar.pixmap != avatar->avatar.pixmap ||
           old->avatar.size != avatar->avatar.size || font_changed ||
           old->primitive.foreground != avatar->primitive.foreground ||
           old->core.background_pixel != avatar->core.background_pixel ||
           old->primitive.bottom_shadow_color != avatar->primitive.bottom_shadow_color ||
           XtIsSensitive(old_widget) != XtIsSensitive(widget) ||
           XmPrim_layout_direction(old) != XmPrim_layout_direction(avatar);
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                       XtWidgetGeometry *reply) {
    XmAvatarWidget avatar = (XmAvatarWidget)widget;
    (void)request;
    if (widget == NULL || reply == NULL ||
        !XtIsSubclass(widget, xmAvatarWidgetClass))
        return XtGeometryNo;
    reply->request_mode = CWWidth | CWHeight;
    reply->width = avatar->avatar.automatic_width ? avatar->avatar.size : avatar->core.width;
    reply->height = avatar->avatar.automatic_height ? avatar->avatar.size : avatar->core.height;
    return XmeReplyToQueryGeometry(widget, request, reply);
}

Widget XmCreateAvatar(Widget parent, char *name, ArgList args, Cardinal count) {
    return XtCreateWidget(name, xmAvatarWidgetClass, parent, args, count);
}
Widget XmVaCreateAvatar(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name, xmAvatarWidgetClass,
                                                     parent, False, var, count);
    va_end(var); return widget;
}
Widget XmVaCreateManagedAvatar(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name, xmAvatarWidgetClass,
                                                     parent, True, var, count);
    va_end(var); return widget;
}

/* Xt's default redisplay request clears the front window. Queue our complete
 * frame instead, after resource and geometry negotiation has finished. */
static Boolean BufferedSetValues(Widget old, Widget request, Widget widget,
                                ArgList args, Cardinal *count)
{
    if (SetValues(old, request, widget, args, count))
        _XmPaintInvalidate(widget);
    return False;
}
