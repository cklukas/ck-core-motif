/* 
 * Motif
 *
 * Copyright (c) 1987-2012, The Open Group. All rights reserved.
 *
 * These libraries and programs are free software; you can
 * redistribute them and/or modify them under the terms of the GNU
 * Lesser General Public License as published by the Free Software
 * Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * These libraries and programs are distributed in the hope that
 * they will be useful, but WITHOUT ANY WARRANTY; without even the
 * implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
 * PURPOSE. See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with these librararies and programs; if not, write
 * to the Free Software Foundation, Inc., 51 Franklin Street, Fifth
 * Floor, Boston, MA 02110-1301 USA
*/ 
/* 
 * HISTORY
*/ 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: Dog.c /main/7 1997/05/02 10:11:08 dbl $"
#endif
#endif

/*****************************************************************************
*
*  Dog.c - Dog widget source file
*  
******************************************************************************/

#include <stdio.h>
#include <limits.h>
#include <X11/StringDefs.h>
#include <Xm/XmP.h>
#include <Xm/DrawP.h>
#include "DogP.h"

#ifdef USING_UIL
#include <Mrm/MrmAppl.h>
#endif /* USING_UIL */

#include "up.bm"
#include "down.bm"
#include "bark.bm"

#ifndef MAX
#define MAX(x,y)        ((x) > (y) ? (x) : (y))
#endif

#define WagTime(w) XmField(w,offsets,Dog,wag_time, int)
#define BarkTime(w) XmField(w,offsets,Dog,bark_time, int)
#define BarkCallback(w) XmField(w,offsets,Dog,bark_callback,XtCallbackList)
#define UpPixmap(w) XmField(w,offsets,Dog,up_pixmap,Pixmap)
#define DownPixmap(w) XmField(w,offsets,Dog,down_pixmap,Pixmap)
#define BarkPixmap(w) XmField(w,offsets,Dog,bark_pixmap,Pixmap)
#define CurrPixmap(w) XmField(w,offsets,Dog,curr_pixmap,Pixmap)
#define CurrPx(w) XmField(w,offsets,Dog,curr_px,int)
#define Wagging(w) XmField(w,offsets,Dog,wagging,Boolean)
#define Barking(w) XmField(w,offsets,Dog,barking,Boolean)
#define DrawGC(w) XmField(w,offsets,Dog,draw_GC,GC)
#define PixmapX(w) XmField(w,offsets,Dog,pixmap_x,Position)
#define PixmapY(w) XmField(w,offsets,Dog,pixmap_y,Position)
#define DrawX(w) XmField(w,offsets,Dog,draw_x,Position)
#define DrawY(w) XmField(w,offsets,Dog,draw_y,Position)
#define DrawWidth(w) XmField(w,offsets,Dog,draw_width,Dimension)
#define DrawHeight(w) XmField(w,offsets,Dog,draw_height,Dimension)
#define CurrWidth(w) XmField(w,offsets,Dog,curr_width,Dimension)
#define CurrHeight(w) XmField(w,offsets,Dog,curr_height,Dimension)
#define HighlightThickness(w) \
		XmField(w,offsets,XmPrimitive,highlight_thickness,Dimension)
#define ShadowThickness(w) \
		XmField(w,offsets,XmPrimitive,shadow_thickness,Dimension)
#define Foreground(w)	XmField(w,offsets,XmPrimitive,foreground,Pixel)
#define Highlighted(w) XmField(w,offsets,XmPrimitive,highlighted,Boolean)
#define TopShadowGC(w) XmField(w,offsets,XmPrimitive,top_shadow_GC,GC)
#define BottomShadowGC(w) XmField(w,offsets,XmPrimitive,bottom_shadow_GC,GC)
#define BackgroundPixel(w) XmField(w,offsets,Core,background_pixel,Pixel)
#define Width(w) XmField(w,offsets,Core,width,Dimension)
#define Height(w) XmField(w,offsets,Core,height,Dimension)
#define WagId(w) XmField(w,offsets,Dog,wagId,XtIntervalId)
#define BarkId(w) XmField(w,offsets,Dog,barkId,XtIntervalId)

#define SetPixmap(w, px, pixmap, width, height) \
    CurrPx(w) = px; CurrPixmap(w) = pixmap; \
    CurrWidth(w) = width; CurrHeight(w) = height

