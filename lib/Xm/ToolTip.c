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

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif

#include <Xm/LabelP.h>
#include <Xm/VendorSEP.h>
#include <Xm/GadgetP.h>
#include <Xm/TraitP.h>
#include <Xm/ToolTipCT.h>
#include <Xm/ToolTipT.h>
#include "BaseClassI.h"
#include "ToolTipI.h"
#include "XmI.h"

static void
ToolTipFrame(Widget label, Position x, Position y, Dimension width,
             Dimension height)
{
   Widget shell = XtParent(label);
   XtVaSetValues(shell, XmNx, x, XmNy, y, XmNwidth, width,
                 XmNheight, height, NULL);
   XtResizeWidget(label, width, height, 0);
}

#ifdef FIX_1388
static void ToolTipLabelDestroyCallback(Widget w, XtPointer client_data, XtPointer call_data)
{
    XmToolTipConfigTrait ttcp;

    if (w == NULL)
	return;
    
    ttcp = (XmToolTipConfigTrait) XmeTraitGet(w, XmQTtoolTipConfig);
    
    if (ttcp != NULL)
	if (ttcp->label != NULL) {
	    Widget shell = XtParent(ttcp->label);
	    XtDestroyWidget(ttcp->label);
	    ttcp->label = NULL;
	    if (shell != NULL)
		XtDestroyWidget(shell);
	}
}
#endif /* FIX_1388 */

static XmToolTipConfigTrait
ToolTipGetData (Widget w)
{
   Widget top = w;
   XmToolTipConfigTrait ttp;

   if (w == NULL)
      return NULL;

   while (XtParent (top))
   {
      top = XtParent (top);
   }

   ttp = (XmToolTipConfigTrait) XmeTraitGet (top, XmQTtoolTipConfig);

   if (ttp != NULL && ttp->label == NULL && !top->core.being_destroyed)
   {
      Widget shell;

      shell = XtVaCreateWidget ("TipShell",
                                transientShellWidgetClass, top,
                                XmNoverrideRedirect, True,
                                XmNallowShellResize, True, NULL);
      if (shell == NULL)
         return ttp;

      ttp->label = XmCreateLabel (shell, "TipLabel", NULL, 0);
      if (ttp->label == NULL) {
         XtDestroyWidget (shell);
         return ttp;
      }
      XtManageChild (ttp->label);
#ifdef FIX_1388
      XtAddCallback(top, XmNdestroyCallback, (XtCallbackProc) ToolTipLabelDestroyCallback, (XtPointer) NULL);
#endif
   }
   return ttp;
}

static void
ToolTipUnpost (XtPointer client_data,
               XtIntervalId * id)
{
   XmToolTipConfigTrait ttp;

   ttp = (XmToolTipConfigTrait) client_data;

   if (ttp == NULL)
      return;

   if (ttp->duration_timer)
   {
      if (!id || (id && (*id != ttp->duration_timer)))
      {
         XtRemoveTimeOut (ttp->duration_timer);
      }
      ttp->duration_timer = (XtIntervalId) NULL;
   }
   if (ttp->slider)
   {
      XtDestroyWidget (ttp->slider);
      ttp->slider = NULL;
   }

   if (ttp->label != NULL) {
      Widget shell = XtParent (ttp->label);
      if (shell != NULL && !((CoreWidget)shell)->core.being_destroyed)
         XtPopdown (shell);
   }

}

static void
ToolTipStartDuration (XmToolTipConfigTrait ttp)
{
   if (ttp == NULL || ttp->label == NULL)
      return;
   if (ttp->post_duration > 0)
      ttp->duration_timer = XtAppAddTimeOut (
         XtWidgetToApplicationContext (ttp->label),
         (unsigned long) ttp->post_duration, ToolTipUnpost, ttp);
}

static void
ToolTipPost (XtPointer client_data,
             XtIntervalId * id)
{
   Widget w = (Widget) client_data;

   int rx,
     ry,
     x,
     y;
   unsigned int key;
   Window root,
     child;
   XtWidgetGeometry geo = {0};
   Dimension width, height, extra_x, extra_y;
   int screen_w, screen_h;
   Pixel outline, background, foreground;
   Widget shell;
   Position destX,
     destY;

   XmToolTipConfigTrait ttp; /* ToolTip pointer */

   ttp = ToolTipGetData (w);

   if (ttp == NULL)
   {
      XtWarning ("ToolTipGetData() returned NULL in ToolTipPost()");
      return;
   }

   ttp->timer = (XtIntervalId) NULL;
   if (ttp->label == NULL || w == NULL ||
       ((CoreWidget)w)->core.being_destroyed)
      return;

   XQueryPointer (XtDisplay (w),
                  XtWindow (w), &root, &child, &rx, &ry, &x, &y, &key);

   if (ttp->duration_timer != (XtIntervalId) NULL)
   {
      XtRemoveTimeOut (ttp->duration_timer);
      ttp->duration_timer = (XtIntervalId) NULL;
   }

   if (XmIsPrimitive(w) || XmIsGadget(w))
   {
      XtVaSetValues (ttp->label,
                     XmNlabelString,
                     XmGetToolTipString(w),
                     NULL);
   }
   else
   {
      XmString string;

      string = XmStringCreateLocalized (XtName (w));
      XtVaSetValues (ttp->label, XmNlabelString, string, NULL);
      XmStringFree (string);
   }
   XtQueryGeometry (ttp->label, NULL, &geo);
   width = (geo.request_mode & CWWidth) ? geo.width : XtWidth(ttp->label);
   height = (geo.request_mode & CWHeight) ? geo.height : XtHeight(ttp->label);
   if (width == 0) width = 1;
   if (height == 0) height = 1;
   /* Keep the one-pixel outline on screen. Grow only the padding, never
    * from a rectangle smaller than the natural label. */
   screen_w = MAX(1, WidthOfScreen(XtScreen(w)) - 2);
   screen_h = MAX(1, HeightOfScreen(XtScreen(w)) - 2);
   width = MIN(width, screen_w);
   height = MIN(height, screen_h);
   extra_x = MIN(10, screen_w - width);
   extra_y = MIN(6, screen_h - height);
   width += extra_x;
   height += extra_y;

   /* rws 25 Feb 2001
      Fix for Bug #1153
      Don't let the tip be off the right/bottom of the screen
    */
   destX = rx + (XmIsGadget (w) ? XtX (w) : 0) - x + XtWidth (w) / 2;
   if (destX + width > screen_w)
   {
      destX = screen_w - width;
   }
   destY = ry + (XmIsGadget (w) ? XtY (w) : 0) - y + XtHeight (w);
   if (destY + height > screen_h)
   {
      destY = ry + (XmIsGadget (w) ? XtY (w) : 0) - y - height - 2;
   }

   if (destX < 0) destX = 0;
   if (destY < 0) destY = 0;

   /* Lay out the full label before mapping. Growing a mapped shell from 1x1
    * clips the text and leaves resize trails on composited X11 servers. */
   shell = XtParent(ttp->label);
   XtVaGetValues(ttp->label, XmNbottomShadowColor, &outline,
                 XmNbackground, &background, XmNforeground, &foreground, NULL);
   if (outline == background) outline = foreground;
   XtVaSetValues(shell, XmNborderWidth, 1, XmNborderColor, outline, NULL);
   ToolTipFrame(ttp->label, destX, destY, width, height);
   XtPopup(XtParent(ttp->label), XtGrabNone);
   ToolTipStartDuration(ttp);
}

