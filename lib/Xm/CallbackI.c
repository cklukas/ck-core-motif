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
#ifdef REV_INFO
#ifndef lint
static char rcsid[] = "$XConsortium: CallbackI.c /main/8 1995/07/14 10:13:10 drk $"
#endif
#endif

#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include "CallbackI.h"
#include <limits.h>
#include <stdint.h>


/********************************************************************
 * VendorShell and Dialog Shell's extension objects are no longer 
 * full fledged Xt objects and therefore  cannot be passed as an
 * argument  to the Xt calls XtAddCallback(), XtRemoveCallback(),
 * and XtCallCallbackList(). The functions _XmAddCallback()
 * _XmRemoveCallback() and _XmCallCallbackList() replace these 
 * Xt calls for non Xt objects.
 ********************************************************************/


/* However it doesn't contain a final NULL record */
#define ToList(p)	((XtCallbackList) (&(p)->callbacks))

void
_XmAddCallback(InternalCallbackList* callbacks,
	       XtCallbackProc        callback,
	       XtPointer             closure)
{
  register XtCallbackList cl;
  register InternalCallbackList icl = *callbacks;
  register InternalCallbackList new_icl;
  register int count = icl ? icl->count : 0;

  if (count == USHRT_MAX ||
      (size_t)count >
          (SIZE_MAX - sizeof(InternalCallbackRec)) /
              sizeof(XtCallbackRec) ||
      sizeof(InternalCallbackRec) + sizeof(XtCallbackRec) *
          (size_t)count > UINT_MAX)
    return;
  
  if (icl && icl->call_state) 
    {
      icl->call_state |= _XtCBFreeAfterCalling;
	  new_icl = (InternalCallbackList)
	XtMalloc(sizeof(InternalCallbackRec) + sizeof(XtCallbackRec) * count);
	  if (new_icl == NULL) {
	    icl->call_state &= ~_XtCBFreeAfterCalling;
	    return;
	  }
	  icl = new_icl;
      
      memcpy((char *)ToList(icl), (char *)ToList(*callbacks),
	     sizeof(XtCallbackRec) * count);
    } 
  else 
    {
	  new_icl = (InternalCallbackList)
	XtRealloc((char *) icl, sizeof(InternalCallbackRec) +
		  sizeof(XtCallbackRec) * count);
	  if (new_icl == NULL)
	    return;
	  icl = new_icl;
    }

  *callbacks = icl;
  icl->count = count + 1;
  icl->is_padded = 0;
  icl->call_state = 0;
  cl = ToList(icl) + count;
  cl->callback = callback;
  cl->closure = closure;
}

void
 _XmRemoveCallback (InternalCallbackList *callbacks,
		    XtCallbackProc        callback,
		    XtPointer             closure)
{
  register int i, j;
  register XtCallbackList cl, ncl, ocl;
  register InternalCallbackList icl = *callbacks;
  register InternalCallbackList new_icl;

  if (!icl) 
    return;
  
  cl = ToList(icl);
  for (i = icl->count; --i >= 0; cl++) 
    {
      if (cl->callback == callback && cl->closure == closure) 
	{
	  if (icl->call_state) 
	    {
	      icl->call_state |= _XtCBFreeAfterCalling;
	      if (icl->count == 1) 
		{
		  *callbacks = NULL;
		} 
	      else 
		{
		  j = icl->count - i - 1;
		  ocl = ToList(icl);
		  new_icl = (InternalCallbackList)
		    XtMalloc(sizeof(InternalCallbackRec) +
			     sizeof(XtCallbackRec) * (i + j - 1));
		  if (new_icl == NULL) {
		    icl->call_state &= ~_XtCBFreeAfterCalling;
		    return;
		  }
		  icl = new_icl;
		  icl->count = i + j;
		  icl->is_padded = 0;
		  icl->call_state = 0;
		  ncl = ToList(icl);
		  while (--j >= 0)
		    *ncl++ = *ocl++;
		  while (--i >= 0)
		    *ncl++ = *++cl;
		  *callbacks = icl;
		}
	    }
	  else 
	    {
	      if (--icl->count) 
		{
		  ncl = cl + 1;
		  while (--i >= 0)
		    *cl++ = *ncl++;
		  new_icl = (InternalCallbackList)
		    XtRealloc((char *) icl, sizeof(InternalCallbackRec)
			      + sizeof(XtCallbackRec) * (icl->count - 1));
		  if (new_icl == NULL) {
		    *callbacks = icl;
		    return;
		  }
		  icl = new_icl;
		  icl->is_padded = 0;
		  *callbacks = icl;
		} else {
		  XtFree((char *) icl);
		  *callbacks = NULL;
		}
	    }
	  return;
	}
    }
}

