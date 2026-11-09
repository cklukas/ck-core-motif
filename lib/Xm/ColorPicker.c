/* Native RGBA editor with explicit preview, commit and revert semantics. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <stdlib.h>
#include <X11/Xutil.h>
#include <Xm/SegmentedControl.h>
#include <Xm/DrawnB.h>
#include "ColorSpaceI.h"
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/ColorPickerP.h>
#include <Xm/TextF.h>
#include <Xm/Scale.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/DrawingA.h>
#include <Xm/Appearance.h>
#include <Xm/XmP.h>
#include "XmI.h"
#include "AppearanceDrawI.h"
#include "CallbackI.h"
static void Initialize(Widget,Widget,ArgList,Cardinal *);
static void Destroy(Widget);
static void Realize(Widget,XtValueMask *,XSetWindowAttributes *);
static void Resize(Widget);
static void ChangeManaged(Widget);
static Boolean SetValues(Widget,Widget,Widget,ArgList,Cardinal *);
static XtGeometryResult GeometryManager(Widget,XtWidgetGeometry *,XtWidgetGeometry *);
static XtGeometryResult QueryGeometry(Widget,XtWidgetGeometry *,XtWidgetGeometry *);
static void Events(Widget,XtPointer,XEvent *,Boolean *);
static void schedule_layout(XmColorPickerWidget);
static Boolean refresh(XmColorPickerWidget,Boolean);
static void update_modes(XmColorPickerWidget);
static void paint_plane(Widget,XtPointer,XtPointer);
static void plane_events(Widget,XtPointer,XEvent *,Boolean *);
static void mode_changed(Widget,XtPointer,XtPointer);
static void model_changed(Widget,XtPointer,XtPointer);
static void pencil_selected(Widget,XtPointer,XtPointer);
static void paint_pencil(Widget,XtPointer,XtPointer);
static void release_palette(XmColorPickerWidget);
static const char *pencil_names[24]={"Black","Graphite","Gray","Silver","White","Rose","Red","Orange","Amber","Yellow","Lime","Green","Mint","Teal","Cyan","Sky","Blue","Indigo","Violet","Purple","Magenta","Pink","Brown","Sand"};
static const uint32_t pencil_colors[24]={0x17191cff,0x444950ff,0x737b85ff,0xb7c0caff,0xffffffff,0xb74058ff,0xe33b37ff,0xf17c32ff,0xe9ae27ff,0xf3d843ff,0x9cc83aff,0x3b9d4aff,0x6ac5a4ff,0x248c89ff,0x36c3d8ff,0x5aa5e2ff,0x3478d4ff,0x4e56a9ff,0x7953bcff,0x98549fff,0xcb4b9fff,0xe798b5ff,0x8b6049ff,0xd3b38aff};
/* XPoint stores coordinates in signed 16-bit protocol fields. */
static short bounded_xpoint_coordinate(int value) {
    return value < SHRT_MIN ? SHRT_MIN :
           value > SHRT_MAX ? SHRT_MAX : (short)value;
}
static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmColorPickerWidget picker = (XmColorPickerWidget)widget;
    (void)offset;
    if (picker->color_picker.check_set_render_table)
        value->addr = NULL;
    else {
        picker->color_picker.check_set_render_table = True;
        value->addr = (char *)&picker->color_picker.render_table;
    }
}
#define Offset(field) XtOffsetOf(XmColorPickerRec, field)
static XtResource resources[] = {
    {XmNcolorPickerValue, XmCColorPickerValue, XmRCardinal,
     sizeof(uint32_t), Offset(color_picker.value), XmRImmediate,
     (XtPointer)0x336699ffUL},
    {XmNcolorPickerPreviewCallback, XmCColorPickerPreviewCallback,
     XmRCallback, sizeof(XtCallbackList), Offset(color_picker.preview_callback),
     XmRPointer, (XtPointer)NULL},
    {XmNcolorPickerValueChangedCallback, XmCColorPickerValueChangedCallback,
     XmRCallback, sizeof(XtCallbackList), Offset(color_picker.value_changed_callback),
     XmRPointer, (XtPointer)NULL},
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmRenderTable),
     Offset(color_picker.render_table), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmRenderTable),
     Offset(color_picker.render_table), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(manager.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(manager.shadow_thickness), XmRImmediate, (XtPointer)0},
    {XmNspacing, XmCSpacing, XmRHorizontalDimension, sizeof(Dimension),
     Offset(color_picker.spacing), XmRImmediate, (XtPointer)8}
};
externaldef(xmcolorpickerclassrec) XmColorPickerClassRec xmColorPickerClassRec = {
    {.superclass=(WidgetClass)&xmManagerClassRec, .class_name="XmColorPicker",
     .widget_size=sizeof(XmColorPickerRec), .initialize=Initialize,
     .realize=Realize, .resources=resources, .num_resources=XtNumber(resources),
     .compress_motion=True, .compress_exposure=XtExposeCompressMultiple,
     .compress_enterleave=True, .destroy=Destroy, .resize=Resize,
     .set_values=SetValues, .set_values_almost=XtInheritSetValuesAlmost,
     .version=XtVersion, .tm_table=XtInheritTranslations, .query_geometry=QueryGeometry},
    {.geometry_manager=GeometryManager, .change_managed=ChangeManaged,
     .insert_child=XtInheritInsertChild, .delete_child=XtInheritDeleteChild},
    {.constraint_size=sizeof(XmManagerConstraintRec)},
    {.translations=XtInheritTranslations, .parent_process=XmInheritParentProcess}, {NULL}
};
externaldef(xmcolorpickerwidgetclass) WidgetClass xmColorPickerWidgetClass = (WidgetClass)&xmColorPickerClassRec;
static Boolean parse(const char *text,uint32_t *value) {
    uint32_t result=0;size_t length;
    if(!text)return False;
    if(*text=='#')++text;
    length=strlen(text);if(length!=6 && length!=8)return False;
    for(size_t i=0;i<length;++i) {
        unsigned char c=(unsigned char)text[i];unsigned digit;
        if(c>='0' && c<='9')digit=c-'0';
        else if(c>='a' && c<='f')digit=c-'a'+10;
        else if(c>='A' && c<='F')digit=c-'A'+10;
        else return False;
        result=(result<<4)|digit;
    }
    *value=length==6?(result<<8)|255:result;return True;
}
static void notify(XmColorPickerWidget picker,String resource,XEvent *event,uint32_t value) {
    if (picker == NULL || picker->core.being_destroyed) return;
    XmColorPickerCallbackStruct callback={XmCR_VALUE_CHANGED,event,(unsigned long)value};
    (void)_XmCallCallbacksUntilDestroyed((Widget)picker,resource,&callback);
}
static Dimension dimension(unsigned long value){return (Dimension)MAX(1,MIN(value,SHRT_MAX));}
static void preferred(Widget child,Dimension *width,Dimension *height) {
    XtWidgetGeometry reply={0};XtQueryGeometry(child,NULL,&reply);
    *width=(reply.request_mode&CWWidth)?reply.width:child->core.width;
    *height=(reply.request_mode&CWHeight)?reply.height:child->core.height;
}
typedef struct {Dimension width,height,swatch,header,label_width,rows[4],error,buttons,apply,revert,hex_label,tabs,plane,hint;} Layout;
static void measure(XmColorPickerWidget p,Layout *m) {
    Dimension ew,eh,lw,lh,bw,bh;unsigned long height,width;Dimension gap=p->color_picker.spacing;
    memset(m,0,sizeof(*m));preferred(p->color_picker.editor,&ew,&eh);
    preferred(p->color_picker.hex_label,&lw,&lh);m->hex_label=lh;
    m->swatch=dimension((unsigned long)eh+lh+gap);
    m->header=m->swatch;width=(unsigned long)m->swatch+gap+MAX(ew,lw);height=m->header;
    preferred(p->color_picker.modes,&bw,&bh);m->tabs=bh;width=MAX(width,bw);height+=gap+bh;
    preferred(p->color_picker.mode_hint,&lw,&lh);m->hint=lh;width=MAX(width,lw);height+=gap+lh;
    /* A shared width prevents the field, hex editor and tab strip jumping
     * when switching modes. Measure the actual inherited/overridden font. */
    XmRenderTable hint_font;XtVaGetValues(p->color_picker.mode_hint,XmNrenderTable,&hint_font,NULL);
    XmString widest=XmStringCreateLocalized("Lab D50: a* horizontal, b* vertical; hatch = outside sRGB");
    XmStringExtent(hint_font,widest,&lw,&lh);XmStringFree(widest);width=MAX(width,(unsigned long)lw+2*gap);
    if(p->color_picker.mode!=XmCOLOR_PICKER_RGB){
        m->plane=dimension((unsigned long)m->swatch*2);height+=gap+m->plane;
    }
    unsigned first=p->color_picker.mode==XmCOLOR_PICKER_PENCILS?3:0;
    for(unsigned i=first;i<4;++i) {
        Widget label=i==3||p->color_picker.mode==XmCOLOR_PICKER_RGB?p->color_picker.labels[i]:p->color_picker.mode_labels[i];
        Widget channel=i==3||p->color_picker.mode==XmCOLOR_PICKER_RGB?p->color_picker.channels[i]:p->color_picker.mode_channels[i];
        preferred(label,&lw,&lh);m->label_width=MAX(m->label_width,lw);
        preferred(channel,&bw,&bh);m->rows[i]=MAX(lh,bh);
        width=MAX(width,(unsigned long)m->label_width+gap+bw);height+=gap+m->rows[i];
    }
    if(p->color_picker.invalid) {
        preferred(p->color_picker.error_label,&lw,&lh);m->error=lh;
        width=MAX(width,lw);height+=gap+lh;
    }
    preferred(p->color_picker.apply,&m->apply,&lh);preferred(p->color_picker.revert,&m->revert,&bh);
    m->buttons=MAX(lh,bh);height+=gap+m->buttons;
    width=MAX(width,(unsigned long)m->apply+gap+m->revert);
    m->width=dimension(width);m->height=dimension(height);
}
/* Clip allocations explicitly; never send wrapped or zero X dimensions. */
static void place(XmColorPickerWidget p,Widget child,int x,int y,int width,int height) {
    int right=MIN((int)p->core.width,x+width),bottom=MIN((int)p->core.height,y+height);
    x=MAX(0,x);y=MAX(0,y);width=right-x;height=bottom-y;
    XtSetMappedWhenManaged(child,width>0 && height>0);
    if(width>0 && height>0)XtConfigureWidget(child,x,y,width,height,0);
}
static void Resize(Widget widget) {
    XmColorPickerWidget p=(XmColorPickerWidget)widget;Layout m;
    if(p->color_picker.initializing || p->color_picker.updating)return;
    p->color_picker.updating=True;measure(p,&m);
    int width=MIN(p->core.width,SHRT_MAX),height=MIN(p->core.height,SHRT_MAX);
    int gap=MIN(p->color_picker.spacing,MAX(0,width/8));
    unsigned char direction;XtVaGetValues(widget,XmNlayoutDirection,&direction,NULL);
    Boolean rtl=XmDirectionMatch(direction,XmRIGHT_TO_LEFT);
    int sw=MIN(m.swatch,MAX(0,(width-gap)/2)),text_width=MAX(1,width-sw-gap);
    int header=MIN((int)m.header,height),tx=rtl?0:sw+gap;
    place(p,p->color_picker.swatch,rtl?width-sw:0,0,sw,header);
    place(p,p->color_picker.hex_label,tx,0,text_width,MIN((int)m.hex_label,header));
    place(p,p->color_picker.editor,tx,m.hex_label+gap,text_width,header-m.hex_label-gap);
    int y=header+gap,label_width=MIN((int)m.label_width,MAX(0,width/3));
    place(p,p->color_picker.modes,0,y,width,m.tabs);y+=m.tabs+gap;
    if(m.plane){
        if(p->color_picker.mode==XmCOLOR_PICKER_PENCILS){
            for(int i=0;i<24;++i){int col=rtl?11-i%12:i%12,row=i/12;
                int x=col*width/12,right=(col+1)*width/12;
                place(p,p->color_picker.pencils[i],x,y+row*m.plane/2,MAX(1,right-x-2),MAX(1,m.plane/2-2));}
        }else place(p,p->color_picker.plane,0,y,width,m.plane);
        y+=m.plane+gap;
    }
    place(p,p->color_picker.mode_hint,0,y,width,m.hint);y+=m.hint+gap;
    unsigned first=p->color_picker.mode==XmCOLOR_PICKER_PENCILS?3:0;
    for(unsigned i=first;i<4;++i) {
        Widget label=i==3||p->color_picker.mode==XmCOLOR_PICKER_RGB?p->color_picker.labels[i]:p->color_picker.mode_labels[i];
        Widget channel=i==3||p->color_picker.mode==XmCOLOR_PICKER_RGB?p->color_picker.channels[i]:p->color_picker.mode_channels[i];
        place(p,label,rtl?width-label_width:0,y,label_width,m.rows[i]);
        place(p,channel,rtl?0:label_width+gap,y,width-label_width-gap,m.rows[i]);
        y+=m.rows[i]+gap;
    }
    if(m.error){place(p,p->color_picker.error_label,0,y,width,m.error);y+=m.error+gap;}
    int apply=MIN((int)m.apply,MAX(0,(width-gap)/2));
    int revert=MIN((int)m.revert,MAX(0,width-apply-gap));
    place(p,p->color_picker.apply,rtl?0:width-apply,y,apply,m.buttons);
    place(p,p->color_picker.revert,rtl?apply+gap:width-apply-gap-revert,y,revert,m.buttons);
    p->color_picker.updating=False;
}
static Boolean layout_work(XtPointer closure) {
    XmColorPickerWidget p=(XmColorPickerWidget)closure;Layout m;p->color_picker.layout_work=0;measure(p,&m);
    Dimension width=p->color_picker.automatic_width?m.width:p->core.width;
    Dimension height=p->color_picker.automatic_height?m.height:p->core.height;
    if(width!=p->core.width || height!=p->core.height) {
        XtWidgetGeometry request={0},reply={0};request.request_mode=CWWidth|CWHeight;request.width=width;request.height=height;
        if(XtMakeGeometryRequest((Widget)p,&request,&reply)==XtGeometryAlmost) {
            if(reply.request_mode&CWWidth)request.width=reply.width;
            if(reply.request_mode&CWHeight)request.height=reply.height;
            XtMakeGeometryRequest((Widget)p,&request,NULL);
        }
    }
    Resize((Widget)p);return True;
}
static void schedule_layout(XmColorPickerWidget p) {
    if(!p->color_picker.initializing && !p->core.being_destroyed && !p->color_picker.layout_work)
        p->color_picker.layout_work=XtAppAddWorkProc(XtWidgetToApplicationContext((Widget)p),layout_work,p);
}
static void Realize(Widget widget,XtValueMask *mask,XSetWindowAttributes *attributes) {
    (*xmManagerClassRec.core_class.realize)(widget,mask,attributes);
    /* Scale changes from its initial allocation to natural geometry after
     * realization. Reconcile once all native child windows have been created. */
    schedule_layout((XmColorPickerWidget)widget);
}
static void ChangeManaged(Widget w){schedule_layout((XmColorPickerWidget)w);}
static XtGeometryResult GeometryManager(Widget child,XtWidgetGeometry *request,XtWidgetGeometry *reply) {
    (void)reply;if(!(request->request_mode&XtCWQueryOnly))schedule_layout((XmColorPickerWidget)XtParent(child));return XtGeometryNo;
}
static void release_colors(XmColorPickerWidget p) {
    for(unsigned i=0;i<2;++i)if(p->color_picker.swatch_allocated[i]) {
        XFreeColors(XtDisplay(p),p->color_picker.swatch_colormap,&p->color_picker.swatch_pixels[i],1,0);
        p->color_picker.swatch_allocated[i]=False;
    }
}
static void update_colors(XmColorPickerWidget p) {
    XColor background;unsigned long value=p->color_picker.preview;unsigned alpha=value&255;
    release_colors(p);p->color_picker.swatch_colormap=p->core.colormap;
    background.pixel=p->core.background_pixel;XQueryColor(XtDisplay(p),p->core.colormap,&background);
    Boolean dark=(unsigned long)background.red+background.green+background.blue<3UL*32768;
    for(unsigned i=0;i<2;++i) {
        unsigned base=dark?(i?96:56):(i?238:190);XColor color;
        color.red=(unsigned short)((((value>>24)&255)*alpha+base*(255-alpha)+127)/255*257);
        color.green=(unsigned short)((((value>>16)&255)*alpha+base*(255-alpha)+127)/255*257);
        color.blue=(unsigned short)((((value>>8)&255)*alpha+base*(255-alpha)+127)/255*257);
        color.flags=DoRed|DoGreen|DoBlue;
        p->color_picker.swatch_allocated[i]=XAllocColor(XtDisplay(p),p->core.colormap,&color);
        p->color_picker.swatch_pixels[i]=p->color_picker.swatch_allocated[i]?color.pixel:
            (i?p->core.background_pixel:p->manager.foreground);
    }
    if(XtIsRealized(p->color_picker.swatch))XClearArea(XtDisplay(p),XtWindow(p->color_picker.swatch),0,0,0,0,True);
}
static void paint_swatch(Widget child,XtPointer closure,XtPointer data) {
    XmColorPickerWidget p=closure;(void)data;
    if(!XtIsRealized(child) || !p->color_picker.swatch_gc)return;
    Display *d=XtDisplay(child);Window window=XtWindow(child);int width=child->core.width,height=child->core.height;
    int radius=0;XmAppearanceProfile profile=_XmAppearanceGeometryProfile(d);
    if(profile==XmAPPEARANCE_MODERN_LIGHT || profile==XmAPPEARANCE_MODERN_DARK)radius=_XmDpiDefaultDimension((Widget)p,4);
    Region region=_XmAppearanceRoundedRectangleRegion(0,0,width,height,radius);
    XSetRegion(d,p->color_picker.swatch_gc,region);XDestroyRegion(region);
    int tile=MAX(1,p->color_picker.spacing);
    for(int y=0;y<height;y+=tile)for(int x=0;x<width;x+=tile) {
        XSetForeground(d,p->color_picker.swatch_gc,p->color_picker.swatch_pixels[((x/tile)+(y/tile))&1]);
        XFillRectangle(d,window,p->color_picker.swatch_gc,x,y,MIN(tile,width-x),MIN(tile,height-y));
    }
    XSetClipMask(d,p->color_picker.swatch_gc,None);
}
static void owner_destroyed(Widget w,XtPointer closure,XtPointer data){(void)w;(void)data;*(Boolean*)closure=True;}
static void update_buttons(XmColorPickerWidget p) {
    char *text=XmTextFieldGetString(p->color_picker.editor);uint32_t value;
    Boolean valid=parse(text,&value);char canonical[16];snprintf(canonical,sizeof(canonical),"#%08lX",(unsigned long)p->color_picker.value);
    XtSetSensitive(p->color_picker.apply,valid && value!=p->color_picker.value);
    XtSetSensitive(p->color_picker.revert,!valid || p->color_picker.preview!=p->color_picker.value || strcmp(text,canonical));
    XtFree(text);
}
static Boolean refresh(XmColorPickerWidget p,Boolean format) {
    Widget widget=(Widget)p;Boolean destroyed=False;
    p->color_picker.updating=True;
    for(unsigned i=0;i<4;++i)XmScaleSetValue(p->color_picker.channels[i],(p->color_picker.preview>>(24-8*i))&255);
    if(p->color_picker.invalid)XtManageChild(p->color_picker.error_label);else XtUnmanageChild(p->color_picker.error_label);
    if(p->color_picker.model_source!=XmCOLOR_PICKER_HSV)color_to_hsv(p->color_picker.preview,p->color_picker.hsv);
    if(p->color_picker.model_source!=XmCOLOR_PICKER_LAB){color_to_lab(p->color_picker.preview,p->color_picker.lab);p->color_picker.gamut=True;}
    update_modes(p);
    update_colors(p);
    if(format) {
        char text[16];snprintf(text,sizeof(text),"#%08lX",(unsigned long)p->color_picker.preview);
        XtAddCallback(widget,XmNdestroyCallback,owner_destroyed,&destroyed);
        XmTextFieldSetString(p->color_picker.editor,text);
        if(destroyed)return False;
        XtRemoveCallback(widget,XmNdestroyCallback,owner_destroyed,&destroyed);
        if(widget->core.being_destroyed)return False;
    }
    p->color_picker.updating=False;update_buttons(p);schedule_layout(p);return True;
}
static void preview_value(XmColorPickerWidget p,uint32_t value,XEvent *event,Boolean format) {
    Boolean changed=p->color_picker.preview!=value;p->color_picker.preview=value;p->color_picker.invalid=False;
    if(!refresh(p,format))return;
    p->color_picker.model_source=-1;
    if(changed)notify(p,XmNcolorPickerPreviewCallback,event,value);
}
static Boolean commit(XmColorPickerWidget p,Boolean notify_change,XEvent *event) {
    char *text=XmTextFieldGetString(p->color_picker.editor);uint32_t value;
    Boolean valid=parse(text,&value);XtFree(text);
    if(!valid){p->color_picker.invalid=True;refresh(p,False);return False;}
    Boolean changed=p->color_picker.value!=value;p->color_picker.value=p->color_picker.preview=value;p->color_picker.invalid=False;
    if(!refresh(p,True))return True;
    if(changed && notify_change)notify(p,XmNcolorPickerValueChangedCallback,event,value);
    return True;
}
static void edited(Widget child,XtPointer closure,XtPointer data) {
    XmColorPickerWidget p=closure;(void)child;XmAnyCallbackStruct *call=data;
    if(!p || !XmIsColorPicker((Widget)p) || p->core.being_destroyed)return;
    if(p->color_picker.updating)return;
    char *text=XmTextFieldGetString(p->color_picker.editor);uint32_t value;Boolean valid=parse(text,&value);XtFree(text);
    if(valid)preview_value(p,value,call?call->event:NULL,False);
    else {p->color_picker.invalid=False;XtUnmanageChild(p->color_picker.error_label);update_buttons(p);schedule_layout(p);}
}
static void channel_changed(Widget child,XtPointer closure,XtPointer data) {
    XmColorPickerWidget p=closure;XmScaleCallbackStruct *call=data;
    if(!p || !XmIsColorPicker((Widget)p) || p->core.being_destroyed)return;
    if(p->color_picker.updating || !XtIsSensitive((Widget)p))return;
    for(unsigned i=0;i<4;++i)if(child==p->color_picker.channels[i]) {
        int value;XmScaleGetValue(child,&value);unsigned shift=24-8*i;
        uint32_t next=(p->color_picker.preview&~(UINT32_C(255)<<shift))|((uint32_t)value<<shift);
        if(i==3 && (p->color_picker.mode==XmCOLOR_PICKER_HSV||p->color_picker.mode==XmCOLOR_PICKER_LAB))p->color_picker.model_source=p->color_picker.mode;
        preview_value(p,next,call?call->event:NULL,True);return;
    }
}
static void activated(Widget child,XtPointer closure,XtPointer data) {
    XmColorPickerWidget p=closure;XmAnyCallbackStruct *call=data;
    if(!p || !XmIsColorPicker((Widget)p) || p->core.being_destroyed || !XtIsSensitive((Widget)p))return;
    if(child==p->color_picker.revert)preview_value(p,p->color_picker.value,call?call->event:NULL,True);
    else (void)commit(p,True,call?call->event:NULL);
}
static void Events(Widget child,XtPointer closure,XEvent *event,Boolean *dispatch) {
    XmColorPickerWidget p=closure;
    if(!p || !XmIsColorPicker((Widget)p) || p->core.being_destroyed || !event || event->type!=KeyPress || !XtIsSensitive((Widget)p))return;
    KeySym key=XLookupKeysym(&event->xkey,0);
    if(key==XK_Escape){p->color_picker.dragging=False;*dispatch=False;preview_value(p,p->color_picker.value,event,True);}
    else if((key==XK_Return || key==XK_KP_Enter) && (XmIsScale(child) || XmIsScale(XtParent(child)))){*dispatch=False;(void)commit(p,True,event);}
}
static Widget label(Widget parent,const char *name,const char *text,XmRenderTable font,Boolean managed) {
    XmString string=XmStringCreateLocalized((char*)text);
    Widget child=XtVaCreateWidget(name,xmLabelWidgetClass,parent,XmNlabelString,string,
        XmNalignment,XmALIGNMENT_BEGINNING,XmNrenderTable,font,NULL);XmStringFree(string);
    if(managed)XtManageChild(child);
    return child;
}
static void Initialize(Widget request,Widget widget,ArgList args,Cardinal *count) {
    XmColorPickerWidget p=(XmColorPickerWidget)widget;(void)args;(void)count;
    p->color_picker.initializing=True;p->color_picker.updating=False;p->color_picker.layout_work=0;p->color_picker.invalid=False;
    p->color_picker.automatic_width=request->core.width==0;p->color_picker.automatic_height=request->core.height==0;
    p->color_picker.preview=p->color_picker.value;
    p->color_picker.mode=XmCOLOR_PICKER_HSV;p->color_picker.model_source=-1;
    memset(p->color_picker.hsv,0,sizeof(p->color_picker.hsv));
    p->color_picker.gamut=True;p->color_picker.dragging=False;p->color_picker.plane_cache=None;
    p->color_picker.plane_width=p->color_picker.plane_height=0;p->color_picker.plane_mode=-1;
    p->color_picker.palette_ready=False;memset(p->color_picker.palette_allocated,0,sizeof(p->color_picker.palette_allocated));
    p->color_picker.swatch_allocated[0]=p->color_picker.swatch_allocated[1]=False;
    p->color_picker.render_table=XmFontListCopy(p->color_picker.render_table?p->color_picker.render_table:XmeGetDefaultRenderTable(widget,XmLABEL_FONTLIST));
    p->color_picker.swatch_gc=NULL;
    Pixmap drawable=XCreatePixmap(XtDisplay(widget),RootWindowOfScreen(XtScreen(widget)),1,1,p->core.depth);
    if(drawable!=None){
        p->color_picker.swatch_gc=XCreateGC(XtDisplay(widget),drawable,0,NULL);
        XFreePixmap(XtDisplay(widget),drawable);
    }
    p->color_picker.swatch=XtVaCreateManagedWidget("swatch",xmDrawingAreaWidgetClass,widget,XmNtraversalOn,False,XmNresizePolicy,XmRESIZE_NONE,NULL);
    XtAddCallback(p->color_picker.swatch,XmNexposeCallback,paint_swatch,p);
    p->color_picker.hex_label=label(widget,"hexLabel","Hex RGBA",p->color_picker.render_table,True);
    p->color_picker.editor=XtVaCreateManagedWidget("hexRGBA",xmTextFieldWidgetClass,widget,XmNcolumns,9,XmNmaxLength,9,XmNrenderTable,p->color_picker.render_table,NULL);
    XtAddCallback(p->color_picker.editor,XmNvalueChangedCallback,edited,p);XtAddCallback(p->color_picker.editor,XmNactivateCallback,activated,p);
    const char *names[]={"red","green","blue","alpha"},*titles[]={"Red","Green","Blue","Alpha"};
    for(unsigned i=0;i<4;++i) {
        char name[32];snprintf(name,sizeof(name),"%sLabel",names[i]);
        p->color_picker.labels[i]=label(widget,name,titles[i],p->color_picker.render_table,True);
        p->color_picker.channels[i]=XtVaCreateManagedWidget(names[i],xmScaleWidgetClass,widget,
            XmNminimum,0,XmNmaximum,255,XmNshowValue,True,XmNorientation,XmHORIZONTAL,
            XmNrenderTable,p->color_picker.render_table,NULL);
        XtAddCallback(p->color_picker.channels[i],XmNvalueChangedCallback,channel_changed,p);
        XtAddCallback(p->color_picker.channels[i],XmNdragCallback,channel_changed,p);
        WidgetList children;Cardinal count;
        XtVaGetValues(p->color_picker.channels[i],XmNchildren,&children,XmNnumChildren,&count,NULL);
        for(Cardinal n=0;n<count;++n)if(XtIsWidget(children[n]))
            XtAddEventHandler(children[n],KeyPressMask,False,Events,p);
    }
    p->color_picker.modes=XtVaCreateManagedWidget("colorModes",xmSegmentedControlWidgetClass,widget,XmNrenderTable,p->color_picker.render_table,NULL);
    const char *mode_names[]={"RGB","HSV","Lab","Pencils"};XmSegmentedControlItem items[4];
    for(int i=0;i<4;++i){items[i].id=mode_names[i];items[i].label=XmStringCreateLocalized((char*)mode_names[i]);items[i].pixmap=XmUNSPECIFIED_PIXMAP;items[i].enabled=True;}
    XmSegmentedControlSetItems(p->color_picker.modes,items,4);
    for(int i=0;i<4;++i)XmStringFree(items[i].label);
    XmSegmentedControlSetSelectedIndex(p->color_picker.modes,p->color_picker.mode,False);
    XtAddCallback(p->color_picker.modes,XmNSegmentedSelectionCallback,mode_changed,p);
    p->color_picker.plane=XtVaCreateWidget("colorField",xmDrawingAreaWidgetClass,widget,XmNresizePolicy,XmRESIZE_NONE,XmNtraversalOn,True,NULL);
    XtAddCallback(p->color_picker.plane,XmNexposeCallback,paint_plane,p);
    XtAddEventHandler(p->color_picker.plane,ButtonPressMask|ButtonReleaseMask|Button1MotionMask|KeyPressMask|FocusChangeMask,False,plane_events,p);
    p->color_picker.mode_hint=label(widget,"modeDescription","",p->color_picker.render_table,True);
    for(int i=0;i<3;++i){char name[32];snprintf(name,sizeof(name),"model%dLabel",i);
        p->color_picker.mode_labels[i]=label(widget,name,"",p->color_picker.render_table,False);
        snprintf(name,sizeof(name),"model%d",i);
        p->color_picker.mode_channels[i]=XtVaCreateWidget(name,xmScaleWidgetClass,widget,XmNminimum,0,XmNmaximum,1000,XmNshowValue,True,XmNorientation,XmHORIZONTAL,XmNrenderTable,p->color_picker.render_table,NULL);
        XtAddCallback(p->color_picker.mode_channels[i],XmNvalueChangedCallback,model_changed,p);
        XtAddCallback(p->color_picker.mode_channels[i],XmNdragCallback,model_changed,p);
        WidgetList children;Cardinal n;XtVaGetValues(p->color_picker.mode_channels[i],XmNchildren,&children,XmNnumChildren,&n,NULL);
        for(Cardinal j=0;j<n;++j)if(XtIsWidget(children[j]))XtAddEventHandler(children[j],KeyPressMask,False,Events,p);
    }
    for(int i=0;i<24;++i){XmString name=XmStringCreateLocalized((char*)pencil_names[i]);char id[24];snprintf(id,sizeof(id),"pencil%d",i);
        p->color_picker.pencils[i]=XtVaCreateWidget(id,xmDrawnButtonWidgetClass,widget,XmNlabelString,name,XmNtoolTipString,name,XmNpushButtonEnabled,True,XmNrenderTable,p->color_picker.render_table,NULL);XmStringFree(name);
        XtAddCallback(p->color_picker.pencils[i],XmNexposeCallback,paint_pencil,p);
        XtAddCallback(p->color_picker.pencils[i],XmNresizeCallback,paint_pencil,p);
        XtAddCallback(p->color_picker.pencils[i],XmNactivateCallback,pencil_selected,p);
    }
    p->color_picker.error_label=label(widget,"validation","Use #RRGGBB or #RRGGBBAA.",p->color_picker.render_table,False);
    XmString apply=XmStringCreateLocalized("Apply"),revert=XmStringCreateLocalized("Revert");
    p->color_picker.apply=XtVaCreateManagedWidget("apply",xmPushButtonWidgetClass,widget,XmNlabelString,apply,XmNrenderTable,p->color_picker.render_table,NULL);
    p->color_picker.revert=XtVaCreateManagedWidget("revert",xmPushButtonWidgetClass,widget,XmNlabelString,revert,XmNrenderTable,p->color_picker.render_table,NULL);
    XmStringFree(apply);XmStringFree(revert);
    XtAddCallback(p->color_picker.apply,XmNactivateCallback,activated,p);XtAddCallback(p->color_picker.revert,XmNactivateCallback,activated,p);
    for(Cardinal i=0;i<p->composite.num_children;++i) {
        Widget child=p->composite.children[i];
        if(child!=p->color_picker.swatch && child!=p->color_picker.plane)_XmAppearanceInheritFont(child,widget);
        _XmAppearanceInheritForeground(child,widget);XtAddEventHandler(child,KeyPressMask,False,Events,p);
    }
    if(!refresh(p,True))return;
    p->color_picker.initializing=False;Layout m;measure(p,&m);
    if(p->color_picker.automatic_width)p->core.width=m.width;
    if(p->color_picker.automatic_height)p->core.height=m.height;
    Resize(widget);
}
static void Destroy(Widget widget) {
    XmColorPickerWidget p=(XmColorPickerWidget)widget;
    if(p->color_picker.layout_work)XtRemoveWorkProc(p->color_picker.layout_work);
    release_palette(p);
    if(p->color_picker.plane_cache)XFreePixmap(XtDisplay(p),p->color_picker.plane_cache);
    release_colors(p);if(p->color_picker.swatch_gc)XFreeGC(XtDisplay(p),p->color_picker.swatch_gc);
    if(p->color_picker.render_table)XmRenderTableFree(p->color_picker.render_table);
}
static Boolean SetValues(Widget old_widget,Widget request,Widget widget,ArgList args,Cardinal *count) {
    XmColorPickerWidget old=(XmColorPickerWidget)old_widget,p=(XmColorPickerWidget)widget;(void)request;
    if(!XtIsSensitive(widget))p->color_picker.dragging=False;
    if(old->color_picker.render_table!=p->color_picker.render_table) {
        XmRenderTable font=XmFontListCopy(p->color_picker.render_table?p->color_picker.render_table:XmeGetDefaultRenderTable(widget,XmLABEL_FONTLIST));
        if(font) {
            if(old->color_picker.render_table)XmRenderTableFree(old->color_picker.render_table);
            p->color_picker.render_table=font;
            for(Cardinal i=0;i<p->composite.num_children;++i) {
                Widget child=p->composite.children[i];
                if(child!=p->color_picker.swatch && child!=p->color_picker.plane && _XmAppearanceOwnsResource(child,XmNrenderTable)) {
                    XtVaSetValues(child,XmNrenderTable,font,NULL);_XmAppearanceInheritFont(child,widget);
                }
            }
        }else p->color_picker.render_table=old->color_picker.render_table;
    }
    Boolean changed=old->color_picker.value!=p->color_picker.value;
    if(changed){p->color_picker.preview=p->color_picker.value;p->color_picker.invalid=False;}
    for(Cardinal i=0;i<*count;++i) {
        if(!strcmp(args[i].name,XmNwidth))p->color_picker.automatic_width=(Dimension)args[i].value==0;
        if(!strcmp(args[i].name,XmNheight))p->color_picker.automatic_height=(Dimension)args[i].value==0;
    }
    if(!refresh(p,changed))return False;
    Layout m;measure(p,&m);
    if(p->color_picker.automatic_width)p->core.width=m.width;
    if(p->color_picker.automatic_height)p->core.height=m.height;
    Resize(widget);return False;
}
static XtGeometryResult QueryGeometry(Widget widget,XtWidgetGeometry *request,XtWidgetGeometry *reply) {
    XmColorPickerWidget p=(XmColorPickerWidget)widget;Layout m;measure(p,&m);
    reply->request_mode=CWWidth|CWHeight;reply->width=p->color_picker.automatic_width?m.width:p->core.width;
    reply->height=p->color_picker.automatic_height?m.height:p->core.height;return XmeReplyToQueryGeometry(widget,request,reply);
}
unsigned long XmColorPickerGetValue(Widget widget){return widget && XmIsColorPicker(widget) && !widget->core.being_destroyed?((XmColorPickerWidget)widget)->color_picker.value:0;}
unsigned long XmColorPickerGetPreviewValue(Widget widget){return widget && XmIsColorPicker(widget) && !widget->core.being_destroyed?((XmColorPickerWidget)widget)->color_picker.preview:0;}
Widget XmColorPickerGetTextField(Widget widget){return widget && XmIsColorPicker(widget) && !widget->core.being_destroyed?((XmColorPickerWidget)widget)->color_picker.editor:NULL;}
Boolean XmColorPickerCommit(Widget widget,Boolean notify_change){return widget && XmIsColorPicker(widget) && !widget->core.being_destroyed?commit((XmColorPickerWidget)widget,notify_change,NULL):False;}
void XmColorPickerRevert(Widget widget){if(widget && XmIsColorPicker(widget) && !widget->core.being_destroyed){XmColorPickerWidget p=(XmColorPickerWidget)widget;preview_value(p,p->color_picker.value,NULL,True);}}
void XmColorPickerSetValue(Widget widget,unsigned long value,Boolean notify_change) {
    if(!widget || !XmIsColorPicker(widget) || widget->core.being_destroyed)return;
    XmColorPickerWidget p=(XmColorPickerWidget)widget;
    Boolean changed=p->color_picker.value!=(uint32_t)value;p->color_picker.value=p->color_picker.preview=(uint32_t)value;p->color_picker.invalid=False;
    if(!refresh(p,True))return;
    if(changed && notify_change)notify(p,XmNcolorPickerValueChangedCallback,NULL,(uint32_t)value);
}
void XmColorPickerPreviewValue(Widget widget,unsigned long value){if(widget && XmIsColorPicker(widget) && !widget->core.being_destroyed)preview_value((XmColorPickerWidget)widget,(uint32_t)value,NULL,True);}
Widget XmCreateColorPicker(Widget parent,char *name,ArgList args,Cardinal count){return XtCreateWidget(name,xmColorPickerWidgetClass,parent,args,count);}