/*
=====================================
Publically available functions follow
=====================================
*/

void
_XmToolTipEnter (Widget wid,
                 XEvent * event,
                 String * params,
                 Cardinal * num_params)
{
   XmToolTipConfigTrait ttp;           /* ToolTip pointer */

   ttp = ToolTipGetData (wid);

   if (ttp == NULL)
   {
      XtWarning ("ToolTipGetData() returned NULL in _XmToolTipEnter()");
      return;
   }
   if ((XmIsPrimitive (wid) || XmIsGadget(wid)) && XmGetToolTipString(wid))
   {
      if (ttp->enable && !ttp->timer)
      {
         unsigned long delay;

         if (event &&
             (event->xcrossing.time - ttp->leave_time < ttp->post_delay))
         {
            delay = 0;
         }
         else
         {
            delay = (unsigned long) ttp->post_delay;
         }
         if (ttp->duration_timer)
         {
            XtRemoveTimeOut (ttp->duration_timer);
            ttp->duration_timer = (XtIntervalId) NULL;
         }
         ttp->timer =
            XtAppAddTimeOut (XtWidgetToApplicationContext (wid), delay,
                             (XtTimerCallbackProc) ToolTipPost, wid);
      }
   }
}

void
_XmToolTipLeave (Widget w,
                 XEvent * event,
                 String * params,
                 Cardinal * num_params)
{
   XmToolTipConfigTrait ttp = ToolTipGetData (w);

   if (!ttp)
   {
      XtWarning ("_XmToolTipLeave() - ttp == NULL.");
      return;
   }


   if (ttp->timer)
   {
      XtRemoveTimeOut (ttp->timer);
      ttp->timer = (XtIntervalId) NULL;
   }
   else
   {
      if (event && (ttp->duration_timer || ttp->post_duration == 0))
      {
         ttp->leave_time = event->xcrossing.time;
      }
      ToolTipUnpost (ttp, NULL);
   }

}

#ifdef FIX_1388
void _XmToolTipRemove(Widget w)
{
    XmToolTipTrait ttp;
    
    _XmToolTipLeave(w, NULL, NULL, NULL);
    ttp = (XmToolTipTrait) XmeTraitGet(w, XmQTtoolTip);
    if (ttp != NULL) {
	XmStringFree(ttp->tool_tip_string); 
	XmeTraitSet(w, XmQTtoolTip, (XtPointer) NULL); 
	XtFree((char*)ttp);
    }
}
#endif /* FIX_1388 */

Widget 
XmToolTipGetLabel(Widget wid)
{
    XmToolTipConfigTrait TipData = ToolTipGetData(wid);
    
#ifdef BUG1232
/* rws 25 Sep 2003
   protect against NULL TipData
 */
#endif
    return(TipData ? TipData->label : NULL);
}

XmString
XmGetToolTipString (Widget w)
{
    XmToolTipTrait ttp;
    ttp = (XmToolTipTrait)XmeTraitGet(w, XmQTtoolTip);
    return ttp ? ttp->tool_tip_string : NULL;    
}

void
XmSetToolTipString (Widget w,
                    XmString s)
{
    XmToolTipTrait ttp;
    XmString replacement;
    ttp = (XmToolTipTrait)XmeTraitGet(w, XmQTtoolTip);
    if (!ttp) {
        ttp = (XmToolTipTrait)XtCalloc(1, sizeof(XmToolTipTraitRec));
        if (!ttp)
            return;
        XmeTraitSet(w, XmQTtoolTip, ttp);
    }
    replacement = s != NULL ? XmStringCopy(s) : NULL;
    if (s != NULL && replacement == NULL)
        return;
    if (ttp->tool_tip_string)
        XmStringFree(ttp->tool_tip_string);
    ttp->tool_tip_string = replacement;
}
