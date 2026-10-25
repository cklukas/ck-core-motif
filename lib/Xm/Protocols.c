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
static char rcsid[] = "$XConsortium: Protocols.c /main/15 1996/10/17 12:00:24 cde-osf $"
#endif
#endif
#ifdef HAVE_CONFIG_H
#include <config.h>
#endif


#include <Xm/XmosP.h>           /* for bzero et al */
#include <Xm/ProtocolsP.h>
#include "BaseClassI.h"
#include "CallbackI.h"
#include "ExtObjectI.h"
#include "MessagesI.h"
#include "ProtocolsI.h"
#include "XmI.h"


#define MSG1	_XmMMsgProtocols_0000
#define MSG2	_XmMMsgProtocols_0001
#define MSG3	_XmMMsgProtocols_0002

#define MAX_PROTOCOLS		32
#define PROTOCOL_BLOCK_SIZE	4

/********    Static Function Declarations    ********/

static void ClassInitialize( void ) ;
static void ClassPartInitialize( 
                        WidgetClass w) ;
static void Initialize( 
                        Widget req,
                        Widget new_w,
                        ArgList args,
                        Cardinal *num_args) ;
static void Destroy( 
                        Widget w) ;
static void RemoveAllPMgrHandler( 
                        Widget w,
                        XtPointer closure,
                        XEvent *event,
                        Boolean *continue_to_dispatch) ;
static void RemoveAllPMgr( 
                        Widget w,
                        XtPointer closure,
                        XtPointer call_data) ;
static XmAllProtocolsMgr GetAllProtocolsMgr( 
                        Widget shell) ;
static void UpdateProtocolMgrProperty( 
                        Widget shell,
                        XmProtocolMgr p_mgr) ;
static void InstallProtocols( 
                        Widget w,
                        XmAllProtocolsMgr ap_mgr) ;
static void RealizeHandler( 
                        Widget w,
                        XtPointer closure,
                        XEvent *event,
                        Boolean *cont) ;
static void ProtocolHandler( 
                        Widget w,
                        XtPointer closure,
                        XEvent *event,
                        Boolean *cont) ;
static XmProtocol GetProtocol( 
                        XmProtocolMgr p_mgr,
                        Atom p_atom) ;
static XmProtocolMgr AddProtocolMgr( 
                        XmAllProtocolsMgr ap_mgr,
                        Atom property) ;
static XmProtocolMgr GetProtocolMgr( 
                        XmAllProtocolsMgr ap_mgr,
                        Atom property) ;
static void RemoveProtocolMgr( 
                        XmAllProtocolsMgr ap_mgr,
                        XmProtocolMgr p_mgr) ;
static void AddProtocols( 
                        Widget shell,
                        XmProtocolMgr p_mgr,
                        Atom *protocols,
                        Cardinal num_protocols) ;
static void RemoveProtocols( 
                        Widget shell,
                        XmProtocolMgr p_mgr,
                        Atom *protocols,
                        Cardinal num_protocols) ;

/********    End Static Function Declarations    ********/


/***************************************************************************
 *
 * ProtocolObject Resources
 *
 ***************************************************************************/

static XContext	allProtocolsMgrContext = 0;


#define Offset(field) XtOffsetOf( struct _XmProtocolRec, protocol.field)

static XtResource protocolResources[] =
{
    {
	XmNextensionType,
	XmCExtensionType, XmRExtensionType, sizeof (unsigned char),
	XtOffsetOf( struct _XmExtRec, ext.extensionType),
	XmRImmediate, (XtPointer)XmPROTOCOL_EXTENSION,
    },
    {
	XmNprotocolCallback,
	XmCProtocolCallback, XmRCallback, sizeof (XtCallbackList),
	Offset (callbacks),
	XmRImmediate, (XtPointer)NULL,
    },
};
#undef Offset


