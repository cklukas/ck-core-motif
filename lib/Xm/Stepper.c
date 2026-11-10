/* Bounded integer stepper with native momentary buttons and value label. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <string.h>
#include <limits.h>
#include <stdint.h>
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/StepperP.h>
#include <Xm/PushB.h>
#include <Xm/Label.h>
#include <Xm/VaSimpleP.h>
#include <Xm/XmP.h>
#include "XmI.h"
#include "CallbackI.h"
static void Initialize(Widget,Widget,ArgList,Cardinal *);
static void Destroy(Widget);
static void Resize(Widget);
static void ChangeManaged(Widget);
static Boolean SetValues(Widget,Widget,Widget,ArgList,Cardinal *);
static XtGeometryResult GeometryManager(Widget,XtWidgetGeometry *,XtWidgetGeometry *);
static XtGeometryResult QueryGeometry(Widget,XtWidgetGeometry *,XtWidgetGeometry *);
static void Events(Widget,XtPointer,XEvent *,Boolean *);
static void step(XmStepperWidget,int,Boolean,XEvent *);
static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmStepperWidget stepper = (XmStepperWidget)widget;
    (void)offset;
    if (stepper->stepper.check_set_render_table)
        value->addr = NULL;
    else {
        stepper->stepper.check_set_render_table = True;
        value->addr = (char *)&stepper->stepper.render_table;
    }
}
#define Offset(field) XtOffsetOf(XmStepperRec, field)
static XtResource resources[] = {
    {XmNstepperMinimum, XmCStepperMinimum, XmRInt, sizeof(int), Offset(stepper.minimum), XmRImmediate, (XtPointer)0},
    {XmNstepperMaximum, XmCStepperMaximum, XmRInt, sizeof(int), Offset(stepper.maximum), XmRImmediate, (XtPointer)100},
    {XmNstepperValue, XmCStepperValue, XmRInt, sizeof(int), Offset(stepper.value), XmRImmediate, (XtPointer)0},
    {XmNstepperStep, XmCStepperStep, XmRInt, sizeof(int), Offset(stepper.step), XmRImmediate, (XtPointer)1},
    {XmNstepperWrap, XmCStepperWrap, XmRBoolean, sizeof(Boolean), Offset(stepper.wrap), XmRImmediate, (XtPointer)False},
    {XmNstepperValueChangedCallback, XmCStepperValueChangedCallback, XmRCallback, sizeof(XtCallbackList), Offset(stepper.value_changed_callback), XmRPointer, (XtPointer)NULL},
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmRenderTable), Offset(stepper.render_table), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmRenderTable), Offset(stepper.render_table), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean), Offset(manager.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension, sizeof(Dimension), Offset(manager.shadow_thickness), XmRImmediate, (XtPointer)0},
    {XmNspacing, XmCSpacing, XmRHorizontalDimension, sizeof(Dimension), Offset(stepper.spacing), XmRImmediate, (XtPointer)4}
};
externaldef(xmstepperclassrec) XmStepperClassRec xmStepperClassRec = {
    {.superclass=(WidgetClass)&xmManagerClassRec, .class_name="XmStepper",
     .widget_size=sizeof(XmStepperRec), .initialize=Initialize,
     .realize=XtInheritRealize, .resources=resources, .num_resources=XtNumber(resources),
     .compress_motion=True, .compress_exposure=XtExposeCompressMultiple,
     .compress_enterleave=True, .destroy=Destroy, .resize=Resize,
     .set_values=SetValues, .set_values_almost=XtInheritSetValuesAlmost,
     .version=XtVersion, .tm_table=XtInheritTranslations, .query_geometry=QueryGeometry},
    {.geometry_manager=GeometryManager, .change_managed=ChangeManaged,
     .insert_child=XtInheritInsertChild, .delete_child=XtInheritDeleteChild},
    {.constraint_size=sizeof(XmManagerConstraintRec)},
    {.translations=XtInheritTranslations, .parent_process=XmInheritParentProcess}, {NULL}
};
externaldef(xmstepperwidgetclass) WidgetClass xmStepperWidgetClass = (WidgetClass)&xmStepperClassRec;
static void normalize(XmStepperWidget s) {
    if(s->stepper.maximum<s->stepper.minimum)s->stepper.maximum=s->stepper.minimum;
    s->stepper.value=MAX(s->stepper.minimum,MIN(s->stepper.maximum,s->stepper.value));
    if(s->stepper.step<1)s->stepper.step=1;
}
static Dimension dimension(unsigned long value) {return (Dimension)MAX(1,MIN(value,SHRT_MAX));}
static void preferred(Widget child,Dimension *width,Dimension *height) {
    XtWidgetGeometry reply={0};XtQueryGeometry(child,NULL,&reply);
    *width=(reply.request_mode&CWWidth)?reply.width:child->core.width;
    *height=(reply.request_mode&CWHeight)?reply.height:child->core.height;
}
/* Reserve the widest digit for every possible place, including the sign.
 * Range changes resize the control; changing its value never shifts buttons. */
