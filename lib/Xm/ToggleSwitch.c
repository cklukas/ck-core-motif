/*
 * Additive binary switch widget.
 *
 * This is deliberately a small XmPrimitive subclass.  Existing ToggleButton
 * widgets and their resource semantics are untouched; applications opt in by
 * creating XmToggleSwitch widgets.
 */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "PaintI.h"

#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <Xm/ToggleSwitchP.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include <limits.h>
#include "PrimitiveI.h"
#include "TravActI.h"
#include "XmI.h"
#include "AppearanceDrawI.h"
#include <limits.h>
#include <math.h>
#include <string.h>
#include <Xm/Appearance.h>
#include "CallbackI.h"

static void ScaleChanged(Widget, double, double, XtPointer);
static void Initialize(Widget, Widget, ArgList, Cardinal *);
static void BufferedRedisplay(Widget, XEvent *, Region);
static Boolean BufferedSetValues(Widget, Widget, Widget, ArgList, Cardinal *);
static void Destroy(Widget);
static void Toggle(Widget, XEvent *, String *, Cardinal *);
static void Arm(Widget, XEvent *, String *, Cardinal *);
static void Release(Widget, XEvent *, String *, Cardinal *);
static void Enter(Widget, XEvent *, String *, Cardinal *);
static void Leave(Widget, XEvent *, String *, Cardinal *);
static void Highlight(Widget);
static void Unhighlight(Widget);
static XtGeometryResult QueryGeometry(Widget, XtWidgetGeometry *,
                                      XtWidgetGeometry *);

static char translations[] =
    "<Key>space: ToggleSwitch()\n"
    "<Key>Return: ToggleSwitch()\n"
    "<Btn1Down>: SwitchArm()\n"
    "<Btn1Up>: SwitchRelease()\n"
    "<EnterWindow>: SwitchEnter()\n"
    "<LeaveWindow>: SwitchLeave()";

static XtActionsRec actions[] = {
    {"ToggleSwitch", Toggle},
    {"SwitchArm", Arm}, {"SwitchRelease", Release},
    {"SwitchEnter", Enter}, {"SwitchLeave", Leave},
    {"ButtonTakeFocus", _XmButtonTakeFocus}
};

#define TOGGLE_SWITCH_TRACK_WIDTH 44
#define TOGGLE_SWITCH_TRACK_HEIGHT 24
#define TOGGLE_SWITCH_KNOB_DIAMETER 18

static void CheckSetRenderTable(Widget w, int offset, XrmValue *value) {
    XmToggleSwitchWidget sw=(XmToggleSwitchWidget)w;(void)offset;
    if(sw->toggle_switch.check_set_render_table)value->addr=NULL;
    else { sw->toggle_switch.check_set_render_table=True;
           value->addr=(char *)&sw->toggle_switch.font; }
}
#define Offset(field) XtOffsetOf(XmToggleSwitchRec, field)
static XtResource resources[] = {
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmFontList),
     Offset(toggle_switch.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmRenderTable),
     Offset(toggle_switch.font), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNset, XmCSet, XmRBoolean, sizeof(Boolean),
     Offset(toggle_switch.set), XmRImmediate, (XtPointer)False},
    {XmNlabelString, XmCXmString, XmRXmString, sizeof(XmString),
     Offset(toggle_switch.label), XmRImmediate, (XtPointer)NULL},
    {XmNvalueChangedCallback, XmCValueChangedCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(toggle_switch.value_changed_callback),
     XmRPointer, (XtPointer)NULL},
    {XmNspacing, XmCSpacing, XmRHorizontalDimension, sizeof(Dimension),
     Offset(toggle_switch.spacing), XmRImmediate, (XtPointer)8},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(primitive.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNhighlightThickness, XmCHighlightThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(primitive.highlight_thickness), XmRCallProc,
     (XtPointer)_XmSetThickness}
};

externaldef(xmtoggleswitchclassrec) XmToggleSwitchClassRec
    xmToggleSwitchClassRec = {
    {
        (WidgetClass)&xmPrimitiveClassRec, "XmToggleSwitch",
        sizeof(XmToggleSwitchRec), NULL, NULL, False, Initialize, NULL,
        XtInheritRealize, actions, XtNumber(actions), resources,
        XtNumber(resources), NULLQUARK, True, XtExposeCompressMaximal, True,
        False, Destroy, NULL, BufferedRedisplay, BufferedSetValues, NULL,
        XtInheritSetValuesAlmost, NULL, NULL, XtVersion, NULL, translations,
        QueryGeometry, NULL, NULL
    },
    {
        Highlight, Unhighlight,
        XtInheritTranslations, NULL, NULL, 0, NULL
    },
    {NULL}
};