externaldef(xmprotocolclassrec)
XmProtocolClassRec xmProtocolClassRec = {
    {	
	(WidgetClass) &xmExtClassRec,/* superclass 		*/   
	"protocol",			/* class_name 		*/   
	sizeof(XmProtocolRec),	 	/* size 		*/   
	ClassInitialize, 		/* Class Initializer 	*/   
	ClassPartInitialize, 		/* class_part_init 	*/ 
	FALSE, 				/* Class init'ed ? 	*/   
	Initialize, 			/* initialize         	*/   
	NULL, 				/* initialize_notify    */ 
	NULL,	 			/* realize            	*/   
	NULL,	 			/* actions            	*/   
	0,				/* num_actions        	*/   
	protocolResources,		/* resources          	*/   
	XtNumber(protocolResources),	/* resource_count     	*/   
	NULLQUARK, 			/* xrm_class          	*/   
	FALSE, 				/* compress_motion    	*/   
	FALSE, 				/* compress_exposure  	*/   
	FALSE, 				/* compress_enterleave	*/   
	FALSE, 				/* visible_interest   	*/   
	Destroy,			/* destroy            	*/   
	NULL,		 		/* resize             	*/   
	NULL, 				/* expose             	*/   
	NULL,		 		/* set_values         	*/   
	NULL, 				/* set_values_hook      */ 
	NULL,			 	/* set_values_almost    */ 
	NULL,				/* get_values_hook      */ 
	NULL, 				/* accept_focus       	*/   
	XtVersion, 			/* intrinsics version 	*/   
	NULL, 				/* callback offsets   	*/   
	NULL,				/* tm_table           	*/   
	NULL, 				/* query_geometry       */ 
	NULL, 				/* display_accelerator  */ 
	NULL, 				/* extension            */ 
    },	
    {
	NULL,				/* synthetic resources	*/
	0,				/* num syn resources	*/
    },
    {
	NULL,				/* extension		*/
    },
};

externaldef(xmprotocolobjectclass) WidgetClass 
  xmProtocolObjectClass = (WidgetClass) (&xmProtocolClassRec);

/************************************************************************
 *
 *  ClassInitialize
 *    Initialize the vendorShell class structure.  This is called only
 *    the first time a vendorShell widget is created.  It registers the
 *    resource type converters unique to this class.
 *
 ************************************************************************/
static void 
ClassInitialize( void )
{

}

/************************************************************************
 *
 *  ClassPartInitialize
 *    Set up the inheritance mechanism for the routines exported by
 *    vendorShells class part.
 *
 ************************************************************************/
static void 
ClassPartInitialize(
        WidgetClass w )
{
    XmProtocolObjectClass wc = (XmProtocolObjectClass) w;
    
    if (wc == (XmProtocolObjectClass)xmProtocolObjectClass)
      return;
}

/*ARGSUSED*/
static void 
Initialize(
        Widget req,		/* unused */
        Widget new_w,
        ArgList args,		/* unused */
        Cardinal *num_args )	/* unused */
{
    XmProtocol				ne = (XmProtocol) new_w;
    XmWidgetExtData			extData;

    /*
     * we should free this in ExtObject's destroy proc, but since all
     * gadgets would need to change to not free it in thier code we'll
     * do it here. |||
     */
    extData = _XmGetWidgetExtData(ne->ext.logicalParent,
				  ne->ext.extensionType);
if(extData == NULL)
{
#ifdef DEBUG
        XmeWarning(NULL, "_XmGetWidgetExtData() returned NULL pointer.");
#endif
        return;
}

    _XmProcessLock();
    _XmExtObjFree((XtPointer) extData->reqWidget);
    _XmProcessUnlock();
    extData->reqWidget = NULL;
}

/************************************************************************
 *
 *  Destroy
 *
 ************************************************************************/
/*ARGSUSED*/
static void 
Destroy(
        Widget w )		/* unused */
{
  /*EMPTY*/
}

/*ARGSUSED*/
static void
RemoveAllPMgrHandler(
        Widget w,
        XtPointer closure,
        XEvent *event,		/* unused */
        Boolean *continue_to_dispatch)
{   
    XmAllProtocolsMgr ap_mgr = (XmAllProtocolsMgr) closure ;
    Cardinal	i;
    
    while (ap_mgr->num_protocol_mgrs != 0 &&
	   ap_mgr->protocol_mgrs != NULL)
      RemoveProtocolMgr(ap_mgr, ap_mgr->protocol_mgrs[0]);
    /* free the context manager entry ||| */
    XDeleteContext(XtDisplay(w), 
		   (Window)w, 
		   allProtocolsMgrContext);
    XtFree((char *)ap_mgr->protocol_mgrs);
    XtFree((char *)ap_mgr);

    *continue_to_dispatch = False;
    return ;
    } 

/************************************<+>*************************************
 *
 *   RemoveAllPMgr
 *
 *************************************<+>************************************/
