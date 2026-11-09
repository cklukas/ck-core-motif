/* Bounded integer editor composed from native text and arrow controls. */
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <errno.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/NumberBoxP.h>
#include <Xm/TextF.h>
#include <Xm/ArrowB.h>
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
static void refresh(XmNumberBoxWidget);
static void schedule_layout(XmNumberBoxWidget);
static void CheckSetRenderTable(Widget widget, int offset, XrmValue *value)
{
    XmNumberBoxWidget number = (XmNumberBoxWidget)widget;
    (void)offset;
    if (number->number_box.check_set_render_table)
        value->addr = NULL;
    else {
        number->number_box.check_set_render_table = True;
        value->addr = (char *)&number->number_box.render_table;
    }
}

#define Offset(field) XtOffsetOf(XmNumberBoxRec, field)
static XtResource resources[] = {
    {XmNnumberMinimum, XmCNumberMinimum, XmRInt, sizeof(int),
     Offset(number_box.minimum), XmRImmediate, (XtPointer)0},
    {XmNnumberMaximum, XmCNumberMaximum, XmRInt, sizeof(int),
     Offset(number_box.maximum), XmRImmediate, (XtPointer)100},
    {XmNnumberValue, XmCNumberValue, XmRInt, sizeof(int),
     Offset(number_box.value), XmRImmediate, (XtPointer)0},
    {XmNnumberStep, XmCNumberStep, XmRInt, sizeof(int),
     Offset(number_box.step), XmRImmediate, (XtPointer)1},
    {XmNnumberPageStep, XmCNumberPageStep, XmRInt, sizeof(int),
     Offset(number_box.page_step), XmRImmediate, (XtPointer)10},
    {XmNnumberWrap, XmCNumberWrap, XmRBoolean, sizeof(Boolean),
     Offset(number_box.wrap), XmRImmediate, (XtPointer)False},
    {XmNnumberValueChangedCallback, XmCNumberValueChangedCallback,
     XmRCallback, sizeof(XtCallbackList), Offset(number_box.value_changed_callback),
     XmRPointer, (XtPointer)NULL},
    {XmNnumberInvalidCallback, XmCNumberInvalidCallback, XmRCallback,
     sizeof(XtCallbackList), Offset(number_box.invalid_callback), XmRPointer,
     (XtPointer)NULL},
    {XmNspacing, XmCSpacing, XmRHorizontalDimension, sizeof(Dimension),
     Offset(number_box.spacing), XmRImmediate, (XtPointer)6},
    {XmNfontList, XmCFontList, XmRFontList, sizeof(XmRenderTable),
     Offset(number_box.render_table), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNrenderTable, XmCRenderTable, XmRRenderTable, sizeof(XmRenderTable),
     Offset(number_box.render_table), XmRCallProc, (XtPointer)CheckSetRenderTable},
    {XmNtraversalOn, XmCTraversalOn, XmRBoolean, sizeof(Boolean),
     Offset(manager.traversal_on), XmRImmediate, (XtPointer)True},
    {XmNshadowThickness, XmCShadowThickness, XmRHorizontalDimension,
     sizeof(Dimension), Offset(manager.shadow_thickness), XmRImmediate, (XtPointer)0}
};
externaldef(xmnumberboxclassrec) XmNumberBoxClassRec xmNumberBoxClassRec = {
    {.superclass=(WidgetClass)&xmManagerClassRec, .class_name="XmNumberBox",
     .widget_size=sizeof(XmNumberBoxRec), .initialize=Initialize,
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
externaldef(xmnumberboxwidgetclass) WidgetClass xmNumberBoxWidgetClass =
    (WidgetClass)&xmNumberBoxClassRec;
static void normalize(XmNumberBoxWidget number) {
    if (number->number_box.maximum < number->number_box.minimum)
        number->number_box.maximum = number->number_box.minimum;
    if (number->number_box.value < number->number_box.minimum)
        number->number_box.value = number->number_box.minimum;
    if (number->number_box.value > number->number_box.maximum)
        number->number_box.value = number->number_box.maximum;
    if (number->number_box.step < 1) number->number_box.step = 1;
    if (number->number_box.page_step < 1)
        number->number_box.page_step = number->number_box.step;
}

static Boolean parse_text(const char *text, int *value) {
    char *end;
    long parsed;
    if (!text || !*text) return False;
    errno = 0;
    parsed = strtol(text, &end, 10);
    if (errno == ERANGE || end == text || *end || parsed < INT_MIN ||
        parsed > INT_MAX)
        return False;
    *value = (int)parsed;
    return True;
}

static Dimension dimension(unsigned long value) {
    return (Dimension)MAX(1,MIN(value,USHRT_MAX));
}
static void preferred(Widget child,Dimension *width,Dimension *height) {
    XtWidgetGeometry reply={0};XtQueryGeometry(child,NULL,&reply);
    *width=(reply.request_mode&CWWidth)?reply.width:child->core.width;
    *height=(reply.request_mode&CWHeight)?reply.height:child->core.height;
}
static void natural_size(XmNumberBoxWidget number,Dimension *width,Dimension *height) {
    Dimension ew,eh,aw,ah,lw=0,lh=0;
    preferred(number->number_box.editor,&ew,&eh);
    preferred(number->number_box.increment,&aw,&ah);
    if(number->number_box.invalid)preferred(number->number_box.error_label,&lw,&lh);
    *width=dimension(MAX((unsigned long)ew+number->number_box.spacing+aw,lw));
    *height=dimension((unsigned long)MAX(eh,ah)+(lh?lh+number->number_box.spacing:0));
}
static void Resize(Widget widget) {
    XmNumberBoxWidget number=(XmNumberBoxWidget)widget;
    if(number->number_box.initializing || number->number_box.updating)return;
    number->number_box.updating=True;
    Dimension aw,ah,lw=0,lh=0;unsigned char direction;
    int width=MIN(number->core.width,SHRT_MAX),height=MIN(number->core.height,SHRT_MAX);
    preferred(number->number_box.increment,&aw,&ah);
    if(number->number_box.invalid)preferred(number->number_box.error_label,&lw,&lh);
    int field_height=MAX(1,height-(lh?MIN((int)lh+number->number_box.spacing,height-1):0));
    int arrow_width=MIN((int)aw,MAX(0,width/3));
    int gap=MIN((int)number->number_box.spacing,MAX(0,width-arrow_width-1));
    int editor_width=MAX(1,width-arrow_width-gap);
    Dimension editor_width_dimension=(Dimension)editor_width;
    Dimension field_height_dimension=(Dimension)field_height;
    Dimension arrow_width_dimension=(Dimension)arrow_width;
    XtVaGetValues(widget,XmNlayoutDirection,&direction,NULL);
    Boolean rtl=XmDirectionMatch(direction,XmRIGHT_TO_LEFT);
    int editor_x=rtl?arrow_width+gap:0,arrow_x=rtl?0:editor_width+gap;
    XtConfigureWidget(number->number_box.editor,(Position)editor_x,0,
                      editor_width_dimension,field_height_dimension,0);
    Boolean arrows=arrow_width>0 && field_height>=2;
    XtSetMappedWhenManaged(number->number_box.increment,arrows);
    XtSetMappedWhenManaged(number->number_box.decrement,arrows);
    if(arrows) {
        int half=field_height/2;
        XtConfigureWidget(number->number_box.increment,(Position)arrow_x,0,
                          arrow_width_dimension,(Dimension)half,0);
        XtConfigureWidget(number->number_box.decrement,(Position)arrow_x,
                          (Position)half,arrow_width_dimension,
                          (Dimension)(field_height-half),0);
    }
    if(lh)XtConfigureWidget(number->number_box.error_label,0,
                           (Position)MAX(0,MIN(height-1,field_height+gap)),
                           (Dimension)width,
                           (Dimension)MAX(1,height-field_height-gap),0);
    number->number_box.updating=False;
}
static Boolean layout_work(XtPointer closure) {
    XmNumberBoxWidget number=(XmNumberBoxWidget)closure;
    number->number_box.layout_work=0;
    Dimension width,height;natural_size(number,&width,&height);
    if(!number->number_box.automatic_width)width=number->core.width;
    if(!number->number_box.automatic_height)height=number->core.height;
    if(width!=number->core.width || height!=number->core.height) {
        XtWidgetGeometry request={0},reply={0};request.request_mode=CWWidth|CWHeight;
        request.width=width;request.height=height;
        if(XtMakeGeometryRequest((Widget)number,&request,&reply)==XtGeometryAlmost) {
            if(reply.request_mode&CWWidth)request.width=reply.width;
            if(reply.request_mode&CWHeight)request.height=reply.height;
            XtMakeGeometryRequest((Widget)number,&request,NULL);
        }
    }
    Resize((Widget)number);return True;
}
static void schedule_layout(XmNumberBoxWidget number) {
    if(!number->number_box.initializing && !number->core.being_destroyed && !number->number_box.layout_work)
        number->number_box.layout_work=XtAppAddWorkProc(XtWidgetToApplicationContext((Widget)number),layout_work,number);
}
static void ChangeManaged(Widget widget) { schedule_layout((XmNumberBoxWidget)widget); }
static XtGeometryResult GeometryManager(Widget child,XtWidgetGeometry *request,XtWidgetGeometry *reply) {
    XmNumberBoxWidget number=(XmNumberBoxWidget)XtParent(child);(void)reply;
    if(!(request->request_mode&XtCWQueryOnly))schedule_layout(number);
    /* Child allocation belongs to this composite, not its tentative request. */
    return XtGeometryNo;
}
static void formatting_destroyed(Widget widget,XtPointer closure,XtPointer data) {
    (void)widget;(void)data;*(Boolean*)closure=True;
}
static Boolean format_value(XmNumberBoxWidget number) {
    char text[32];Boolean destroyed=False;
    Widget widget=(Widget)number,editor=number->number_box.editor;
    snprintf(text,sizeof(text),"%d",number->number_box.value);
    number->number_box.updating=True;
    XtAddCallback(widget,XmNdestroyCallback,formatting_destroyed,&destroyed);
    XmTextFieldSetString(editor,text);
    if (!destroyed && !widget->core.being_destroyed)
        XmTextFieldSetInsertionPosition(editor,(XmTextPosition)strlen(text));
    if (destroyed) return False;
    XtRemoveCallback(widget,XmNdestroyCallback,formatting_destroyed,&destroyed);
    if (widget->core.being_destroyed) return False;
    number->number_box.updating=False;
    return True;
}
static void refresh(XmNumberBoxWidget number) {
    char low[32],high[32],message[128];
    snprintf(low,sizeof(low),"%d",number->number_box.minimum);
    snprintf(high,sizeof(high),"%d",number->number_box.maximum);
    short columns=(short)MAX(3,MAX(strlen(low),strlen(high)));
    short current;XtVaGetValues(number->number_box.editor,XmNcolumns,&current,NULL);
    if(current!=columns)XtVaSetValues(number->number_box.editor,XmNcolumns,columns,NULL);
    snprintf(message,sizeof(message),"Enter an integer from %d to %d.",number->number_box.minimum,number->number_box.maximum);
    XmString label=XmStringCreateLocalized(message);
    XtVaSetValues(number->number_box.error_label,XmNlabelString,label,NULL);XmStringFree(label);
    if(number->number_box.invalid)XtManageChild(number->number_box.error_label);
    else XtUnmanageChild(number->number_box.error_label);
    schedule_layout(number);
}
static void notify_value(XmNumberBoxWidget number,XEvent *event) {
    XmNumberBoxCallbackStruct callback={XmCR_VALUE_CHANGED,event,number->number_box.minimum,
        number->number_box.maximum,number->number_box.value,number->number_box.step,number->number_box.page_step};
    (void)_XmCallCallbacksUntilDestroyed((Widget)number,XmNnumberValueChangedCallback,&callback);
}
static void commit_text(XmNumberBoxWidget number,XEvent *event) {
    char *text=XmTextFieldGetString(number->number_box.editor);int value;
    if(!parse_text(text,&value) || value<number->number_box.minimum || value>number->number_box.maximum) {
        number->number_box.invalid=True;refresh(number);
        XmNumberBoxInvalidCallbackStruct callback={XmCR_VALUE_CHANGED,event,text};
        (void)_XmCallCallbacksUntilDestroyed((Widget)number,XmNnumberInvalidCallback,&callback);
        XtFree(text);return;
    }
    XtFree(text);Boolean changed=value!=number->number_box.value;
    number->number_box.value=value;number->number_box.invalid=False;refresh(number);
    if(changed)notify_value(number,event);
}
static void step_value(XmNumberBoxWidget number,int direction,Boolean page,Boolean notify,XEvent *event) {
    int64_t delta=(int64_t)(page?number->number_box.page_step:number->number_box.step);
    int64_t next=(int64_t)number->number_box.value+(direction<0?-delta:delta);
    if(next<number->number_box.minimum || next>number->number_box.maximum) {
        if(number->number_box.wrap)next=next<number->number_box.minimum?number->number_box.maximum:number->number_box.minimum;
        else next=MAX(number->number_box.minimum,MIN(next,number->number_box.maximum));
    }
    Boolean changed=number->number_box.value!=(int)next;
    number->number_box.value=(int)next;number->number_box.invalid=False;
    if(!format_value(number))return;
    refresh(number);
    if(changed && notify)notify_value(number,event);
}
static void edited(Widget widget,XtPointer closure,XtPointer data) {
    XmNumberBoxWidget number=closure;(void)widget;(void)data;
    if(!number || !XmIsNumberBox((Widget)number) || number->core.being_destroyed || number->number_box.updating)return;
    if(number->number_box.invalid){number->number_box.invalid=False;refresh(number);}
}
static void activated(Widget widget,XtPointer closure,XtPointer data) {
    XmNumberBoxWidget number=closure;XmAnyCallbackStruct *call=data;(void)widget;
    if(number && XmIsNumberBox((Widget)number) && !number->core.being_destroyed && XtIsSensitive((Widget)number))commit_text(number,call?call->event:NULL);
}
static void spin(Widget widget,XtPointer closure,XtPointer data) {
    XmNumberBoxWidget number=closure;XmArrowButtonCallbackStruct *call=data;
    if(!number || !XmIsNumberBox((Widget)number) || number->core.being_destroyed || !XtIsSensitive((Widget)number))return;
    step_value(number,widget==number->number_box.increment?1:-1,False,True,call?call->event:NULL);
}
static void Events(Widget widget,XtPointer closure,XEvent *event,Boolean *dispatch) {
    XmNumberBoxWidget number=closure;(void)widget;
    if(!number || !XmIsNumberBox((Widget)number) || number->core.being_destroyed || !event || event->type!=KeyPress || !XtIsSensitive((Widget)number))return;
    KeySym symbol=XLookupKeysym(&event->xkey,0);
    if(event->xkey.state&(ControlMask|Mod1Mask))return;
    switch(symbol) {
    case XK_Up:case XK_Down:case XK_Page_Up:case XK_Page_Down:
        *dispatch=False;step_value(number,(symbol==XK_Up || symbol==XK_Page_Up)?1:-1,
            symbol==XK_Page_Up || symbol==XK_Page_Down,True,event);return;
    case XK_Escape:*dispatch=False;number->number_box.invalid=False;
        if(format_value(number))refresh(number);
        return;
    default:return;
    }
}
static void Initialize(Widget request,Widget widget,ArgList args,Cardinal *count) {
    XmNumberBoxWidget number=(XmNumberBoxWidget)widget;(void)args;(void)count;
    number->number_box.initializing=True;number->number_box.updating=False;number->number_box.layout_work=0;
    number->number_box.automatic_width=request->core.width==0;
    number->number_box.automatic_height=request->core.height==0;
    normalize(number);
    number->number_box.render_table=XmFontListCopy(number->number_box.render_table?
        number->number_box.render_table:XmeGetDefaultRenderTable(widget,XmTEXT_FONTLIST));
    number->number_box.editor=XtVaCreateManagedWidget("editor",xmTextFieldWidgetClass,widget,
        XmNrenderTable,number->number_box.render_table,XmNmaxLength,63,XmNresizeWidth,False,NULL);
    _XmAppearanceInheritFont(number->number_box.editor,widget);
    number->number_box.increment=XtVaCreateManagedWidget("increment",xmArrowButtonWidgetClass,widget,
        XmNarrowDirection,XmARROW_UP,XmNtraversalOn,False,XmNhighlightThickness,0,NULL);
    number->number_box.decrement=XtVaCreateManagedWidget("decrement",xmArrowButtonWidgetClass,widget,
        XmNarrowDirection,XmARROW_DOWN,XmNtraversalOn,False,XmNhighlightThickness,0,NULL);
    number->number_box.error_label=XtVaCreateWidget("validation",xmLabelWidgetClass,widget,
        XmNalignment,XmALIGNMENT_BEGINNING,XmNrenderTable,number->number_box.render_table,NULL);
    _XmAppearanceInheritFont(number->number_box.error_label,widget);
    Widget color_children[]={number->number_box.editor,number->number_box.increment,
        number->number_box.decrement,number->number_box.error_label};
    for(unsigned i=0;i<XtNumber(color_children);++i)
        _XmAppearanceInheritForeground(color_children[i],widget);
    XtAddEventHandler(number->number_box.editor,KeyPressMask,False,Events,number);
    XtAddCallback(number->number_box.editor,XmNvalueChangedCallback,edited,number);
    XtAddCallback(number->number_box.editor,XmNactivateCallback,activated,number);
    XtAddCallback(number->number_box.increment,XmNactivateCallback,spin,number);
    XtAddCallback(number->number_box.decrement,XmNactivateCallback,spin,number);
    if(!format_value(number))return;
    refresh(number);
    number->number_box.initializing=False;
    Dimension width,height;natural_size(number,&width,&height);
    if(number->number_box.automatic_width)number->core.width=width;
    if(number->number_box.automatic_height)number->core.height=height;
    Resize(widget);
}
static void Destroy(Widget widget) {
    XmNumberBoxWidget number=(XmNumberBoxWidget)widget;
    if(number->number_box.layout_work)XtRemoveWorkProc(number->number_box.layout_work);
    if(number->number_box.render_table)XmRenderTableFree(number->number_box.render_table);
}
static Boolean SetValues(Widget old_widget,Widget request,Widget widget,ArgList args,Cardinal *count) {
    XmNumberBoxWidget old=(XmNumberBoxWidget)old_widget,number=(XmNumberBoxWidget)widget;(void)request;
    normalize(number);
    if(old->number_box.render_table!=number->number_box.render_table) {
        XmRenderTable new_table=XmFontListCopy(number->number_box.render_table?
            number->number_box.render_table:XmeGetDefaultRenderTable(widget,XmTEXT_FONTLIST));
        if(new_table) {
            if(old->number_box.render_table)XmRenderTableFree(old->number_box.render_table);
            number->number_box.render_table=new_table;
            Widget children[]={number->number_box.editor,number->number_box.error_label};
            for(unsigned i=0;i<XtNumber(children);++i)if(_XmAppearanceOwnsResource(children[i],XmNrenderTable)) {
                XtVaSetValues(children[i],XmNrenderTable,new_table,NULL);
                _XmAppearanceInheritFont(children[i],widget);
            }
        } else number->number_box.render_table=old->number_box.render_table;
    }
    if(old->number_box.value!=number->number_box.value || old->number_box.minimum!=number->number_box.minimum ||
       old->number_box.maximum!=number->number_box.maximum) {
        number->number_box.invalid=False;
        if(!format_value(number))return False;
    }
    for(Cardinal i=0;i<*count;++i) {
        if(!strcmp(args[i].name,XmNwidth))number->number_box.automatic_width=(Dimension)args[i].value==0;
        if(!strcmp(args[i].name,XmNheight))number->number_box.automatic_height=(Dimension)args[i].value==0;
    }
    refresh(number);
    Dimension width,height;natural_size(number,&width,&height);
    if(number->number_box.automatic_width)number->core.width=width;
    if(number->number_box.automatic_height)number->core.height=height;
    Resize(widget);return False;
}
static XtGeometryResult QueryGeometry(Widget widget,XtWidgetGeometry *request,XtWidgetGeometry *reply) {
    XmNumberBoxWidget number=(XmNumberBoxWidget)widget;
    reply->request_mode=CWWidth|CWHeight;natural_size(number,&reply->width,&reply->height);
    if(!number->number_box.automatic_width)reply->width=number->core.width;
    if(!number->number_box.automatic_height)reply->height=number->core.height;
    return XmeReplyToQueryGeometry(widget,request,reply);
}
void XmNumberBoxGetRange(Widget widget, int *minimum, int *maximum) {
    XmNumberBoxWidget number;
    if (minimum) *minimum = 0;
    if (maximum) *maximum = 0;
    if (widget == NULL || !XmIsNumberBox(widget) || widget->core.being_destroyed)
        return;
    number = (XmNumberBoxWidget)widget;
    if (minimum) *minimum = number->number_box.minimum;
    if (maximum) *maximum = number->number_box.maximum;
}
int XmNumberBoxGetValue(Widget widget) {
    return widget != NULL && XmIsNumberBox(widget) && !widget->core.being_destroyed
        ? ((XmNumberBoxWidget)widget)->number_box.value : 0;
}
void XmNumberBoxSetRange(Widget widget, int minimum, int maximum, Boolean notify) {
    XmNumberBoxWidget number;
    int old;
    if (widget == NULL || !XmIsNumberBox(widget) || widget->core.being_destroyed)
        return;
    number = (XmNumberBoxWidget)widget; old = number->number_box.value;
    number->number_box.minimum = minimum; number->number_box.maximum = maximum;
    normalize(number); number->number_box.invalid = False;
    if(!format_value(number))return;
    refresh(number);
    if (notify && old != number->number_box.value) notify_value(number, NULL);
}
void XmNumberBoxSetValue(Widget widget, int value, Boolean notify) {
    XmNumberBoxWidget number;
    int old;
    if (widget == NULL || !XmIsNumberBox(widget) || widget->core.being_destroyed) return;
    number = (XmNumberBoxWidget)widget; old = number->number_box.value;
    number->number_box.value = value; normalize(number);
    if(!format_value(number))return;
    number->number_box.invalid = False; refresh(number);
    if (notify && old != number->number_box.value) notify_value(number, NULL);
}
void XmNumberBoxStep(Widget widget, int direction, Boolean page, Boolean notify) {
    if (widget != NULL && XmIsNumberBox(widget) && !widget->core.being_destroyed)
        step_value((XmNumberBoxWidget)widget, direction, page, notify, NULL);
}
Boolean XmNumberBoxIsInvalid(Widget widget) {
    return widget != NULL && XmIsNumberBox(widget) && !widget->core.being_destroyed
        ? ((XmNumberBoxWidget)widget)->number_box.invalid : False;
}
Widget XmNumberBoxGetTextField(Widget widget) {
    return widget != NULL && XmIsNumberBox(widget) && !widget->core.being_destroyed
        ? ((XmNumberBoxWidget)widget)->number_box.editor : NULL;
}
Widget XmCreateNumberBox(Widget parent, char *name, ArgList args, Cardinal count) {
    return XtCreateWidget(name, xmNumberBoxWidgetClass, parent, args, count);
}
Widget XmVaCreateNumberBox(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name, xmNumberBoxWidgetClass,
        parent, False, var, count); va_end(var); return widget;
}
Widget XmVaCreateManagedNumberBox(Widget parent, char *name, ...) {
    Widget widget; va_list var; int count;
    Va_start(var, name); count = XmeCountVaListSimple(var); va_end(var);
    Va_start(var, name); widget = XmeVLCreateWidget(name, xmNumberBoxWidgetClass,
        parent, True, var, count); va_end(var); return widget;
}