externaldef(xmtoggleswitchwidgetclass) WidgetClass xmToggleSwitchWidgetClass =
    (WidgetClass)&xmToggleSwitchClassRec;

typedef struct {
    Dimension width,height,inset,track_width,track_height,knob;
    Dimension text_width,text_height;
    Boolean rtl,modern;
} SwitchMetrics;

static Dimension bounded(unsigned long value) {
    return (Dimension)MIN(USHRT_MAX,MAX(1UL,value));
}
static short bounded_position(int value) {
    if(value < SHRT_MIN)return SHRT_MIN;
    if(value > SHRT_MAX)return SHRT_MAX;
    return (short)value;
}
static unsigned short bounded_extent(int value) {
    if(value <= 0)return 0;
    if(value > USHRT_MAX)return USHRT_MAX;
    return (unsigned short)value;
}
static XRectangle clip_rectangle(int left, int top, int right, int bottom) {
    XRectangle rectangle;
    rectangle.x=bounded_position(left);
    rectangle.y=bounded_position(top);
    rectangle.width=bounded_extent(right-left);
    rectangle.height=bounded_extent(bottom-top);
    return rectangle;
}
static void measure(XmToggleSwitchWidget sw, SwitchMetrics *m) {
    Widget w=(Widget)sw;
    XmAppearanceProfile profile=_XmAppearanceGeometryProfile(XtDisplay(w));
    Boolean compact=_XmAppearanceGeometryDensity(XtDisplay(w))==XmAPPEARANCE_DENSITY_COMPACT;
    unsigned long frame=(unsigned long)sw->primitive.highlight_thickness+sw->primitive.shadow_thickness;
    memset(m,0,sizeof(*m));
    m->modern=profile==XmAPPEARANCE_MODERN_LIGHT || profile==XmAPPEARANCE_MODERN_DARK;
    m->rtl=XmDirectionMatchPartial(XmPrim_layout_direction(sw),XmRIGHT_TO_LEFT,XmHORIZONTAL_MASK);
    m->track_width=_XmDpiDefaultDimension(w,TOGGLE_SWITCH_TRACK_WIDTH);
    m->track_height=_XmDpiDefaultDimension(w,TOGGLE_SWITCH_TRACK_HEIGHT);
    m->knob=_XmDpiDefaultDimension(w,TOGGLE_SWITCH_KNOB_DIAMETER);
    if(m->knob>m->track_height)m->knob=m->track_height;
    /* Equal parity gives an integral center and identical opposite margins. */
    if ((m->track_height - m->knob) & 1) ++m->knob;
    m->inset=(Dimension)MIN(USHRT_MAX,MAX(frame,(unsigned long)_XmDpiDefaultDimension(w,compact?2:4)));
    if(sw->toggle_switch.font && sw->toggle_switch.label)
        XmStringExtent(sw->toggle_switch.font,sw->toggle_switch.label,&m->text_width,&m->text_height);
    m->width=bounded(2UL*m->inset+m->track_width+m->text_width+
                    (m->text_width?sw->toggle_switch.spacing:0));
    m->height=bounded(2UL*m->inset+MAX(m->track_height,m->text_height));
    if(m->modern)m->height=MAX(m->height,_XmDpiDefaultDimension(w,compact?28:34));
}
static void size_automatic(XmToggleSwitchWidget sw) {
    SwitchMetrics m;measure(sw,&m);
    if(sw->toggle_switch.automatic_width)sw->core.width=m.width;
    if(sw->toggle_switch.automatic_height)sw->core.height=m.height;
}
static void redraw(Widget w) {
    if(XtIsRealized(w))_XmPaintInvalidate(w);
}
static void fill_round_rect(Display *display, Window window, GC gc,
                            int x, int y, unsigned int width,
                            unsigned int height) {
    int width_int,height_int,radius;
    if(!gc || !width || !height || width > INT_MAX || height > INT_MAX)return;
    width_int=(int)width;height_int=(int)height;
    radius=MIN(width_int,height_int)/2;
    _XmAppearanceFillRoundedRectangle(display,window,gc,x,y,width_int,height_int,radius);
}
static void Initialize(Widget request, Widget widget, ArgList args, Cardinal *count) {
    XmToggleSwitchWidget sw=(XmToggleSwitchWidget)widget;
    XGCValues values;XmString copy;(void)args;(void)count;
    copy=sw->toggle_switch.label?XmStringCopy(sw->toggle_switch.label):NULL;
    sw->toggle_switch.label=copy?copy:XmStringCreateLocalized("");
    sw->toggle_switch.font=XmFontListCopy(sw->toggle_switch.font?sw->toggle_switch.font:
                                         XmeGetDefaultRenderTable(widget,XmBUTTON_FONTLIST));
    sw->toggle_switch.automatic_width=request->core.width==0;
    sw->toggle_switch.automatic_height=request->core.height==0;
    sw->toggle_switch.armed=False;sw->toggle_switch.hover=False;
    values.foreground=sw->primitive.foreground;values.background=sw->core.background_pixel;
    values.graphics_exposures=False;
    sw->toggle_switch.track_gc=XCreateGC(XtDisplay(widget),DefaultRootWindow(XtDisplay(widget)),
        GCForeground|GCBackground|GCGraphicsExposures,&values);
    size_automatic(sw);
    (void)_XmRegisterScaleChange(widget, ScaleChanged, NULL);
}
static void ScaleChanged(Widget widget, double old_scale, double new_scale, XtPointer closure) {
    (void)closure;
    if (old_scale == new_scale || widget->core.being_destroyed) return;
    /* Recompute natural axes through Xt geometry negotiation, preserving
     * explicit resource ownership. No widget access after callbacks. */
    XtSetValues(widget, NULL, 0);
}
static void Destroy(Widget widget) {
    XmToggleSwitchWidget sw=(XmToggleSwitchWidget)widget;
    _XmUnregisterScaleChange(widget, ScaleChanged, NULL);
    if(sw->toggle_switch.track_gc)XFreeGC(XtDisplay(widget),sw->toggle_switch.track_gc);
    if(sw->toggle_switch.label)XmStringFree(sw->toggle_switch.label);
    if(sw->toggle_switch.font)XmFontListFree(sw->toggle_switch.font);
}
static double linear_channel(unsigned short value) {
    double channel = value / 65535.0;
    return channel <= 0.04045 ? channel / 12.92 :
        pow((channel + 0.055) / 1.055, 2.4);
}
static double luminance(const XColor *color) {
    return 0.2126 * linear_channel(color->red) +
           0.7152 * linear_channel(color->green) +
           0.0722 * linear_channel(color->blue);
}
static Pixel contrast(XmToggleSwitchWidget sw, Pixel background) {
    /* Reuse application-owned colors: screen black/white pixel IDs are not
     * portable across colormaps, and expose must not allocate color cells. */
    XColor colors[5] = {{0}};
    double best = 0.0, backdrop;
    Pixel result = sw->primitive.foreground;
    colors[0].pixel = background;
    colors[1].pixel = result;
    colors[2].pixel = sw->core.background_pixel;
    colors[3].pixel = sw->primitive.top_shadow_color;
    colors[4].pixel = sw->primitive.bottom_shadow_color;
    if (!XQueryColors(XtDisplay((Widget)sw), sw->core.colormap, colors, 5))
        return result;
    backdrop = luminance(&colors[0]);
    for (unsigned i = 1; i < 5; ++i) {
        double candidate = luminance(&colors[i]);
        double ratio = (MAX(candidate, backdrop) + 0.05) /
                       (MIN(candidate, backdrop) + 0.05);
        if (ratio > best) { best = ratio; result = colors[i].pixel; }
    }
    return result;
}
static void Redisplay(Widget widget, XEvent *event, Region region) {
    XmToggleSwitchWidget sw=(XmToggleSwitchWidget)widget;SwitchMetrics m;
    Display *d=XtDisplay(widget);Drawable window=_XmPaintDrawable(widget);GC gc=sw->toggle_switch.track_gc;
    int left,right,top,bottom,tx,ty,kx,ky,knob,border;XRectangle clip;
    Pixel edge,fill,thumb;Boolean sensitive=XtIsSensitive(widget);
    (void)event;(void)region;if(!window || !gc)return;
    measure(sw,&m);
    left=top=m.inset;right=(int)sw->core.width-m.inset;bottom=(int)sw->core.height-m.inset;
    if(right>left && bottom>top) {
        clip=clip_rectangle(left,top,right,bottom);XSetClipRectangles(d,gc,0,0,&clip,1,Unsorted);
        tx=m.rtl?right-m.track_width:left;ty=((int)sw->core.height-m.track_height)/2;
        edge=sensitive?(m.modern?sw->primitive.bottom_shadow_color:sw->primitive.foreground):sw->primitive.bottom_shadow_color;
        if(sensitive && (sw->toggle_switch.hover || sw->toggle_switch.set))edge=sw->primitive.highlight_color;
        fill=m.modern && sensitive && sw->toggle_switch.set?sw->primitive.highlight_color:sw->core.background_pixel;
        thumb=!sensitive?sw->primitive.bottom_shadow_color:
            (m.modern && sw->toggle_switch.set?contrast(sw,fill):sw->primitive.foreground);
        border=MIN((int)_XmDpiDefaultDimension(widget,1),m.track_height/2);
        XSetForeground(d,gc,edge);fill_round_rect(d,window,gc,tx,ty,m.track_width,m.track_height);
        XSetForeground(d,gc,fill);
        if(m.track_width>2*border && m.track_height>2*border)
            fill_round_rect(d,window,gc,tx+border,ty+border,
                            (unsigned int)(m.track_width-2*border),
                            (unsigned int)(m.track_height-2*border));
        knob=(int)m.knob;
        if(sensitive && sw->toggle_switch.armed && sw->toggle_switch.hover)
            knob=MIN(m.track_height-2*border,knob+(int)_XmDpiDefaultDimension(widget,2));
        if ((m.track_height-knob) & 1) ++knob;
        kx=tx+(m.track_height-knob)/2;
        if(sw->toggle_switch.set != m.rtl)kx+=m.track_width-m.track_height;
        ky=ty+((int)m.track_height-knob)/2;
        XSetForeground(d,gc,thumb);fill_round_rect(d,window,gc,kx,ky,
                                                    (unsigned int)knob,(unsigned int)knob);
        if(m.rtl)right=tx-sw->toggle_switch.spacing;
        else left=tx+m.track_width+sw->toggle_switch.spacing;
        XSetClipMask(d,gc,None);
        if(right>left && sw->toggle_switch.font && sw->toggle_switch.label) {
            clip=clip_rectangle(left,top,right,bottom);
            XSetForeground(d,gc,sensitive?sw->primitive.foreground:sw->primitive.bottom_shadow_color);
            XmStringDraw(d,window,sw->toggle_switch.font,sw->toggle_switch.label,gc,left,
                ((int)sw->core.height-m.text_height)/2,clip.width,XmALIGNMENT_BEGINNING,
                XmPrim_layout_direction(sw),&clip);
        }
    }
    if(!_XmAppearanceDrawButtonFocus(widget,sw->primitive.highlighted) && sw->primitive.highlighted &&
       sw->core.width>2*sw->primitive.highlight_thickness && sw->core.height>2*sw->primitive.highlight_thickness)
        xmPrimitiveClassRec.primitive_class.border_highlight(widget);
}