/*ARGSUSED*/
static void 
RemoveAllPMgr(
        Widget w,
        XtPointer closure,
        XtPointer call_data )	/* unused */
{ 
	XEvent ev ;
        Boolean save_sensitive = w->core.sensitive ;
        Boolean save_ancestor_sensitive = w->core.ancestor_sensitive ;

    XtInsertEventHandler( w, KeyPressMask, TRUE, RemoveAllPMgrHandler,
                                                         closure, XtListHead) ;
    bzero((void *) &ev, sizeof(XEvent));
    ev.xkey.type = KeyPress ;
    ev.xkey.display = XtDisplay( w) ;
    ev.xkey.time = XtLastTimestampProcessed( XtDisplay( w)) ;
    ev.xkey.send_event = True ;
    ev.xkey.serial = LastKnownRequestProcessed( XtDisplay( w)) ;
    ev.xkey.window = XtWindow( w) ;
    ev.xkey.keycode = 0;
    ev.xkey.state = 0;

    /* make sure we get it even if we're unrealized, or if widget
     * is insensitive.
     */
    XtAddGrab( w, True, True) ;
    w->core.sensitive = TRUE ;
    w->core.ancestor_sensitive = TRUE ;
    XtDispatchEvent(&ev) ;
    w->core.sensitive = save_sensitive ;
    w->core.ancestor_sensitive = save_ancestor_sensitive ;
    XtRemoveGrab( w) ;

    XtRemoveEventHandler(w, (EventMask)NULL, TRUE, RemoveAllPMgrHandler,
			 closure) ;
    }

/************************************<+>*************************************
 *
 *   GetAllProtocolsMgr
 *
 *************************************<+>************************************/
static XmAllProtocolsMgr 
GetAllProtocolsMgr(
        Widget shell )
{
    XmAllProtocolsMgr	ap_mgr;
    Display		*display;
    
    if (!XmIsVendorShell(shell))
      {
	  XmeWarning(NULL, MSG1);
	  return ((XmAllProtocolsMgr)0);
      }
    else
      {
	  display = XtDisplay(shell);
	  
	  _XmProcessLock();
	  if (allProtocolsMgrContext == 0)
	    allProtocolsMgrContext = XUniqueContext();
	  _XmProcessUnlock();
	  
	  if (XFindContext(display,
			   (Window) shell,
			   allProtocolsMgrContext,
			   (char **)&ap_mgr))
	    {
		ap_mgr = XtNew(XmAllProtocolsMgrRec);
		if (ap_mgr == NULL)
		    return NULL;
		
		ap_mgr->shell = shell;
		ap_mgr->num_protocol_mgrs = 
		  ap_mgr->max_protocol_mgrs = 0;
		ap_mgr->protocol_mgrs = NULL;
		(void) XSaveContext(display, 
				    (Window) shell, 
				    allProtocolsMgrContext, 
				    (XPointer) ap_mgr);
		
		/* !!! should this be in some init code for vendor shell ? */
		/* if shell isn't realized, add an event handler for everybody */
		
		if (!XtIsRealized(shell))
		  {
		      XtAddEventHandler((Widget) shell, StructureNotifyMask,
                                    FALSE, RealizeHandler, (XtPointer) ap_mgr);
		  }
		XtAddCallback((Widget) shell, XmNdestroyCallback, 
                                             RemoveAllPMgr, (XtPointer)ap_mgr);
		
	    }
	  return ap_mgr;
      }
}
/************************************<+>*************************************
 *
 *   SetProtocolProperty
 *
 *************************************<+>************************************/
#define SetProtocolProperty(shell, property, prop_type, atoms, num_atoms) \
  XChangeProperty((shell)->core.screen->display, XtWindow(shell), \
		  property, prop_type, 32, PropModeReplace, \
		  atoms, num_atoms)


/************************************<+>*************************************
 *
 *   UpdateProtocolMgrProperty
 *
 *************************************<+>************************************/
