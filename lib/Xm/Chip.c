/* Additive compact chip with explicit activation/removal callbacks. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "PaintI.h"

#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/ChipP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <limits.h>
#include <string.h>
#include "PrimitiveI.h"
#include "XmI.h"
#include "CallbackI.h"
#include "AppearanceDrawI.h"
#include <Xm/Appearance.h>

static void natural_size(XmChipWidget, Dimension *, Dimension *);
static void Highlight(Widget);
static void Unhighlight(Widget);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void BufferedRedisplay(Widget, XEvent *, Region);
static Boolean BufferedSetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);
static void Events(Widget, XtPointer, XEvent *, Boolean *);
static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmChipWidget chip = (XmChipWidget)widget;
    (void)offset;
    if (chip->chip.check_set_render_table)
        value->addr = NULL;
    else {
        chip->chip.check_set_render_table = True;
        value->addr = (char *)&chip->chip.font;
    }
}

#define Offset(field) XtOffsetOf(XmChipRec, field)
static XtResource resources[] = {
    {XmNchipLabel, XmCChipLabel, XmRXmString, sizeof(XmString),
     Offset(chip.label), XmRImmediate, (XtPointer)NULL},
    {XmNchipPixmap, XmCChipPixmap, XmRDynamicPixmap, sizeof(Pixmap),
     Offset(chip.pixmap), XmRImmediate, (XtPointer)XmUNSPECIFIED_PIXMAP},
    {XmNchipRemovable, XmCChipRemovable, XmRBoolean, sizeof(Boolean),
     Offset(chip.removable), XmRImmediate, (XtPointer)False},
    {XmNchipActivateCallback, XmCChipActivateCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(chip.activate_callback), XmRPointer,
     (XtPointer)NULL},
    {XmNchipRemoveCallback, XmCChipRemoveCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(chip.remove_callback), XmRPointer,
     (XtPointer)NULL},
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
     Offset(chip.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmRenderTable),
     Offset(chip.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc,
     (XtPointer)_XmSetThickness}
};

externaldef(xmchipclassrec) XmChipClassRec xmChipClassRec = {
    {(WidgetClass)&xmPrimitiveClassRec, "XmChip", sizeof(XmChipRec), NULL,
     NULL, False, Initialize, NULL, XtInheritRealize, NULL, 0, resources,
     XtNumber(resources), NULLQUARK, True, XtExposeCompressMaximal, True,
     False, Destroy, NULL, BufferedRedisplay, BufferedSetValues, NULL,
     XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL,
     (String)_XmPrimitiveTabFocusTranslations, QueryGeometry, NULL, NULL},
    {Highlight, Unhighlight,
     XtInheritTranslations, NULL, NULL, 0, NULL},
    {NULL}};

externaldef(xmchipwidgetclass) WidgetClass xmChipWidgetClass =
    (WidgetClass)&xmChipClassRec;

static void create_gc(XmChipWidget chip) {
    XGCValues values;
    Display *display = XtDisplay((Widget)chip);
    values.foreground = chip->primitive.foreground;
    values.background = chip->core.background_pixel;
    if (chip->chip.font == NULL)
        chip->chip.font = XmFontListCopy(
            XmeGetDefaultRenderTable((Widget)chip, XmLABEL_FONTLIST));
    else
        chip->chip.font = XmFontListCopy(chip->chip.font);
    chip->chip.foreground_gc = XCreateGC(display, DefaultRootWindow(display),
                                         GCForeground | GCBackground, &values);
}

static XmString copy_label(XmString source) {
    return source ? XmStringCopy(source) : XmStringCreateLocalized("");
}

static void Initialize(Widget request, Widget widget, ArgList args,
                       Cardinal *count) {
    XmChipWidget chip = (XmChipWidget)widget;
    (void)request; (void)args; (void)count;
    chip->chip.label = copy_label(chip->chip.label);
    create_gc(chip);
    chip->chip.automatic_width=request->core.width==0;
    chip->chip.automatic_height=request->core.height==0;
    chip->chip.hot_target=chip->chip.armed_target=0;
    Dimension width,height;
    natural_size(chip,&width,&height);
    if(chip->chip.automatic_width) chip->core.width=width;
    if(chip->chip.automatic_height) chip->core.height=height;
    XtAddEventHandler(widget, KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask, False, Events,
                      NULL);
}

static void Destroy(Widget widget) {
    XmChipWidget chip = (XmChipWidget)widget;
    Display *display = XtDisplay(widget);
    XtRemoveEventHandler(widget, KeyPressMask | ButtonPressMask | ButtonReleaseMask | PointerMotionMask | EnterWindowMask | LeaveWindowMask, False, Events,
                         NULL);
    if (chip->chip.foreground_gc) XFreeGC(display, chip->chip.foreground_gc);
    if (chip->chip.font) XmRenderTableFree(chip->chip.font);
    if (chip->chip.label) XmStringFree(chip->chip.label);
}

typedef struct {
    Dimension text_width, text_height;
    unsigned icon_width, icon_height, icon_depth;
    int inset, horizontal, vertical, gap, remove_extent;
} ChipMetrics;
typedef struct {
    XRectangle face, text, icon, remove, remove_hit;
} ChipLayout;

static Boolean modern(Widget widget) {
    XmAppearanceProfile profile=XmAPPEARANCE_CLASSIC;
    return XmGetAppearanceProfile(XtDisplay(widget),&profile) &&
        (profile==XmAPPEARANCE_MODERN_LIGHT || profile==XmAPPEARANCE_MODERN_DARK);
}
static Boolean modern_geometry(Widget widget) {
    XmAppearanceProfile profile=_XmAppearanceGeometryProfile(XtDisplay(widget));
    return profile==XmAPPEARANCE_MODERN_LIGHT || profile==XmAPPEARANCE_MODERN_DARK;
}
static void metrics(XmChipWidget chip, ChipMetrics *m) {
    memset(m,0,sizeof(*m));
    if (chip->chip.label && chip->chip.font)
        XmStringExtent(chip->chip.font, chip->chip.label,&m->text_width,&m->text_height);
    if(chip->chip.pixmap!=None && chip->chip.pixmap!=XmUNSPECIFIED_PIXMAP) {
        Window root;int x,y;unsigned border;
        if(!XGetGeometry(XtDisplay((Widget)chip),chip->chip.pixmap,&root,&x,&y,
                        &m->icon_width,&m->icon_height,&border,&m->icon_depth))
            m->icon_width=m->icon_height=0;
    }
    XmAppearanceDensity density=_XmAppearanceGeometryDensity(XtDisplay((Widget)chip));
    m->inset=chip->primitive.highlight_thickness+1;
    m->horizontal=_XmDpiDefaultDimension((Widget)chip,10);
    m->vertical=_XmDpiDefaultDimension((Widget)chip,
        modern_geometry((Widget)chip) && density==XmAPPEARANCE_DENSITY_COMPACT?3:5);
    m->gap=_XmDpiDefaultDimension((Widget)chip,6);
    m->remove_extent=chip->chip.removable?_XmDpiDefaultDimension((Widget)chip,18):0;
}
static void natural_size(XmChipWidget chip, Dimension *width, Dimension *height) {
    ChipMetrics m;metrics(chip,&m);
    unsigned long w=(unsigned long)m.text_width+2UL*((unsigned long)m.horizontal+(unsigned long)m.inset);
    if(m.icon_width) w+=(unsigned long)m.icon_width+(unsigned long)m.gap;
    if(m.remove_extent) w+=(unsigned long)m.remove_extent+(unsigned long)m.gap;
    unsigned long h=MAX(m.text_height,MAX(m.icon_height,(unsigned)m.remove_extent));
    h+=2UL*((unsigned long)m.vertical+(unsigned long)m.inset);
    XmAppearanceDensity density=_XmAppearanceGeometryDensity(XtDisplay((Widget)chip));
    h=MAX(h,(unsigned long)_XmDpiDefaultDimension((Widget)chip,
        modern_geometry((Widget)chip) && density==XmAPPEARANCE_DENSITY_COMFORTABLE?34:28));
    *height=(Dimension)MIN(USHRT_MAX,MAX(1UL,h));
    *width=(Dimension)MIN(USHRT_MAX,MAX((unsigned long)*height,w));
}
static XRectangle rectangle(int x,int y,int width,int height) {
    XRectangle r={(short)MAX(0,MIN(SHRT_MAX,x)),(short)MAX(0,MIN(SHRT_MAX,y)),
        (unsigned short)MAX(0,MIN(USHRT_MAX,width)),(unsigned short)MAX(0,MIN(USHRT_MAX,height))};
    return r;
}
static void layout(XmChipWidget chip, const ChipMetrics *m, ChipLayout *l) {
    int width=chip->core.width,height=chip->core.height;
    int inset=MIN(m->inset,MIN(width,height)/2);
    l->face=rectangle(inset,inset,width-2*inset,height-2*inset);
    int left=inset+m->horizontal, right=MAX(left,width-inset-m->horizontal);
    int top=inset+m->vertical, available_height=MAX(0,height-2*top);
    memset(&l->remove,0,sizeof(l->remove));memset(&l->remove_hit,0,sizeof(l->remove_hit));
    if(m->remove_extent) {
        int extent=MIN(m->remove_extent,MIN(right-left,available_height));
        l->remove=rectangle(right-extent,(height-extent)/2,extent,extent);
        l->remove_hit=rectangle(MAX(inset,right-extent-m->gap/2),inset,
            width-inset-MAX(inset,right-extent-m->gap/2),height-2*inset);
        right=MAX(left,right-extent-m->gap);
    }
    int iw=(int)MIN(m->icon_width,(unsigned int)MAX(0,right-left));
    int ih=(int)MIN(m->icon_height,(unsigned int)available_height);
    l->icon=rectangle(left,(height-ih)/2,iw,ih);
    if(iw)left=MIN(right,left+iw+m->gap);
    l->text=rectangle(left,top,right-left,available_height);
    if(XmPrim_layout_direction(chip)==XmRIGHT_TO_LEFT) {
        XRectangle *parts[]={&l->text,&l->icon,&l->remove,&l->remove_hit};
        for(unsigned i=0;i<XtNumber(parts);++i)
            parts[i]->x=(short)MAX(0,MIN(SHRT_MAX,width-parts[i]->x-parts[i]->width));
    }
}
static Boolean contains(XRectangle r,int x,int y) {
    return r.width && r.height && x>=r.x && y>=r.y &&
        x<(int)r.x+r.width && y<(int)r.y+r.height;
}
static unsigned char target(XmChipWidget chip,int x,int y) {
    ChipMetrics m;ChipLayout l;metrics(chip,&m);layout(chip,&m,&l);
    if(!contains(l.face,x,y))return 0;
    return chip->chip.removable && contains(l.remove_hit,x,y)?2:1;
}
static void rounded(Display *d,Drawable w,GC gc,XRectangle r,int inset) {
    _XmAppearanceFillRoundedRectangle(d,w,gc,r.x+inset,r.y+inset,
        (int)r.width-2*inset,(int)r.height-2*inset,MAX(0,MIN(r.width,r.height)/2-inset));
}
static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmChipWidget chip=(XmChipWidget)widget;
    Display *display=XtDisplay(widget);Drawable window=_XmPaintDrawable(widget);
    GC gc=chip->chip.foreground_gc;ChipMetrics m;ChipLayout l;
    Boolean is_modern=modern(widget), sensitive=XtIsSensitive(widget);
    (void)event;(void)region;
    if(!window || !gc)return;
    metrics(chip,&m);layout(chip,&m,&l);
    XSetClipMask(display,gc,None);
    Pixel ink=sensitive?chip->primitive.foreground:chip->primitive.bottom_shadow_color;
    Pixel fill=chip->core.background_pixel;
    Boolean pressed=sensitive && chip->chip.armed_target &&
        chip->chip.armed_target==chip->chip.hot_target;
    if(is_modern && sensitive && chip->chip.hot_target==1)
        fill=chip->primitive.top_shadow_color;
    XSetBackground(display,gc,fill);
    if(chip->primitive.highlighted && chip->primitive.highlight_thickness) {
        XRectangle outer=rectangle(0,0,chip->core.width,chip->core.height);
        XSetForeground(display,gc,chip->primitive.highlight_color);rounded(display,window,gc,outer,0);
        XSetForeground(display,gc,chip->core.background_pixel);
        rounded(display,window,gc,outer,chip->primitive.highlight_thickness);
    }
    XSetForeground(display,gc,is_modern?(pressed?chip->primitive.highlight_color:
                      chip->primitive.bottom_shadow_color):ink);
    rounded(display,window,gc,l.face,0);
    XSetForeground(display,gc,fill);rounded(display,window,gc,l.face,1);
    if(is_modern && sensitive && chip->chip.hot_target==2) {
        XSetForeground(display,gc,chip->primitive.top_shadow_color);rounded(display,window,gc,l.remove,0);
    }
    XSetForeground(display,gc,ink);
    if(l.icon.width && l.icon.height) {
        XSetClipRectangles(display,gc,0,0,&l.icon,1,Unsorted);
        if(m.icon_depth==1)
            XCopyPlane(display,chip->chip.pixmap,window,gc,0,0,l.icon.width,l.icon.height,l.icon.x,l.icon.y,1);
        else if(m.icon_depth==chip->core.depth)
            XCopyArea(display,chip->chip.pixmap,window,gc,0,0,l.icon.width,l.icon.height,l.icon.x,l.icon.y);
        XSetClipMask(display,gc,None);
    }
    if(chip->chip.label && chip->chip.font && l.text.width && l.text.height) {
        XmStringDraw(display, window, chip->chip.font,chip->chip.label,gc,l.text.x,
            ((int)chip->core.height-m.text_height)/2,l.text.width,XmALIGNMENT_BEGINNING,
            XmPrim_layout_direction(chip),&l.text);
        XSetClipMask(display,gc,None);
    }
    if(l.remove.width && l.remove.height) {
        int extent=MIN(_XmDpiDefaultDimension(widget,8),MIN(l.remove.width,l.remove.height));
        int x=l.remove.x+(l.remove.width-extent)/2,y=l.remove.y+(l.remove.height-extent)/2;
        if(extent>0) {
            XDrawLine(display,window,gc,x,y,x+extent-1,y+extent-1);
            XDrawLine(display,window,gc,x+extent-1,y,x,y+extent-1);
        }
    }
}

static void BufferedRedisplay(Widget widget, XEvent *event, Region region)
{
    _XmBufferedPaint(widget, Redisplay, event, region);
}
static void Highlight(Widget widget) {
    XmChipWidget chip=(XmChipWidget)widget;
    chip->primitive.highlighted=True;chip->primitive.highlight_drawn=True;
    BufferedRedisplay(widget,NULL,NULL);
}
static void Unhighlight(Widget widget) {
    XmChipWidget chip=(XmChipWidget)widget;
    chip->primitive.highlighted=False;chip->primitive.highlight_drawn=False;
    BufferedRedisplay(widget,NULL,NULL);
}

static void emit(XmChipWidget chip, String resource, int reason, XEvent *event) {
    XmChipCallbackStruct callback;
    callback.reason = reason;
    callback.event = event;
    (void)_XmCallCallbacksUntilDestroyed((Widget)chip, resource, &callback);
}

void XmChipInvoke(Widget widget, XEvent *event) {
    if (widget && XmIsChip(widget) && XtIsSensitive(widget) &&
        !widget->core.being_destroyed)
        emit((XmChipWidget)widget, XmNchipActivateCallback, XmCR_ACTIVATE,
             event);
}

void XmChipRemove(Widget widget, XEvent *event) {
    if (widget && XmIsChip(widget) && XtIsSensitive(widget) &&
        !widget->core.being_destroyed && ((XmChipWidget)widget)->chip.removable)
        emit((XmChipWidget)widget, XmNchipRemoveCallback,
             XmCHIP_REASON_REMOVE, event);
}

static void Events(Widget widget, XtPointer data, XEvent *event, Boolean *dispatch) {
    XmChipWidget chip=(XmChipWidget)widget;unsigned char hit=0;
    (void)data;(void)dispatch;
    if (widget == NULL || event == NULL || !XmIsChip(widget)) return;
    if(widget->core.being_destroyed || !XtIsSensitive(widget))return;
    switch(event->type) {
    case ButtonPress:
        if(event->xbutton.button!=Button1)return;
        chip->chip.armed_target=chip->chip.hot_target=target(chip,event->xbutton.x,event->xbutton.y);
        if(chip->chip.armed_target)XmProcessTraversal(widget,XmTRAVERSE_CURRENT);
        break;
    case ButtonRelease:
        if(event->xbutton.button!=Button1)return;
        hit=target(chip,event->xbutton.x,event->xbutton.y);
        unsigned char activate=chip->chip.armed_target;
        chip->chip.armed_target=0;chip->chip.hot_target=hit;
        BufferedRedisplay(widget,NULL,NULL);
        /* Clear all interaction state before callbacks, which may destroy us. */
        if(activate && activate==hit) {
            if(activate==2)XmChipRemove(widget,event);else XmChipInvoke(widget,event);
        }
        return;
    case MotionNotify: chip->chip.hot_target=target(chip,event->xmotion.x,event->xmotion.y);break;
    case EnterNotify: chip->chip.hot_target=target(chip,event->xcrossing.x,event->xcrossing.y);break;
    case LeaveNotify: chip->chip.hot_target=0;break;
    case KeyPress: {
        KeySym key;char input[8];XLookupString(&event->xkey,input,sizeof(input),&key,NULL);
        if(dispatch && (key==XK_space || key==XK_Return || key==XK_KP_Enter ||
            (chip->chip.removable && (key==XK_Delete || key==XK_BackSpace)))) *dispatch=False;
        if(key==XK_space || key==XK_Return || key==XK_KP_Enter)XmChipInvoke(widget,event);
        else if(chip->chip.removable && (key==XK_Delete || key==XK_BackSpace))XmChipRemove(widget,event);
        return;
    }
    default:return;
    }
    BufferedRedisplay(widget,NULL,NULL);
}

