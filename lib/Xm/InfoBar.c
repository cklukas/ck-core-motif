#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include "PaintI.h"
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/InfoBarP.h>
#include <Xm/XmP.h>
#include "PrimitiveI.h"
#include "XmI.h"
static void Initialize(Widget,Widget,ArgList,Cardinal*); static void BufferedRedisplay(Widget,XEvent*,Region); static Boolean BufferedSetValues(Widget,Widget,Widget,ArgList,Cardinal*); static void Destroy(Widget); static XtGeometryResult QueryGeometry(Widget,XtWidgetGeometry*,XtWidgetGeometry*); static void Events(Widget,XtPointer,XEvent*,Boolean*);
static void CheckSetRenderTable(Widget widget,int offset,XrmValue*value){XmInfoBarWidget bar=(XmInfoBarWidget)widget;(void)offset;if(bar->info.check_set_render_table)value->addr=NULL;else{bar->info.check_set_render_table=True;value->addr=(char*)&bar->info.font;}}
static Position bounded_position(long long value){if(value<SHRT_MIN)return(Position)SHRT_MIN;if(value>SHRT_MAX)return(Position)SHRT_MAX;return(Position)value;}
static Boolean IsRtl(XmInfoBarWidget bar){return XmDirectionMatch(XmPrim_layout_direction(bar),XmRIGHT_TO_LEFT);}
static int CloseLeft(XmInfoBarWidget bar){return IsRtl(bar)?4:(int)bar->core.width-22;}
static int ContentLeft(XmInfoBarWidget bar){int left=8;if(bar->info.closable&&bar->core.width>=24&&IsRtl(bar))left=30;return left;}
static int ContentRight(XmInfoBarWidget bar){int right=(int)bar->core.width-8;if(bar->info.closable&&bar->core.width>=24&&!IsRtl(bar))right=(int)bar->core.width-30;return right>ContentLeft(bar)?right:ContentLeft(bar);}
static int ActionWidth(XmInfoBarWidget bar){XmString label;Dimension width=0,height=0;if(!bar->info.action_label||!bar->info.font)return 0;label=XmStringCreateLocalized(bar->info.action_label);if(label){XmStringExtent(bar->info.font,label,&width,&height);XmStringFree(label);}return MIN((int)width,ContentRight(bar)-ContentLeft(bar));}
#define Offset(field) XtOffsetOf(XmInfoBarRec,field)
static XtResource resources[]={{XmNinfoMessage,XmCInfoMessage,XmRString,sizeof(String),Offset(info.message),XmRImmediate,(XtPointer)""},{XmNinfoSeverity,XmCInfoSeverity,XmRUnsignedChar,sizeof(unsigned char),Offset(info.severity),XmRImmediate,(XtPointer)XmINFO_SEVERITY_INFO},{XmNinfoClosable,XmCInfoClosable,XmRBoolean,sizeof(Boolean),Offset(info.closable),XmRImmediate,(XtPointer)True},{XmNinfoActionLabel,XmCInfoActionLabel,XmRString,sizeof(String),Offset(info.action_label),XmRImmediate,(XtPointer)NULL},{XmNinfoDismissCallback,XmCInfoDismissCallback,XmRCallback,sizeof(XtCallbackList),Offset(info.dismiss_callback),XmRPointer,(XtPointer)NULL},{XmNinfoActionCallback,XmCInfoActionCallback,XmRCallback,sizeof(XtCallbackList),Offset(info.action_callback),XmRPointer,(XtPointer)NULL},{XmNfontList,XmCFontList,XmRFontList,sizeof(XmRenderTable),Offset(info.font),XmRCallProc,(XtPointer)CheckSetRenderTable},{XmNrenderTable,XmCRenderTable,XmRRenderTable,sizeof(XmRenderTable),Offset(info.font),XmRCallProc,(XtPointer)CheckSetRenderTable},{XmNtraversalOn,XmCTraversalOn,XmRBoolean,sizeof(Boolean),Offset(primitive.traversal_on),XmRImmediate,(XtPointer)True},{XmNhighlightThickness,XmCHighlightThickness,XmRHorizontalDimension,sizeof(Dimension),Offset(primitive.highlight_thickness),XmRCallProc,(XtPointer)_XmSetThickness}};
externaldef(xminfobarclassrec) XmInfoBarClassRec xmInfoBarClassRec={{(WidgetClass)&xmPrimitiveClassRec,"XmInfoBar",sizeof(XmInfoBarRec),NULL,NULL,False,Initialize,NULL,XtInheritRealize,NULL,0,resources,XtNumber(resources),NULLQUARK,True,XtExposeCompressMaximal,True,False,Destroy,NULL,BufferedRedisplay,BufferedSetValues,NULL,XtInheritSetValuesAlmost,NULL,NULL,XtVersion,NULL,(String)_XmPrimitiveTabFocusTranslations,QueryGeometry,NULL,NULL},{XmInheritBorderHighlight,XmInheritBorderUnhighlight,XtInheritTranslations,NULL,NULL,0,NULL},{NULL}};
externaldef(xminfobarwidgetclass) WidgetClass xmInfoBarWidgetClass=(WidgetClass)&xmInfoBarClassRec;
static void Initialize(Widget request,Widget widget,ArgList args,Cardinal*count){XmInfoBarWidget bar=(XmInfoBarWidget)widget;XGCValues values;Display *display=XtDisplay(widget);(void)request;(void)args;(void)count;if(bar->info.severity>XmINFO_SEVERITY_ERROR)bar->info.severity=XmINFO_SEVERITY_INFO;if(!bar->core.width)bar->core.width=360;if(!bar->core.height)bar->core.height=34;if(bar->info.font==NULL)bar->info.font=XmFontListCopy(XmeGetDefaultRenderTable(widget,XmLABEL_FONTLIST));else bar->info.font=XmFontListCopy(bar->info.font);values.foreground=bar->primitive.foreground;values.background=bar->core.background_pixel;bar->info.gc=XCreateGC(display,DefaultRootWindow(display),GCForeground|GCBackground,&values);XtAddEventHandler(widget,KeyPressMask|ButtonPressMask,False,Events,NULL);}
static void Destroy(Widget widget){XmInfoBarWidget bar=(XmInfoBarWidget)widget;XtRemoveEventHandler(widget,KeyPressMask|ButtonPressMask,False,Events,NULL);if(bar->info.gc)XFreeGC(XtDisplay(widget),bar->info.gc);if(bar->info.font)XmRenderTableFree(bar->info.font);}
static void Redisplay(Widget widget, XEvent *event, Region region)
{
    XmInfoBarWidget bar = (XmInfoBarWidget)widget;
    Display *display = XtDisplay(widget);
    Drawable window = _XmPaintDrawable(widget);
    const char *prefix;
    XmString prefix_string, message_string, content_string, action_string = NULL;
    Dimension content_width = 0, content_height = 0;
    Dimension action_width = 0, action_height = 0;
    int content_left, content_right, action_left, action_right, message_left, message_right;
    Boolean rtl;

    (void)event;
    (void)region;
    if (!window || !bar->info.gc)
        return;


    prefix = bar->info.severity == XmINFO_SEVERITY_ERROR ? "Error: " :
             bar->info.severity == XmINFO_SEVERITY_WARNING ? "Warning: " :
             "Info: ";
    if (!bar->info.font) return;
    prefix_string = XmStringCreateLocalized((char *)prefix);
    message_string = XmStringCreateLocalized(bar->info.message ? bar->info.message : "");
    content_string = prefix_string && message_string ? XmStringConcat(prefix_string,message_string) : NULL;
    if (bar->info.action_label)
        action_string = XmStringCreateLocalized(bar->info.action_label);
    if (content_string) XmStringExtent(bar->info.font, content_string, &content_width, &content_height);
    (void)content_width;
    if (action_string) XmStringExtent(bar->info.font, action_string, &action_width, &action_height);
    rtl=IsRtl(bar);
    content_left=ContentLeft(bar);
    content_right = ContentRight(bar);
    action_left=content_left;action_right=content_right;
    if(action_string){if(rtl)action_right=MIN(content_right,content_left+(int)action_width);else action_left=MAX(content_left,content_right-(int)action_width);}
    message_left=content_left;message_right=content_right;
    if(action_string){if(rtl)message_left=MIN(content_right,action_right+8);else message_right=MAX(content_left,action_left-8);}
    if (content_string)
        XmStringDraw(display, window, bar->info.font, content_string, bar->info.gc,
                     bounded_position(message_left),
                     bounded_position(((long long)bar->core.height - content_height) / 2),
                     (Dimension)(message_right-message_left), XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(bar), NULL);
    if (action_string)
        XmStringDraw(display, window, bar->info.font, action_string, bar->info.gc,
                     bounded_position(action_left),
                     bounded_position(((long long)bar->core.height - action_height) / 2),
                     (Dimension)(action_right-action_left), XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(bar), NULL);
    if (bar->info.closable && bar->core.width >= 24 &&
        bar->core.height >= 18) {
        int button_x = CloseLeft(bar);
        int button_y = ((int)bar->core.height - 18) / 2;
        int button_cx = button_x + 9;
        int button_cy = button_y + 9;
        XDrawRectangle(display, window, bar->info.gc,
                       button_x, button_y, 18, 18);
        XDrawLine(display, window, bar->info.gc,
                  button_cx - 4, button_cy - 4,
                  button_cx + 4, button_cy + 4);
        XDrawLine(display, window, bar->info.gc,
                  button_cx + 4, button_cy - 4,
                  button_cx - 4, button_cy + 4);
    }
    if (prefix_string) XmStringFree(prefix_string);
    if (message_string) XmStringFree(message_string);
    if (content_string) XmStringFree(content_string);
    if (action_string) XmStringFree(action_string);
}