#define MakePixmap(b,wd,ht) \
    XCreatePixmapFromBitmapData(XtDisplay(w),RootWindowOfScreen(XtScreen(w)), \
    (char *)(b), (unsigned int)(wd), (unsigned int)(ht), Foreground(w), \
    BackgroundPixel(w), (unsigned int)DefaultDepthOfScreen(XtScreen(w)))

static void ClassInitialize(void);
static void Initialize(Widget request, Widget new_widget, ArgList args,
		       Cardinal *num_args);
static void Redisplay(Widget w, XEvent *event, Region region);
static void Resize(Widget w);
static void Destroy(Widget w);
static Boolean SetValues(Widget current, Widget request, Widget new_widget,
			 ArgList args, Cardinal *num_args);
static XtGeometryResult QueryGeometry(Widget w, XtWidgetGeometry *intended,
				      XtWidgetGeometry *reply);

static void bark_dog(Widget w, XEvent *event, String *params,
		     Cardinal *num_params);
static void end_bark(XtPointer client_data, XtIntervalId *id);
static void start_wag(Widget w, XEvent *event, String *params,
		      Cardinal *num_params);
static void stop_wag(Widget w, XEvent *event, String *params,
		     Cardinal *num_params);
static void do_wag(XtPointer client_data, XtIntervalId *id);
static void create_GC(DogWidget w);
static void create_pixmaps(DogWidget w);
static void destroy_pixmaps(DogWidget w);
static Position clamp_position(Dimension value);
static unsigned long timeout_delay(int value);

static char defaultTranslations[] =
   "<Btn1Down>:         Bark()\n\
    ~Shift<Btn2Down>:   StartWag()\n\
    Shift<Btn2Down>:    StopWag()\n\
    <Key>Return:        Bark()\n\
    Ctrl <Key>Return:   Bark()\n\
    <Key>osfActivate:   Bark()\n\
    <Key>space:         Bark()\n\
    Ctrl <Key>space:    Bark()\n\
    <Key>osfSelect:     Bark()\n\
    <Key>W:             StartWag()\n\
    <Key>S:             StopWag()\n\
    <Key>osfHelp:       PrimitiveHelp()";

static XtActionsRec actionsList[] = {
    { "Bark", (XtActionProc) bark_dog},
    { "StartWag", (XtActionProc) start_wag},
    { "StopWag", (XtActionProc) stop_wag}
};

static XmPartResource resources[] = {
    {DogNwagTime, DogCWagTime, XtRInt, sizeof(int),
	XmPartOffset(Dog,wag_time), XmRImmediate, (caddr_t)100},
    {DogNbarkTime, DogCBarkTime, XtRInt, sizeof(int),
	XmPartOffset(Dog,bark_time), XmRImmediate, (caddr_t)1000},
    {DogNbarkCallback, XtCCallback, XtRCallback, sizeof(caddr_t),
	XmPartOffset(Dog,bark_callback), XtRCallback, NULL}
};

DogClassRec dogClassRec = {
    {                                   /* core_class fields    */
    (WidgetClass) &xmPrimitiveClassRec, /* superclass           */
    "Dog",                              /* class_name           */
    sizeof(DogPart),                    /* widget_size          */
    ClassInitialize,                    /* class_initialize     */
    NULL,                               /* class_part_initialize*/
    False,                              /* class_inited         */
    Initialize,                         /* initialize           */
    NULL,                               /* initialize_notify    */
    XtInheritRealize,                   /* realize              */
    actionsList,                        /* actions              */
    XtNumber(actionsList),              /* num_actions          */
    (XtResourceList)resources,          /* resources            */
    XtNumber(resources),                /* num_resources        */
    NULLQUARK,                          /* xrm_class            */
    True,                               /* compress_motion      */
    True,                               /* compress_exposure    */
    True,                               /* compress_enterleave  */
    False,                              /* visible_interest     */
    Destroy,                            /* destroy              */
    Resize,                             /* resize               */
    Redisplay,                          /* expose               */
    SetValues,                          /* set_values           */
    NULL,                               /* set_values_hook      */
    XtInheritSetValuesAlmost,           /* set_values_almost    */
    NULL,                               /* get_values_hook      */
    NULL,                               /* accept_focus         */
    XtVersionDontCheck,                 /* version              */
    NULL,                               /* callback_private     */
    defaultTranslations,                /* tm_table             */
    QueryGeometry,                      /* query_geometry       */
    NULL,                               /* disp accelerator     */
    NULL                                /* extension            */
    },
    {				        /* primitive_class record */
    XmInheritWidgetProc,               	/* border_highlight     */
    XmInheritWidgetProc,               	/* border_unhighlight   */
    XtInheritTranslations,       	/* translations         */
    bark_dog,                           /* arm_and_activate     */
    NULL,   	    			/* syn resources        */
    0,					/* num syn_resources    */
    NULL,                             	/* extension            */
    },
    {					/* dog_class record     */
    NULL,                             	/* extension            */
    }
};

