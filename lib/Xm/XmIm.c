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
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

 
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$TOG: XmIm.c /main/28 1997/10/13 14:57:31 cshi $"
#endif
#endif
  
#include <stdio.h>
#include <limits.h>
#include <stdint.h>
#include <string.h>
#include <wchar.h>
#include <wctype.h>
#include <Xm/DisplayP.h>
#include <Xm/DrawP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/VendorSEP.h>
#include <Xm/VendorSP.h>
#include <Xm/XmosP.h>		/* for bzero */
#include "BaseClassI.h"
#include "MessagesI.h"
#include "XmI.h"
#include "XmImI.h"
#include <X11/Xlib.h>
  
#define FIX_1534
  
# include <stdarg.h>
# define Va_start(a,b) va_start(a,b)

#define FIX_1510
#define FIX_1129
#define FIX_1196

#ifdef NO_XICPROC
typedef Bool (*XICProc)( XIC, XPointer, XPointer);
#endif

/* Data structures:
 *	While multiple XIMs are not currently supported, some thought
 * was given to how they might be implemented.  Currently both
 * XmImDisplayInfo and XmImShellInfo contain per-XIM fields.  Also the
 * locale and XmNinputMethod are implicit in the XmImDisplayInfo.
 * The back-pointer for the original source of shared XICs is perhaps
 * overly general, but will ease XmPER_MANAGER sharing if implemented.
 *
 * If an XIC is shared among several widgets, all will reference a
 * single XmImXICInfo.
 */

typedef struct _XmImRefRec {
  Cardinal	num_refs;	/* Number of referencing widgets. */
  Cardinal	max_refs;	/* Maximum length of refs array. */
  Widget*	refs;		/* Array of referencing widgets. */
  XIMCallback   **callbacks;
} XmImRefRec, *XmImRefInfo;

typedef struct _PreeditBufferRec {
  unsigned short length;
  wchar_t 	 *text;
  XIMFeedback  	 *feedback;
  int 		 caret;
  XIMCaretStyle	 style;
} PreeditBufferRec, *PreeditBuffer;

typedef struct _XmImXICRec {
  struct _XmImXICRec *next;	/* Links all have the same XIM. */
  XIC		xic;		/* The XIC. */
  Window	focus_window;	/* Cached information about the XIC. */
  XIMStyle	input_style;	/* ...ditto... */
  int		status_width;	/* ...ditto... */
  int		preedit_width;	/* ...ditto... */
  int		sp_height;	/* ...ditto... */
  Boolean preedit_active;
  Boolean	has_focus;	/* Does this XIC have keyboard focus. */
  Boolean	anonymous;	/* Do we have exclusive rights to this XIC. */
  XmImRefRec	widget_refs;	/* Widgets referencing this XIC. */
  struct _XmImXICRec **source; /* Original source of shared XICs. */
  PreeditBuffer preedit_buffer;
} XmImXICRec, *XmImXICInfo;

typedef struct _XmImShellRec {
  /* per-Shell fields. */
  Widget 	im_widget;	/* Dummy widget to make intrinsics behave. */
  Widget	current_widget;	/* Widget whose visual we're matching. */
  
  /* per <Shell,XIM> fields. */
  XmImXICInfo	shell_xic;	/* For PER_SHELL sharing policy. */
  XmImXICInfo	iclist;		/* All known XICs for this <XIM,Shell>. */
} XmImShellRec, *XmImShellInfo;

/* Only toolkit-defined resources have copyable ownership contracts.  Opaque
 * application-created XICs remain application-owned across server loss. */
typedef struct _XmImRegistrationRec {
  struct _XmImRegistrationRec *next;
  Widget widget;
  ArgList args;
  Cardinal count;
  Boolean focused;
  Boolean external;
} XmImRegistrationRec, *XmImRegistration;

typedef struct {
  /* per-Display fields. */
  XmImRegistration registrations;
  Display *display;
  String application_name, application_class;
  Boolean instantiate_registered, transitioning, restoring;
  XContext	current_xics;	/* Map widget -> current XmImXICInfo. */
  
  /* per-XIM fields. */
  XIM		xim;		/* The XIM. */
  XIMStyles	*styles;	/* XNQueryInputStyle result. */
  XmImRefRec	shell_refs;	/* Shells referencing this XIM. */
} XmImDisplayRec, *XmImDisplayInfo;


/*
 * Although the current implementation of XVaNestedList is similar
 * to an Xt ArgList, this is not guaranteed by the spec.  The only
 * approved interface for creating XVaNestedLists takes pairs of
 * (char*, XPointer) paramters.
 */
typedef struct {
  char    *name;
  XPointer value;
} VaArg;

typedef struct {
  Cardinal count;
  Cardinal max;
  VaArg   *args;
} VaArgListRec, *VaArgList;


/********    Static Function Declarations    ********/

static int add_sp(String name,
		  XPointer value,
		  VaArgList slp,
		  VaArgList plp,
		  VaArgList vlp);
static int add_p(String name,
		 XPointer value,
		 VaArgList slp,
		 VaArgList plp,
		 VaArgList vlp);
static int add_fs(String name,
		  XPointer value,
		  VaArgList slp,
		  VaArgList plp,
		  VaArgList vlp);
static int add_bgpxmp(String name,
		      XPointer value,
		      VaArgList slp,
		      VaArgList plp,
		      VaArgList vlp);

static XIMStyle check_style(XIMStyles *styles,
			    XIMStyle preedit_style,
			    XIMStyle status_style);
static int ImGetGeo(Widget vw,
		    XmImXICInfo this_icp );
static void ImSetGeo(Widget vw,
		     XmImXICInfo this_icp );
static void ImGeoReq(Widget vw);
static XFontSet extract_fontset(XmFontList fl);
static XmImDisplayInfo get_xim_info(Widget w);
static XmImRegistration remember_widget(XmImDisplayInfo info, Widget w);
static void forget_widget(XmImDisplayInfo info, Widget w);
static void remember_values(XmImDisplayInfo info, Widget w, ArgList args, Cardinal n);
static void im_available(Display *display, XPointer closure, XPointer call_data);
static void im_destroyed(XIM xim, XPointer closure, XPointer call_data);
static Boolean open_input_method(XmImDisplayInfo info);
static XtPointer* get_im_info_ptr(Widget w,
				  Boolean create);
static XmImShellInfo get_im_info(Widget w,
				 Boolean create);
static void draw_separator(Widget vw);
static void null_proc(Widget w,
		      XtPointer ptr,
		      XEvent *ev,
		      Boolean *b);
static void ImCountVaList(va_list var,
			  int *total_count);
static ArgList ImCreateArgList(va_list var,
			       int total_count);
     
static XmImXICInfo create_xic_info(Widget	   shell,
				   XmImDisplayInfo xim_info,
				   XmImShellInfo   im_info,
#if NeedWidePrototypes
				   unsigned int    input_policy);
#else
                                   XmInputPolicy   input_policy);
#endif /*NeedWidePrototypes*/
static XmImXICInfo recreate_xic_info(XIC		xic,
                                     Widget		shell,
				     XmImDisplayInfo xim_info,
				     XmImShellInfo   im_info);
static void set_values(Widget w,
		       ArgList args,
		       Cardinal num_args,
#if NeedWidePrototypes
		       unsigned int  policy);
#else
                       XmInputPolicy policy);
#endif /*NeedWidePrototypes*/
     
static XmImXICInfo get_current_xic(XmImDisplayInfo xim_info,
				   Widget 	   widget);
static void set_current_xic(XmImXICInfo     xic_info, 
			    XmImDisplayInfo xim_info,
			    Widget 	    widget);
static void unset_current_xic(XmImXICInfo     xic_info,
			      XmImShellInfo   im_info,
			      XmImDisplayInfo xim_info,
			      Widget 	      widget);
     
static Cardinal add_ref(XmImRefInfo refs,
			Widget      widget);
static Cardinal remove_ref(XmImRefInfo refs,
			   Widget      widget);
     
static XVaNestedList VaCopy(VaArgList   list);
static void VaSetArg(VaArgList   list,
		     char       *name,
		     XPointer    value);

static int ImPreeditStartCallback(XIC xic,
				  XPointer client_data,
				  XPointer call_data);
static void ImPreeditDoneCallback(XIC xic,
				  XPointer client_data,
				  XPointer call_data);
static void ImPreeditDrawCallback(XIC xic,
				  XPointer client_data,
				  XPointer call_data);
static void ImPreeditCaretCallback(XIC xic,
				   XPointer client_data,
				   XPointer call_data);
static void ImFreePreeditBuffer(PreeditBuffer pb);
static void set_callback_values(Widget w,
                    String name,
                    XIMCallback *value,
                    VaArgList vlp,
                    XmInputPolicy input_policy);
static void regist_real_callback(Widget w,
                     const XIMCallback *call,
                     int swc);
static XICProc get_real_callback(Widget w,
                  int swc,
		  Widget *real_widget);
static void move_preedit_string(XmImXICInfo icp,
			  Widget wfrom,
			  Widget wto);
     
     /********    End Static Function Declarations    ********/
     
     
typedef int (*XmImResLProc)(String, XPointer,
			   VaArgList, VaArgList, VaArgList);
     
     
typedef struct {
  String xmstring;
  String xstring;
  XrmName xrmname;
  XmImResLProc proc;
} XmImResListRec;
     
static XmImResListRec XmImResList[] = {
  {XmNbackground,       XNBackground,       NULLQUARK, add_sp},
  {XmNforeground,       XNForeground,       NULLQUARK, add_sp},
  {XmNbackgroundPixmap, XNBackgroundPixmap, NULLQUARK, add_bgpxmp},
  {XmNspotLocation,     XNSpotLocation,     NULLQUARK, add_p},
  {XmNfontList,         XNFontSet,          NULLQUARK, add_fs},
  {XmNrenderTable,      XNFontSet,          NULLQUARK, add_fs},
  {XmNlineSpace,        XNLineSpace,        NULLQUARK, add_sp},
  {XmNarea,             XNArea,             NULLQUARK, add_p},
  {XmNpreeditStartCallback, XNPreeditStartCallback, NULLQUARK, NULL},
  {XmNpreeditDoneCallback,  XNPreeditDoneCallback,  NULLQUARK, NULL},
  {XmNpreeditDrawCallback,  XNPreeditDrawCallback,  NULLQUARK, NULL},
  {XmNpreeditCaretCallback, XNPreeditCaretCallback, NULLQUARK, NULL},
};

static XmImXICInfo
callback_xic(Widget anchor)
{
  XmImShellInfo shell_info;
  if (!XtIsShell(anchor)) return get_current_xic(get_xim_info(anchor), anchor);
  shell_info = get_im_info(anchor, False);
  return shell_info ? shell_info->shell_xic : NULL;
}

static XPointer
callback_closure(Widget anchor, Widget target, int kind)
{
  XmImXICInfo ic = callback_xic(anchor);
  Cardinal i;
  for (i = 0; ic && i < ic->widget_refs.num_refs; ++i)
    if (ic->widget_refs.refs[i] == target && ic->widget_refs.callbacks[i])
      return ic->widget_refs.callbacks[i][kind].client_data;
  return NULL;
}

/* Counted locale text acceptance is shared by insertion and IC reset.  Logical
 * combining/format characters cannot be rejected for having no glyph advance. */
Boolean
_XmImInputPrintable(const char *text, size_t bytes, Boolean allow_tab)
{
  mbstate_t state = {0};
  size_t offset = 0;
  if (!text && bytes) return False;
  while (offset < bytes) {
    wchar_t wc;
    size_t count = mbrtowc(&wc, text + offset, bytes - offset, &state);
    if (count == (size_t)-1 || count == (size_t)-2 || count == 0 ||
        (!iswprint(wc) && !(allow_tab && wc == L'\t'))) return False;
    offset += count;
  }
  return True;
}

