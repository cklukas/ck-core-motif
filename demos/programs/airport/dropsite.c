/* $XConsortium: dropsite.c /main/5 1995/07/15 20:44:42 drk $ */
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

/*
 * The Airport Drag And Drop Demo
 * The module that contains all drop site / receiver code
 */

#include "airport.h"
#include "dropsite.h"

/* ============================ prototypes  ======================= */

static void TrafficSignal(Widget dropSite, XtPointer client_data,
			  XmDragProcCallbackStruct *call_data);
/* drag proc callback on airport. Something is flying over */

static void SomethingLanding(Widget dc, XtPointer client_data,
			     XmDropProcCallbackStruct *call_data);
/* drop proc callback on airport. Something is landing */

typedef struct { Airport airport; int gate; Boolean delivered; } ArrivalRec;
static void ArrivalFinished(Widget widget, XtPointer closure, XtPointer data)
{
    ArrivalRec *arrival=closure;(void)widget;(void)data;
    if(!arrival->delivered)arrival->airport->park.spots[arrival->gate].reserved=False;
    XtFree((char *)arrival);
}

static void TaxiIn(Widget transfer, XtPointer closure,
		   Atom * selection, Atom * type, XtPointer value,
		   unsigned long *length, int *format);
/* Xt Selection Callback. Transfer flight number from source */

/* ===============================================================
 *   Expose callback. Redraw the airport
 */
void MakeAirportDropSite(Widget area, Airport this)
{
   Arg args[10];
   int n = 0;
   
   XtSetArg(args[n], XmNimportTargets, &FlightAtom); n++;
   XtSetArg(args[n], XmNnumImportTargets, 1); n++;
   XtSetArg(args[n], XmNdragProc, TrafficSignal); n++;
   XtSetArg(args[n], XmNdropProc, SomethingLanding); n++;
   XtVaSetValues(area, XmNuserData, this, NULL);
   XmDropSiteRegister(area, args, n);
}

/* ===============================================================
 * Drag proc callback of the drop site.
 * The airport control is detecting/loosing radar echo on enter/leave
 * On motion events do animation of the tracks (flashing)
 */

static void TrafficSignal(Widget dropSite, XtPointer client_data,
			  XmDragProcCallbackStruct *call_data)
{
   Airport this;

   XtVaGetValues(dropSite, XmNuserData, &this, NULL);
   Boolean available=False;
   for(int i=0;i<this->park.spot_count;i++)
       if(this->park.spots[i].empty && !this->park.spots[i].reserved)available=True;
   if(!available && call_data->reason!=XmCR_DROP_SITE_LEAVE_MESSAGE) {
       call_data->dropSiteStatus=XmINVALID_DROP_SITE;
       AirportSetStatus(this,False,"Full — no free gates");return;
   }
   switch(call_data->reason) { 
      
   case XmCR_DROP_SITE_MOTION_MESSAGE:
      DrawAirport(this, XtWindow(dropSite),
		  AirportResources.drag_under_background,
		  AirportResources.drag_under_foreground);
      break;
      
   case XmCR_DROP_SITE_ENTER_MESSAGE:
      AirportSetStatus(this,False,"Plane approaching");
      break;

   case XmCR_DROP_SITE_LEAVE_MESSAGE: 
      AirportSetStatus(this,False,"Ready");
      DrawAirport(this, XtWindow(dropSite),
		  AirportResources.track_background,
		  AirportResources.track_foreground);
      break;

   }
}

/* ===============================================================
 * Drop proc callback of the drop site. 
 * The airport knows that something is landing. What is it ?
 * If it is a plane then check there is a gate available.
 * 	If no gate available, reject the drop
 * If it is not a plane landing, reject the drop too.
 */

static void SomethingLanding(Widget dropSite, XtPointer client_data,
			     XmDropProcCallbackStruct *call_data)
{
   Airport airport;Arg args[4];int n=0,gate;
   XmDropTransferEntryRec target;ArrivalRec *arrival=NULL;
   unsigned char status=XmTRANSFER_FAILURE;
   XtVaGetValues(dropSite,XmNuserData,&airport,NULL);
   (void)client_data;
   for(gate=0;gate<airport->park.spot_count;gate++)
       if(airport->park.spots[gate].empty && !airport->park.spots[gate].reserved)break;
   if(gate==airport->park.spot_count) {
       AirportSetStatus(airport,False,"Full — no free gates");
       Paging(airport,"Airport full. Fly a plane out before accepting arrivals.");
   } else if(call_data->dropAction!=XmDROP || call_data->operation!=XmDROP_MOVE) {
       Paging(airport,"Flight cancelled. Planes move between airports; use How to fly for instructions.");
   } else if(call_data->dropSiteStatus!=XmVALID_DROP_SITE) {
       Paging(airport,"Unsupported drop. Drag an Airport plane to this airport.");
   } else {
       arrival=(ArrivalRec *)XtCalloc(1,sizeof(*arrival));
       if(arrival) {
           arrival->airport=airport;arrival->gate=gate;airport->park.spots[gate].reserved=True;
           target.client_data=arrival;target.target=FlightAtom;status=XmTRANSFER_SUCCESS;
           AirportSetStatus(airport,False,"Landing");
           XtSetArg(args[n],XmNtransferProc,TaxiIn);n++;
           XtSetArg(args[n],XmNdropTransfers,&target);n++;
       } else Paging(airport,"Unable to reserve a gate. The plane will return.");
   }
   XtSetArg(args[n],XmNnumDropTransfers,arrival?1:0);n++;
   XtSetArg(args[n],XmNtransferStatus,status);n++;
   Widget transfer=XmDropTransferStart(call_data->dragContext,args,n);
   if(arrival) {
       if(transfer)XtAddCallback(transfer,XmNdestroyCallback,ArrivalFinished,arrival);
       else {airport->park.spots[gate].reserved=False;XtFree((char *)arrival);}
   }
}

/* ===============================================================
 * The selection callback proc after drop transfer
 * Accept plane at free gate and signal arrival on paging.
 * If some error occured during the transfer, do plane crash
 */

static void TaxiIn(Widget transfer, XtPointer closure,
                   Atom *selection, Atom *type, XtPointer value,
                   unsigned long *length, int *format)
{
   ArrivalRec *arrival=closure; Airport airport=arrival->airport;
   (void)selection;
   airport->park.spots[arrival->gate].reserved=False;arrival->delivered=True;
   if (value && *format==32 && *length==1 && *type==FlightAtom &&
       AirportAcceptPlane(airport,*((long *)value))>=0) {
       /* Source removes its plane only after the successful transfer completes. */
   } else {
       Paging(airport,"Flight transfer failed. The plane stays at its departure gate.");
       AirportSetStatus(airport,False,"Transfer failed");
       if(value)XtFree((char *)value);
       XtVaSetValues(transfer,XmNtransferStatus,XmTRANSFER_FAILURE,XmNnumDropTransfers,0,NULL);
       return;
   }
   if(value)XtFree((char *)value);
}