static void BufferedRedisplay(Widget widget, XEvent *event, Region region)
{
    _XmBufferedPaint(widget, Redisplay, event, region);
}
static void Highlight(Widget w) {
    ((XmToggleSwitchWidget)w)->primitive.highlighted=True;
    ((XmToggleSwitchWidget)w)->primitive.highlight_drawn=True;
    BufferedRedisplay(w,NULL,NULL);
}
static void Unhighlight(Widget w) {
    ((XmToggleSwitchWidget)w)->primitive.highlighted=False;
    ((XmToggleSwitchWidget)w)->primitive.highlight_drawn=False;
    BufferedRedisplay(w,NULL,NULL);
}
static Boolean SetValues(Widget current,Widget request,Widget widget,ArgList args,Cardinal *count) {
    XmToggleSwitchWidget old=(XmToggleSwitchWidget)current,sw=(XmToggleSwitchWidget)widget;
    (void)request;
    if(old->toggle_switch.font!=sw->toggle_switch.font) {
        XmFontList font=XmFontListCopy(sw->toggle_switch.font?sw->toggle_switch.font:
                                      XmeGetDefaultRenderTable(widget,XmBUTTON_FONTLIST));
        if(font) { if(old->toggle_switch.font)XmFontListFree(old->toggle_switch.font);sw->toggle_switch.font=font; }
        else sw->toggle_switch.font=old->toggle_switch.font;
    }
    if(old->toggle_switch.label!=sw->toggle_switch.label) {
        XmString label=sw->toggle_switch.label?XmStringCopy(sw->toggle_switch.label):XmStringCreateLocalized("");
        if(label) {if(old->toggle_switch.label)XmStringFree(old->toggle_switch.label);sw->toggle_switch.label=label;}
        else sw->toggle_switch.label=old->toggle_switch.label;
    }
    for(Cardinal i=0;i<*count;++i) {
        if(!strcmp(args[i].name,XmNwidth))sw->toggle_switch.automatic_width=(Dimension)args[i].value==0;
        if(!strcmp(args[i].name,XmNheight))sw->toggle_switch.automatic_height=(Dimension)args[i].value==0;
    }
    if(!XtIsSensitive(widget)) {sw->toggle_switch.armed=False;sw->toggle_switch.hover=False;}
    size_automatic(sw);return True;
}
static XtGeometryResult QueryGeometry(Widget widget,XtWidgetGeometry *request,XtWidgetGeometry *reply) {
    XmToggleSwitchWidget sw=(XmToggleSwitchWidget)widget;SwitchMetrics m;measure(sw,&m);
    reply->request_mode=CWWidth|CWHeight;
    reply->width=sw->toggle_switch.automatic_width?m.width:sw->core.width;
    reply->height=sw->toggle_switch.automatic_height?m.height:sw->core.height;
    return XmeReplyToQueryGeometry(widget,request,reply);
}
static Boolean inside(Widget w,XEvent *event) {
    return event && event->xbutton.x>=0 && event->xbutton.y>=0 &&
        event->xbutton.x<(int)w->core.width && event->xbutton.y<(int)w->core.height;
}
static void set_state(Widget widget,Boolean state,Boolean notify,XEvent *event) {
    XmToggleSwitchWidget sw=(XmToggleSwitchWidget)widget;
    XmToggleSwitchCallbackStruct callback;Boolean changed;
    if(widget->core.being_destroyed)return;
    state=state?True:False;changed=sw->toggle_switch.set!=state;
    sw->toggle_switch.set=state;redraw(widget);
    if(!notify || !changed || widget->core.being_destroyed)return;
    callback.reason=XmCR_VALUE_CHANGED;callback.event=event;callback.set=state;
    (void)_XmCallCallbacksUntilDestroyed(widget,XmNvalueChangedCallback,&callback);
}
static void Toggle(Widget w,XEvent *event,String *params,Cardinal *count) {
    (void)params;(void)count;if(!XtIsSensitive(w) || w->core.being_destroyed)return;
    set_state(w,!((XmToggleSwitchWidget)w)->toggle_switch.set,True,event);
}
static void Arm(Widget w,XEvent *event,String *params,Cardinal *count) {
    XmToggleSwitchWidget sw=(XmToggleSwitchWidget)w;(void)params;(void)count;
    if(!XtIsSensitive(w) || w->core.being_destroyed || !inside(w,event))return;
    sw->toggle_switch.armed=True;sw->toggle_switch.hover=True;redraw(w);
    XmProcessTraversal(w,XmTRAVERSE_CURRENT);
}
static void Release(Widget w,XEvent *event,String *params,Cardinal *count) {
    XmToggleSwitchWidget sw=(XmToggleSwitchWidget)w;Boolean activate;
    (void)params;(void)count;
    if(w->core.being_destroyed)return;
    activate=sw->toggle_switch.armed && XtIsSensitive(w) && inside(w,event);
    sw->toggle_switch.armed=False;redraw(w);
    if(activate)set_state(w,!sw->toggle_switch.set,True,event);
}
static void Enter(Widget w,XEvent *event,String *params,Cardinal *count) {
    (void)event;(void)params;(void)count;
    if(w->core.being_destroyed)return;
    ((XmToggleSwitchWidget)w)->toggle_switch.hover=XtIsSensitive(w);redraw(w);
}
static void Leave(Widget w,XEvent *event,String *params,Cardinal *count) {
    (void)event;(void)params;(void)count;
    if(w->core.being_destroyed)return;
    ((XmToggleSwitchWidget)w)->toggle_switch.hover=False;redraw(w);
}
Boolean XmToggleSwitchGetState(Widget widget) {
    if(widget == NULL || !XmIsToggleSwitch(widget) || widget->core.being_destroyed)return False;
    return ((XmToggleSwitchWidget)widget)->toggle_switch.set;
}
void XmToggleSwitchSetState(Widget widget,Boolean state,Boolean notify) {
    if(widget == NULL || !XmIsToggleSwitch(widget) || widget->core.being_destroyed)return;
    set_state(widget,state,notify,NULL);
}

Widget XmCreateToggleSwitch(Widget parent, char *name, ArgList args,
                            Cardinal arg_count) {
    return XtCreateWidget(name, xmToggleSwitchWidgetClass, parent, args,
                          arg_count);
}

Widget XmVaCreateToggleSwitch(Widget parent, char *name, ...) {
    Widget widget;
    va_list var;
    int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name);
    widget = XmeVLCreateWidget(name, xmToggleSwitchWidgetClass, parent, False,
                               var, count);
    va_end(var);
    return widget;
}

Widget XmVaCreateManagedToggleSwitch(Widget parent, char *name, ...) {
    Widget widget;
    va_list var;
    int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name);
    widget = XmeVLCreateWidget(name, xmToggleSwitchWidgetClass, parent, True,
                               var, count);
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