static void
UpdateProtocolMgrProperty(
        Widget shell,
        XmProtocolMgr p_mgr )
{
    Cardinal        i, num_active = 0;
    Atom            active_protocols_stack[MAX_PROTOCOLS];
    Atom            *active_protocols = active_protocols_stack;
    XmProtocolList  protocols = p_mgr->protocols;

    if (p_mgr->num_protocols > XtNumber(active_protocols_stack))
    {
        size_t protocol_bytes = (size_t)p_mgr->num_protocols *
                                sizeof(*active_protocols);
        if (protocol_bytes / sizeof(*active_protocols) !=
            (size_t)p_mgr->num_protocols)
        {
            XmeWarning(NULL, MSG3);
            return;
        }
        active_protocols = (Atom *)XtMalloc(protocol_bytes);
        if (!active_protocols)
            return;
    }

    for (i = 0; i < p_mgr->num_protocols; i++)
        if (protocols[i]->protocol.active)
            active_protocols[num_active++] = protocols[i]->protocol.atom;

    SetProtocolProperty(shell, p_mgr->property, XA_ATOM,
                        (unsigned char *)active_protocols, num_active);
    if (active_protocols != active_protocols_stack)
        XtFree((char *)active_protocols);
}


/************************************<+>*************************************
 *
 *   InstallProtocols
 *
 *************************************<+>************************************/
static void 
InstallProtocols(
        Widget w,
        XmAllProtocolsMgr ap_mgr )
{
    Cardinal		i;
    
    XtAddRawEventHandler(w, (EventMask)0, TRUE, 
			 ProtocolHandler, (XtPointer) ap_mgr);
    XtRemoveEventHandler(w,StructureNotifyMask , FALSE, 
			 RealizeHandler, ap_mgr);
    
    for (i=0; i < ap_mgr->num_protocol_mgrs; i++)
      UpdateProtocolMgrProperty(w, ap_mgr->protocol_mgrs[i]);
    
}

/************************************<+>*************************************
 *
 *   RealizeHandler
 *
 *************************************<+>************************************/
/*ARGSUSED*/
static void 
RealizeHandler(
        Widget w,
        XtPointer closure,
        XEvent *event,
        Boolean *cont )		/* unused */
{
    XmAllProtocolsMgr	ap_mgr = (XmAllProtocolsMgr)closure;
    
    switch (event->type) 
      {
	case MapNotify:
	  InstallProtocols(w, ap_mgr);
	default:
	  break;
      }
}

/************************************<+>*************************************
 *
 *   ProtocolHandler
 *
 *************************************<+>************************************/
/*ARGSUSED*/
static void 
ProtocolHandler(
        Widget w,
        XtPointer closure,
        XEvent *event,
        Boolean *cont )		/* unused */
{
  XmAllProtocolsMgr	ap_mgr = (XmAllProtocolsMgr)closure;
  XmProtocolMgr	p_mgr;
  XmProtocol		protocol;
  XmAnyCallbackStruct	call_data_rec;
  XtCallbackProc	func;
  
  call_data_rec.reason = XmCR_PROTOCOLS;
  call_data_rec.event = event;
  
  switch (event->type) {
  case ClientMessage: {
      XClientMessageEvent	*p_event = (XClientMessageEvent *) event;
      Atom			p_atom = (Atom) p_event->data.l[0];
	      
      if (((p_mgr = GetProtocolMgr(ap_mgr, (Atom)p_event->message_type)) 
	   == (XmProtocolMgr)0) ||
	  ((protocol = GetProtocol(p_mgr, p_atom)) == (XmProtocol)0))
	return;
      else {
	if ((func = protocol->protocol.pre_hook.callback) != (XtCallbackProc)0)
	  (*func) (w, protocol->protocol.pre_hook.closure, (XtPointer) &call_data_rec);
		  
	if (protocol->protocol.callbacks)
	  _XmCallCallbackList(w,
			      protocol->protocol.callbacks, 
			      (XtPointer) &call_data_rec);
		  
	if ((func = protocol->protocol.post_hook.callback) != (XtCallbackProc)0)
	  (*func) (w, protocol->protocol.post_hook.closure, (XtPointer) &call_data_rec);
      }
      break;
    }
    default: {
      break;
    }
  }
}



/************************************<+>*************************************
 *
 *   GetProtocol
 *
 *************************************<+>************************************/
static XmProtocol 
GetProtocol(
        XmProtocolMgr p_mgr,
        Atom p_atom )
{
    Cardinal	i;
    XmProtocol	protocol;
    
    i = 0;
    while ((i < p_mgr->num_protocols) && 
	   (p_mgr->protocols[i]->protocol.atom != p_atom))
      i++;
    
    if (i < p_mgr->num_protocols)
      {
	  protocol = p_mgr->protocols[i];
      }
    else 
      {
	  protocol = (XmProtocol)0;
      }
    return(protocol);
}


/************************************<+>*************************************
 *
 *   AddProtocolMgr
 *
 *************************************<+>************************************/
