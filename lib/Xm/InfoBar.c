#ifdef HAVE_CONFIG_H
#include <config.h>
#endif
#include <stdio.h>
#include <limits.h>
#include <string.h>
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/InfoBarP.h>
#include <Xm/XmP.h>
#include "PrimitiveI.h"
#include "XmI.h"
static void Initialize(Widget,Widget,ArgList,Cardinal*); static void Redisplay(Widget,XEvent*,Region); static Boolean SetValues(Widget,Widget,Widget,ArgList,Cardinal*); static void Destroy(Widget); static XtGeometryResult QueryGeometry(Widget,XtWidgetGeometry*,XtWidgetGeometry*); static void Events(Widget,XtPointer,XEvent*,Boolean*);
static void CheckSetRenderTable(Widget widget,int offset,XrmValue*value){XmInfoBarWidget bar=(XmInfoBarWidget)widget;(void)offset;if(bar->info.check_set_render_table)value->addr=NULL;else{bar->info.check_set_render_table=True;value->addr=(char*)&bar->info.font;}}
#define Offset(field) XtOffsetOf(XmInfoBarRec,field)
static XtResource resources[]={{XmNinfoMessage,XmCInfoMessage,XmRString,sizeof(String),Offset(info.message),XmRImmediate,(XtPointer)""},{XmNinfoSeverity,XmCInfoSeverity,XmRUnsignedChar,sizeof(unsigned char),Offset(info.severity),XmRImmediate,(XtPointer)XmINFO_SEVERITY_INFO},{XmNinfoClosable,XmCInfoClosable,XmRBoolean,sizeof(Boolean),Offset(info.closable),XmRImmediate,(XtPointer)True},{XmNinfoActionLabel,XmCInfoActionLabel,XmRString,sizeof(String),Offset(info.action_label),XmRImmediate,(XtPointer)NULL},{XmNinfoDismissCallback,XmCInfoDismissCallback,XmRCallback,sizeof(XtCallbackList),Offset(info.dismiss_callback),XmRPointer,(XtPointer)NULL},{XmNinfoActionCallback,XmCInfoActionCallback,XmRCallback,sizeof(XtCallbackList),Offset(info.action_callback),XmRPointer,(XtPointer)NULL},{XmNfontList,XmCFontList,XmRFontList,sizeof(XmRenderTable),Offset(info.font),XmRCallProc,(XtPointer)CheckSetRenderTable},{XmNrenderTable,XmCRenderTable,XmRRenderTable,sizeof(XmRenderTable),Offset(info.font),XmRCallProc,(XtPointer)CheckSetRenderTable},{XmNtraversalOn,XmCTraversalOn,XmRBoolean,sizeof(Boolean),Offset(primitive.traversal_on),XmRImmediate,(XtPointer)True},{XmNhighlightThickness,XmCHighlightThickness,XmRHorizontalDimension,sizeof(Dimension),Offset(primitive.highlight_thickness),XmRCallProc,(XtPointer)_XmSetThickness}};
externaldef(xminfobarclassrec) XmInfoBarClassRec xmInfoBarClassRec={{(WidgetClass)&xmPrimitiveClassRec,"XmInfoBar",sizeof(XmInfoBarRec),NULL,NULL,False,Initialize,NULL,XtInheritRealize,NULL,0,resources,XtNumber(resources),NULLQUARK,True,XtExposeCompressMaximal,True,False,Destroy,NULL,Redisplay,SetValues,NULL,XtInheritSetValuesAlmost,NULL,NULL,XtVersion,NULL,XtInheritTranslations,QueryGeometry,NULL,NULL},{XmInheritBorderHighlight,XmInheritBorderUnhighlight,XtInheritTranslations,NULL,NULL,0,NULL},{NULL}};
externaldef(xminfobarwidgetclass) WidgetClass xmInfoBarWidgetClass=(WidgetClass)&xmInfoBarClassRec;
static void Initialize(Widget request,Widget widget,ArgList args,Cardinal*count){XmInfoBarWidget bar=(XmInfoBarWidget)widget;XGCValues values;Display *display=XtDisplay(widget);(void)request;(void)args;(void)count;if(bar->info.severity>XmINFO_SEVERITY_ERROR)bar->info.severity=XmINFO_SEVERITY_INFO;if(!bar->core.width)bar->core.width=360;if(!bar->core.height)bar->core.height=34;if(bar->info.font==NULL)bar->info.font=XmFontListCopy(XmeGetDefaultRenderTable(widget,XmLABEL_FONTLIST));else bar->info.font=XmFontListCopy(bar->info.font);values.foreground=bar->primitive.foreground;values.background=bar->core.background_pixel;bar->info.gc=XCreateGC(display,DefaultRootWindow(display),GCForeground|GCBackground,&values);XtAddEventHandler(widget,KeyPressMask|ButtonPressMask,False,Events,NULL);}
static void Destroy(Widget widget){XmInfoBarWidget bar=(XmInfoBarWidget)widget;XtRemoveEventHandler(widget,KeyPressMask|ButtonPressMask,False,Events,NULL);if(bar->info.gc)XFreeGC(XtDisplay(widget),bar->info.gc);if(bar->info.font)XmRenderTableFree(bar->info.font);}
static void Redisplay(Widget widget, XEvent *event, Region region)
{
    XmInfoBarWidget bar = (XmInfoBarWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    const char *prefix;
    XmString prefix_string, message_string, action_string = NULL;
    Dimension prefix_width = 0, prefix_height = 0;
    Dimension message_width = 0, message_height = 0;
    Dimension action_width = 0, action_height = 0;

    (void)event;
    (void)region;
    if (!window || !bar->info.gc)
        return;

    XClearWindow(display, window);
    prefix = bar->info.severity == XmINFO_SEVERITY_ERROR ? "Error: " :
             bar->info.severity == XmINFO_SEVERITY_WARNING ? "Warning: " :
             "Info: ";
    if (!bar->info.font) return;
    prefix_string = XmStringCreateLocalized((char *)prefix);
    message_string = XmStringCreateLocalized(bar->info.message ? bar->info.message : "");
    if (bar->info.action_label)
        action_string = XmStringCreateLocalized(bar->info.action_label);
    if (prefix_string) XmStringExtent(bar->info.font, prefix_string, &prefix_width, &prefix_height);
    if (message_string) XmStringExtent(bar->info.font, message_string, &message_width, &message_height);
    if (action_string) XmStringExtent(bar->info.font, action_string, &action_width, &action_height);
    if (prefix_string)
        XmStringDraw(display, window, bar->info.font, prefix_string, bar->info.gc,
                     8, ((int)bar->core.height - (int)prefix_height) / 2,
                     prefix_width, XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(bar), NULL);
    if (message_string)
        XmStringDraw(display, window, bar->info.font, message_string, bar->info.gc,
                     8 + (int)prefix_width, ((int)bar->core.height - (int)message_height) / 2,
                     message_width, XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(bar), NULL);
    if (action_string)
        XmStringDraw(display, window, bar->info.font, action_string, bar->info.gc,
                     (int)bar->core.width - 100, ((int)bar->core.height - (int)action_height) / 2,
                     action_width, XmALIGNMENT_BEGINNING,
                     XmPrim_layout_direction(bar), NULL);
    if (bar->info.closable && bar->core.width >= 24 &&
        bar->core.height >= 18) {
        int button_x = (int)bar->core.width - 22;
        int button_y = ((int)bar->core.height - 18) / 2;
        XDrawRectangle(display, window, bar->info.gc,
                       button_x, button_y, 18, 18);
        XDrawLine(display, window, bar->info.gc,
                  button_x + 5, button_y + 6,
                  button_x + 13, button_y + 16);
        XDrawLine(display, window, bar->info.gc,
                  button_x + 13, button_y + 6,
                  button_x + 5, button_y + 16);
    }
    if (prefix_string) XmStringFree(prefix_string);
    if (message_string) XmStringFree(message_string);
    if (action_string) XmStringFree(action_string);
}
static void Events(Widget widget,XtPointer data,XEvent*event,Boolean*dispatch){XmInfoBarWidget bar=(XmInfoBarWidget)widget;(void)data;(void)dispatch;if(event->type==ButtonPress){int close_x=(int)bar->core.width>=24?(int)bar->core.width-22:0;int action_x=(int)bar->core.width>120?(int)bar->core.width-120:0;if(bar->info.closable&&bar->core.width>=24&&bar->core.height>=18&&event->xbutton.x>=close_x){XmInfoBarDismiss(widget);return;}if(bar->info.action_label&&event->xbutton.x>=action_x){XmInfoBarInvokeAction(widget);return;}}if(event->type==KeyPress){KeySym key;char input[2];XLookupString(&event->xkey,input,sizeof(input),&key,NULL);if(key==XK_Escape&&bar->info.closable)XmInfoBarDismiss(widget);}}
static Boolean SetValues(Widget old_widget,Widget request,Widget widget,ArgList args,Cardinal*count){XmInfoBarWidget old=(XmInfoBarWidget)old_widget,bar=(XmInfoBarWidget)widget;Boolean redraw=False;(void)request;(void)args;(void)count;if(old->info.font!=bar->info.font){XmRenderTable new_font=bar->info.font?XmFontListCopy(bar->info.font):XmFontListCopy(XmeGetDefaultRenderTable(widget,XmLABEL_FONTLIST));if(new_font){if(old->info.font)XmRenderTableFree(old->info.font);bar->info.font=new_font;redraw=True;}else bar->info.font=old->info.font;}if(old->info.message!=bar->info.message||old->info.action_label!=bar->info.action_label||old->info.severity!=bar->info.severity||old->info.closable!=bar->info.closable)redraw=True;return redraw;}
static XtGeometryResult QueryGeometry(Widget widget,XtWidgetGeometry*request,XtWidgetGeometry*reply){(void)widget;(void)request;reply->request_mode=CWWidth|CWHeight;reply->width=360;reply->height=34;return XtGeometryAlmost;}
void XmInfoBarDismiss(Widget widget){XmInfoBarWidget bar;XmInfoBarCallbackStruct c={XmCR_UNMAP,NULL,0};if(widget==NULL||!XmIsInfoBar(widget))return;bar=(XmInfoBarWidget)widget;c.severity=bar->info.severity;XtCallCallbacks(widget,XmNinfoDismissCallback,&c);}
void XmInfoBarInvokeAction(Widget widget){XmInfoBarCallbackStruct c={XmCR_ACTIVATE,NULL,0};if(widget!=NULL&&XmIsInfoBar(widget))XtCallCallbacks(widget,XmNinfoActionCallback,&c);}
Widget XmCreateInfoBar(Widget parent,char*name,ArgList args,Cardinal count){return XtCreateWidget(name,xmInfoBarWidgetClass,parent,args,count);}