/* Registration outlives a transport XIC, never the registered widget. */
static XmImRegistration
remember_widget(XmImDisplayInfo info, Widget w)
{
  XmImRegistration r;
  if (!info) return NULL;
  for (r = info->registrations; r; r = r->next)
    if (r->widget == w) return r;
  r = (XmImRegistration)XtCalloc(1, sizeof(*r));
  r->widget = w;
  r->next = info->registrations;
  info->registrations = r;
  return r;
}

static void
free_saved_arg(Arg *arg)
{
  if (!strcmp(arg->name, XmNfontList) || !strcmp(arg->name, XmNrenderTable))
    XmFontListFree((XmFontList)arg->value);
  else if (!strcmp(arg->name, XmNspotLocation) || !strcmp(arg->name, XmNarea) ||
           strstr(arg->name, "Callback"))
    XtFree((char *)arg->value);
}

static void
forget_widget(XmImDisplayInfo info, Widget w)
{
  XmImRegistration *link, r;
  Cardinal i;
  if (!info) return;
  for (link = &info->registrations; (r = *link) != NULL; link = &r->next)
    if (r->widget == w) {
      *link = r->next;
      for (i = 0; i < r->count; ++i) free_saved_arg(&r->args[i]);
      XtFree((char *)r->args);
      XtFree((char *)r);
      return;
    }
}

static void
remember_values(XmImDisplayInfo info, Widget w, ArgList args, Cardinal n)
{
  XmImRegistration r;
  Cardinal i, j, k;
  if (!info || info->restoring || !n) return;
  r = remember_widget(info, w);
  for (i = 0; i < n; ++i) {
    XtArgVal value = args[i].value;
    size_t bytes = 0;
    for (k = 0; k < XtNumber(XmImResList); ++k)
      if (!strcmp(args[i].name, XmImResList[k].xmstring)) break;
    if (k == XtNumber(XmImResList)) continue;
    if (!strcmp(args[i].name, XmNfontList) || !strcmp(args[i].name, XmNrenderTable))
      value = (XtArgVal)XmFontListCopy((XmFontList)value);
    else {
      if (!strcmp(args[i].name, XmNspotLocation)) bytes = sizeof(XPoint);
      else if (!strcmp(args[i].name, XmNarea)) bytes = sizeof(XRectangle);
      else if (strstr(args[i].name, "Callback")) bytes = sizeof(XIMCallback);
      if (bytes && value) {
        char *copy = XtMalloc(bytes);
        memcpy(copy, (void *)value, bytes);
        value = (XtArgVal)copy;
      }
    }
    for (j = 0; j < r->count; ++j)
      if (!strcmp(r->args[j].name, args[i].name)) break;
    if (j < r->count) free_saved_arg(&r->args[j]);
    else {
      ++r->count;
      r->args = (ArgList)XtRealloc((char *)r->args, r->count * sizeof(Arg));
    }
    r->args[j].name = XmImResList[k].xmstring;
    r->args[j].value = value;
  }
}

static Boolean
open_input_method(XmImDisplayInfo info)
{
  XIMCallback destroyed;
  if (info->xim) return True;
  info->xim = XOpenIM(info->display, XtDatabase(info->display),
                     info->application_name, info->application_class);
  if (!info->xim) return False;
  destroyed.client_data = (XPointer)info;
  destroyed.callback = (XIMProc)im_destroyed;
  if (XSetIMValues(info->xim, XNDestroyCallback, &destroyed, NULL) ||
      XGetIMValues(info->xim, XNQueryInputStyle, &info->styles, NULL)) {
    XIMCallback none = { NULL, NULL };
    XSetIMValues(info->xim, XNDestroyCallback, &none, NULL);
    XCloseIM(info->xim);
    info->xim = NULL;
    return False;
  }
  return True;
}

static void
im_destroyed(XIM xim, XPointer closure, XPointer call_data)
{
  XmImDisplayInfo info = (XmImDisplayInfo)closure;
  Cardinal i;
  (void)xim; (void)call_data;
  /* Xlib destroys the transport and its ICs.  They must never be queried,
   * reset or destroyed again, including from preedit-done callbacks. */
  info->transitioning = True;
  info->xim = NULL;
  XFree(info->styles); info->styles = NULL;
  for (i = 0; i < info->shell_refs.num_refs; ++i) {
    XmImShellInfo shell_info = get_im_info(info->shell_refs.refs[i], False);
    XmImXICInfo ic;
    for (ic = shell_info ? shell_info->iclist : NULL; ic; ic = ic->next)
      ic->xic = NULL;
  }
  /* End transient composition, preserving only committed widget contents. */
  while (info->shell_refs.num_refs) {
    Widget shell = info->shell_refs.refs[0];
    XmImShellInfo shell_info = get_im_info(shell, False);
    XmImXICInfo ic;
    for (ic = shell_info ? shell_info->iclist : NULL; ic; ic = ic->next) {
      Cardinal j;
      if (!ic->preedit_active) continue;
      ic->preedit_active = False;
      for (j = 0; j < ic->widget_refs.num_refs; ++j) {
        Widget w = ic->widget_refs.refs[j];
        XmImRegistration r;
        Cardinal k;
        if (XtWindow(w) != ic->focus_window) continue;
        r = remember_widget(info, w);
        for (k = 0; k < r->count; ++k)
          if (!strcmp(r->args[k].name, XmNpreeditDoneCallback)) {
            XIMCallback *done = (XIMCallback *)r->args[k].value;
            if (done && done->callback)
              ((XICProc)done->callback)(NULL, done->client_data, NULL);
            break;
          }
      }
    }
    _XmImFreeShellData(shell, get_im_info_ptr(shell, False));
  }
  info->transitioning = False;
  if (!info->instantiate_registered)
    info->instantiate_registered = XRegisterIMInstantiateCallback(info->display,
      XtDatabase(info->display), info->application_name, info->application_class,
      im_available, (XPointer)info);
}

static void
im_available(Display *display, XPointer closure, XPointer call_data)
{
  XmImDisplayInfo info = (XmImDisplayInfo)closure;
  XmImRegistration r;
  (void)display; (void)call_data;
  if (info->transitioning || !open_input_method(info)) return;
  info->restoring = True;
  for (r = info->registrations; r; r = r->next) {
    if (r->external || r->widget->core.being_destroyed) continue;
    XmImRegister(r->widget, 0);
    set_values(r->widget, r->args, r->count, XmINHERIT_POLICY);
  }
  for (r = info->registrations; r; r = r->next)
    if (r->focused && !r->external && !r->widget->core.being_destroyed)
      XmImSetFocusValues(r->widget, r->args, r->count);
  info->restoring = False;
}

#define OVERTHESPOT	"overthespot"
#define OFFTHESPOT	"offthespot"
#define ROOT		"root"
#define ONTHESPOT	"onthespot"

#define PREEDIT_START   0
#define PREEDIT_DONE    1
#define PREEDIT_DRAW    2
#define PREEDIT_CARET   3

#define SEPARATOR_HEIGHT 2

#define GEO_CHG 0x1
#define BG_CHG  0x2

#define MSG1	_XmMMsgXmIm_0000

/*ARGSUSED*/
void 
XmImRegister(Widget w,
	     unsigned int reserved) /* unused */
{
  Widget p;
  XmImShellInfo im_info;
  XmImDisplayInfo xim_info;
  XmInputPolicy input_policy = XmINHERIT_POLICY;

  _XmWidgetToAppContext(w);
  
  _XmAppLock(app);
  /* Find the enclosing shell. */
  p = XtParent(w);
  while (!XtIsShell(p))
    p = XtParent(p);
  
  /* Lookup or create per-shell IM info and an XIM. */
  xim_info = get_xim_info(p);
  if (xim_info && !xim_info->restoring) remember_widget(xim_info, w)->external = False;
  else (void)remember_widget(xim_info, w);
  if ((xim_info == NULL) || (xim_info->xim == NULL)) {
    _XmAppUnlock(app);
    return;
  }
  if ((im_info = get_im_info(p, True)) == NULL) {
    _XmAppUnlock(app);
    return;
  }
  
  /* Check that this widget doesn't already have a current XIC. */
  if (get_current_xic(xim_info, w) != NULL) {
    _XmAppUnlock(app);
    return;
  }
  
  /* See if this widget will be sharing an existing XIC. */
  XtVaGetValues(p, XmNinputPolicy, &input_policy, NULL);
  switch (input_policy)
    {
    case XmPER_SHELL:
      if (im_info->shell_xic == NULL)
	(void) create_xic_info(p, xim_info, im_info, input_policy);
      set_current_xic(im_info->shell_xic, xim_info, w);
      break;
      
    case XmPER_WIDGET:
      set_current_xic(create_xic_info(p, xim_info, im_info, input_policy),
		      xim_info, w);
      break;
      
    case XmINHERIT_POLICY:
      break;

    default:
      assert(False);
    }
  _XmAppUnlock(app);
}

void 
XmImUnregister(Widget w)
{
  register XmImDisplayInfo xim_info;
  register XmImShellInfo im_info;
  register XmImXICInfo xic_info;
  XtAppContext app;
  
  /* Punt if insufficient information was provided. */
  if (w == NULL)
    return;
  
  app = XtWidgetToApplicationContext(w);
  _XmAppLock(app);
  /* Locate this record. */
  xim_info = get_xim_info(w);
  forget_widget(xim_info, w);
  if ((xic_info = get_current_xic(xim_info, w)) == NULL) {
    _XmAppUnlock(app);
    return;
  }
  if ((im_info = get_im_info(w, False)) == NULL) {
    _XmAppUnlock(app);
    return;
  }
  
  /* Unregister this record. */
  unset_current_xic(xic_info, im_info, xim_info, w);

  if (im_info->iclist == NULL) {
    Widget vw = XtParent(w);
    while (!XtIsShell(vw)) vw = XtParent(vw);
    ImGeoReq(vw);
  }
  _XmAppUnlock(app);
}

void 
XmImSetFocusValues(Widget w,
		   ArgList args,
		   Cardinal num_args)
{
  register XmImXICInfo xic_info;
  Widget p;
  Pixel fg, bg;
  XmFontList fl=NULL;
  XFontSet fs=NULL;
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  XmImShellInfo im_info;
  XVaNestedList list;
  Window wind;
  XmInputPolicy input_policy;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  p = w;
  while (!XtIsShell(p))
    p = XtParent(p);
  
  {
    XmImDisplayInfo info = get_xim_info(p);
    XmImRegistration r;
    for (r = info ? info->registrations : NULL; r; r = r->next)
      r->focused = r->widget == w;
    remember_values(info, w, args, num_args);
  }
  if ((xic_info = get_current_xic(get_xim_info(p), w)) == NULL) {
    _XmAppUnlock(app);
    return;
  }
  
  wind = xic_info->focus_window;
  xic_info->focus_window = XtWindow(w);

  set_values(w, args, num_args, XmINHERIT_POLICY);
  xic_info = get_current_xic(get_xim_info(p), w);
  if (!xic_info || !xic_info->xic) {
    _XmAppUnlock(app);
    return;
  }
  
  if (wind != XtWindow(w)) {
    /* Safe, since we have a window - so it's no gadget */
    XtVaGetValues(w, XmNbackground, &bg, NULL);
    XtVaGetValues(w, XmNforeground, &fg, NULL);
    XtVaGetValues(w, XmNfontList, &fl, NULL);
    if (fl) fs = extract_fontset(fl);
    if (fs)
      list = XVaCreateNestedList(0, 
				 XNBackground, bg,
				 XNForeground, fg, 
				 XNFontSet,    fs, NULL);
    else
      list = XVaCreateNestedList(0, 
				 XNBackground, bg,
				 XNForeground, fg, NULL);
    XSetICValues(xic_info->xic,	
		 XNFocusWindow, XtWindow(w),
		 XNStatusAttributes,  list,
		 XNPreeditAttributes, list,
		 NULL);
    XFree(list);

    if (xic_info->input_style & XIMPreeditCallbacks) {
      XtVaGetValues(p, XmNinputPolicy, &input_policy, NULL);
      if (input_policy == XmPER_SHELL && wind) 
	move_preedit_string(xic_info, 
			XtWindowToWidget(XtDisplay(w), wind), w);
    }
  }
  XSetICFocus(xic_info->xic);
  xic_info->has_focus = True;
  
  extData = _XmGetWidgetExtData((Widget)p, XmSHELL_EXTENSION);
  if (extData)
    {
      ve = (XmVendorShellExtObject) extData->widget;
  
      if (ve->vendor.im_height)
        {
          im_info = (XmImShellInfo)ve->vendor.im_info;
          im_info->current_widget = w;
          XtVaGetValues(w, XmNbackground, &bg, NULL);
          XtVaSetValues(p, XmNbackground, bg, NULL);
#ifdef FIX_1129
          ImGeoReq(p);
#endif
          draw_separator(p);
        }
    }
  _XmAppUnlock(app);
}