static XmProtocolMgr 
AddProtocolMgr(
        XmAllProtocolsMgr ap_mgr,
        Atom property )
{
    XmProtocolMgr	p_mgr;
    XmProtocolMgrList resized;
    Cardinal		new_max;
    Cardinal		max_cardinal = (Cardinal)~0;
    Cardinal		i;

    if (ap_mgr == NULL)
	return NULL;

    /* Allocate the manager before publishing any capacity change. */
    p_mgr = XtNew(XmProtocolMgrRec);
    if (p_mgr == NULL)
	return NULL;
    
    i = 0;
    while ((i < ap_mgr->num_protocol_mgrs) &&
	   (ap_mgr->protocol_mgrs[i]->property != property))
      i++;
    
    if (i < ap_mgr->num_protocol_mgrs)
      {
	  XmeWarning(NULL, MSG2);
      }
    
    if (ap_mgr->num_protocol_mgrs > max_cardinal - 2 ||
	 ap_mgr->num_protocol_mgrs + 2 >= ap_mgr->max_protocol_mgrs)
      {
	  if (ap_mgr->max_protocol_mgrs > max_cardinal - 2) {
	      XtFree((char *)p_mgr);
	      return NULL;
	  }
	  new_max = ap_mgr->max_protocol_mgrs + 2;
	  if (new_max > max_cardinal / sizeof(XmProtocolMgr)) {
	      XtFree((char *)p_mgr);
	      return NULL;
	  }
	  resized = (XmProtocolMgrList)
	    XtRealloc((char *) ap_mgr->protocol_mgrs,
		      (size_t)new_max * sizeof(XmProtocolMgr));
	  if (resized == NULL) {
	      XtFree((char *)p_mgr);
	      return NULL;
	  }
	  ap_mgr->protocol_mgrs = resized;
	  ap_mgr->max_protocol_mgrs = new_max;
      }
    ap_mgr->protocol_mgrs[ap_mgr->num_protocol_mgrs++] 
      = p_mgr;
    
    p_mgr->property = property;
    p_mgr->num_protocols =
      p_mgr->max_protocols = 0;
    
    p_mgr->protocols = NULL;
    
    return(p_mgr);
}
/************************************<+>*************************************
 *
 *   GetProtcolMgr
 *
 *************************************<+>************************************/
static XmProtocolMgr 
GetProtocolMgr(
        XmAllProtocolsMgr ap_mgr,
        Atom property )
{
    XmProtocolMgr	p_mgr = (XmProtocolMgr)0;
    Cardinal		i;
    
    if (!ap_mgr) return p_mgr;
    
    i = 0;
    while ((i < ap_mgr->num_protocol_mgrs) &&
	   (ap_mgr->protocol_mgrs[i]->property != property))
      i++;
    
    if (i < ap_mgr->num_protocol_mgrs)
      {
	  p_mgr = ap_mgr->protocol_mgrs[i];
      }
    else
      p_mgr = (XmProtocolMgr)0;

    return p_mgr;
}


/************************************<+>*************************************
 *
 *   RemoveProtocolMgr
 *
 *************************************<+>************************************/
static void 
RemoveProtocolMgr(
        XmAllProtocolsMgr ap_mgr,
        XmProtocolMgr p_mgr )
{
    Widget	shell;
    Cardinal 	i;

    if (ap_mgr == NULL || p_mgr == NULL ||
	 ap_mgr->protocol_mgrs == NULL)
	return;

    for (i = 0; i < ap_mgr->num_protocol_mgrs; i++)
      if (ap_mgr->protocol_mgrs[i] == p_mgr)
	break;
    if (i == ap_mgr->num_protocol_mgrs)
	return;

    shell = ap_mgr->shell;
    
    for (i = 0; i < p_mgr->num_protocols; i++)
      {
          if (p_mgr->protocols[i] == NULL)
            continue;
          _XmRemoveAllCallbacks(
		(InternalCallbackList *)&(p_mgr->protocols[i]->protocol.callbacks) );
          XtFree((char *) p_mgr->protocols[i]);
      }
    if (XtIsRealized(shell))
	XDeleteProperty(XtDisplay(shell), 
			XtWindow(shell), 
			p_mgr->property);
    
    XtFree((char *) p_mgr->protocols);
    XtFree((char *) p_mgr);

    /* ripple mgrs down */
    for ( ; i < ap_mgr->num_protocol_mgrs-1; i++)
      ap_mgr->protocol_mgrs[i] = ap_mgr->protocol_mgrs[i+1];
    ap_mgr->num_protocol_mgrs--;
}
/************************************<+>*************************************
 *
 *  AddProtocols
 *
 *************************************<+>************************************/