externaldef(dogwidgetclass) WidgetClass dogWidgetClass =
						(WidgetClass) &dogClassRec;

static XmOffsetPtr offsets; /* Part Offset table for XmResolvePartOffsets */

/**********************************************************************
 *
 * DogCreate - Convenience routine, used by Uil/Mrm.
 *
 *********************************************************************/

Widget
DogCreate(Widget parent, char *name, ArgList arglist, Cardinal nargs)
{
    return(XtCreateWidget (name, dogWidgetClass, parent, arglist, nargs));
}

#ifdef USING_UIL
/**********************************************************************
 *
 * DogMrmInitialize - register Dog widget class with Mrm
 *
 *********************************************************************/

int
DogMrmInitialize(void)
{
    return (int)MrmRegisterClass(MrmwcUnknown, "Dog", "DogCreate", DogCreate,
				 (WidgetClass)&dogClassRec);
}
#endif /* USING_UIL */

/**********************************************************************
 *
 * _DogDrawPixmap - draw the current pixmap
 *
 *********************************************************************/

void
_DogDrawPixmap(DogWidget dw)
{
    Widget w = (Widget) dw;

    if (XtIsRealized(w)) {
	XCopyArea(XtDisplay(w),CurrPixmap(w),
		XtWindow(w),DrawGC(w),
		PixmapX(w), PixmapY(w),
		DrawWidth(w), DrawHeight(w),
		DrawX(w), DrawY(w));
    }
}

/**********************************************************************
 *
 * _DogPosition(w) - position the current pixmap
 *
 *********************************************************************/

void
_DogPosition(DogWidget w)
{
    Dimension margin = ShadowThickness(w) + HighlightThickness(w);
    Dimension inner_width =
	Width(w) > 2 * margin ? Width(w) - 2 * margin : 0;
    Dimension inner_height =
	Height(w) > 2 * margin ? Height(w) - 2 * margin : 0;

    if (CurrWidth(w) < inner_width) {
	PixmapX(w) = 0;
	DrawX(w) = Width(w)/2 - CurrWidth(w)/2;
	DrawWidth(w) = CurrWidth(w);
    }
    else {
	PixmapX(w) = (CurrWidth(w) - inner_width) / 2;
	DrawX(w) = clamp_position(margin);
	DrawWidth(w) = inner_width;
    }

    if (CurrHeight(w) < inner_height) {
	PixmapY(w)= 0;
	DrawY(w) = Height(w)/2 - CurrHeight(w)/2;
	DrawHeight(w) = CurrHeight(w);
    }
    else {
	PixmapY(w) = (CurrHeight(w) - inner_height) / 2;
	DrawY(w) = clamp_position(margin);
	DrawHeight(w) = inner_height;
    }
}

static Position
clamp_position(Dimension value)
{
    return value > (Dimension)SHRT_MAX ? (Position)SHRT_MAX : (Position)value;
}

static unsigned long
timeout_delay(int value)
{
    return value > 0 ? (unsigned long)value : 1UL;
}

/**********************************************************************
 *
 * Class methods
 *
 *********************************************************************/

static void
ClassInitialize(void)
{
    XmResolvePartOffsets(dogWidgetClass, &offsets);
}


static void
create_GC(DogWidget w)
{
    XGCValues       values;
    XtGCMask        valueMask;

    valueMask = GCForeground | GCBackground | GCGraphicsExposures;
    values.foreground = Foreground(w);
    values.background = BackgroundPixel(w);
    values.graphics_exposures = False;
    DrawGC(w) = XtGetGC((Widget)w,valueMask,&values);
}

static void
create_pixmaps(DogWidget w)
{
    UpPixmap(w) = MakePixmap(up_bits, up_width, up_height);
    DownPixmap(w) = MakePixmap(down_bits, down_width, down_height);
    BarkPixmap(w) = MakePixmap(bark_bits, bark_width, bark_height);
}