void 
XmImSetValues(Widget w,
	      ArgList args,
	      Cardinal num_args )
{
  _XmWidgetToAppContext(w);
  _XmAppLock(app);
  set_values(w, args, num_args, XmINHERIT_POLICY);
  _XmAppUnlock(app);
}

void 
XmImUnsetFocus(Widget w)
{
  register XmImXICInfo xic_info;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  {
    XmImDisplayInfo info = get_xim_info(w);
    XmImRegistration r;
    for (r = info ? info->registrations : NULL; r; r = r->next)
      if (r->widget == w) r->focused = False;
  }
  if ((xic_info = get_current_xic(get_xim_info(w), w)) == NULL) {
    _XmAppUnlock(app);
    return;
  }
  
  if (xic_info->xic)
    XUnsetICFocus(xic_info->xic);
  xic_info->has_focus = False;
  _XmAppUnlock(app);
}

XIM 
XmImGetXIM(Widget w)
{
  XmImDisplayInfo xim_info;
  _XmWidgetToAppContext(w);

  _XmAppLock(app);
  xim_info = get_xim_info(w);
  if (xim_info != NULL) {
    _XmAppUnlock(app);
    return xim_info->xim;
  }
  else {
    _XmAppUnlock(app);
    return NULL;
  }
}

void
XmImCloseXIM(Widget w)
{
  XmDisplay xmDisplay;
  XmImDisplayInfo xim_info;
  Widget shell;
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  int height, base_height;
  Arg args[1];
  XtWidgetGeometry my_request;
  _XmWidgetToAppContext(w);
  
  _XmAppLock(app);
  /* Allow (xim_info->xim == NULL) so we can reset the "failed" flag. */
  if ((xim_info = get_xim_info(w)) == NULL) {
    _XmAppUnlock(app);
    return;
  }
  
  /* Remove all references to all XICs */
  while (xim_info->shell_refs.refs != NULL)
    {
      shell = xim_info->shell_refs.refs[0];
      _XmImFreeShellData(shell, get_im_info_ptr(shell, False));
      assert((xim_info->shell_refs.refs == NULL) ||
	     (xim_info->shell_refs.refs[0] != shell));
    }
  
  shell = w;
  while (!XtIsShell(shell))
    shell = XtParent(shell);

  extData = _XmGetWidgetExtData((Widget)shell, XmSHELL_EXTENSION);
  if (extData) {
    ve = (XmVendorShellExtObject) extData->widget;
    height = ve->vendor.im_height;
    
    if (height != 0){
      XtSetArg(args[0], XtNbaseHeight, &base_height);
      XtGetValues(shell, args, 1);
      if (base_height > 0){
	base_height -= height;
	XtSetArg(args[0], XtNbaseHeight, base_height);
	XtSetValues(shell, args, 1);
      }
      if(!(XtIsRealized(shell)))
	shell->core.height -= height;
      else {
	my_request.height = shell->core.height - height;
	my_request.request_mode = CWHeight;
	XtMakeGeometryRequest(shell, &my_request, NULL);
      }
      ve->vendor.im_height = 0;
    }
  }

  if (xim_info->instantiate_registered)
    XUnregisterIMInstantiateCallback(xim_info->display,
      XtDatabase(xim_info->display), xim_info->application_name,
      xim_info->application_class, im_available, (XPointer)xim_info);
  while (xim_info->registrations)
    forget_widget(xim_info, xim_info->registrations->widget);
  /* Explicit close is not a service-loss notification. */
  if (xim_info->xim != NULL) {
    XIMCallback none = { NULL, NULL };
    XSetIMValues(xim_info->xim, XNDestroyCallback, &none, NULL);
  }
  /* Close the XIM. */
  if (xim_info->xim != NULL)
    {
      XCloseIM(xim_info->xim);
      xim_info->xim = NULL;
    }
  
  XFree(xim_info->styles);
  xim_info->styles = NULL;
  
  xmDisplay = (XmDisplay) XmGetXmDisplay(XtDisplay(w));
  xmDisplay->display.xmim_info = NULL;
  XtFree(xim_info->application_name);
  XtFree(xim_info->application_class);
  XtFree((char *) xim_info);
  _XmAppUnlock(app);
}

int 
XmImMbLookupString(Widget w,
		   XKeyPressedEvent *event,
		   char *buf,
		   int nbytes,
		   KeySym *keysym,
		   int *status )
{
  register XmImXICInfo icp;
  int ret_val;
  _XmWidgetToAppContext(w);
  
  _XmAppLock(app);
  if ((icp = get_current_xic(get_xim_info(w), w)) == NULL || 
      icp->xic == NULL)
    {
      if (status)
	*status = XLookupBoth;
      ret_val = XLookupString(event, buf, nbytes, keysym, 0);
      _XmAppUnlock(app);
      return ret_val;
    }
  
  ret_val = XmbLookupString( icp->xic, event, buf, nbytes,
				keysym, status );
  /* Servers may deliver Commit before the final preedit delete/done.  Finish
   * transient text before the caller inserts committed bytes, so a late delete
   * cannot consume a combining commit or move its caret back to preedit start. */
  if (ret_val > 0 && (!status || *status != XBufferOverflow) && icp->preedit_active)
    ImPreeditDoneCallback(icp->xic, (XPointer)w, NULL);
  _XmAppUnlock(app);
  return ret_val;
}

XIC
XmImGetXIC(Widget 	 w,
#if NeedWidePrototypes
	   unsigned int  input_policy,
#else
	   XmInputPolicy input_policy,
#endif /*NeedWidePrototypes*/
	   ArgList 	 args,
	   Cardinal 	 num_args)
{
  XmImDisplayInfo xim_info;
  XmImShellInfo im_info;
  XmImXICInfo xic_info;
  Widget shell;
  _XmWidgetToAppContext(w);
  
  _XmAppLock(app);
  xim_info = get_xim_info(w);
  (void)remember_widget(xim_info, w);
  remember_values(xim_info, w, args, num_args);
  im_info = get_im_info(w, True);
  xic_info = get_current_xic(xim_info, w);

  if ((xim_info == NULL) || (xim_info->xim == NULL)) {
    _XmAppUnlock(app);
    return NULL;
  }
  
  /* Find the enclosing shell. */
  shell = w;
  while (!XtIsShell(shell))
    shell = XtParent(shell);
  
  /* Resolve the true input policy. */
  if (input_policy == XmINHERIT_POLICY)
    XtVaGetValues(shell, XmNinputPolicy, &input_policy, NULL);
  
  /* If there is already a current XIC, we may want to unregister it. */
  switch (input_policy)
    {
    case XmPER_SHELL:
      if ((xic_info != NULL) && (im_info->shell_xic != xic_info))
	{
	  unset_current_xic(xic_info, im_info, xim_info, w);
	  xic_info = NULL;
	}
      break;
      
    case XmPER_WIDGET:
      if (xic_info != NULL)
	{
	  unset_current_xic(xic_info, im_info, xim_info, w);
	  xic_info = NULL;
	}
      break;
      
    default:
      assert(False);
    }
  
  /* Register an XIC with the desired input policy. */
  if (xic_info == NULL)
    {
      remember_widget(xim_info, w)->external = False;
      xic_info = create_xic_info(shell, xim_info, im_info, input_policy);
      set_current_xic(xic_info, xim_info, w);
    }
  
  /* Set the values, which creates an XIC. */
  set_values(w, args, num_args, input_policy);
  
  /* Return the current XIC. */
  if (xic_info != NULL) {
    _XmAppUnlock(app);
    return xic_info->xic;
  }
  _XmAppUnlock(app);
  return NULL;
}

XIC
XmImSetXIC(Widget widget, 
	   XIC    xic)
{
  XmImDisplayInfo xim_info;
  XmImShellInfo im_info;
  XmImXICInfo xic_info;
  Widget shell;
  _XmWidgetToAppContext(widget);

  _XmAppLock(app);
  xim_info = get_xim_info(widget);
  im_info = get_im_info(widget, True);
  xic_info = get_current_xic(xim_info, widget);
  if ((xim_info == NULL) || (xim_info->xim == NULL)) {
    _XmAppUnlock(app);
    return NULL;
  }
  
  /* This may be a simple query. */
  if (xic == NULL)
    {
      /* No XIC is registered for this widget. */
      if (xic_info == NULL) {
	_XmAppUnlock(app);
	return NULL;
      }
      
      /* Force creation of the XIC. */
      if (xic_info->xic == NULL)
	set_values(widget, NULL, 0, XmINHERIT_POLICY);

      _XmAppUnlock(app);
      return xic_info->xic;
    }
  
  /* We don't support multiple IMs. */
  if (XIMOfIC(xic) != xim_info->xim) {
    _XmAppUnlock(app);
    return NULL;
  }
  /* Opaque application-created XICs cannot be reconstructed by the toolkit. */
  remember_widget(xim_info, widget)->external = True;
  
  /* Unregister the current XIC. */
  if (xic_info != NULL)
    {
      /* Setting the current XIC to itself is a no-op. */
      if (xic_info->xic == xic) {
	_XmAppUnlock(app);
	return xic;
      }
      
      unset_current_xic(xic_info, im_info, xim_info, widget);
      xic_info = NULL;
    }
  
  /* Find the enclosing shell. */
  shell = widget;
  while (!XtIsShell(shell))
    shell = XtParent(shell);
  
  /* Get or create xic_info for this xic. */
  xic_info = recreate_xic_info(xic, shell, xim_info, im_info);
  
  /* Make this the current XIC for this widget. */
  set_current_xic(xic_info, xim_info, widget);
  
  _XmAppUnlock(app);
  return xic;
}

void
XmImFreeXIC(Widget w,
	    XIC    context)
{
  register int index;
  register XmImDisplayInfo xim_info;
  register XmImShellInfo im_info;
  register XmImXICInfo xic_info;
  XtAppContext app;
  
  /* Punt if insufficient information was provided. */
  if (w == NULL)
    return;
  
  app = XtWidgetToApplicationContext(w);
  _XmAppLock(app);
  /* Locate this record. */
  xim_info = get_xim_info(w);
  if ((xic_info = get_current_xic(xim_info, w)) == NULL) {
    _XmAppUnlock(app);
    return;
  }
  if ((im_info = get_im_info(w, False)) == NULL) {
    _XmAppUnlock(app);
    return;
  }
  if ((context != NULL) && (xic_info->xic != context)) {
    _XmAppUnlock(app);
    return;
  }
  
  /* Remove all references. */
  index = xic_info->widget_refs.num_refs;
  while (--index >= 0) {
    Widget reference = xic_info->widget_refs.refs[index];
    forget_widget(xim_info, reference);
    unset_current_xic(xic_info, im_info, xim_info, reference);
  }
  _XmAppUnlock(app);
}

