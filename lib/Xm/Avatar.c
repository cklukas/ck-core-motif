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
#include <X11/Xutil.h>
#include <Xm/Appearance.h>
#include <Xm/TraitP.h>
#include <Xm/CareVisualT.h>
#include "XmI.h"
#include "XmRenderTI.h"
#include "AppearanceDrawI.h"
#ifdef HAVE_XRENDER
#include <X11/extensions/Xrender.h>
#endif

static Position bounded_position(long long value) {
    if (value < SHRT_MIN) return (Position)SHRT_MIN;
    if (value > SHRT_MAX) return (Position)SHRT_MAX;
    return (Position)value;
}

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void ClassPartInitialize(WidgetClass);
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
     NULL, ClassPartInitialize, False, Initialize, NULL, XtInheritRealize, NULL, 0,
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
                        &root, &x, &y, width, height, &border, depth) != 0;
}

static Boolean modern_avatar(Widget widget) {
    XmAppearanceProfile profile;
    return XmGetAppearanceProfile(XtDisplay(widget), &profile) &&
        (profile == XmAPPEARANCE_MODERN_LIGHT || profile == XmAPPEARANCE_MODERN_DARK);
}

static Boolean ParentVisualChanged(Widget widget, Widget old_parent,
                                   Widget parent, Mask flags) {
    XmCareVisualTrait inherited = (XmCareVisualTrait)XmeTraitGet(
        (XtPointer)xmPrimitiveWidgetClass, XmQTcareParentVisual);
    Boolean redraw = inherited && inherited->redraw
        ? inherited->redraw(widget, old_parent, parent, flags) : False;
    if (modern_avatar(widget) &&
        (flags & (VisualBackgroundPixel | VisualBackgroundPixmap)))
        _XmPaintInvalidate(widget);
    return redraw;
}

static XmCareVisualTraitRec avatar_visual = {0, ParentVisualChanged};
static void ClassPartInitialize(WidgetClass class) {
    XmeTraitSet((XtPointer)class, XmQTcareParentVisual, &avatar_visual);
}

/* An A8 coverage mask gives the circle a smooth edge without changing the
 * native window shape or assuming that the parent uses the avatar's color. */
static Boolean composite_circle(Widget widget, Drawable target, Pixmap face,
                                 int x, int y, int diameter) {
#ifdef HAVE_XRENDER
    Display *display = XtDisplay(widget);
    Screen *screen = XtScreen(widget);
    int event, error;
    XRenderPictFormat *format, *alpha;
    XImage *image;
    Pixmap mask;
    GC gc;
    Picture source, destination, coverage;
    if (diameter <= 0 || diameter > 2048 ||
        widget->core.depth != DefaultDepthOfScreen(screen) ||
        !XRenderQueryExtension(display, &event, &error)) return False;
    format = XRenderFindVisualFormat(display, DefaultVisualOfScreen(screen));
    alpha = XRenderFindStandardFormat(display, PictStandardA8);
    if (!format || format->direct.alphaMask || !alpha) return False;
    image = XCreateImage(display, DefaultVisualOfScreen(screen), 8, ZPixmap,
                         0, NULL, diameter, diameter, 8, 0);
    if (!image) return False;
    if (image->bytes_per_line <= 0 ||
        (size_t)diameter > UINT_MAX / (size_t)image->bytes_per_line) {
        XDestroyImage(image);
        return False;
    }
    image->data = XtCalloc((Cardinal)diameter,
                           (Cardinal)image->bytes_per_line);
    if (!image->data) { XDestroyImage(image); return False; }
    double radius = diameter / 2.0, squared = radius * radius;
    for (int yy = 0; yy < diameter; ++yy)
        for (int xx = 0; xx < diameter; ++xx) {
            unsigned inside = 0;
            for (unsigned sy = 0; sy < 4; ++sy)
                for (unsigned sx = 0; sx < 4; ++sx) {
                    double dx = xx + (sx + .5) / 4.0 - radius;
                    double dy = yy + (sy + .5) / 4.0 - radius;
                    inside += dx * dx + dy * dy <= squared;
                }
            XPutPixel(image, xx, yy, (inside * 255 + 8) / 16);
        }
    mask = XCreatePixmap(display, target, diameter, diameter, 8);
    if (mask == None) {
        XDestroyImage(image);
        return False;
    }
    gc = XCreateGC(display, mask, 0, NULL);
    if (gc == NULL) {
        XFreePixmap(display, mask);
        XDestroyImage(image);
        return False;
    }
    XPutImage(display, mask, gc, image, 0, 0, 0, 0, diameter, diameter);
    XFreeGC(display, gc);
    XDestroyImage(image);
    source = XRenderCreatePicture(display, face, format, 0, NULL);
    if (source == None) {
        XFreePixmap(display, mask);
        return False;
    }
    destination = XRenderCreatePicture(display, target, format, 0, NULL);
    if (destination == None) {
        XRenderFreePicture(display, source);
        XFreePixmap(display, mask);
        return False;
    }
    coverage = XRenderCreatePicture(display, mask, alpha, 0, NULL);
    if (coverage == None) {
        XRenderFreePicture(display, destination);
        XRenderFreePicture(display, source);
        XFreePixmap(display, mask);
        return False;
    }
    XRenderComposite(display, PictOpOver, source, coverage, destination,
                     0, 0, 0, 0, x, y, diameter, diameter);
    XRenderFreePicture(display, coverage);
    XRenderFreePicture(display, destination);
    XRenderFreePicture(display, source);
    XFreePixmap(display, mask);
    return True;
#else
    (void)widget; (void)target; (void)face; (void)x; (void)y; (void)diameter;
    return False;
#endif
}