static Boolean SetValues(Widget old_widget, Widget request, Widget widget,
                         ArgList args, Cardinal *count) {
    XmChipWidget old = (XmChipWidget)old_widget;
    XmChipWidget chip = (XmChipWidget)widget;
    Boolean redraw = False;
    Boolean colors_changed = old->primitive.foreground != chip->primitive.foreground ||
                             old->core.background_pixel != chip->core.background_pixel;
    (void)request; (void)args; (void)count;
    if (colors_changed) {
        if (chip->chip.foreground_gc) {
            XSetForeground(XtDisplay(widget), chip->chip.foreground_gc,
                           chip->primitive.foreground);
            XSetBackground(XtDisplay(widget), chip->chip.foreground_gc,
                           chip->core.background_pixel);
        }
        redraw = True;
    }
    if (old->chip.font != chip->chip.font) {
        XmFontList new_font = chip->chip.font ? XmFontListCopy(chip->chip.font) :
            XmFontListCopy(XmeGetDefaultRenderTable(widget, XmLABEL_FONTLIST));
        if (new_font) {
            if (old->chip.font) XmRenderTableFree(old->chip.font);
            chip->chip.font = new_font;
            redraw = True;
        } else {
            chip->chip.font = old->chip.font;
        }
    }
    if (old->chip.label != chip->chip.label) {
        XmString replacement = copy_label(chip->chip.label);
        if (replacement) {
            if (old->chip.label) XmStringFree(old->chip.label);
            chip->chip.label = replacement;
            redraw = True;
        } else {
            chip->chip.label = old->chip.label;
        }
    }
    if (old->chip.pixmap != chip->chip.pixmap ||
        old->chip.removable != chip->chip.removable ||
        old->primitive.highlighted != chip->primitive.highlighted)
        redraw = True;
    if(old->chip.removable!=chip->chip.removable || !XtIsSensitive(widget))
        chip->chip.armed_target=chip->chip.hot_target=0;
    for(Cardinal i=0;i<*count;++i) {
        if(!strcmp(args[i].name,XmNwidth))chip->chip.automatic_width=(Dimension)args[i].value==0;
        if(!strcmp(args[i].name,XmNheight))chip->chip.automatic_height=(Dimension)args[i].value==0;
    }
    Dimension width,height;natural_size(chip,&width,&height);
    if(chip->chip.automatic_width)chip->core.width=width;
    if(chip->chip.automatic_height)chip->core.height=height;
    return redraw || old->primitive.foreground!=chip->primitive.foreground ||
        old->core.background_pixel!=chip->core.background_pixel ||
        old->primitive.bottom_shadow_color!=chip->primitive.bottom_shadow_color ||
        old->primitive.top_shadow_color!=chip->primitive.top_shadow_color ||
        old->primitive.highlight_color!=chip->primitive.highlight_color ||
        XmPrim_layout_direction(old)!=XmPrim_layout_direction(chip) ||
        XtIsSensitive(old_widget)!=XtIsSensitive(widget);
}

static XtGeometryResult QueryGeometry(Widget widget, XtWidgetGeometry *request,
                                      XtWidgetGeometry *reply) {
    XmChipWidget chip = (XmChipWidget)widget;
    reply->request_mode=CWWidth|CWHeight;
    natural_size(chip,&reply->width,&reply->height);
    if(!chip->chip.automatic_width)reply->width=chip->core.width;
    if(!chip->chip.automatic_height)reply->height=chip->core.height;
    return XmeReplyToQueryGeometry(widget,request,reply);
}

Widget XmCreateChip(Widget parent, char *name, ArgList args, Cardinal count) {
    return XtCreateWidget(name, xmChipWidgetClass, parent, args, count);
}

Widget XmVaCreateChip(Widget parent, char *name, ...) {
    Widget widget;
    va_list var;
    int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name);
    widget = XmeVLCreateWidget(name, xmChipWidgetClass, parent, False, var,
                               count);
    va_end(var);
    return widget;
}

Widget XmVaCreateManagedChip(Widget parent, char *name, ...) {
    Widget widget;
    va_list var;
    int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name);
    widget = XmeVLCreateWidget(name, xmChipWidgetClass, parent, True, var,
                               count);
    va_end(var);
    return widget;
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