void
_XmRemoveAllCallbacks(InternalCallbackList *callbacks)
{
  register InternalCallbackList icl = *callbacks;
  
  if (icl) 
    {
      if (icl->call_state)
	icl->call_state |= _XtCBFreeAfterCalling;
      else
	XtFree((char *) icl);

      *callbacks = NULL;
    }
}

void 
_XmCallCallbackList(Widget widget,
		    XtCallbackList callbacks,
		    XtPointer call_data)
{
  register InternalCallbackList icl;
  register XtCallbackList cl;
  register int i;
  char ostate;
  
  if (!callbacks) 
    return;

  icl = (InternalCallbackList)callbacks;
  cl = ToList(icl);
  if (icl->count == 1) 
    {
      (*cl->callback) (widget, cl->closure, call_data);
      return;
    }

  ostate = icl->call_state;
  icl->call_state = _XtCBCalling;

  for (i = icl->count; --i >= 0; cl++)
    (*cl->callback) (widget, cl->closure, call_data);

  if (ostate)
    icl->call_state |= ostate;
  else if (icl->call_state & _XtCBFreeAfterCalling)
    XtFree((char *)icl);
  else
    icl->call_state = 0;
}

/* Observe immediate destruction during application callbacks. */
static void
CallbackDestroyed(Widget widget, XtPointer closure, XtPointer call_data)
{
  (void)widget;
  (void)call_data;
  *(Boolean *)closure = True;
}

/* Xt's compiled callback storage can be freed by immediate widget destruction
 * while XtCallCallbackList is still iterating it. Own the dispatch snapshot and
 * stop when the object is destroyed, including deferred event destruction. */
Boolean
_XmCallCallbacksUntilDestroyed(Widget w, String resource, XtPointer data)
{
  XtCallbackList callbacks = NULL;
  XtCallbackRec *snapshot;
  Cardinal count = 0;
  const Cardinal maximum = UINT_MAX / sizeof(*snapshot);
  Boolean destroyed = False, alive;
  if (w == NULL || resource == NULL)
    return False;
  _XmWidgetToAppContext(w);
  _XmAppLock(app);
  XtVaGetValues(w, resource, &callbacks, NULL);
  if (callbacks) {
    while (callbacks[count].callback) {
      if (count == maximum) {
        alive = !((Object)w)->object.being_destroyed;
        _XmAppUnlock(app);
        return alive;
      }
      ++count;
    }
  }
  if (count == 0) {
    alive = !((Object)w)->object.being_destroyed;
    _XmAppUnlock(app);
    return alive;
  }
  snapshot = (XtCallbackRec *)XtMalloc((Cardinal)((size_t)count * sizeof(*snapshot)));
  if (snapshot == NULL) {
    alive = !((Object)w)->object.being_destroyed;
    _XmAppUnlock(app);
    return alive;
  }
  memcpy(snapshot, callbacks, (size_t)count * sizeof(*snapshot));
  XtAddCallback(w, XmNdestroyCallback, CallbackDestroyed, &destroyed);
  for (Cardinal i = 0; i < count; ++i) {
    if (destroyed || ((Object)w)->object.being_destroyed) break;
    snapshot[i].callback(w, snapshot[i].closure, data);
  }
  if (!destroyed)
    XtRemoveCallback(w, XmNdestroyCallback, CallbackDestroyed, &destroyed);
  XtFree((char *)snapshot);
  alive = !destroyed && !((Object)w)->object.being_destroyed;
  _XmAppUnlock(app);
  return alive;
}