static void set_label(Widget w,const char *text) {
    XmString value=XmStringCreateLocalized((char*)text);
    XtVaSetValues(w,XmNlabelString,value,NULL);XmStringFree(value);
}
static void managed(Widget w,Boolean visible){if(visible){if(!XtIsManaged(w))XtManageChild(w);}else if(XtIsManaged(w))XtUnmanageChild(w);}
static void update_modes(XmColorPickerWidget p) {
    int mode=p->color_picker.mode;
    const char *hsv_names[]={"Hue","Saturation %","Value %"},*lab_names[]={"Lightness","a*","b*"};
    for(int i=0;i<3;++i){
        managed(p->color_picker.labels[i],mode==XmCOLOR_PICKER_RGB);managed(p->color_picker.channels[i],mode==XmCOLOR_PICKER_RGB);
        managed(p->color_picker.mode_labels[i],mode==XmCOLOR_PICKER_HSV||mode==XmCOLOR_PICKER_LAB);
        managed(p->color_picker.mode_channels[i],mode==XmCOLOR_PICKER_HSV||mode==XmCOLOR_PICKER_LAB);
        if(mode==XmCOLOR_PICKER_HSV||mode==XmCOLOR_PICKER_LAB){
            int lo=mode==XmCOLOR_PICKER_LAB&&i? -1280:0;
            int hi=mode==XmCOLOR_PICKER_HSV?(i?1000:3600):(i?1270:1000);
            double v=mode==XmCOLOR_PICKER_HSV?p->color_picker.hsv[i]*(i?1000:10):p->color_picker.lab[i]*10;
            set_label(p->color_picker.mode_labels[i],mode==XmCOLOR_PICKER_HSV?hsv_names[i]:lab_names[i]);
            XtVaSetValues(p->color_picker.mode_channels[i],XmNminimum,lo,XmNmaximum,hi,XmNdecimalPoints,1,XmNvalue,(int)floor(color_clamp(v,lo,hi)+.5),NULL);
        }
    }
    managed(p->color_picker.plane,mode==XmCOLOR_PICKER_HSV||mode==XmCOLOR_PICKER_LAB);
    for(int i=0;i<24;++i){managed(p->color_picker.pencils[i],mode==XmCOLOR_PICKER_PENCILS);
        if(XtIsRealized(p->color_picker.pencils[i]) && mode==XmCOLOR_PICKER_PENCILS)XClearArea(XtDisplay(p),XtWindow(p->color_picker.pencils[i]),0,0,0,0,True);}
    const char *hint=mode==XmCOLOR_PICKER_RGB?"sRGB channels, 0-255":mode==XmCOLOR_PICKER_HSV?"Saturation left to right; value bottom to top":mode==XmCOLOR_PICKER_LAB?(p->color_picker.gamut?"Lab D50: a* horizontal, b* vertical; hatch = outside sRGB":"Outside sRGB: preview clipped to displayable RGB"):"Choose a pencil; opacity is independent";
    set_label(p->color_picker.mode_hint,hint);
    if(XtIsRealized(p->color_picker.plane))XClearArea(XtDisplay(p),XtWindow(p->color_picker.plane),0,0,0,0,True);
}
Boolean XmColorPickerSetMode(Widget widget,XmColorPickerMode mode) {
    if(!widget || !XmIsColorPicker(widget) || widget->core.being_destroyed || mode<XmCOLOR_PICKER_RGB || mode>XmCOLOR_PICKER_PENCILS)return False;
    XmColorPickerWidget p=(XmColorPickerWidget)widget;p->color_picker.mode=mode;p->color_picker.dragging=False;
    XmSegmentedControlSetSelectedIndex(p->color_picker.modes,mode,False);
    /* Mode changes preserve both pending text and the selected color. */
    p->color_picker.updating=True;update_modes(p);p->color_picker.updating=False;
    Resize(widget);schedule_layout(p);return True;
}
XmColorPickerMode XmColorPickerGetMode(Widget w){return w && XmIsColorPicker(w)?((XmColorPickerWidget)w)->color_picker.mode:XmCOLOR_PICKER_RGB;}
static void mode_changed(Widget child,XtPointer closure,XtPointer data) {
    XmColorPickerWidget p=closure;XmSegmentedControlCallbackStruct *cb=data;(void)child;
    if(!p->color_picker.updating && XtIsSensitive((Widget)p) && cb)XmColorPickerSetMode((Widget)p,(XmColorPickerMode)cb->selected_index);
}
static void model_preview(XmColorPickerWidget p,int mode,XEvent *event) {
    double rgb[3];p->color_picker.model_source=mode;
    if(mode==XmCOLOR_PICKER_HSV)color_from_hsv(p->color_picker.hsv,rgb);
    else p->color_picker.gamut=color_from_lab(p->color_picker.lab,rgb);
    preview_value(p,color_pack(rgb,p->color_picker.preview&255),event,True);
}
Boolean XmColorPickerPreviewHSV(Widget w,double h,double s,double v) {
    if(!w||!XmIsColorPicker(w)||w->core.being_destroyed||!isfinite(h)||!isfinite(s)||!isfinite(v)||h<0||h>360||s<0||s>1||v<0||v>1)return False;
    XmColorPickerWidget p=(XmColorPickerWidget)w;p->color_picker.hsv[0]=h==360?0:h;p->color_picker.hsv[1]=s;p->color_picker.hsv[2]=v;
    model_preview(p,XmCOLOR_PICKER_HSV,NULL);return True;
}
Boolean XmColorPickerPreviewLab(Widget w,double l,double a,double b) {
    if(!w||!XmIsColorPicker(w)||w->core.being_destroyed||!isfinite(l)||!isfinite(a)||!isfinite(b)||l<0||l>100||a< -128||a>127||b< -128||b>127)return False;
    XmColorPickerWidget p=(XmColorPickerWidget)w;p->color_picker.lab[0]=l;p->color_picker.lab[1]=a;p->color_picker.lab[2]=b;
    model_preview(p,XmCOLOR_PICKER_LAB,NULL);return True;
}
static void model_changed(Widget child,XtPointer closure,XtPointer data) {
    XmColorPickerWidget p=closure;XmScaleCallbackStruct *cb=data;
    if(!p || !XmIsColorPicker((Widget)p) || p->core.being_destroyed)return;
    if(p->color_picker.updating || !XtIsSensitive((Widget)p))return;
    for(int i=0;i<3;++i)if(child==p->color_picker.mode_channels[i]){
        int value;XmScaleGetValue(child,&value);
        if(p->color_picker.mode==XmCOLOR_PICKER_HSV)p->color_picker.hsv[i]=value/(i?1000.:10.);
        else if(p->color_picker.mode==XmCOLOR_PICKER_LAB)p->color_picker.lab[i]=value/10.;else return;
        model_preview(p,p->color_picker.mode,cb?cb->event:NULL);return;
    }
}
static void release_palette(XmColorPickerWidget p) {
    for(int i=0;i<216;++i)if(p->color_picker.palette_allocated[i]){
        XFreeColors(XtDisplay(p),p->color_picker.palette_colormap,&p->color_picker.palette[i],1,0);p->color_picker.palette_allocated[i]=False;
    }
    p->color_picker.palette_ready=False;
}
static void ensure_palette(XmColorPickerWidget p,Visual *visual) {
    if(visual->class==TrueColor)return;
    if(p->color_picker.palette_ready && p->color_picker.palette_colormap==p->core.colormap)return;
    release_palette(p);p->color_picker.palette_colormap=p->core.colormap;
    XColor available[256];Boolean queried=False;int count=MIN(visual->map_entries,256);
    for(int r=0;r<6;++r)for(int g=0;g<6;++g)for(int b=0;b<6;++b){
        int i=r*36+g*6+b;XColor c={0};c.red=r*13107;c.green=g*13107;c.blue=b*13107;c.flags=DoRed|DoGreen|DoBlue;
        p->color_picker.palette_allocated[i]=XAllocColor(XtDisplay(p),p->core.colormap,&c);
        p->color_picker.palette[i]=p->color_picker.palette_allocated[i]?c.pixel:p->core.background_pixel;
        if(!p->color_picker.palette_allocated[i]){
            /* Query a bounded existing map; never allocate during expose again. */
            if(!queried){for(int j=0;j<count;++j)available[j].pixel=j;XQueryColors(XtDisplay(p),p->core.colormap,available,count);queried=True;}
            uint64_t best=UINT64_MAX;
            for(int j=0;j<count;++j){
                int64_t dr=(int64_t)c.red-available[j].red,dg=(int64_t)c.green-available[j].green,db=(int64_t)c.blue-available[j].blue;
                uint64_t distance=(uint64_t)(dr*dr+dg*dg+db*db);
                if(distance<best){best=distance;p->color_picker.palette[i]=available[j].pixel;}}
        }
    }
    p->color_picker.palette_ready=True;
}
static unsigned long component_pixel(double v,unsigned long mask) {
    if(!mask)return 0;
    unsigned shift=0;while(!(mask&1)){mask>>=1;++shift;}
    return ((unsigned long)floor(color_clamp(v,0,1)*mask+.5)<<shift);
}
static Pixel rgb_pixel(XmColorPickerWidget p,Visual *visual,const double rgb[3]) {
    if(visual->class==TrueColor)return component_pixel(rgb[0],visual->red_mask)|component_pixel(rgb[1],visual->green_mask)|component_pixel(rgb[2],visual->blue_mask);
    int r=(int)floor(color_clamp(rgb[0],0,1)*5+.5),g=(int)floor(color_clamp(rgb[1],0,1)*5+.5),b=(int)floor(color_clamp(rgb[2],0,1)*5+.5);
    return p->color_picker.palette[r*36+g*6+b];
}
static void paint_plane(Widget child,XtPointer closure,XtPointer data) {
    XmColorPickerWidget p=closure;(void)data;if(!XtIsRealized(child)||!XtIsManaged(child))return;
    if(!p->color_picker.swatch_gc)return;
    Display *d=XtDisplay(child);Window window=XtWindow(child);XWindowAttributes attr;
    if(!XGetWindowAttributes(d,window,&attr))return;
    unsigned width=child->core.width,height=child->core.height;int mode=p->color_picker.mode;
    /* X drawable dimensions are nonzero.  A constrained or not-yet-laid-out
     * child can transiently report zero here; do not turn that into a BadValue
     * request, or try to draw through a failed pixmap allocation. */
    if(width==0||height==0)return;
    double parameter=mode==XmCOLOR_PICKER_HSV?p->color_picker.hsv[0]:p->color_picker.lab[0];
    ensure_palette(p,attr.visual);
    if(!p->color_picker.plane_cache || p->color_picker.plane_width!=width || p->color_picker.plane_height!=height || p->color_picker.plane_parameter!=parameter || p->color_picker.plane_mode!=mode || p->color_picker.palette_background!=p->core.background_pixel || p->color_picker.plane_colormap!=p->core.colormap || p->color_picker.plane_sensitive!=XtIsSensitive(child)){
        if(p->color_picker.plane_cache)XFreePixmap(d,p->color_picker.plane_cache);
        p->color_picker.plane_cache=XCreatePixmap(d,window,width,height,attr.depth);
        if(p->color_picker.plane_cache==None)return;
        p->color_picker.plane_width=width;p->color_picker.plane_height=height;p->color_picker.plane_parameter=parameter;p->color_picker.plane_mode=mode;p->color_picker.palette_background=p->core.background_pixel;p->color_picker.plane_colormap=p->core.colormap;p->color_picker.plane_sensitive=XtIsSensitive(child);
        /* Bounded strips limit client memory without one X request per row. */
        unsigned strip=MIN(height,32);
        XImage *image=XCreateImage(d,attr.visual,attr.depth,ZPixmap,0,NULL,width,strip,32,0);
        if(!image){XFreePixmap(d,p->color_picker.plane_cache);p->color_picker.plane_cache=None;return;}
        image->data=calloc(strip,image->bytes_per_line);
        if(!image->data){XDestroyImage(image);XFreePixmap(d,p->color_picker.plane_cache);p->color_picker.plane_cache=None;return;}
        XColor paper={0};paper.pixel=p->core.background_pixel;XQueryColor(d,p->core.colormap,&paper);
        double background[3]={paper.red/65535.,paper.green/65535.,paper.blue/65535.};
        for(unsigned start=0;start<height;start+=strip){
          unsigned rows=MIN(strip,height-start);
          for(unsigned row=0;row<rows;++row){unsigned y=start+row;
           for(unsigned x=0;x<width;++x){
            double rgb[3],nx=width>1?(double)x/(width-1):0,ny=height>1?1-(double)y/(height-1):0;Boolean gamut=True;
            if(mode==XmCOLOR_PICKER_HSV){double hsv[]={parameter,nx,ny};color_from_hsv(hsv,rgb);}
            else {double lab[]={parameter,-128+255*nx,-128+255*ny};gamut=color_from_lab(lab,rgb);}
            if(!gamut){
                double coverage=(x+y)%10<1?.18:.35;
                for(int i=0;i<3;++i)rgb[i]=color_clamp(rgb[i],0,1)*coverage+background[i]*(1-coverage);
            }
            if(!XtIsSensitive(child))for(int i=0;i<3;++i)rgb[i]=color_clamp(rgb[i],0,1)*.35+background[i]*.65;
            XPutPixel(image,x,row,rgb_pixel(p,attr.visual,rgb));
           }
          }
          XPutImage(d,p->color_picker.plane_cache,p->color_picker.swatch_gc,image,0,0,0,start,width,rows);
        }
        XDestroyImage(image);
    }
    XSetForeground(d,p->color_picker.swatch_gc,p->core.background_pixel);
    XFillRectangle(d,window,p->color_picker.swatch_gc,0,0,width,height);
    XmAppearanceProfile profile=_XmAppearanceGeometryProfile(d);
    int radius=profile==XmAPPEARANCE_CLASSIC?0:_XmDpiDefaultDimension((Widget)p,4);
    Region clip=_XmAppearanceRoundedRectangleRegion(0,0,width,height,radius);
    XSetRegion(d,p->color_picker.swatch_gc,clip);XDestroyRegion(clip);
    XCopyArea(d,p->color_picker.plane_cache,window,p->color_picker.swatch_gc,0,0,width,height,0,0);
    double nx=mode==XmCOLOR_PICKER_HSV?p->color_picker.hsv[1]:(p->color_picker.lab[1]+128)/255;
    double ny=mode==XmCOLOR_PICKER_HSV?p->color_picker.hsv[2]:(p->color_picker.lab[2]+128)/255;
    int x=(int)floor(nx*(width-1)+.5),y=(int)floor((1-ny)*(height-1)+.5),r=MAX(3,p->color_picker.spacing/2);
    double black[]={0,0,0},white[]={1,1,1};
    XSetForeground(d,p->color_picker.swatch_gc,rgb_pixel(p,attr.visual,black));
    XDrawRectangle(d,window,p->color_picker.swatch_gc,x-r,y-r,2*r,2*r);
    XSetForeground(d,p->color_picker.swatch_gc,rgb_pixel(p,attr.visual,white));
    XDrawRectangle(d,window,p->color_picker.swatch_gc,x-r+1,y-r+1,2*r-2,2*r-2);
    if(XmGetFocusWidget((Widget)p)==child){XSetForeground(d,p->color_picker.swatch_gc,p->manager.foreground);if(width>2&&height>2)XDrawRectangle(d,window,p->color_picker.swatch_gc,1,1,width-3,height-3);}
    XSetClipMask(d,p->color_picker.swatch_gc,None);
}
static void plane_events(Widget child,XtPointer closure,XEvent *event,Boolean *dispatch) {
    XmColorPickerWidget p=closure;
    if(!p || !XmIsColorPicker((Widget)p) || p->core.being_destroyed || !event || !dispatch)return;
    int mode=p->color_picker.mode;
    if(event->type==FocusIn||event->type==FocusOut){paint_plane(child,p,NULL);return;}
    if(!XtIsSensitive((Widget)p)||!XtIsSensitive(child)){p->color_picker.dragging=False;return;}
    double nx=mode==XmCOLOR_PICKER_HSV?p->color_picker.hsv[1]:(p->color_picker.lab[1]+128)/255;
    double ny=mode==XmCOLOR_PICKER_HSV?p->color_picker.hsv[2]:(p->color_picker.lab[2]+128)/255;
    if(event->type==ButtonPress && event->xbutton.button==Button1){p->color_picker.dragging=True;XmProcessTraversal(child,XmTRAVERSE_CURRENT);}
    else if(event->type==ButtonRelease && event->xbutton.button==Button1){if(!p->color_picker.dragging)return;p->color_picker.dragging=False;}
    else if(event->type==MotionNotify){if(!p->color_picker.dragging)return;}
    else if(event->type==KeyPress){
        KeySym key=XLookupKeysym(&event->xkey,0);double step=(event->xkey.state&ShiftMask)?.1:.01;
        if(key==XK_Left)nx-=step;else if(key==XK_Right)nx+=step;else if(key==XK_Up)ny+=step;else if(key==XK_Down)ny-=step;
        else if(key==XK_Home)nx=0;else if(key==XK_End)nx=1;
        else if(key==XK_Return||key==XK_KP_Enter){*dispatch=False;(void)commit(p,True,event);return;}else return;
        *dispatch=False;
    }else return;
    if(event->type!=KeyPress){int x=event->type==MotionNotify?event->xmotion.x:event->xbutton.x,y=event->type==MotionNotify?event->xmotion.y:event->xbutton.y;
        nx=child->core.width>1?(double)x/(child->core.width-1):0;ny=child->core.height>1?1-(double)y/(child->core.height-1):0;}
    nx=color_clamp(nx,0,1);ny=color_clamp(ny,0,1);
    if(mode==XmCOLOR_PICKER_HSV){p->color_picker.hsv[1]=nx;p->color_picker.hsv[2]=ny;}
    else if(mode==XmCOLOR_PICKER_LAB){p->color_picker.lab[1]=-128+255*nx;p->color_picker.lab[2]=-128+255*ny;}else return;
    model_preview(p,mode,event);
}
static void pencil_selected(Widget child,XtPointer closure,XtPointer data) {
    XmColorPickerWidget p=closure;XmAnyCallbackStruct *cb=data;
    if(!p || !XmIsColorPicker((Widget)p) || p->core.being_destroyed || !XtIsSensitive((Widget)p))return;
    for(int i=0;i<24;++i)if(p->color_picker.pencils[i]==child){preview_value(p,(pencil_colors[i]&0xffffff00)|(p->color_picker.preview&255),cb?cb->event:NULL,True);return;}
}
static void paint_pencil(Widget child,XtPointer closure,XtPointer data) {
    XmColorPickerWidget p=closure;(void)data;if(!XtIsRealized(child)||!XtIsManaged(child))return;
    int index=0;while(index<24&&p->color_picker.pencils[index]!=child)++index;if(index==24)return;
    Display *d=XtDisplay(child);Window window=XtWindow(child);XWindowAttributes attr;if(!XGetWindowAttributes(d,window,&attr))return;
    ensure_palette(p,attr.visual);GC gc=p->color_picker.swatch_gc;
    if(!gc)return;
    int width=child->core.width,height=child->core.height,inset=MAX(3,p->color_picker.spacing/2);
    if(width<=2*inset||height<=2*inset)return;
    Pixel bg;XtVaGetValues(child,XmNbackground,&bg,NULL);XSetForeground(d,gc,bg);XFillRectangle(d,window,gc,inset,inset,width-2*inset,height-2*inset);
    int left=inset+1,right=width-inset-1,top=inset+1,bottom=height-inset-1,tip=MIN((right-left),MAX(1,(bottom-top)/3));
    double rgb[3];color_rgb(pencil_colors[index],rgb);XSetForeground(d,gc,rgb_pixel(p,attr.visual,rgb));
    XFillRectangle(d,window,gc,left,top+tip,MAX(1,right-left),MAX(1,bottom-top-tip));
    double wood[]={.83,.68,.48};XSetForeground(d,gc,rgb_pixel(p,attr.visual,wood));
    XPoint points[3]={{bounded_xpoint_coordinate((left+right)/2),bounded_xpoint_coordinate(top)},{bounded_xpoint_coordinate(right),bounded_xpoint_coordinate(top+tip)},{bounded_xpoint_coordinate(left),bounded_xpoint_coordinate(top+tip)}};XFillPolygon(d,window,gc,points,3,Convex,CoordModeOrigin);
    XSetForeground(d,gc,rgb_pixel(p,attr.visual,rgb));points[1].x=bounded_xpoint_coordinate((left+right)/2+MAX(1,(right-left)/5));points[1].y=bounded_xpoint_coordinate(top+MAX(1,tip/2));points[2].x=bounded_xpoint_coordinate((left+right)/2-MAX(1,(right-left)/5));points[2].y=points[1].y;XFillPolygon(d,window,gc,points,3,Convex,CoordModeOrigin);
    for(int i=0;i<3;++i)rgb[i]=rgb[i]*.65+.35;
    XSetForeground(d,gc,rgb_pixel(p,attr.visual,rgb));XFillRectangle(d,window,gc,left+1,top+tip,MAX(1,(right-left)/4),MAX(1,bottom-top-tip));
    if((p->color_picker.preview>>8)==(pencil_colors[index]>>8)){XSetForeground(d,gc,p->manager.foreground);XDrawRectangle(d,window,gc,inset,inset,width-2*inset-1,height-2*inset-1);}
}