/*********************
 * Private Functions *
 *********************/

/* Free a VendorShellExt's im_info field. */
void
_XmImFreeShellData(Widget     widget,
		   XtPointer* data)
{
  XmImShellInfo   im_info;
  XmImDisplayInfo xim_info;
  XmImXICInfo	  xic_info;
  Widget	  reference;
  
  if ((data == NULL) ||
      (im_info = (XmImShellInfo) *data) == NULL)
    return;
  
  /* Ignore (xim_info->xim == NULL), since it is immaterial here. */
  xim_info = get_xim_info(widget);
  if (xim_info == NULL)
    return;
  
  /* Remove any dangling references. */
  while (im_info->iclist != NULL)
    {
      xic_info = im_info->iclist;
      reference = xic_info->widget_refs.refs[0];
      unset_current_xic(xic_info, im_info, xim_info, reference);
      assert((xic_info != im_info->iclist) ||
	     (reference != xic_info->widget_refs.refs[0]));
    }
  assert(im_info->shell_xic == NULL);
  
  /* Delete the dummy widget. */
#ifdef FIX_1534
  if (im_info->im_widget != NULL &&
      !widget->core.being_destroyed)
#else
  if (im_info->im_widget != NULL)
#endif
    {
      XtDestroyWidget(im_info->im_widget);
      im_info->im_widget = NULL;
    }
  
  /* Remove this shell as a reference to the XIM. */
  (void) remove_ref(&xim_info->shell_refs, widget);
  
  /* Delete the data. */
  XtFree((char *) im_info);
  *data = NULL;
}

void 
_XmImChangeManaged(
		   Widget vw )
{
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  register int height, old_height;
  
  extData = _XmGetWidgetExtData((Widget)vw, XmSHELL_EXTENSION);
  if (extData) {
    ve = (XmVendorShellExtObject) extData->widget;

    old_height = ve->vendor.im_height;  

    height = ImGetGeo(vw, NULL);
    if (!ve->vendor.im_vs_height_set) {
      Arg args[1];
      int base_height;
      XtSetArg(args[0], XtNbaseHeight, &base_height);
      XtGetValues(vw, args, 1);
      if (base_height > 0) {
        base_height += (height - old_height);
        XtSetArg(args[0], XtNbaseHeight, base_height);
        XtSetValues(vw, args, 1);
      }
      vw->core.height += (height - old_height);
    }
  }
}

void
_XmImRealize(
	     Widget vw )
{
  XmImXICInfo icp;
  Pixel bg;
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  XmImShellInfo im_info;
  XmImDisplayInfo xim_info;
  
  xim_info = get_xim_info(vw);
  im_info = get_im_info(vw, False);
  
  if ((xim_info == NULL) || 
      (im_info == NULL) ||
      (im_info->iclist == NULL))
    return;
  
  /* We need to synchronize here to make sure the server has created
   * the client window before the input server attempts to reparent
   * any windows to it
   */
  XSync(XtDisplay(vw), False);
  
  for (icp = im_info->iclist; icp != NULL; icp = icp->next)
    {
      if (!icp->xic) continue;
      XSetICValues(icp->xic, XNClientWindow, XtWindow(vw), NULL);
    }
  
  extData = _XmGetWidgetExtData((Widget)vw, XmSHELL_EXTENSION);
  if (extData)
    ve = (XmVendorShellExtObject) extData->widget;
  else
    ve = NULL;
  if (ve && ve->vendor.im_height == 0) {
    ShellWidget shell = (ShellWidget)(vw);
    Boolean resize = shell->shell.allow_shell_resize;

    if (!resize) shell->shell.allow_shell_resize = True;
    ImGeoReq(vw);
    if (!resize) shell->shell.allow_shell_resize = False;
  } else
    ImSetGeo(vw, NULL);
  
  /* For some reason we need to wait till now before we set the 
   * initial background pixmap.
   */
  if (ve && ve->vendor.im_height && im_info->current_widget)
    {
      XtVaGetValues(im_info->current_widget, XmNbackground, &bg, NULL);
      XtVaSetValues(vw, XmNbackground, bg, NULL);
    }
}

void
_XmImResize(
	    Widget vw )
{
  ImGetGeo(vw, NULL);
  ImSetGeo(vw, NULL);
}

void
_XmImRedisplay(
	       Widget vw )
{
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  
  if ((extData = _XmGetWidgetExtData((Widget)vw, XmSHELL_EXTENSION)) == NULL)
    return;
  
  ve = (XmVendorShellExtObject) extData->widget;
  
  if (ve->vendor.im_height == 0)
    return;
  
  draw_separator(vw);
}

/********************
 * Static functions *
 ********************/

/* Locate an XmImXICInfo struct for an existing XIC. */
static XmImXICInfo
recreate_xic_info(XIC		  xic,
		  Widget	  shell,
		  XmImDisplayInfo xim_info,
		  XmImShellInfo   im_info)
{
  Cardinal index;
  XmImXICInfo xic_info;
  assert(xic != NULL);
  
  /* Search for an existing record in this shell's im_info. */
  for (xic_info = im_info->iclist;
       xic_info != NULL;
       xic_info = xic_info->next)
    {
      if (xic_info->xic == xic)
	return xic_info;
    }
  
  /* Search for an existing record in another shell's im_info? */
  for (index = 0; index < xim_info->shell_refs.num_refs; index++)
    if (shell != xim_info->shell_refs.refs[index])
      {
	XmImShellInfo tmp_info = 
	  get_im_info(xim_info->shell_refs.refs[index], False);
	assert(tmp_info != NULL);
	
	for (xic_info = tmp_info->iclist;
	     xic_info != NULL;
	     xic_info = xic_info->next)
	  {
	    if (xic_info->xic == xic)
	      return xic_info;
	  }
      }
  
  /* This XIC must have been created by the application directly. */
  xic_info = XtNew(XmImXICRec);
  bzero((char*) xic_info, sizeof(XmImXICRec));
  (void) XGetICValues(xic, XNInputStyle, &xic_info->input_style, NULL);
  xic_info->next = im_info->iclist;
  im_info->iclist = xic_info;
  
  if (XtIsRealized (shell))
    {
      /* If client_window hasn't been set already, set it now. */
      (void) XSetICValues(xic, XNClientWindow, XtWindow(shell), NULL);
      
      /* Update cached geometry fields */
      ImGetGeo(shell, xic_info);
      ImSetGeo(shell, xic_info);
    }
  
  return xic_info;
}

/* Attempt to create an XmImXICInfo struct.  Return it or NULL. */
static XmImXICInfo
create_xic_info(Widget		shell,
		XmImDisplayInfo xim_info,
		XmImShellInfo   im_info,
#if NeedWidePrototypes
		unsigned int 	input_policy)
#else
                XmInputPolicy	input_policy)
#endif /*NeedWidePrototypes*/
{
  XIMStyle style = 0;
  char tmp[BUFSIZ];
  char *cp = NULL; 
  char *tp = NULL;
  char *cpend = NULL;
  register XIMStyles *styles;
  XmImXICInfo xic_info;
  
  /* Determine the input style to be used for this XIC. */
  styles = xim_info->styles;
  XtVaGetValues(shell, XmNpreeditType, &cp, NULL);

  if (cp != NULL)
    {
	/* Parse for the successive commas.  The resource is application-owned;
	 * do not copy an arbitrarily long value into the historical stack buffer. */
	if (strlen(cp) < sizeof(tmp))
	  {
	    memcpy(tmp, cp, strlen(cp) + 1);
	    cp = tmp;
	    cpend = &tmp[strlen(tmp)];
	    while((style == 0) && (cp < cpend))
        {
          tp = strchr(cp,',');
          if (tp)
            *tp = 0;
          else
            tp = cpend;

          /* Look for an acceptable supported style. */
          if (XmeNamesAreEqual(cp, OVERTHESPOT))
            style = check_style(styles, XIMPreeditPosition,
                                XIMStatusArea|XIMStatusNothing|XIMStatusNone);
          else if (XmeNamesAreEqual(cp, OFFTHESPOT))
            style = check_style(styles, XIMPreeditArea,
                                XIMStatusArea|XIMStatusNothing|XIMStatusNone);
          else if (XmeNamesAreEqual(cp, ROOT))
            style = check_style(styles, XIMPreeditNothing,
                                XIMStatusNothing|XIMStatusNone);
          else if (XmeNamesAreEqual(cp, ONTHESPOT))
            style = check_style(styles, XIMPreeditCallbacks,
                                XIMStatusArea|XIMStatusNothing|XIMStatusNone);
         
          cp = tp+1;
        }
	  }
    }
  if (style == 0)
    {
      /* Try for a fallback style, or give up and use XLookupString. */
      if ((style = check_style(styles, XIMPreeditNone, XIMStatusNone)) == 0)
        return NULL;
    }

  /* We need to create this widget whenever there is a non-simple
   * input method in order to stop the intrinsics from calling
   * XMapSubwindows, thereby improperly mapping input method
   * windows which have been made children of the client or
   * focus windows.
   */
  if ((im_info->im_widget == NULL) &&
      (style & (XIMStatusArea | XIMPreeditArea | XIMPreeditPosition)))
    im_info->im_widget = 
      XtVaCreateWidget("xmim_wrapper", coreWidgetClass,
		       shell, XmNwidth, 10, XmNheight, 10, NULL);
  
  /* Create the XIC info record. */
  xic_info = XtNew(XmImXICRec);
  bzero((char*) xic_info, sizeof(XmImXICRec));
  xic_info->input_style = style;
  xic_info->anonymous = True;
  xic_info->preedit_buffer = XtNew(PreeditBufferRec);
  bzero((char *) xic_info->preedit_buffer, sizeof(PreeditBufferRec));
  
  xic_info->next = im_info->iclist;
  im_info->iclist = xic_info;
  
  /* Setup sharing for this XIC. */
  switch (input_policy)
    {
    case XmPER_SHELL:
      assert (im_info->shell_xic == NULL);
      im_info->shell_xic = xic_info;
      im_info->shell_xic->source = &im_info->shell_xic;
      break;
      
    case XmPER_WIDGET: 
      break;

    case XmINHERIT_POLICY:
      break;
 
    default:
      assert(False);
    }
  
  return xic_info;
}


#define IsCallback(name) \
  if (name == XrmStringToName(XmNpreeditStartCallback) || \
      name == XrmStringToName(XmNpreeditDoneCallback) || \
      name == XrmStringToName(XmNpreeditDrawCallback) || \
      name == XrmStringToName(XmNpreeditCaretCallback))


static void 
set_values(Widget w,
	   ArgList args,
	   Cardinal num_args,
#if NeedWidePrototypes
	   unsigned int  input_policy )
#else
           XmInputPolicy input_policy )