static void PaintModernAvatar(Widget widget, Drawable target, GC gc,
                               int x, int y, int diameter, Pixel foreground) {
    XmAvatarWidget avatar = (XmAvatarWidget)widget;
    Display *display = XtDisplay(widget);
    Pixel outside = avatar->core.background_pixel;
    unsigned width = 0, height = 0, depth = 0;
    Dimension text_width = 0, text_height = 0;
    Pixmap face;
    unsigned int extent;
    if (diameter <= 0) return;
    extent = (unsigned int)diameter;
    if (_XmAppearanceOwnsResource(widget, XmNbackground) && XtParent(widget))
        outside = XtParent(widget)->core.background_pixel;
    XSetClipMask(display, gc, None);
    XSetForeground(display, gc, outside);
    if (_XmAppearanceOwnsResource(widget, XmNbackground) && XtParent(widget)) {
        Widget parent = XtParent(widget);
        int origin_x = -widget->core.x - widget->core.border_width;
        int origin_y = -widget->core.y - widget->core.border_width;
        while (parent->core.background_pixmap == ParentRelative && XtParent(parent)) {
            origin_x -= parent->core.x + parent->core.border_width;
            origin_y -= parent->core.y + parent->core.border_width;
            parent = XtParent(parent);
        }
        XSetForeground(display, gc, parent->core.background_pixel);
        Pixmap tile = parent->core.background_pixmap;
        if (tile != None && tile != ParentRelative && tile != XmUNSPECIFIED_PIXMAP) {
            Window root;
            int tile_x, tile_y;
            unsigned tile_width, tile_height, border, tile_depth;
            if (XGetGeometry(display, tile, &root, &tile_x, &tile_y,
                    &tile_width, &tile_height, &border, &tile_depth) &&
                tile_depth == widget->core.depth) {
                XSetTile(display, gc, tile);
                XSetFillStyle(display, gc, FillTiled);
                XSetTSOrigin(display, gc, origin_x, origin_y);
            }
        }
    }
    XFillRectangle(display, target, gc, 0, 0, widget->core.width, widget->core.height);
    XSetFillStyle(display, gc, FillSolid);
    XSetTSOrigin(display, gc, 0, 0);
    face = XCreatePixmap(display, target, extent, extent, widget->core.depth);
    if (face == None) {
        XSetForeground(display, gc, foreground);
        _XmAppearanceFillRoundedRectangle(display, target, gc, x, y,
                                          diameter, diameter, diameter / 2);
        return;
    }
    XSetForeground(display, gc, avatar->core.background_pixel);
    XSetBackground(display, gc, avatar->core.background_pixel);
    XFillRectangle(display, face, gc, 0, 0, extent, extent);
    XSetForeground(display, gc, foreground);
    if (pixmap_size(avatar, &width, &height, &depth) &&
        (depth == 1 || depth == widget->core.depth)) {
        unsigned copy_width = MIN(width, (unsigned)diameter);
        unsigned copy_height = MIN(height, (unsigned)diameter);
        if (depth == 1)
            XCopyPlane(display, avatar->avatar.pixmap, face, gc,
                (width-copy_width)/2, (height-copy_height)/2, copy_width, copy_height,
                (diameter-(int)copy_width)/2, (diameter-(int)copy_height)/2, 1);
        else
            XCopyArea(display, avatar->avatar.pixmap, face, gc,
                (width-copy_width)/2, (height-copy_height)/2, copy_width, copy_height,
                (diameter-(int)copy_width)/2, (diameter-(int)copy_height)/2);
    } else if (avatar->avatar.initials && avatar->avatar.font) {
        XmStringExtent(avatar->avatar.font, avatar->avatar.initials,
                         &text_width, &text_height);
        if (text_width && text_height)
            XmStringDraw(display, face, avatar->avatar.font, avatar->avatar.initials,
                gc, bounded_position((diameter-(int)text_width)/2),
                bounded_position((diameter-(int)text_height)/2), text_width,
                XmALIGNMENT_BEGINNING, XmPrim_layout_direction(avatar), NULL);
    }
    if (!composite_circle(widget, target, face, x, y, diameter)) {
        Region clip = _XmAppearanceRoundedRectangleRegion(x, y, diameter,
                                                           diameter, diameter/2);
        XSetRegion(display, gc, clip);
        XCopyArea(display, face, target, gc, 0, 0, extent, extent, x, y);
        XSetClipMask(display, gc, None);
        XDestroyRegion(clip);
    }
#ifdef USE_XFT
    _XmXftDrawEvict(display, face);
#endif
    XFreePixmap(display, face);
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
    unsigned int extent;
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
    extent = (unsigned int)diameter;
    x = ((int)avatar->core.width - diameter) / 2;
    y = ((int)avatar->core.height - diameter) / 2;
    if (modern_avatar(widget) && diameter <= 2048) {
        PaintModernAvatar(widget, window, gc, x, y, diameter, foreground);
        return;
    }
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
            Pixmap text = XCreatePixmap(display, window, extent, extent,
                                        avatar->core.depth);
            if (text != None) {
                XSetForeground(display, gc, avatar->core.background_pixel);
                XFillRectangle(display, text, gc, 0, 0, extent, extent);
                XSetForeground(display, gc, foreground);
                XmStringDraw(display, text, avatar->avatar.font,
                             avatar->avatar.initials, gc,
                             (diameter - (int)text_width) / 2,
                             (diameter - (int)text_height) / 2,
                             text_width, XmALIGNMENT_BEGINNING,
                             XmPrim_layout_direction(avatar), NULL);
                XSetRegion(display, gc, clip);
                XCopyArea(display, text, window, gc, 0, 0, extent, extent, x, y);
#ifdef USE_XFT
                _XmXftDrawEvict(display, text);
#endif
                XFreePixmap(display, text);
            }
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