static void natural_size(XmStepperWidget s,Dimension *width,Dimension *height,Dimension *button) {
    Dimension dw,dh,iw,ih,vw,vh,digit=0,sign=0,tw,th,mw,ml,mr;
    XmRenderTable font;char text[32];unsigned places=1;
    preferred(s->stepper.decrement,&dw,&dh);preferred(s->stepper.increment,&iw,&ih);
    preferred(s->stepper.label,&vw,&vh);
    XtVaGetValues(s->stepper.label,XmNrenderTable,&font,XmNmarginWidth,&mw,
                  XmNmarginLeft,&ml,XmNmarginRight,&mr,NULL);
    for(int i=0;i<11;++i) {
        text[0]=(char)(i==10?'-':'0'+i);text[1]=0;
        XmString string=XmStringCreateLocalized(text);XmStringExtent(font,string,&tw,&th);XmStringFree(string);
        if(i==10)sign=tw;else digit=MAX(digit,tw);
    }
    int endpoints[]={s->stepper.minimum,s->stepper.maximum};
    for(unsigned i=0;i<XtNumber(endpoints);++i) {
        snprintf(text,sizeof(text),"%d",endpoints[i]);
        places=MAX(places,(unsigned)strlen(text)-(text[0]=='-'));
    }
    vw=dimension((unsigned long)places*digit+(s->stepper.minimum<0?sign:0)+2UL*mw+ml+mr);
    *button=MAX(MAX(dw,iw),MAX(dh,ih));
    *height=MAX(vh,*button);
    *width=dimension((unsigned long)vw+2UL*(*button)+2UL*s->stepper.spacing);
}
static void Resize(Widget widget) {
    XmStepperWidget s=(XmStepperWidget)widget;
    if(s->stepper.initializing || s->stepper.updating)return;
    s->stepper.updating=True;
    Dimension nw,nh,b;natural_size(s,&nw,&nh,&b);
    int width=MIN(s->core.width,SHRT_MAX),height=MIN(s->core.height,SHRT_MAX);
    int bw=MIN(b,MAX(0,(width-1)/2));
    int gap=MIN(s->stepper.spacing,MAX(0,(width-2*bw-1)/2));
    int value_width=MAX(1,width-2*bw-2*gap);
    Dimension control_height=(Dimension)MAX(1,height);
    Dimension button_width=(Dimension)bw;
    Dimension label_width=(Dimension)value_width;
    unsigned char direction;XtVaGetValues(widget,XmNlayoutDirection,&direction,NULL);
    Boolean rtl=XmDirectionMatch(direction,XmRIGHT_TO_LEFT);
    XtSetMappedWhenManaged(s->stepper.decrement,bw>0);
    XtSetMappedWhenManaged(s->stepper.increment,bw>0);
    if(bw>0) {
        XtConfigureWidget(s->stepper.decrement,(Position)(rtl?width-bw:0),0,
                          button_width,control_height,0);
        XtConfigureWidget(s->stepper.increment,(Position)(rtl?0:width-bw),0,
                          button_width,control_height,0);
    }
    XtConfigureWidget(s->stepper.label,(Position)(bw+gap),0,label_width,
                      control_height,0);
    s->stepper.updating=False;
}
static Boolean layout_work(XtPointer closure) {
    XmStepperWidget s=(XmStepperWidget)closure;Dimension width,height,b;
    s->stepper.layout_work=0;natural_size(s,&width,&height,&b);
    if(!s->stepper.automatic_width)width=s->core.width;
    if(!s->stepper.automatic_height)height=s->core.height;
    if(width!=s->core.width || height!=s->core.height) {
        XtWidgetGeometry request={0},reply={0};request.request_mode=CWWidth|CWHeight;
        request.width=width;request.height=height;
        if(XtMakeGeometryRequest((Widget)s,&request,&reply)==XtGeometryAlmost) {
            if(reply.request_mode&CWWidth)request.width=reply.width;
            if(reply.request_mode&CWHeight)request.height=reply.height;
            XtMakeGeometryRequest((Widget)s,&request,NULL);
        }
    }
    Resize((Widget)s);return True;
}
static void schedule_layout(XmStepperWidget s) {
    if(!s->stepper.initializing && !s->core.being_destroyed && !s->stepper.layout_work)
        s->stepper.layout_work=XtAppAddWorkProc(XtWidgetToApplicationContext((Widget)s),layout_work,s);
}
static void ChangeManaged(Widget widget){schedule_layout((XmStepperWidget)widget);}
static XtGeometryResult GeometryManager(Widget child,XtWidgetGeometry *request,XtWidgetGeometry *reply) {
    (void)reply;if(!(request->request_mode&XtCWQueryOnly))schedule_layout((XmStepperWidget)XtParent(child));
    return XtGeometryNo;
}
static void refresh(XmStepperWidget s) {
    char text[32];snprintf(text,sizeof(text),"%d",s->stepper.value);
    XmString label=XmStringCreateLocalized(text);
    XtVaSetValues(s->stepper.label,XmNlabelString,label,NULL);XmStringFree(label);
    XtSetSensitive(s->stepper.decrement,s->stepper.minimum<s->stepper.maximum &&
        (s->stepper.wrap || s->stepper.value>s->stepper.minimum));
    XtSetSensitive(s->stepper.increment,s->stepper.minimum<s->stepper.maximum &&
        (s->stepper.wrap || s->stepper.value<s->stepper.maximum));
    schedule_layout(s);
}
static void notify(XmStepperWidget s,XEvent *event) {
    if (!s || s->core.being_destroyed) return;
    XmStepperCallbackStruct c={XmCR_VALUE_CHANGED,event,s->stepper.minimum,s->stepper.maximum,s->stepper.value,s->stepper.step};
    _XmCallCallbacksUntilDestroyed((Widget)s,XmNstepperValueChangedCallback,&c);
}
static void activated(Widget child,XtPointer closure,XtPointer data) {
    XmStepperWidget s=(XmStepperWidget)closure;
    XmPushButtonCallbackStruct *callback=data;
    if(s && XmIsStepper((Widget)s) && !s->core.being_destroyed && XtIsSensitive((Widget)s) && XtIsSensitive(child))
        step(s,child==s->stepper.increment?1:-1,True,callback?callback->event:NULL);
}
static void Events(Widget child,XtPointer closure,XEvent *event,Boolean *dispatch) {
    XmStepperWidget s=(XmStepperWidget)closure;(void)child;
    if(!s || !XmIsStepper((Widget)s) || s->core.being_destroyed || !event || event->type!=KeyPress || !XtIsSensitive((Widget)s) || !XtIsSensitive(child))return;
    KeySym key=XLookupKeysym(&event->xkey,0);int direction=0;Boolean rtl;
    rtl=XmDirectionMatch(s->manager.string_direction,XmRIGHT_TO_LEFT);
    if(key==XK_Up || (key==XK_Right&&!rtl) || (key==XK_Left&&rtl))direction=1;
    else if(key==XK_Down || (key==XK_Left&&!rtl) || (key==XK_Right&&rtl))direction=-1;
    if(direction){*dispatch=False;step(s,direction,True,event);}
}
static void Initialize(Widget request,Widget widget,ArgList args,Cardinal *count) {
    XmStepperWidget s=(XmStepperWidget)widget;(void)args;(void)count;
    s->stepper.initializing=True;s->stepper.updating=False;s->stepper.layout_work=0;
    s->stepper.automatic_width=request->core.width==0;s->stepper.automatic_height=request->core.height==0;
    normalize(s);
    s->stepper.render_table=XmFontListCopy(s->stepper.render_table?s->stepper.render_table:
        XmeGetDefaultRenderTable(widget,XmLABEL_FONTLIST));
    XmString minus=XmStringCreateLocalized("-"),plus=XmStringCreateLocalized("+");
    s->stepper.decrement=XtVaCreateManagedWidget("decrement",xmPushButtonWidgetClass,widget,
        XmNlabelString,minus,XmNrenderTable,s->stepper.render_table,NULL);
    s->stepper.label=XtVaCreateManagedWidget("value",xmLabelWidgetClass,widget,
        XmNalignment,XmALIGNMENT_CENTER,XmNrenderTable,s->stepper.render_table,NULL);
    s->stepper.increment=XtVaCreateManagedWidget("increment",xmPushButtonWidgetClass,widget,
        XmNlabelString,plus,XmNrenderTable,s->stepper.render_table,NULL);
    XmStringFree(minus);XmStringFree(plus);
    Widget children[]={s->stepper.decrement,s->stepper.label,s->stepper.increment};
    for(unsigned i=0;i<XtNumber(children);++i) {
        _XmAppearanceInheritFont(children[i],widget);_XmAppearanceInheritForeground(children[i],widget);
        XtAddEventHandler(children[i],KeyPressMask,False,Events,s);
    }
    XtAddEventHandler(widget,KeyPressMask,False,Events,s);
    XtAddCallback(s->stepper.decrement,XmNactivateCallback,activated,s);
    XtAddCallback(s->stepper.increment,XmNactivateCallback,activated,s);
    refresh(s);s->stepper.initializing=False;
    Dimension width,height,b;natural_size(s,&width,&height,&b);
    if(s->stepper.automatic_width)s->core.width=width;
    if(s->stepper.automatic_height)s->core.height=height;
    Resize(widget);
}
static void Destroy(Widget widget) {
    XmStepperWidget s=(XmStepperWidget)widget;
    if(s->stepper.layout_work)XtRemoveWorkProc(s->stepper.layout_work);
    if(s->stepper.render_table)XmRenderTableFree(s->stepper.render_table);
}
static void step(XmStepperWidget s,int direction,Boolean notify_change,XEvent *event) {
    int64_t next=(int64_t)s->stepper.value+(direction<0?-(int64_t)s->stepper.step:(int64_t)s->stepper.step);
    if(next<s->stepper.minimum || next>s->stepper.maximum)
        next=s->stepper.wrap?(next<s->stepper.minimum?s->stepper.maximum:s->stepper.minimum):
             (next<s->stepper.minimum?s->stepper.minimum:s->stepper.maximum);
    if(s->stepper.value!=(int)next){s->stepper.value=(int)next;refresh(s);if(notify_change)notify(s,event);}
}
static Boolean SetValues(Widget old_widget,Widget request,Widget widget,ArgList args,Cardinal *count) {
    XmStepperWidget old=(XmStepperWidget)old_widget,s=(XmStepperWidget)widget;(void)request;
    normalize(s);
    if(old->stepper.render_table!=s->stepper.render_table) {
        XmRenderTable table=XmFontListCopy(s->stepper.render_table?s->stepper.render_table:
            XmeGetDefaultRenderTable(widget,XmLABEL_FONTLIST));
        if(table) {
            if(old->stepper.render_table)XmRenderTableFree(old->stepper.render_table);
            s->stepper.render_table=table;
            Widget children[]={s->stepper.decrement,s->stepper.label,s->stepper.increment};
            for(unsigned i=0;i<XtNumber(children);++i)if(_XmAppearanceOwnsResource(children[i],XmNrenderTable)) {
                XtVaSetValues(children[i],XmNrenderTable,table,NULL);_XmAppearanceInheritFont(children[i],widget);
            }
        }else s->stepper.render_table=old->stepper.render_table;
    }
    for(Cardinal i=0;i<*count;++i) {
        if(!strcmp(args[i].name,XmNwidth))s->stepper.automatic_width=(Dimension)args[i].value==0;
        if(!strcmp(args[i].name,XmNheight))s->stepper.automatic_height=(Dimension)args[i].value==0;
    }
    refresh(s);Dimension width,height,b;natural_size(s,&width,&height,&b);
    if(s->stepper.automatic_width)s->core.width=width;
    if(s->stepper.automatic_height)s->core.height=height;
    Resize(widget);return False;
}
static XtGeometryResult QueryGeometry(Widget widget,XtWidgetGeometry *request,XtWidgetGeometry *reply) {
    XmStepperWidget s=(XmStepperWidget)widget;Dimension b;
    reply->request_mode=CWWidth|CWHeight;natural_size(s,&reply->width,&reply->height,&b);
    if(!s->stepper.automatic_width)reply->width=s->core.width;
    if(!s->stepper.automatic_height)reply->height=s->core.height;
    return XmeReplyToQueryGeometry(widget,request,reply);
}
void XmStepperGetRange(Widget widget, int *minimum, int *maximum) { XmStepperWidget s; if (minimum) *minimum = 0; if (maximum) *maximum = 0; if (widget == NULL || !XmIsStepper(widget) || widget->core.being_destroyed) return; s = (XmStepperWidget)widget; if (minimum) *minimum = s->stepper.minimum; if (maximum) *maximum = s->stepper.maximum; }
int XmStepperGetValue(Widget widget) { return widget != NULL && XmIsStepper(widget) && !widget->core.being_destroyed ? ((XmStepperWidget)widget)->stepper.value : 0; }
void XmStepperSetRange(Widget widget, int minimum, int maximum, Boolean notify_change) { XmStepperWidget s; int old; if (widget == NULL || !XmIsStepper(widget) || widget->core.being_destroyed) return; s = (XmStepperWidget)widget; old = s->stepper.value; s->stepper.minimum = minimum; s->stepper.maximum = maximum; normalize(s); refresh(s); if (notify_change && old != s->stepper.value) notify(s, NULL); }
void XmStepperSetValue(Widget widget, int value, Boolean notify_change) { XmStepperWidget s; int old; if (widget == NULL || !XmIsStepper(widget) || widget->core.being_destroyed) return; s = (XmStepperWidget)widget; old = s->stepper.value; s->stepper.value = value; normalize(s); refresh(s); if (notify_change && old != s->stepper.value) notify(s, NULL); }
void XmStepperStep(Widget widget, int direction, Boolean notify_change) { if (widget != NULL && XmIsStepper(widget) && !widget->core.being_destroyed) step((XmStepperWidget)widget, direction, notify_change, NULL); }
Widget XmCreateStepper(Widget parent, char *name, ArgList args, Cardinal count) { return XtCreateWidget(name, xmStepperWidgetClass, parent, args, count); }
Widget XmVaCreateStepper(Widget parent, char *name, ...) { Widget widget; va_list var; int count; Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var); Va_start(var, name); widget = XmeVLCreateWidget(name, xmStepperWidgetClass, parent, False, var, count); va_end(var); return widget; }
Widget XmVaCreateManagedStepper(Widget parent, char *name, ...) { Widget widget; va_list var; int count; Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var); Va_start(var, name); widget = XmeVLCreateWidget(name, xmStepperWidgetClass, parent, True, var, count); va_end(var); return widget; }