#endif /*NeedWidePrototypes*/
{
  register XmImXICInfo icp;
  XmImDisplayInfo xim_info;
  XmImResListRec *rlp;
  register int i, j;
  register ArgList argp = args;
  VaArgListRec status_vlist, preedit_vlist, xic_vlist;
  XVaNestedList va_slist, va_plist, va_vlist;
  XrmName name, area_name = XrmStringToName(XmNarea);
  Widget p;
  XmImShellInfo im_info;
  int flags = 0;
  Pixel bg;
  char *ret;
  unsigned long mask = 0;
  Boolean unrecognized = False;
  XIMCallback callback_values[num_args ? num_args : 1];
  Cardinal callback_count = 0;
  
  p = w;
  while (!XtIsShell(p))
    p = XtParent(p);
  
  xim_info = get_xim_info(p);
  remember_values(xim_info, w, args, num_args);
  if (!xim_info || !xim_info->xim ||
      (icp = get_current_xic(xim_info, w)) == NULL)
    return;
  
  im_info = get_im_info(p, False);
  assert(im_info != NULL);
  
  if (!XtIsRealized(p)) {
    /* If vendor widget not realized, then the current info
     * is that for the last widget to set values.
     */
    im_info->current_widget = w;
  }
  
  if (icp->xic && 
      icp->focus_window && icp->focus_window != XtWindow(w))
    return;
  
  bzero((char*) &status_vlist, sizeof(VaArgListRec));
  bzero((char*) &preedit_vlist, sizeof(VaArgListRec));
  bzero((char*) &xic_vlist, sizeof(VaArgListRec));
  for (i = num_args; i > 0; i--, argp++) {
    name = XrmStringToName(argp->name);
    if (name == area_name && !(icp->input_style & XIMPreeditPosition))
      continue;

    IsCallback(name){
      if (icp->input_style & XIMPreeditCallbacks){
        assert(callback_count < XtNumber(callback_values));
        callback_values[callback_count] = *(XIMCallback *)argp->value;
        set_callback_values(w, argp->name, &callback_values[callback_count++],
                                                &preedit_vlist, input_policy);
        continue;
      } else
        continue;
    }

    _XmProcessLock();
    for (rlp = XmImResList, j = XtNumber(XmImResList); j != 0; j--, rlp++) {
      if (rlp->xrmname == name)	{
	flags |= (*rlp->proc)(rlp->xstring, (XPointer) argp->value,
			 &status_vlist, &preedit_vlist, &xic_vlist);
	break;
      }
    }
    _XmProcessUnlock();
    if (j == 0) {
      /* Simply pass unrecognized values along */
      VaSetArg(&xic_vlist, argp->name, (XPointer) argp->value);
      unrecognized = True;
    }
  }
  
  /* We do not create the IC until the initial data is ready to be passed */
  assert(xim_info != NULL);
  if (icp->xic == NULL) {
    if (XtIsRealized(p)) {
      XSync(XtDisplay(p), False);
      VaSetArg(&xic_vlist, XNClientWindow, (XPointer) XtWindow(p));
    }
    if (icp->focus_window) {
      VaSetArg(&xic_vlist, XNFocusWindow, (XPointer) icp->focus_window);
    }
    VaSetArg(&xic_vlist, XNInputStyle, (XPointer) icp->input_style);
    
    va_plist = VaCopy(&preedit_vlist);
    if (va_plist)
      VaSetArg(&xic_vlist, XNPreeditAttributes, (XPointer)va_plist);
    va_slist = VaCopy(&status_vlist);
    if (va_slist)
      VaSetArg(&xic_vlist, XNStatusAttributes, (XPointer)va_slist);
    va_vlist = VaCopy(&xic_vlist);
    
    if (va_vlist)
    {
      icp->xic = XCreateIC(xim_info->xim, XNVaNestedList, va_vlist, NULL);
#ifdef FIX_1510
      if (icp->xic == NULL)
      {
        icp->input_style = XIMPreeditNothing | XIMStatusNothing;
        icp->xic = XCreateIC(xim_info->xim, XNInputStyle, icp->input_style,
            XNClientWindow, XtWindow(p), XNFocusWindow, XtWindow(p), NULL);
      }
#endif
    }
    else
      icp->xic = XCreateIC(xim_info->xim, NULL);
    
    if (va_vlist) XFree(va_vlist);
    if (va_plist) XFree(va_plist);
    if (va_slist) XFree(va_slist);
    if (preedit_vlist.args) XtFree((char *)preedit_vlist.args);
    if (status_vlist.args) XtFree((char *)status_vlist.args);
    if (xic_vlist.args) XtFree((char *)xic_vlist.args);
    
    if (icp->xic == NULL) {
      unset_current_xic(icp, im_info, xim_info, w);
      return;
    }
    XGetICValues(icp->xic, XNFilterEvents, &mask, NULL);
    if (mask) {
      XtAddEventHandler(p, (EventMask)mask, False, null_proc, NULL);
    }
    if (XtIsRealized(p)) {
#ifdef FIX_1129
      im_info->current_widget = w;
#endif
      if (XmIsDialogShell(p)) {
	for (i = 0; 
	     i < ((CompositeWidget)p)->composite.num_children; 
	     i++)
	  if (XtIsManaged(((CompositeWidget)p)->composite.children[i])) {
	    ImGeoReq(p);
	    break;
	  }
      } else
	ImGeoReq(p);
#ifndef FIX_1129
      im_info->current_widget = w;
#endif
    }
    /* Is this new XIC supposed to be shared? */
    switch (input_policy)
      {
      case XmPER_SHELL:
	assert(im_info->shell_xic == NULL);
	im_info->shell_xic = icp;
	break;
	
      case XmPER_WIDGET:
	break;
      default:
	assert(False);
      }
  } else {
    /* Try to modify the existing XIC. */
    va_plist = VaCopy(&preedit_vlist);
    if (va_plist)
      VaSetArg(&xic_vlist, XNPreeditAttributes, (XPointer)va_plist);
    va_slist = VaCopy(&status_vlist);
    if (va_slist)
      VaSetArg(&xic_vlist, XNStatusAttributes, (XPointer)va_slist);
    va_vlist = VaCopy(&xic_vlist);
    
    if (va_vlist)
      ret = XSetICValues(icp->xic, XNVaNestedList, va_vlist, NULL);
    else 
      ret = NULL;
    
    if (va_vlist) XFree(va_vlist);
    if (va_plist) XFree(va_plist);
    if (va_slist) XFree(va_slist);
    if (preedit_vlist.args) XtFree((char *)preedit_vlist.args);
    if (status_vlist.args) XtFree((char *)status_vlist.args);
    if (xic_vlist.args) XtFree((char *)xic_vlist.args);
    
    /* ??? Both a write-once and an unrecognized arg might be present. */
    if ((ret != NULL) && unrecognized)	{
      unsigned long status_bg, status_fg;
      unsigned long preedit_bg, preedit_fg;
      
      /* ??? This code assumes that the XIM hasn't changed. */
      assert(XIMOfIC(icp->xic) == xim_info->xim);
      
      /* We do this in case an input method does not support
       * change of some value, but does allow it to be set on
       * create.  If however the value is not one of the 
       * standard values, this im may not support it so we
       * should ignore it.
       */
      
      va_slist = XVaCreateNestedList(0, 
				     XNBackground, &status_bg,
				     XNForeground, &status_fg, 
				     NULL);
      va_plist = XVaCreateNestedList(0, 
				     XNBackground, &preedit_bg,
				     XNForeground, &preedit_fg, 
				     NULL);
      XGetICValues(icp->xic, 
		   XNStatusAttributes,  va_slist,
		   XNPreeditAttributes, va_plist,
		   NULL);
      XFree(va_slist);
      XFree(va_plist);
      
      if (icp->anonymous)
	XDestroyIC(icp->xic);
      icp->anonymous = TRUE;
      icp->xic = NULL;
      
      VaSetArg(&status_vlist, XNBackground, (XPointer) status_bg);
      VaSetArg(&status_vlist, XNForeground, (XPointer) status_fg);
      
      VaSetArg(&preedit_vlist, XNBackground, (XPointer) preedit_bg);
      VaSetArg(&preedit_vlist, XNForeground, (XPointer) preedit_fg);
      
      if (XtIsRealized(p)) {
	XSync(XtDisplay(p), False);
	VaSetArg(&xic_vlist, XNClientWindow, (XPointer) XtWindow(p));
      }
      if (icp->focus_window) {
	VaSetArg(&xic_vlist, XNFocusWindow, (XPointer)icp->focus_window);
      }
      VaSetArg(&xic_vlist, XNInputStyle, (XPointer) icp->input_style);
      
      va_plist = VaCopy(&preedit_vlist);
      if (va_plist)
	VaSetArg(&xic_vlist, XNPreeditAttributes, (XPointer)va_plist);
      va_slist = VaCopy(&status_vlist);
      if (va_slist)
	VaSetArg(&xic_vlist, XNStatusAttributes, (XPointer)va_slist);
      va_vlist = VaCopy(&xic_vlist);
      
      if (va_vlist)
      {
	icp->xic = XCreateIC(xim_info->xim, XNVaNestedList, va_vlist, NULL);
#ifdef FIX_1510
        if (icp->xic == NULL)
        {
          icp->input_style = XIMPreeditNothing | XIMStatusNothing;
          icp->xic = XCreateIC(xim_info->xim, XNInputStyle, icp->input_style,
              XNClientWindow, XtWindow(p), XNFocusWindow, XtWindow(p), NULL);
        }
#endif
      }
      else
	icp->xic = XCreateIC(xim_info->xim, NULL);
      
      if (va_vlist) XFree(va_vlist);
      if (va_plist) XFree(va_plist);
      if (va_slist) XFree(va_slist);
      if (preedit_vlist.args) XtFree((char *)preedit_vlist.args);
      if (status_vlist.args) XtFree((char *)status_vlist.args);
      if (xic_vlist.args) XtFree((char *)xic_vlist.args);
      
      if (icp->xic == NULL) {
	unset_current_xic(icp, im_info, xim_info, w);
	return;
      }
      ImGeoReq(p);
      if (icp->has_focus)
	XSetICFocus(icp->xic);
      return;
    }
    if (flags & GEO_CHG) {
      ImGeoReq(p);
      if (icp->has_focus)
	XSetICFocus(icp->xic);
    }
  }
  
  /* Since we do not know whether a set values may have been done
   * on top shadow or bottom shadow (used for the separator), we
   * will redraw the separator in order to keep the visuals in sync
   * with the current text widget. Also repaint background if needed.
   */
  if ((im_info->current_widget == w) && (flags & BG_CHG)) {
    XtVaGetValues(w, XmNbackground, &bg, NULL);
    XtVaSetValues(p, XmNbackground, bg, NULL);
  }
}


static void
ImFreePreeditBuffer(PreeditBuffer pb)
{
  if (pb->text) XtFree((char *)pb->text);
  if (pb->feedback) XtFree((char *)pb->feedback);
  XtFree((char *)pb);
} 


static int
ImPreeditStartCallback(XIC xic,
		       XPointer client_data,
		       XPointer call_data)
{
  XICProc proc;
  Widget real = NULL;

  if (!client_data){
    assert(False);
  }

  {
    XmImXICInfo ic = callback_xic((Widget)client_data);
    if (!ic) return 0;
    ic->preedit_active = True;
  }
  proc = get_real_callback((Widget)client_data, PREEDIT_START, &real);
  if (proc)
    (*proc)(xic, callback_closure((Widget)client_data, real, PREEDIT_START), call_data);

  return (-1);
}

static void
ImPreeditDoneCallback(XIC xic,
		      XPointer client_data,
		      XPointer call_data)
{
  XICProc proc;
  Widget w = (Widget)client_data;
  XmImXICInfo icp;
  Widget real = NULL;

  if (!client_data){
    assert(False);
  }
 
  if ((icp = callback_xic(w)) == NULL) return;

  if (!icp->preedit_active) return;
  icp->preedit_active = False;
  proc = get_real_callback((Widget)client_data, PREEDIT_DONE, &real);
  if (proc)
    (*proc)(xic, callback_closure(w, real, PREEDIT_DONE), call_data);

  if (icp->preedit_buffer->text) 
    XtFree((char *)icp->preedit_buffer->text);
  if (icp->preedit_buffer->feedback)
    XtFree((char *)icp->preedit_buffer->feedback);
  bzero((char *)icp->preedit_buffer, sizeof(PreeditBufferRec)); 
}