static void 
AddProtocols(
        Widget shell,
        XmProtocolMgr p_mgr,
        Atom *protocols,
        Cardinal num_protocols )
{	
    Cardinal		new_num_protocols, i, j;
    Cardinal            max_cardinal = (Cardinal)~0;
    Cardinal            new_max, add_size;
    XmProtocolList      resized;
    XtPointer           newSec;
    WidgetClass         wc;
    Cardinal            size;

    wc = XtClass(shell);
    size = wc->core_class.widget_size;
    
    if (num_protocols > max_cardinal - p_mgr->num_protocols)
      return;
    new_num_protocols = p_mgr->num_protocols + num_protocols;
    
    if (new_num_protocols >= p_mgr->max_protocols) 
      {
	  /* Allocate more space without publishing an unallocated capacity. */
	  if (num_protocols >= PROTOCOL_BLOCK_SIZE) {
	    if (num_protocols > max_cardinal - PROTOCOL_BLOCK_SIZE)
	      return;
	    add_size = num_protocols + PROTOCOL_BLOCK_SIZE;
	  } else
	    add_size = PROTOCOL_BLOCK_SIZE;
	  if (add_size > max_cardinal - p_mgr->max_protocols)
	    return;
	  new_max = p_mgr->max_protocols + add_size;
	  if (new_max > max_cardinal / sizeof(XmProtocol))
	    return;
	  resized = (XmProtocolList)
	    XtRealloc((char *)p_mgr->protocols,
		      (unsigned)new_max * sizeof(XmProtocol));
	  if (resized == NULL)
	    return;
	  p_mgr->protocols = resized;
	  p_mgr->max_protocols = new_max;
      }
    
    for (i = p_mgr->num_protocols, j = 0;
	 i < new_num_protocols; 
	 i++,j++)
      {
	  
          newSec = XtMalloc(size);
          if (newSec == NULL) {
              while (i > p_mgr->num_protocols) {
                  --i;
                  XtFree((char *)p_mgr->protocols[i]);
              }
              return;
          }

          ((XmProtocol) newSec)->protocol.atom = protocols[j];
	  ((XmProtocol)newSec)->protocol.active = TRUE; /*default */
	  ((XmProtocol)newSec)->protocol.callbacks = (XtCallbackList)0;
	  ((XmProtocol)newSec)->protocol.pre_hook.callback = 
          ((XmProtocol)newSec)->protocol.post_hook.callback = (XtCallbackProc)0;
	  ((XmProtocol)newSec)->protocol.pre_hook.closure = 
	  ((XmProtocol)newSec)->protocol.post_hook.closure = (XtPointer)0;

          p_mgr->protocols[i] = (XmProtocol)newSec;
      }
    p_mgr->num_protocols = new_num_protocols;
    
}

/************************************<+>*************************************
 *
 *   RemoveProtocols
 *
 *************************************<+>************************************/
/*ARGSUSED*/
static void
RemoveProtocols(
        Widget shell,          /* unused */
        XmProtocolMgr p_mgr,
        Atom *protocols,
        Cardinal num_protocols )
{
    Cardinal i, j, kept = 0;

    if (!p_mgr || !p_mgr->num_protocols || !protocols || !num_protocols)
        return;

    for (i = 0; i < p_mgr->num_protocols; i++)
    {
        Boolean matched = False;

        for (j = 0; j < num_protocols; j++)
            if (p_mgr->protocols[i]->protocol.atom == protocols[j])
            {
                matched = True;
                break;
            }

        if (matched)
        {
            _XmRemoveAllCallbacks((InternalCallbackList *)
                &(p_mgr->protocols[i]->protocol.callbacks));
            XtFree((char *)p_mgr->protocols[i]);
        }
        else
            p_mgr->protocols[kept++] = p_mgr->protocols[i];
    }

    p_mgr->num_protocols = kept;
}




/*
 *  
 * PUBLIC INTERFACES
 *
 */


/************************************<+>*************************************
 *
 *   _XmInstallProtocols
 *
 *************************************<+>************************************/
void 
_XmInstallProtocols(
        Widget w )
{
    XmAllProtocolsMgr	ap_mgr;

    if ((ap_mgr = GetAllProtocolsMgr(w)) != NULL)
      InstallProtocols(w, ap_mgr);
}



