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
static int bounded_text_length(const char *text){size_t length=text?strlen(text):0;return length>(size_t)INT_MAX?INT_MAX:(int)length;}
#define Offset(field) XtOffsetOf(XmInfoBarRec,field)
static XtResource resources[]={{XmNinfoMessage,XmCInfoMessage,XmRString,sizeof(String),Offset(info.message),XmRImmediate,(XtPointer)""},{XmNinfoSeverity,XmCInfoSeverity,XmRUnsignedChar,sizeof(unsigned char),Offset(info.severity),XmRImmediate,(XtPointer)XmINFO_SEVERITY_INFO},{XmNinfoClosable,XmCInfoClosable,XmRBoolean,sizeof(Boolean),Offset(info.closable),XmRImmediate,(XtPointer)True},{XmNinfoActionLabel,XmCInfoActionLabel,XmRString,sizeof(String),Offset(info.action_label),XmRImmediate,(XtPointer)NULL},{XmNinfoDismissCallback,XmCInfoDismissCallback,XmRCallback,sizeof(XtCallbackList),Offset(info.dismiss_callback),XmRPointer,(XtPointer)NULL},{XmNinfoActionCallback,XmCInfoActionCallback,XmRCallback,sizeof(XtCallbackList),Offset(info.action_callback),XmRPointer,(XtPointer)NULL},{XmNtraversalOn,XmCTraversalOn,XmRBoolean,sizeof(Boolean),Offset(primitive.traversal_on),XmRImmediate,(XtPointer)True},{XmNhighlightThickness,XmCHighlightThickness,XmRHorizontalDimension,sizeof(Dimension),Offset(primitive.highlight_thickness),XmRCallProc,(XtPointer)_XmSetThickness}};
externaldef(xminfobarclassrec) XmInfoBarClassRec xmInfoBarClassRec={{(WidgetClass)&xmPrimitiveClassRec,"XmInfoBar",sizeof(XmInfoBarRec),NULL,NULL,False,Initialize,NULL,XtInheritRealize,NULL,0,resources,XtNumber(resources),NULLQUARK,True,XtExposeCompressMaximal,True,False,Destroy,NULL,Redisplay,SetValues,NULL,XtInheritSetValuesAlmost,NULL,NULL,XtVersion,NULL,XtInheritTranslations,QueryGeometry,NULL,NULL},{XmInheritBorderHighlight,XmInheritBorderUnhighlight,XtInheritTranslations,NULL,NULL,0,NULL},{NULL}};
externaldef(xminfobarwidgetclass) WidgetClass xmInfoBarWidgetClass=(WidgetClass)&xmInfoBarClassRec;
static void Initialize(Widget request,Widget widget,ArgList args,Cardinal*count){XmInfoBarWidget bar=(XmInfoBarWidget)widget;XGCValues values;Display *display=XtDisplay(widget);(void)request;(void)args;(void)count;if(bar->info.severity>XmINFO_SEVERITY_ERROR)bar->info.severity=XmINFO_SEVERITY_INFO;if(!bar->core.width)bar->core.width=360;if(!bar->core.height)bar->core.height=34;bar->info.font=XLoadQueryFont(display,"fixed");values.foreground=bar->primitive.foreground;values.background=bar->core.background_pixel;if(bar->info.font)values.font=bar->info.font->fid;bar->info.gc=XCreateGC(display,DefaultRootWindow(display),GCForeground|GCBackground|(bar->info.font?GCFont:0),&values);XtAddEventHandler(widget,KeyPressMask|ButtonPressMask,False,Events,NULL);}
static void Destroy(Widget widget){XmInfoBarWidget bar=(XmInfoBarWidget)widget;XtRemoveEventHandler(widget,KeyPressMask|ButtonPressMask,False,Events,NULL);if(bar->info.gc)XFreeGC(XtDisplay(widget),bar->info.gc);if(bar->info.font)XFreeFont(XtDisplay(widget),bar->info.font);}
static void Redisplay(Widget widget, XEvent *event, Region region)
{
    XmInfoBarWidget bar = (XmInfoBarWidget)widget;
    Display *display = XtDisplay(widget);
    Window window = XtWindow(widget);
    const char *prefix;
    XFontStruct *font;
    int baseline;

    (void)event;
    (void)region;
    if (!window || !bar->info.gc)
        return;

    XClearWindow(display, window);
    prefix = bar->info.severity == XmINFO_SEVERITY_ERROR ? "Error: " :
             bar->info.severity == XmINFO_SEVERITY_WARNING ? "Warning: " :
             "Info: ";
    font = bar->info.font;
    if (!font)
        return;

    baseline = (int)((bar->core.height + font->ascent - font->descent) / 2);
    XDrawString(display, window, bar->info.gc, 8, baseline, prefix,
                bounded_text_length(prefix));
    {
        size_t prefix_length = strlen(prefix);
        long long message_x = 8 + (long long)prefix_length * 7;
        int bounded_x = message_x > INT_MAX ? INT_MAX :
                        (message_x < INT_MIN ? INT_MIN : (int)message_x);
    XDrawString(display, window, bar->info.gc, bounded_x,
                baseline, bar->info.message ? bar->info.message : "",
                bounded_text_length(bar->info.message));
    }
    if (bar->info.action_label)
        XDrawString(display, window, bar->info.gc,
                    (int)bar->core.width - 100, baseline,
                    bar->info.action_label, bounded_text_length(bar->info.action_label));
    if (bar->info.closable && bar->core.width >= 18 &&
        bar->core.height >= 8) {
        int button_x = (int)bar->core.width - 24;
        int button_y = ((int)bar->core.height - 22) / 2;
        XDrawRectangle(display, window, bar->info.gc,
                       button_x, button_y, 18, 22);
        XDrawLine(display, window, bar->info.gc,
                  button_x + 5, button_y + 6,
                  button_x + 13, button_y + 16);
        XDrawLine(display, window, bar->info.gc,
                  button_x + 13, button_y + 6,
                  button_x + 5, button_y + 16);
    }
    XFreeFont(display, font);
}
static void Events(Widget widget,XtPointer data,XEvent*event,Boolean*dispatch){XmInfoBarWidget bar=(XmInfoBarWidget)widget;(void)data;(void)dispatch;if(event->type==ButtonPress){int close_x=(int)bar->core.width>28?(int)bar->core.width-28:0;int action_x=(int)bar->core.width>120?(int)bar->core.width-120:0;if(bar->info.closable&&event->xbutton.x>=close_x){XmInfoBarDismiss(widget);return;}if(bar->info.action_label&&event->xbutton.x>=action_x){XmInfoBarInvokeAction(widget);return;}}if(event->type==KeyPress){KeySym key;char input[2];XLookupString(&event->xkey,input,sizeof(input),&key,NULL);if(key==XK_Escape&&bar->info.closable)XmInfoBarDismiss(widget);}}
static Boolean SetValues(Widget old_widget,Widget request,Widget widget,ArgList args,Cardinal*count){XmInfoBarWidget old=(XmInfoBarWidget)old_widget,bar=(XmInfoBarWidget)widget;Boolean redraw=False;(void)request;(void)args;(void)count;if(old->info.message!=bar->info.message||old->info.action_label!=bar->info.action_label||old->info.severity!=bar->info.severity||old->info.closable!=bar->info.closable)redraw=True;return redraw;}
static XtGeometryResult QueryGeometry(Widget widget,XtWidgetGeometry*request,XtWidgetGeometry*reply){(void)widget;(void)request;reply->request_mode=CWWidth|CWHeight;reply->width=360;reply->height=34;return XtGeometryAlmost;}
void XmInfoBarDismiss(Widget widget){XmInfoBarWidget bar;XmInfoBarCallbackStruct c={XmCR_UNMAP,NULL,0};if(!XmIsInfoBar(widget))return;bar=(XmInfoBarWidget)widget;c.severity=bar->info.severity;XtCallCallbacks(widget,XmNinfoDismissCallback,&c);}
void XmInfoBarInvokeAction(Widget widget){XmInfoBarCallbackStruct c={XmCR_ACTIVATE,NULL,0};if(XmIsInfoBar(widget))XtCallCallbacks(widget,XmNinfoActionCallback,&c);}
Widget XmCreateInfoBar(Widget parent,char*name,ArgList args,Cardinal count){return XtCreateWidget(name,xmInfoBarWidgetClass,parent,args,count);}