Boolean
_XmImPreeditRangeValid(unsigned int length, int first, int removed,
                       unsigned int inserted, int caret)
{
  unsigned int remaining, result;
  if (length > 65535U || first < 0 || removed < 0 ||
      (unsigned int)first > length || (unsigned int)removed > length - (unsigned int)first)
    return False;
  remaining = length - (unsigned int)removed;
  if (inserted > 65535U - remaining) return False;
  result = remaining + inserted;
  return caret >= 0 && (unsigned int)caret <= result;
}

static void
ImPreeditDrawCallback(XIC xic, XPointer client_data, XPointer call_data)
{
  XICProc proc;
  Widget w = (Widget)client_data, real = NULL;
  XmImXICInfo icp;
  PreeditBuffer pb;
  XIMPreeditDrawCallbackStruct *data = (XIMPreeditDrawCallbackStruct *)call_data;
  XIMText *text;
  XIMPreeditDrawCallbackStruct dispatch;
  XIMText dispatch_text;
  wchar_t *dispatch_characters = NULL;
  unsigned int inserted, result, suffix;
  wchar_t *replacement;
  XIMFeedback *feedback;

  if (!w || !data || (icp = callback_xic(w)) == NULL ||
      !icp->preedit_active || !icp->preedit_buffer)
    return;
  pb = icp->preedit_buffer;
  text = data->text;
  inserted = text ? text->length : 0;
  if (!_XmImPreeditRangeValid(pb->length, data->chg_first, data->chg_length,
                              inserted, 0) || data->caret < 0)
    return;
  if (inserted && ((text->encoding_is_wchar && !text->string.wide_char) ||
                   (!text->encoding_is_wchar && !text->string.multi_byte))) {
    /* A string-less XIMText changes feedback, not the preedit characters. */
    if (inserted != (unsigned int)data->chg_length) return;
    if (text->feedback)
      memcpy(pb->feedback + data->chg_first, text->feedback,
             inserted * sizeof(XIMFeedback));
    dispatch = *data;
    if ((unsigned int)dispatch.caret > pb->length) dispatch.caret = pb->length;
    pb->caret = dispatch.caret;
    proc = get_real_callback(w, PREEDIT_DRAW, &real);
    if (proc) (*proc)(xic, callback_closure(w, real, PREEDIT_DRAW), (XPointer)&dispatch);
    return;
  }
  result = pb->length - data->chg_length + inserted;
  replacement = (wchar_t *)XtCalloc(result + 1, sizeof(wchar_t));
  feedback = (XIMFeedback *)XtCalloc(result + 1, sizeof(XIMFeedback));
  if (inserted) {
    if (text->encoding_is_wchar)
      memcpy(replacement + data->chg_first, text->string.wide_char,
             inserted * sizeof(wchar_t));
    else if (mbstowcs(replacement + data->chg_first, text->string.multi_byte,
                      inserted) != inserted) {
      XtFree((char *)replacement); XtFree((char *)feedback);
      return;
    }
    if (text->feedback)
      memcpy(feedback + data->chg_first, text->feedback,
             inserted * sizeof(XIMFeedback));
  }
  if (data->chg_first) {
    memcpy(replacement, pb->text, data->chg_first * sizeof(wchar_t));
    memcpy(feedback, pb->feedback, data->chg_first * sizeof(XIMFeedback));
  }
  suffix = pb->length - data->chg_first - data->chg_length;
  if (suffix) {
    memcpy(replacement + data->chg_first + inserted,
           pb->text + data->chg_first + data->chg_length, suffix * sizeof(wchar_t));
    memcpy(feedback + data->chg_first + inserted,
           pb->feedback + data->chg_first + data->chg_length, suffix * sizeof(XIMFeedback));
  }
  XtFree((char *)pb->text); XtFree((char *)pb->feedback);
  pb->text = replacement; pb->feedback = feedback;
  pb->length = result; pb->caret = data->caret;
  proc = get_real_callback(w, PREEDIT_DRAW, &real);
  /* Some servers retain the previous caret while deleting all preedit.
   * A caret hint must not suppress an otherwise valid text replacement. */
  dispatch = *data;
  if ((unsigned int)dispatch.caret > result) dispatch.caret = (int)result;
  pb->caret = dispatch.caret;
  if (text && inserted) {
    /* XIMText is counted.  Widgets receive an owned terminated slice, never
     * read past provider storage when converting a multibyte preedit. */
    dispatch_characters = (wchar_t *)XtCalloc(inserted + 1, sizeof(wchar_t));
    memcpy(dispatch_characters, replacement + data->chg_first,
           inserted * sizeof(wchar_t));
    dispatch_text = *text;
    dispatch_text.encoding_is_wchar = True;
    dispatch_text.string.wide_char = dispatch_characters;
    dispatch_text.feedback = feedback + data->chg_first;
    dispatch.text = &dispatch_text;
  }
  if (proc) (*proc)(xic, callback_closure(w, real, PREEDIT_DRAW), (XPointer)&dispatch);
  XtFree((char *)dispatch_characters);
}

static void
ImPreeditCaretCallback(XIC xic,
		       XPointer client_data,
		       XPointer call_data)
{
  XICProc proc;
  Widget w = (Widget)client_data;
  XmImXICInfo icp;
  PreeditBuffer pb;
  XIMPreeditCaretCallbackStruct *data = 
		(XIMPreeditCaretCallbackStruct *) call_data;
  Widget real = NULL;

  if (!client_data || !data) return;

/* update the preedit buffer */
  if ((icp = callback_xic(w)) == NULL) return;

  pb = icp->preedit_buffer;
  if (!pb || pb->caret < 0 || pb->caret > pb->length) return;
  pb->style = data->style;

  switch (data->direction) {
    case XIMForwardChar:
	if (pb->caret < pb->length) ++pb->caret;
	break;
    case XIMBackwardChar:
	if (pb->caret > 0) --pb->caret;
	break;
    case XIMAbsolutePosition:
	if (data->position < 0 || data->position > pb->length) return;
        pb->caret = data->position;
	break;
    default:
	break;    
  }

  proc = get_real_callback((Widget)client_data, PREEDIT_CARET, &real);
  if (proc)
    (*proc)(xic, callback_closure(w, real, PREEDIT_CARET), call_data);
}

static XICProc
get_real_callback(Widget w,
                  int swc,
		  Widget *real_widget)
{
  XmImXICInfo icp;
  int i, target;
  XmImRefRec refs;

  if ((icp = callback_xic(w)) == NULL) return (XICProc)NULL;

  if (*real_widget == NULL) 
    *real_widget = XtWindowToWidget(XtDisplay(w), icp->focus_window);

  refs = icp->widget_refs;
  target = refs.num_refs;
  for (i = 0; i < refs.num_refs; i++){
    if (refs.refs[i] == *real_widget){
      target = i;
      break;
    }
  }
  if (target == refs.num_refs)
    return (XICProc)NULL;

  if (refs.callbacks[target])
    return (XICProc)refs.callbacks[target][swc].callback;
  else
    return (XICProc)NULL;
}

static void
regist_real_callback(Widget w,
                     const XIMCallback *call,
                     int swc)
{
  Widget p;
  register XmImXICInfo icp;
  XmImDisplayInfo xim_info;
  XmImRefRec refs;
  int i, target = 0;

  p = w;
  while (!XtIsShell(p))
  {
    p = XtParent(p);
  }

  xim_info = get_xim_info(p);
  if ((icp = get_current_xic(xim_info, w)) == NULL)
  {
    return;
  }

  refs = icp->widget_refs;

  for (i = 0; i < refs.num_refs; i++)
  {
    if (refs.refs[i] == w)
    {
      target = i;
      break;
    }
  }

  if (i == refs.num_refs)
    return;

  if (!refs.callbacks[target])
    refs.callbacks[target] = (XIMCallback *)XtCalloc(4, sizeof(XIMCallback));
  if (refs.callbacks[target] == NULL)
    return;

  refs.callbacks[target][swc] = *call;
}

static int
NameToSwitch(String name)
{

  if (!strcmp(name, XmNpreeditStartCallback))
    return PREEDIT_START;
  if (!strcmp(name, XmNpreeditDoneCallback))
    return PREEDIT_DONE;
  if (!strcmp(name, XmNpreeditDrawCallback))
    return PREEDIT_DRAW;
  if (!strcmp(name, XmNpreeditCaretCallback))
    return PREEDIT_CARET;

  return 100;
}

static void
set_callback_values(Widget w, String name, XIMCallback *value,
                    VaArgList vlp, XmInputPolicy input_policy)
{
  Widget shell = w;
  XmInputPolicy policy = input_policy;
  int kind = NameToSwitch(name);
  static XIMProc bridge[] = {
    (XIMProc)ImPreeditStartCallback, (XIMProc)ImPreeditDoneCallback,
    (XIMProc)ImPreeditDrawCallback, (XIMProc)ImPreeditCaretCallback
  };
  static String resource[] = {
    XNPreeditStartCallback, XNPreeditDoneCallback,
    XNPreeditDrawCallback, XNPreeditCaretCallback
  };
  while (!XtIsShell(shell)) shell = XtParent(shell);
  if (policy == XmINHERIT_POLICY)
    XtVaGetValues(shell, XmNinputPolicy, &policy, NULL);
  if (kind < 0 || kind >= 4) return;
  regist_real_callback(w, value, kind);
  value->client_data = (XPointer)(policy == XmPER_SHELL ? shell : w);
  value->callback = bridge[kind];
  VaSetArg(vlp, resource[kind], (XPointer)value);
}


static void
move_preedit_string(XmImXICInfo icp,
		    Widget wfrom,
		    Widget wto)
{
  PreeditBuffer pb = icp->preedit_buffer;
  XIMPreeditDrawCallbackStruct draw_data;
  XIMText text;
  XICProc proc;

  proc = get_real_callback(wfrom, PREEDIT_DONE, &wfrom);
  if (proc)
    (*proc)(icp->xic, callback_closure(wfrom, wfrom, PREEDIT_DONE), NULL);

  proc = get_real_callback(wto, PREEDIT_START, &wto);
  if (proc)
    (*proc)(icp->xic, callback_closure(wto, wto, PREEDIT_START), NULL);

  if (pb->length == 0)
    return;

  draw_data.caret = pb->caret;
  draw_data.chg_first = 0;
  draw_data.chg_length = 0;
  text.length = pb->length;
  text.feedback = pb->feedback;
  text.encoding_is_wchar = True;
  text.string.wide_char = pb->text;
  draw_data.text = &text;
  proc = get_real_callback(wto, PREEDIT_DRAW, &wto);
  if (proc)
    (*proc)(icp->xic, callback_closure(wto, wto, PREEDIT_DRAW), (XPointer)&draw_data);
}   



/*ARGSUSED*/
static int 
add_sp(String name,
       XPointer value,
       VaArgList slp,
       VaArgList plp,
       VaArgList vlp )		/* unused */
{
  VaSetArg(slp, name, value);
  VaSetArg(plp, name, value);
  
  return BG_CHG;
}

/*ARGSUSED*/
static int 
add_p(String name,
      XPointer value,
      VaArgList slp,		/* unused */
      VaArgList plp,
      VaArgList vlp )		/* unused */
{
  VaSetArg(plp, name, value);
  
  return 0;
}


/*ARGSUSED*/
static int 
add_fs(String name,
       XPointer value,
       VaArgList slp,
       VaArgList plp,
       VaArgList vlp )		/* unused */
{
  XFontSet fs;
  
  if ( (fs = extract_fontset((XmFontList)value)) == NULL)
    return 0;
  
  VaSetArg(slp, name, (XPointer) fs);
  VaSetArg(plp, name, (XPointer) fs);
  
  return GEO_CHG;
}

static int 
add_bgpxmp(String name,
	   XPointer value,
	   VaArgList slp,
	   VaArgList plp,
	   VaArgList vlp )
{
  if ( (Pixmap)value == XtUnspecifiedPixmap )
    return 0;
  
  return add_sp( name, value, slp, plp, vlp );
}