/************************************<+>*************************************
 *
 *   XmAddProtocols
 *
 *************************************<+>************************************/
void 
XmAddProtocols(
        Widget shell,
        Atom property,
        Atom *protocols,
        Cardinal num_protocols )
{
    XmAllProtocolsMgr	ap_mgr; 
    XmProtocolMgr	p_mgr ;
    if (shell == NULL || protocols == NULL)
        return;
    _XmWidgetToAppContext(shell);

    _XmAppLock(app);
   
    if (shell->core.being_destroyed) {
	_XmAppUnlock(app);
        return;
    }
    if (((ap_mgr = GetAllProtocolsMgr(shell)) == 0) ||	!num_protocols)
    {
      _XmAppUnlock(app);
      return;
    }
    if ((p_mgr = GetProtocolMgr(ap_mgr, property)) == 0)
      p_mgr = AddProtocolMgr(ap_mgr, property);

    if (p_mgr == NULL) {
	_XmAppUnlock(app);
	return;
    }

    /* get rid of duplicates and then append to end */
    RemoveProtocols(shell, p_mgr, protocols, num_protocols);
    AddProtocols(shell, p_mgr, protocols, num_protocols);
    
    if (XtIsRealized(shell))
      UpdateProtocolMgrProperty(shell, p_mgr);
    _XmAppUnlock(app);
}



/************************************<+>*************************************
 *
 *   XmRemoveProtocols
 *
 *************************************<+>************************************/
void 
XmRemoveProtocols(
        Widget shell,
        Atom property,
        Atom *protocols,
        Cardinal num_protocols )
{
    XmAllProtocolsMgr	ap_mgr; 
    XmProtocolMgr	p_mgr ;
    if (shell == NULL || protocols == NULL)
        return;
    _XmWidgetToAppContext(shell);

    _XmAppLock(app);
    
    if (shell->core.being_destroyed) {
	_XmAppUnlock(app);
        return;
    }
    if (((ap_mgr = GetAllProtocolsMgr(shell)) == 0) 		||
	((p_mgr = GetProtocolMgr(ap_mgr, property)) == 0) 	||
	!num_protocols) {
      _XmAppUnlock(app);
      return;
    }

    
    RemoveProtocols(shell, p_mgr, protocols, num_protocols);

    if (XtIsRealized(shell))
      UpdateProtocolMgrProperty(shell, p_mgr);
    _XmAppUnlock(app);
}

/************************************<+>*************************************
 *
 *   XmAddProtocolCallback
 *
 *************************************<+>************************************/
void 
XmAddProtocolCallback(
        Widget shell,
        Atom property,
        Atom proto_atom,
        XtCallbackProc callback,
        XtPointer closure )
{
    XmAllProtocolsMgr	ap_mgr; 
    XmProtocolMgr	p_mgr ;
    XmProtocol		protocol;
    if (shell == NULL)
        return;
    _XmWidgetToAppContext(shell);

    _XmAppLock(app);
   
    if (shell->core.being_destroyed) {
	 _XmAppUnlock(app);
         return;
    }
    if ((ap_mgr = GetAllProtocolsMgr(shell)) == (XmAllProtocolsMgr)0) {
      _XmAppUnlock(app);
      return;	
    }
    if ((p_mgr = GetProtocolMgr(ap_mgr, property)) == (XmProtocolMgr)0)
      p_mgr = AddProtocolMgr(ap_mgr, property);
    if (p_mgr == NULL) {
	_XmAppUnlock(app);
	return;
    }
    if ((protocol = GetProtocol(p_mgr, proto_atom)) == (XmProtocol)0)
      {
	  XmAddProtocols(shell, property, &proto_atom, 1);
	  protocol = GetProtocol(p_mgr, proto_atom);
      }

    if (protocol == NULL) {
	_XmAppUnlock(app);
	return;
    }

    _XmAddCallback((InternalCallbackList *) &(protocol->protocol.callbacks),
                    callback,
                    closure) ;
    _XmAppUnlock(app);
}

/************************************<+>*************************************
 *
 *   XmRemoveProtocolCallback
 *
 *************************************<+>************************************/
