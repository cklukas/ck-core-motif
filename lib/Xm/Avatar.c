/* Additive non-interactive avatar with pixmap/initials fallback. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <X11/IntrinsicP.h>
#include <Xm/AvatarP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <limits.h>
#include <string.h>
#include "XmI.h"

static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Redisplay(Widget, XEvent *, Region);
static Boolean SetValues(Widget, Widget, Widget, ArgList, Cardinal *);
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
     True, False, Destroy, NULL, Redisplay, SetValues, NULL,
     XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
     XtInheritTranslations, QueryGeometry, NULL, NULL},
    {XmInheritBorderHighlight, XmInheritBorderUnhighlight,
     XtInheritTranslations, NULL, NULL, 0, NULL},
    {NULL}};

externaldef(xmavatarwidgetclass) WidgetClass xmAvatarWidgetClass =
    (WidgetClass)&xmAvatarClassRec;

static XmString copy_initials(XmString source) {
    char *text;
    XmString result;
    if (!source) return XmStringCreateLocalized("");
    text = XmStringUnparse(source, NULL, XmCHARSET_TEXT, XmCHARSET_TEXT,
                           NULL, 0, XmOUTPUT_ALL);
    if (!text) return XmStringCreateLocalized("");
    result = XmStringCreateLocalized(text);
    XtFree(text);
    return result;
}

static Boolean pixmap_size(XmAvatarWidget avatar, unsigned int *width,
                           unsigned int *height) {
    Window root;
    int x, y;
    unsigned int border, depth;
    if (avatar->avatar.pixmap == XmUNSPECIFIED_PIXMAP ||
        avatar->avatar.pixmap == None)
        return False;
    return XGetGeometry(XtDisplay((Widget)avatar), avatar->avatar.pixmap,
                        &root, &x, &y, width, height, &border, &depth);
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmAvatarWidget avatar = (XmAvatarWidget)widget;
    XGCValues values;
    (void)request; (void)args; (void)count;
    avatar->avatar.initials = copy_initials(avatar->avatar.initials);
    if (!avatar->avatar.font)
        avatar->avatar.font = XmFontListCopy(
            XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
    else
        avatar->avatar.font = XmFontListCopy(avatar->avatar.font);
    if (avatar->avatar.size == 0) avatar->avatar.size = 1;
    values.foreground = avatar->primitive.foreground;
    values.background = avatar->core.background_pixel;
    avatar->avatar.fallback_font = XLoadQueryFont(XtDisplay(widget), "fixed");
    if (avatar->avatar.fallback_font)
        values.font = avatar->avatar.fallback_font->fid;
    avatar->avatar.foreground_gc = XCreateGC(
        XtDisplay(widget), DefaultRootWindow(XtDisplay(widget)),
        GCForeground | GCBackground |
        (avatar->avatar.fallback_font ? GCFont : 0), &values);
    if (!avatar->core.width) avatar->core.width = avatar->avatar.size;
    if (!avatar->core.height) avatar->core.height = avatar->avatar.size;
}

static void Destroy(Widget widget) {
    XmAvatarWidget avatar = (XmAvatarWidget)widget;
    if (avatar->avatar.foreground_gc)
        XFreeGC(XtDisplay(widget), avatar->avatar.foreground_gc);
    if (avatar->avatar.fallback_font)
        XFreeFont(XtDisplay(widget), avatar->avatar.fallback_font);
    if (avatar->avatar.font)
        XmRenderTableFree(avatar->avatar.font);
}

static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmAvatarWidget avatar = (XmAvatarWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    unsigned int pixmap_width = 0, pixmap_height = 0;
    int diameter, x, y;
    Dimension text_width = 0, text_height = 0;
    (void)event; (void)region;
    if (!window) return;
    XClearWindow(display, window);
    diameter = (int)(avatar->core.width < avatar->core.height
                         ? avatar->core.width : avatar->core.height);
    if (pixmap_size(avatar, &pixmap_width, &pixmap_height)) {
        unsigned int copy_width = pixmap_width < (unsigned int)diameter
                                      ? pixmap_width : (unsigned int)diameter;
        unsigned int copy_height = pixmap_height < (unsigned int)diameter
                                       ? pixmap_height : (unsigned int)diameter;
        x = ((int)avatar->core.width - (int)copy_width) / 2;
        y = ((int)avatar->core.height - (int)copy_height) / 2;
        XCopyArea(display, avatar->avatar.pixmap, window,
                  avatar->avatar.foreground_gc, 0, 0, copy_width, copy_height,
                  x, y);
    } else {
        XDrawArc(display, window, avatar->avatar.foreground_gc,
                 ((int)avatar->core.width - diameter) / 2,
                 ((int)avatar->core.height - diameter) / 2, diameter - 1,
                 diameter - 1, 0, 360 * 64);
        if (avatar->avatar.initials && avatar->avatar.font)
            XmStringExtent(avatar->avatar.font, avatar->avatar.initials,
                           &text_width, &text_height);
        if (text_width && text_height) {
            x = ((int)avatar->core.width - (int)text_width) / 2;
            y = ((int)avatar->core.height - (int)text_height) / 2;
            XmStringDraw(display, window, avatar->avatar.font,
                         avatar->avatar.initials, avatar->avatar.foreground_gc,
                         x, y, text_width, XmALIGNMENT_BEGINNING,
                         XmPrim_layout_direction(avatar), NULL);
        }
    }
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmAvatarWidget old = (XmAvatarWidget)old_widget;
    XmAvatarWidget avatar = (XmAvatarWidget)widget;
    (void)request; (void)args; (void)count;
    if (old->avatar.initials != avatar->avatar.initials)
        avatar->avatar.initials = copy_initials(avatar->avatar.initials);
    if (!avatar->avatar.size) avatar->avatar.size = 1;
    if (old->avatar.font != avatar->avatar.font) {
        XmFontList replacement = avatar->avatar.font
            ? XmFontListCopy(avatar->avatar.font)
            : XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
        if (old->avatar.font) XmRenderTableFree(old->avatar.font);
        avatar->avatar.font = replacement;
    }
    return old->avatar.initials != avatar->avatar.initials ||
           old->avatar.pixmap != avatar->avatar.pixmap ||
           old->avatar.size != avatar->avatar.size;
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                       XtWidgetGeometry *reply) {
    XmAvatarWidget avatar = (XmAvatarWidget)widget;
    (void)request;
    reply->request_mode = CWWidth | CWHeight;
    reply->width = avatar->avatar.size;
    reply->height = avatar->avatar.size;
    return (widget->core.width == reply->width && widget->core.height == reply->height)
               ? XtGeometryYes : XtGeometryAlmost;
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