static XIMStyle 
check_style(XIMStyles *styles,
	    XIMStyle preedit_style,
	    XIMStyle status_style )
{
  register int i;
  
  /* Is this preedit & status style combination supported? */
  for (i=0; i < (int) styles->count_styles; i++)
    {
      if ((styles->supported_styles[i] & preedit_style) &&
	  (styles->supported_styles[i] & status_style))
	return styles->supported_styles[i];
    }
  return 0;
}


/* if this_icp is non-null, operations will only be performed on the
   corresponding IC. (Basically disables looping) */

static int 
ImGetGeo(Widget  vw,
	 XmImXICInfo this_icp )
{
  XmImXICInfo icp;
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  int height = 0;
  XRectangle rect;
  XRectangle *rp;
  XmImShellInfo im_info;
  XVaNestedList set_list, get_list;
  
  extData = _XmGetWidgetExtData((Widget)vw, XmSHELL_EXTENSION);
  if (extData)
    ve = (XmVendorShellExtObject) extData->widget;
  else
    ve = NULL;
  
  im_info = get_im_info(vw, False);
  
  if (ve && ((im_info == NULL) || (im_info->iclist == NULL))) {
    ve->vendor.im_height = 0;
    return 0;
  }

  set_list = XVaCreateNestedList(0, XNAreaNeeded, (XPointer) &rect, NULL);
  get_list = XVaCreateNestedList(0, XNAreaNeeded, (XPointer) &rp, NULL);
  
  for (icp = this_icp ? this_icp : im_info->iclist; 
       icp != NULL; 
       icp = icp->next) {
    if (icp->xic) {
      if (icp->input_style & XIMStatusArea) {
	rect.width = vw->core.width;
	rect.height = 0;
	
	XSetICValues(icp->xic, XNStatusAttributes, set_list, NULL);
	XGetICValues(icp->xic, XNStatusAttributes, get_list, NULL);
	
	if ((int) rp->height > height)
	  height = rp->height;
	
	icp->status_width = MIN(rp->width, vw->core.width);
	icp->sp_height = rp->height;
	  XFree(rp);
      }
      if (icp->input_style & XIMPreeditArea) {
	rect.width = vw->core.width;
	rect.height = 0;
	
	XSetICValues(icp->xic, XNPreeditAttributes, set_list, NULL);
	XGetICValues(icp->xic, XNPreeditAttributes, get_list, NULL);
	
	if ((int) rp->height > height)
	  height = rp->height;
	
	icp->preedit_width = MIN((int) rp->width,
				 (int) (vw->core.width - icp->status_width));
	if (icp->sp_height < (int) rp->height)
	  icp->sp_height = rp->height;
	XFree(rp);
      }
    }
    
    if (this_icp)
      break;
  }
  
  XFree(set_list);
  XFree(get_list);
  
  if (height)
    height += SEPARATOR_HEIGHT;
  
  if (ve)
    ve->vendor.im_height = height;
  return height;
}


/* if this_icp is non-null, operations will only be performed on the
   corresponding IC. (Basically disables looping) */

static void 
ImSetGeo(Widget  vw,
	 XmImXICInfo this_icp )
{
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  register XmImXICInfo icp;
  XRectangle rect_status;
  XRectangle rect_preedit;
  XmImShellInfo im_info;
  XVaNestedList va_slist, va_plist;
  unsigned long use_slist, use_plist;
  
  im_info = get_im_info(vw, False);
  if ((im_info == NULL) || (im_info->iclist == NULL))
    return;
  
  extData = _XmGetWidgetExtData((Widget)vw, XmSHELL_EXTENSION);
  if (!extData) return;
  ve = (XmVendorShellExtObject) extData->widget;
  
  if (ve->vendor.im_height == 0)
    return;
  
  va_slist = XVaCreateNestedList(0, XNArea, &rect_status, NULL);
  va_plist = XVaCreateNestedList(0, XNArea, &rect_preedit, NULL);
  
  for (icp = this_icp ? this_icp : im_info->iclist;
       icp != NULL; 
       icp = icp->next)
    {
      if ((use_slist = (icp->input_style & XIMStatusArea)) != 0)
	{
	  rect_status.x = 0;
	  rect_status.y = vw->core.height - icp->sp_height;
	  rect_status.width = icp->status_width;
	  rect_status.height = icp->sp_height;
	}
      
      if ((use_plist = (icp->input_style & XIMPreeditArea)) != 0)
	{
	  rect_preedit.x = icp->status_width;
	  rect_preedit.y = vw->core.height - icp->sp_height;
	  rect_preedit.width = icp->preedit_width;
	  rect_preedit.height = icp->sp_height;
        } else if ((use_plist = (icp->input_style & XIMPreeditPosition)) != 0)
        {
          unsigned int  margin;
#ifdef FIX_1129
        /*
         * im_info->current_widget can contains NULL,
         * for example, when widget having XIC focus is disposed.
         * Thus, we should check this and avoid dereferencing NULL pointer.
         */
          if (im_info->current_widget == NULL)
            break;
#endif
          margin = ((XmPrimitiveWidget)im_info->current_widget)
                  ->primitive.shadow_thickness
              + ((XmPrimitiveWidget)im_info->current_widget)
                  ->primitive.highlight_thickness;

            rect_preedit.width = MIN(icp->preedit_width,
                  XtWidth(im_info->current_widget) - 2*margin);
            rect_preedit.height = MIN(icp->sp_height,
                  XtHeight(im_info->current_widget) - 2*margin);
	}
      
      if (use_slist && use_plist)
	XSetICValues(icp->xic, 
		     XNStatusAttributes,  va_slist,
		     XNPreeditAttributes, va_plist,
		     NULL);
      else if (use_slist)
	XSetICValues(icp->xic,
		     XNStatusAttributes,  va_slist,
		     NULL);
      else if (use_plist)
	XSetICValues(icp->xic, 
		     XNPreeditAttributes, va_plist,
		     NULL);
      
      if (this_icp)
	break;
    }
  
  XFree(va_slist);
  XFree(va_plist);
}

static void 
ImGeoReq(Widget vw )
{
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  XtWidgetGeometry my_request;
  int old_height;
  int delta_height;
  ShellWidget shell = (ShellWidget)(vw);
  
  if (!(shell->shell.allow_shell_resize) && XtIsRealized(vw))
    return;
  
  extData = _XmGetWidgetExtData(vw, XmSHELL_EXTENSION);
  if (!extData) return;
  ve = (XmVendorShellExtObject) extData->widget;
  
  old_height = ve->vendor.im_height;
  ImGetGeo(vw, NULL);
  if ((delta_height = ve->vendor.im_height - old_height) != 0)
    {
      int base_height;
      Arg args[1];
      XtSetArg(args[0], XtNbaseHeight, &base_height);
      XtGetValues(vw, args, 1);
      if (base_height > 0) {
	base_height += delta_height;
	XtSetArg(args[0], XtNbaseHeight, base_height);
	XtSetValues(vw, args, 1);
      }
      my_request.height = vw->core.height + delta_height;
      my_request.request_mode = CWHeight;
      XtMakeGeometryRequest(vw, &my_request, NULL);
    }
  ImSetGeo(vw, NULL);
}

static XFontSet 
extract_fontset(
		XmFontList fl )
{
  XmFontContext context;
  XmFontListEntry next_entry;
  XmFontType type_return;
  XtPointer tmp_font;
  XFontSet first_fs = NULL;
  char *font_tag = NULL;
  
  if (!XmFontListInitFontContext(&context, fl))
    return NULL;
  
  do {
    next_entry = XmFontListNextEntry(context);
    if (next_entry)
      {
	tmp_font = XmFontListEntryGetFont(next_entry, &type_return);
	if (type_return == XmFONT_IS_FONTSET)
	  {
	    font_tag = XmFontListEntryGetTag(next_entry);
	    if (!strcmp(font_tag, XmFONTLIST_DEFAULT_TAG))
	      {
		XmFontListFreeFontContext(context);
		if (font_tag) XtFree(font_tag);
		return (XFontSet)tmp_font;
	      }
	    if (font_tag) XtFree(font_tag);
	    if (first_fs == NULL)
	      first_fs = (XFontSet)tmp_font;
	  }
      }
  } while (next_entry);
  
  XmFontListFreeFontContext(context);
  return first_fs;
}

/* Fetch (creating if necessary) the Display's xmim_info. */
static XmImDisplayInfo
get_xim_info(Widget  widget)
{
  XmDisplay xmDisplay;
  char *cp = NULL;
  XmImDisplayInfo xim_info;
  String name, w_class;
  Display *dpy;
  Widget shell;
  
  if (widget == NULL)
    return NULL;
  
  /* Find the parent shell. */
  shell = widget;
  while (!XtIsShell(shell))
    shell = XtParent(shell);
  
  dpy = XtDisplay(shell);
  xmDisplay = (XmDisplay) XmGetXmDisplay(dpy);
  xim_info = (XmImDisplayInfo)xmDisplay->display.xmim_info;
  
  /* If this is a simple lookup we're done. */
  if (xim_info != NULL)
    return xim_info;
  
  /* Keep registration ownership independent of the current XIM transport. */
  xim_info = XtNew(XmImDisplayRec);
  if (xim_info == NULL)
    return NULL;
  bzero((char*) xim_info, sizeof(XmImDisplayRec));
  xmDisplay->display.xmim_info = (XtPointer)xim_info;
  
  /* Setup any specified locale modifiers. */
  XtVaGetValues(shell, XmNinputMethod, &cp, NULL);
  if (cp != NULL)
    {
      size_t cp_length = strlen(cp);
      if (cp_length <= (size_t)UINT_MAX - 5) {
        char *tmp = (char *)XtMalloc((Cardinal)(cp_length + 5));
        if (tmp != NULL) {
          memcpy(tmp, "@im=", 4);
          memcpy(tmp + 4, cp, cp_length + 1);
          XSetLocaleModifiers(tmp);
          XtFree(tmp);
        }
      }
    }
  
  XtGetApplicationNameAndClass(dpy, &name, &w_class);
  
  xim_info->display = dpy;
  xim_info->application_name = XtNewString(name);
  xim_info->application_class = XtNewString(w_class);
  if (!open_input_method(xim_info))
    xim_info->instantiate_registered = XRegisterIMInstantiateCallback(dpy,
      XtDatabase(dpy), name, w_class, im_available, (XPointer)xim_info);

  /* Initialize the list of xrm names */
  {
    XmImResListRec *rlp;
    register int i;

    _XmProcessLock();
    for (rlp = XmImResList, i = XtNumber(XmImResList); 
	 i != 0; 
	 i--, rlp++)
      rlp->xrmname = XrmStringToName(rlp->xmstring);
    _XmProcessUnlock();
  }
  
  return xim_info;
}

static XtPointer*
get_im_info_ptr(Widget  w,
		Boolean create)
{
  Widget p;
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  XmImShellInfo im_info;
  XmImDisplayInfo xim_info;
  
  if (w == NULL)
    return NULL;
  
  p = w;
  while (!XtIsShell(p))
    p = XtParent(p);
  
  /* Check extension data since app could be attempting to create
   * a text widget as child of menu shell.  This is illegal, and will
   * be detected later, but check here so we don't core dump.
   */
  if ((extData = _XmGetWidgetExtData((Widget)p, XmSHELL_EXTENSION)) == NULL)
    return NULL;
  
  ve = (XmVendorShellExtObject) extData->widget;
  
  if ((ve->vendor.im_info == NULL) && create)
    {
      im_info = XtNew(XmImShellRec);
      bzero((char*) im_info, sizeof(XmImShellRec));
      ve->vendor.im_info = (XtPointer)im_info;
      
      xim_info = get_xim_info(p);
      (void) add_ref(&xim_info->shell_refs, p);
    }
  
  return &ve->vendor.im_info;
}