static void
Initialize(Widget request_widget, Widget new_widget, ArgList args,
	   Cardinal *num_args)
{
    DogWidget request = (DogWidget)request_widget;
    DogWidget new = (DogWidget)new_widget;

    (void)args;
    (void)num_args;
    if (Width(request) == 0)
	Width(new) = MAX(MAX(up_width, down_width),bark_width) +
		2*(ShadowThickness(new)+HighlightThickness(new));
    if (Height(request) == 0)
	Height(new) = MAX(MAX(up_height, down_height),bark_height) +
		2*(ShadowThickness(new)+HighlightThickness(new));
    create_GC(new);
    create_pixmaps(new);
    SetPixmap(new, DownPx, DownPixmap(new), down_width, down_height);
    Wagging(new) = False;
    Barking(new) = False;
    WagId(new) = 0;
    BarkId(new) = 0;

    Resize((Widget)new);
}

static void
destroy_pixmaps(DogWidget w)
{
    XFreePixmap (XtDisplay(w), UpPixmap(w));
    XFreePixmap (XtDisplay(w), DownPixmap(w));
    XFreePixmap (XtDisplay(w), BarkPixmap(w));
}


static void
Destroy(Widget widget)
{
    DogWidget w = (DogWidget)widget;
    XtReleaseGC ((Widget)w, DrawGC(w));
    destroy_pixmaps(w);

    if (WagId(w) != 0)
                XtRemoveTimeOut( WagId(w));
    if (BarkId(w) != 0)
                XtRemoveTimeOut( BarkId(w));

    XtRemoveAllCallbacks ((Widget)w, DogNbarkCallback);
}

static void
Resize(Widget widget)
{
    DogWidget w = (DogWidget)widget;
    _DogPosition(w);
}

static Boolean
DifferentBackground(Widget w, Widget p)
{
  if (XmIsPrimitive(w) && XmIsManager(p)) 
    {
      Pixel w_bg, p_bg;
      Pixmap w_px, p_px;
      
      XtVaGetValues(w, XmNbackground, &w_bg, XmNbackgroundPixmap, &w_px, NULL);
      XtVaGetValues(p, XmNbackground, &p_bg, XmNbackgroundPixmap, &p_px, NULL);
      
      return ((w_bg == p_bg) && (w_px == p_px));
    }
  
  return (False);
}

static void
Redisplay(Widget w, XEvent *event, Region region)
{
    (void)event;
    (void)region;
    if (XtIsRealized(w)) {
	XmeDrawShadows(XtDisplay (w), XtWindow (w),
		       TopShadowGC(w), BottomShadowGC(w), 
		       HighlightThickness(w), HighlightThickness(w),
		       Width(w) - 2 * HighlightThickness(w),
		       Height(w) - 2 * HighlightThickness(w),
		       ShadowThickness(w),
		       XmSHADOW_OUT);

	if (Highlighted(w))
	   (*((DogWidgetClass)XtClass(w)) ->
	    primitive_class.border_highlight)((Widget)w);
	else if (DifferentBackground ((Widget)w, XtParent (w)))
	   (*((DogWidgetClass)XtClass(w)) ->
	      primitive_class.border_unhighlight)((Widget)w);

	_DogDrawPixmap((DogWidget)w);
    }
}

static Boolean
SetValues(Widget current_widget, Widget request_widget, Widget new_widget,
	  ArgList args, Cardinal *num_args)
{
    DogWidget current = (DogWidget)current_widget;
    DogWidget request = (DogWidget)request_widget;
    DogWidget new = (DogWidget)new_widget;
    Boolean redraw = False;

    (void)request;
    (void)args;
    (void)num_args;

    if (ShadowThickness(new) != ShadowThickness(current) ||
	HighlightThickness(new) != HighlightThickness(current)) {
	_DogPosition(new);
	redraw = True;
    }
    if (Foreground(new) != Foreground(current) ||
        BackgroundPixel(new) != BackgroundPixel(current)) {
	XtReleaseGC ((Widget)current, DrawGC(current));
        create_GC(new);
	destroy_pixmaps(new);
	create_pixmaps(new);
	switch (CurrPx(new)) {
	    case(UpPx) : 
		SetPixmap(new,UpPx,UpPixmap(new),up_width,up_height);
		break;
	    case(DownPx) : 
		SetPixmap(new,DownPx,DownPixmap(new),down_width,down_height);
		break;
	    case(BarkPx) : 
		SetPixmap(new,BarkPx,BarkPixmap(new),bark_width,bark_height);
		break;
	}
	redraw = True;
    }
    return (redraw);
}