void 
XmRemoveProtocolCallback(
        Widget shell,
        Atom property,
        Atom proto_atom,
        XtCallbackProc callback,
        XtPointer closure )
{
    XmAllProtocolsMgr	ap_mgr; 
    XmProtocolMgr	p_mgr ;
    XmProtocol		protocol;
    if (shell == NULL)
        return;
    _XmWidgetToAppContext(shell);

    _XmAppLock(app);
   
    if (shell->core.being_destroyed) {
	_XmAppUnlock(app);
        return;
    }
    if (((ap_mgr = GetAllProtocolsMgr(shell)) == 0) 		||
	((p_mgr = GetProtocolMgr(ap_mgr, property)) == 0) 	||
	((protocol = GetProtocol(p_mgr, proto_atom)) == 0)) {
      _XmAppUnlock(app);
      return;
    }

    _XmRemoveCallback((InternalCallbackList *) &(protocol->protocol.callbacks),
                    callback,
                    closure) ;
    _XmAppUnlock(app);
}

/************************************<+>*************************************
 *
 *   XmActivateProtocol
 *
 *************************************<+>************************************/
void 
XmActivateProtocol(
        Widget shell,
        Atom property,
        Atom proto_atom )
{
    XmAllProtocolsMgr	ap_mgr; 
    XmProtocolMgr	p_mgr ;
    XmProtocol		protocol;
    if (shell == NULL)
        return;
    _XmWidgetToAppContext(shell);

    _XmAppLock(app);

    if (shell->core.being_destroyed) {
	 _XmAppUnlock(app);
         return;
    }
    if (((ap_mgr = GetAllProtocolsMgr(shell)) == 0) 		||
	((p_mgr = GetProtocolMgr(ap_mgr, property)) == 0) 	||
	((protocol = GetProtocol(p_mgr, proto_atom)) == 0) 	||
	protocol->protocol.active) {
      _XmAppUnlock(app);
      return;
    }
    else
      {
	  protocol->protocol.active = TRUE;
	  if (XtIsRealized(shell))
	    UpdateProtocolMgrProperty(shell, p_mgr);
      }
    _XmAppUnlock(app);
}

/************************************<+>*************************************
 *
 *   XmDeactivateProtocol
 *
 *************************************<+>************************************/
void 
XmDeactivateProtocol(
        Widget shell,
        Atom property,
        Atom proto_atom )
{
    XmAllProtocolsMgr	ap_mgr; 
    XmProtocolMgr	p_mgr ;
    XmProtocol		protocol;
    if (shell == NULL)
        return;
    _XmWidgetToAppContext(shell);

    _XmAppLock(app);
   
    if (shell->core.being_destroyed) {
	_XmAppUnlock(app);
        return;
    }
    if (((ap_mgr = GetAllProtocolsMgr(shell)) == 0) 		||
	((p_mgr = GetProtocolMgr(ap_mgr, property)) == 0) 	||
	((protocol = GetProtocol(p_mgr, proto_atom)) == 0) 	||
	!protocol->protocol.active) {
      _XmAppUnlock(app);
      return;
    }
    else
      {
	  protocol->protocol.active = FALSE;
	  if (XtIsRealized(shell))
	    UpdateProtocolMgrProperty(shell, p_mgr);
      }
    _XmAppUnlock(app);
}

/************************************<+>*************************************
 *
 *   XmSetProtocolHooks
 *
 *************************************<+>************************************/
void 
XmSetProtocolHooks(
        Widget shell,
        Atom property,
        Atom proto_atom,
        XtCallbackProc pre_hook,
        XtPointer pre_closure,
        XtCallbackProc post_hook,
        XtPointer post_closure )
{
    XmAllProtocolsMgr	ap_mgr; 
    XmProtocolMgr	p_mgr ;
    XmProtocol		protocol;
    if (shell == NULL)
        return;
    _XmWidgetToAppContext(shell);

    _XmAppLock(app);
    if (shell->core.being_destroyed) {
	_XmAppUnlock(app);
        return;
    }
    if (((ap_mgr = GetAllProtocolsMgr(shell)) == 0) 		||
	((p_mgr = GetProtocolMgr(ap_mgr, property)) == 0) 	||
	((protocol = GetProtocol(p_mgr, proto_atom)) == 0)) {
      _XmAppUnlock(app);
      return;
    }
    
    protocol->protocol.pre_hook.callback = pre_hook;
    protocol->protocol.pre_hook.closure = pre_closure;
    protocol->protocol.post_hook.callback = post_hook;
    protocol->protocol.post_hook.closure = post_closure;
    _XmAppUnlock(app);
}