static void BufferedRedisplay(Widget widget, XEvent *event, Region region)
{
    _XmBufferedPaint(widget, Redisplay, event, region);
}
static void Events(Widget widget,XtPointer data,XEvent*event,Boolean*dispatch){XmInfoBarWidget bar=(XmInfoBarWidget)widget;(void)data;(void)dispatch;if(bar==NULL||event==NULL||!XmIsInfoBar(widget)||widget->core.being_destroyed||!XtIsSensitive(widget))return;if(!_XmPrimitiveFocusOnPrimaryClick(widget,event))return;if(event->type==ButtonPress){int close_x=bar->core.width>=24?CloseLeft(bar):0;int close_y=bar->core.height>=18?((int)bar->core.height-18)/2:0;int action_left=ContentLeft(bar),action_right=ContentRight(bar);if(bar->info.action_label){int width=ActionWidth(bar);if(IsRtl(bar))action_right=MIN(action_right,action_left+width);else action_left=MAX(action_left,action_right-width);}if(bar->info.closable&&bar->core.width>=24&&bar->core.height>=18&&event->xbutton.x>=close_x&&event->xbutton.x<close_x+18&&event->xbutton.y>=close_y&&event->xbutton.y<close_y+18){XmInfoBarDismiss(widget);return;}if(bar->info.action_label&&event->xbutton.x>=action_left&&event->xbutton.x<action_right){XmInfoBarInvokeAction(widget);return;}}if(event->type==KeyPress){KeySym key;char input[2];XLookupString(&event->xkey,input,sizeof(input),&key,NULL);if(key==XK_Escape&&bar->info.closable){if(dispatch)*dispatch=False;XmInfoBarDismiss(widget);}}}
static Boolean SetValues(Widget old_widget,Widget request,Widget widget,ArgList args,Cardinal*count){XmInfoBarWidget old=(XmInfoBarWidget)old_widget,bar=(XmInfoBarWidget)widget;Boolean redraw=False;Boolean colors_changed=old->primitive.foreground!=bar->primitive.foreground||old->core.background_pixel!=bar->core.background_pixel;(void)request;(void)args;(void)count;if(colors_changed&&bar->info.gc){XSetForeground(XtDisplay(widget),bar->info.gc,bar->primitive.foreground);XSetBackground(XtDisplay(widget),bar->info.gc,bar->core.background_pixel);redraw=True;}if(old->info.font!=bar->info.font){XmRenderTable new_font=bar->info.font?XmFontListCopy(bar->info.font):XmFontListCopy(XmeGetDefaultRenderTable(widget,XmLABEL_FONTLIST));if(new_font){if(old->info.font)XmRenderTableFree(old->info.font);bar->info.font=new_font;redraw=True;}else bar->info.font=old->info.font;}if(old->info.message!=bar->info.message||old->info.action_label!=bar->info.action_label||old->info.severity!=bar->info.severity||old->info.closable!=bar->info.closable||XmPrim_layout_direction(old)!=XmPrim_layout_direction(bar))redraw=True;return redraw;}
static XtGeometryResult QueryGeometry(Widget widget,XtWidgetGeometry*request,XtWidgetGeometry*reply){reply->request_mode=CWWidth|CWHeight;reply->width=widget->core.width;reply->height=widget->core.height;return XmeReplyToQueryGeometry(widget,request,reply);}
void XmInfoBarDismiss(Widget widget){XmInfoBarWidget bar;XmInfoBarCallbackStruct c={XmCR_UNMAP,NULL,0};if(widget==NULL||!XmIsInfoBar(widget)||widget->core.being_destroyed)return;bar=(XmInfoBarWidget)widget;c.severity=bar->info.severity;/* Dismissal must visibly remove the banner even when an application only observes the callback. Unmanage before callbacks so a callback may safely replace or destroy the bar without a later widget access here. */if(XtIsManaged(widget))XtUnmanageChild(widget);XtCallCallbacks(widget,XmNinfoDismissCallback,&c);}
void XmInfoBarInvokeAction(Widget widget){XmInfoBarCallbackStruct c={XmCR_ACTIVATE,NULL,0};if(widget!=NULL&&XmIsInfoBar(widget)&&!widget->core.being_destroyed)XtCallCallbacks(widget,XmNinfoActionCallback,&c);}
Widget XmCreateInfoBar(Widget parent,char*name,ArgList args,Cardinal count){return XtCreateWidget(name,xmInfoBarWidgetClass,parent,args,count);}

/* Xt's default redisplay request clears the front window. Queue our complete
 * frame instead, after resource and geometry negotiation has finished. */
static Boolean BufferedSetValues(Widget old, Widget request, Widget widget,
                                ArgList args, Cardinal *count)
{
    if (SetValues(old, request, widget, args, count))
        _XmPaintInvalidate(widget);
    return False;
}