static XtGeometryResult
QueryGeometry(Widget widget, XtWidgetGeometry *intended,
	      XtWidgetGeometry *reply)
{
    DogWidget w = (DogWidget)widget;
    reply->request_mode = 0;

    if ((intended->request_mode &
	 ~(XtGeometryMask)(CWWidth | CWHeight)) != 0)
        return (XtGeometryNo);

    reply->request_mode = (CWWidth | CWHeight);
    reply->width = MAX(MAX(down_width,up_width),bark_width) +
			2*(ShadowThickness(w)+HighlightThickness(w));
    reply->height = MAX(MAX(down_height,up_height),bark_height) +
			2*(ShadowThickness(w)+HighlightThickness(w));

    if (reply->width != intended->width ||
	reply->height != intended->height ||
	intended->request_mode != reply->request_mode)
	return (XtGeometryAlmost);
    else {
	reply->request_mode = 0;
	return (XtGeometryYes);
    }
}
/**********************************************************************
 *
 * Widget actions
 *
 *********************************************************************/

static void
bark_dog(Widget widget, XEvent *event, String *params, Cardinal *num_params)
{
    DogWidget w = (DogWidget)widget;

    (void)event;
    (void)params;
    (void)num_params;
    XmProcessTraversal((Widget)w, XmTRAVERSE_CURRENT);
    XtCallCallbacks ((Widget)w, DogNbarkCallback, NULL);
    if (Barking(w) == True) return;
    Barking(w) = True;
    SetPixmap(w,BarkPx,BarkPixmap(w),bark_width,bark_height);
    _DogPosition(w);
    _DogDrawPixmap(w);
    BarkId(w) = XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)w),
				timeout_delay(BarkTime(w)), end_bark, w);
}

static void
end_bark(XtPointer client_data, XtIntervalId *id)
{
    DogWidget w = (DogWidget)client_data;

    (void)id;
    SetPixmap(w,DownPx,DownPixmap(w),down_width,down_height);
    _DogPosition(w);
    _DogDrawPixmap(w);
    Barking(w) = False;
    BarkId(w) = 0;
    if (Wagging(w) == True)
        WagId(w) = XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)w),
					timeout_delay(WagTime(w)), do_wag, w);

}

static void
start_wag(Widget widget, XEvent *event, String *params, Cardinal *num_params)
{
    DogWidget w = (DogWidget)widget;

    (void)event;
    (void)params;
    (void)num_params;
    XmProcessTraversal((Widget)w, XmTRAVERSE_CURRENT);
    if (Wagging(w) == True) return;
    Wagging(w) = True;
    if (Barking(w) == True) return;
    WagId(w) = XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)w),
			       timeout_delay(WagTime(w)), do_wag, w);
}

static void
stop_wag(Widget widget, XEvent *event, String *params, Cardinal *num_params)
{
    DogWidget w = (DogWidget)widget;

    (void)event;
    (void)params;
    (void)num_params;
    XmProcessTraversal((Widget)w, XmTRAVERSE_CURRENT);
    Wagging(w) = False;
    if (WagId(w) != 0)
                XtRemoveTimeOut( WagId(w));
    WagId(w) = 0;

}

static void
do_wag(XtPointer client_data, XtIntervalId *id)
{
    DogWidget dw = (DogWidget)client_data;

    (void)id;

    if (Barking(dw) == True || Wagging(dw) == False) {
	WagId(dw) = 0;
	return;
    }
    switch(CurrPx(dw)) {
	case(UpPx):
	    SetPixmap(dw,DownPx,DownPixmap(dw),down_width,down_height);
	    break;
	case(DownPx):
	    SetPixmap(dw,UpPx,UpPixmap(dw),up_width,up_height);
	    break;
    }
    _DogPosition(dw);
    _DogDrawPixmap(dw);
    WagId(dw) = XtAppAddTimeOut(XtWidgetToApplicationContext((Widget)dw),
				  timeout_delay(WagTime(dw)), do_wag, dw);
}