static XmImShellInfo
get_im_info(Widget w,
	    Boolean create)
{
  XmImShellInfo* ptr = (XmImShellInfo *) get_im_info_ptr(w, create);
  if (ptr != NULL)
    return *ptr;
  else
    return NULL;
}

static void 
draw_separator(Widget vw )
{
  XmPrimitiveWidget pw;
  XmVendorShellExtObject ve;
  XmWidgetExtData extData;
  XmImShellInfo im_info;
  
  extData = _XmGetWidgetExtData((Widget)vw, XmSHELL_EXTENSION);
  if (!extData) return;
  ve = (XmVendorShellExtObject) extData->widget;
  if ((im_info = (XmImShellInfo)ve->vendor.im_info) == NULL)
    return; 
  pw = (XmPrimitiveWidget)im_info->current_widget;
  if (!pw || !XmIsPrimitive(pw))
    return;
  
  XmeDrawSeparator(XtDisplay(vw), XtWindow(vw),
		   pw->primitive.top_shadow_GC,
		   pw->primitive.bottom_shadow_GC,
		   0,
		   0,
		   vw->core.height - ve->vendor.im_height,
		   vw->core.width,
		   SEPARATOR_HEIGHT,
		   SEPARATOR_HEIGHT,
		   0, 			/* separator.margin */
		   XmHORIZONTAL,		/* separator.orientation */
		   XmSHADOW_ETCHED_IN); /* separator.separator_type */
}

/*ARGSUSED*/
static void 
null_proc(Widget w,		/* unused */
	  XtPointer ptr,		/* unused */
	  XEvent *ev,		/* unused */
	  Boolean *b )		/* unused */
{
  /* This function does nothing.  It is only there to allow the
   * event mask required by the input method to be added to
   * the client window.
   */
}

/* The following section contains the varargs functions */


/*VARARGS*/
void 
XmImVaSetFocusValues(Widget w,
		     ... )
{
  va_list	var;
  int	    	total_count;
  ArgList     args;
  _XmWidgetToAppContext(w);
  
  _XmAppLock(app);
  Va_start(var,w);
  ImCountVaList(var, &total_count);
  va_end(var);
  
  Va_start(var,w);
  args  = ImCreateArgList(var, total_count);
  va_end(var);
  
  XmImSetFocusValues(w, args, total_count);
  XtFree((char *)args);
  _XmAppUnlock(app);
}

/*VARARGS*/
void
  XmImVaSetValues(Widget w,
		  ... )
{
  va_list	var;
  int	    	total_count;
  ArgList     args;
  _XmWidgetToAppContext(w);
  
  _XmAppLock(app);
  Va_start(var,w);
  ImCountVaList(var, &total_count);
  va_end(var);
  
  Va_start(var,w);
  args  = ImCreateArgList(var, total_count);
  va_end(var);
  
  XmImSetValues(w, args, total_count);
  XtFree((char *)args);
  _XmAppUnlock(app);
}


static void 
ImCountVaList(va_list var,
	      int *total_count )
{
  String          attr;
  
  *total_count = 0;
  
  for(attr = va_arg(var, String); attr != NULL; attr = va_arg(var, String)) 
    {
      (void) va_arg(var, XPointer);
      ++(*total_count);
    }
}

static ArgList
ImCreateArgList(va_list var,
		int total_count )
{
  ArgList args = (ArgList)XtCalloc(total_count, sizeof(Arg));
  register int i;
  
  assert(args || (total_count == 0));
  for (i = 0; i < total_count; i++)
    {
      args[i].name = va_arg(var,String);
      args[i].value = (XtArgVal)va_arg(var,XPointer);
    }
  
  return args;
}

/* Return the current xic info for a widget, or NULL. */
static XmImXICInfo
get_current_xic(XmImDisplayInfo xim_info, 
		Widget          widget)
{
  XmImXICInfo xic_info;
  
  if ((xim_info == NULL) ||
      (xim_info->current_xics == (XContext) 0))
    return NULL;
  
  if (XFindContext(XtDisplay(widget), (XID) widget, 
		   xim_info->current_xics, (XPointer*) &xic_info) != 0)
    return NULL;
  else
    return xic_info;
}

/* Set the current XIC for an unregistered widget. */
static void 
set_current_xic(XmImXICInfo 	xic_info,
		XmImDisplayInfo	xim_info,
		Widget		widget)
{
  if (xic_info == NULL)
    return;
  
  /* Record this widget as a reference to this XIC. */
  (void) add_ref(&xic_info->widget_refs, widget);
  
  /* Set the current XIC for this widget. */
  if (xim_info->current_xics == (XContext)0)
    xim_info->current_xics = XUniqueContext();
  (void) XSaveContext(XtDisplay(widget), (XID) widget, 
		      xim_info->current_xics, (XPointer) xic_info);
}

/* Unset the current XIC for a widget, freeing data as necesary. */
static void 
unset_current_xic(XmImXICInfo	  xic_info,
		  XmImShellInfo   im_info,
		  XmImDisplayInfo xim_info,
		  Widget 	  widget)
{
  /* Remove the current xic for this widget. */
  assert(xim_info->current_xics != (XContext) 0);
  (void) XDeleteContext(XtDisplay(widget), (XID) widget, 
			xim_info->current_xics);
#ifdef FIX_1196
  if (im_info->current_widget == widget)
    im_info->current_widget = NULL;
#endif
  /* Remove this widget as a reference to this XIC. */
  if (remove_ref(&xic_info->widget_refs, widget) == 0)
    {
      /* Remove this xic_info from the master list. */
      XmImXICInfo *ptr;
      for (ptr = &(im_info->iclist); *ptr != NULL; ptr = &((*ptr)->next))
	if (*ptr == xic_info)
	  {
	    *ptr = xic_info->next;
	    break;
	  }
#ifndef FIX_1196
      if (im_info->current_widget == widget)
	im_info->current_widget = NULL;
#endif
      /* Don't let anyone share this XIC. */
      if (xic_info->source != NULL)
	*(xic_info->source) = NULL;
      
      /* Destroy the XIC */
      if ((xic_info->anonymous) && (xic_info->xic != NULL))
	XDestroyIC(xic_info->xic);
      
      ImFreePreeditBuffer (xic_info->preedit_buffer);
      XtFree((char *) xic_info);
    }
}

/* Add a widget to a list of references. */
static Cardinal
add_ref(XmImRefInfo refs,
	Widget	    widget)
{
#ifdef DEBUG
  /* Verify that we don't already have a reference. */
  register Cardinal index;
  for (index = 0; index < refs->num_refs; index++)
    assert(refs->refs[index] != widget);
#endif
  
  /* Make room in the array. */
  if (refs->num_refs == refs->max_refs)
    {
      if (refs->max_refs == 0)
	refs->max_refs = 10;
      else
	refs->max_refs += (refs->max_refs / 2);
      
      refs->refs = (Widget*) XtRealloc((char *) refs->refs, 
				       refs->max_refs * sizeof(Widget));
      refs->callbacks = (XIMCallback **) XtRealloc((char *) refs->callbacks,
                                       refs->max_refs * sizeof(XIMCallback *));
    }
  assert(refs->num_refs < refs->max_refs);
  
  refs->callbacks[refs->num_refs] = NULL;

  /* Insert this reference. */
  refs->refs[refs->num_refs++] = widget;
  
  return refs->num_refs;
}

/* Remove a widget from a list of references. */
static Cardinal
remove_ref(XmImRefInfo refs,
	   Widget      widget)
{
	int index;

	if (refs == NULL || refs->num_refs == 0)
	  return 0;

	for (index = 0; index < (int)refs->num_refs; index++)
	  if (refs->refs[index] == widget)
	    break;
	if (index == (int)refs->num_refs)
	  return refs->num_refs;

  /* Is this the last reference? */
  refs->num_refs--;
  if (refs->num_refs > 0)
    {
      /* Just remove this reference. */
      {
	refs->refs[index] = refs->refs[refs->num_refs];
	refs->refs[refs->num_refs] = NULL;
            XtFree((char *)refs->callbacks[index]);
            refs->callbacks[index] = refs->callbacks[refs->num_refs];
            refs->callbacks[refs->num_refs] = NULL;
      }
      
      /* Free some storage from the array? */
      if ((refs->num_refs * 3 < refs->max_refs) &&
	  (refs->max_refs >= 20))
	{
	  refs->max_refs /= 2;
	  refs->refs = (Widget*) XtRealloc((char *) refs->refs, 
					   refs->max_refs * sizeof(Widget));
          refs->callbacks = (XIMCallback **) XtRealloc((char *) refs->callbacks,
                                   refs->max_refs * sizeof(XIMCallback *));
	}
    }
  else
    {
      /* Free the references array. */
      XtFree((char *) refs->refs);
      refs->refs = NULL;
      XtFree((char *) refs->callbacks[0]);
      XtFree((char *) refs->callbacks);
      refs->callbacks = NULL;
      refs->max_refs = 0;
    }
  
  return refs->num_refs;
}

/* Convert a VaArgList into a true XVaNestedList. */
static XVaNestedList
VaCopy(VaArgList list)
{
  /* This is ugly, but it's a legal way to construct a nested */
  /* list whose length is unknown at compile time.  If MAXARGS is */
  /* increased more parameter pairs should be added below.   A */
  /* recursive approach would leak memory. */
  register Cardinal count = list->count;
  register VaArg *args = list->args;
  
#define VA_NAME(index)	(index < count ? args[index].name : NULL)
#define VA_VALUE(index)	(index < count ? args[index].value : NULL)
  
  assert(count <= 10);
  return XVaCreateNestedList(0, 
			     VA_NAME(0), VA_VALUE(0),
			     VA_NAME(1), VA_VALUE(1),
			     VA_NAME(2), VA_VALUE(2),
			     VA_NAME(3), VA_VALUE(3),
			     VA_NAME(4), VA_VALUE(4),
			     VA_NAME(5), VA_VALUE(5),
			     VA_NAME(6), VA_VALUE(6),
			     VA_NAME(7), VA_VALUE(7),
			     VA_NAME(8), VA_VALUE(8),
			     VA_NAME(9), VA_VALUE(9),
			     NULL);
  
#undef VA_NAME
#undef VA_VALUE
}

static void
VaSetArg(VaArgList list, 
	 char     *name, 
	 XPointer  value)
{
  if (list->max <= list->count)
    {
      list->max += 10;
      list->args = (VaArg*) XtRealloc((char*) list->args, 
				      list->max * sizeof(VaArg));
    }
  
  list->args[list->count].name = name;
  list->args[list->count].value = value;
  list->count++;
}


void
XmImMbResetIC(
    Widget w,
    char **mb)
{
    register XmImXICInfo icp;
    _XmWidgetToAppContext(w);

    _XmAppLock(app);

    *mb = NULL;

    if ((icp = get_current_xic(get_xim_info(w), w)) == NULL || 
	icp->xic == NULL) {
	_XmAppUnlock(app);
        return;
    }

    if (!(icp->input_style & XIMPreeditCallbacks)) {
	_XmAppUnlock(app);
	return;
    }

    *mb = XmbResetIC(icp->xic);
    if (icp->preedit_active && XmImGetXICResetState(w) != XIMPreserveState)
        ImPreeditDoneCallback(icp->xic, (XPointer)w, NULL);
    _XmAppUnlock(app);
}

XIMResetState
XmImGetXICResetState(Widget w)
{
    XmImXICInfo icp;
    XIMResetState state = XIMInitialState;
    icp = get_current_xic(get_xim_info(w), w);
    if (icp != NULL && icp->xic != NULL)
        XGetICValues(icp->xic, XNResetState, &state, NULL);
    return state;
}
