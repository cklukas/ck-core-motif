/* $TOG: main.c /main/6 1997/03/31 13:47:48 dbl $ */
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

/* Two-airport drag-and-drop demonstration with click and keyboard operation. */
#include "main.h"
#include <Xm/Display.h>
#include <X11/keysym.h>
#include <X11/Xatom.h>
#include <stdint.h>
#include <math.h>
#include <string.h>
#include <unistd.h>
#include <limits.h>

#define GATES 8
#define COLUMNS 4
static Airport airports[2];
static Widget app_shell, help_dialog, reset_button, help_button, close_button;
static const char *report_path;
static char last_message[512];
static XtAppContext app;

static int px(Airport a, int logical) { return (int)floor(logical*a->scale+0.5); }
static void Report(void);
static void ReportChanged(Widget w, XtPointer client, XtPointer data)
{ (void)w;(void)client;(void)data;Report(); }
static void ConfigureAirport(Widget w, XtPointer client, XtPointer data);
static void AirportInput(Widget w, XtPointer client, XEvent *event, Boolean *dispatch);
static void ShowAirport(Widget w, XtPointer client, XtPointer data);

static void SetLabel(Widget w, const char *value)
{
    XmString s=XmStringCreateLocalized((char *)value);
    XtVaSetValues(w,XmNlabelString,s,NULL); XmStringFree(s);
}
void AirportSetStatus(Airport a, Boolean outgoing, const char *status)
{
    if(a->state!=open)return;
    char text[256]; snprintf(text,sizeof(text),"%s: %s",outgoing?"Outgoing":"Incoming",status);
    SetLabel(outgoing?a->outgoing:a->incoming,text); Report();
}
void Paging(Airport a, String message)
{
    if(a->state!=open)return;
    char text[512]; snprintf(text,sizeof(text),"Airport %c — %s",a->code,message);
    XmString item=XmStringCreateLocalized(text); int count=0;
    XtVaGetValues(a->msg_area,XmNitemCount,&count,NULL);
    if(count>=100)XmListDeletePos(a->msg_area,1);
    XmListAddItemUnselected(a->msg_area,item,0); XmListSetBottomItem(a->msg_area,item);
    XmStringFree(item); snprintf(last_message,sizeof(last_message),"%s",text); Report();
}
static void DrawText(Airport a, const char *text, int x, int y, int width, Pixel color)
{
    XmRenderTable table=NULL; XtVaGetValues(a->title,XmNrenderTable,&table,NULL);
    XmString s=XmStringCreateLocalized((char *)text);
    XSetForeground(a->display,a->park.gc,color);
    XmStringDraw(a->display,XtWindow(a->airport),table,s,a->park.gc,x,y,width,
                 XmALIGNMENT_CENTER,XmSTRING_DIRECTION_L_TO_R,NULL);
    XmStringFree(s);
}
void DrawAirport(Airport a, Window window, Pixel bg, Pixel fg)
{
    if(a->state!=open)return;
    Dimension width,height; Pixel surface,muted,accent,text_color;
    XtVaGetValues(a->airport,XmNwidth,&width,XmNheight,&height,XmNbackground,&surface,
                 XmNbottomShadowColor,&muted,XmNhighlightColor,&accent,XmNforeground,&text_color,NULL);
    GC gc=a->park.gc; if(!gc || !a->park.spots)return;
    XSetForeground(a->display,gc,surface); XFillRectangle(a->display,window,gc,0,0,width,height);
    XSetForeground(a->display,gc,bg);
    XFillRectangle(a->display,window,gc,a->track.bbox.x,a->track.bbox.y,a->track.bbox.width,a->track.bbox.height);
    XSetForeground(a->display,gc,fg);
    XSetLineAttributes(a->display,gc,px(a,2),LineOnOffDash,CapButt,JoinRound);
    XDrawLine(a->display,window,gc,a->track.bbox.x+px(a,12),a->track.bbox.y+a->track.bbox.height/2,
              a->track.bbox.x+a->track.bbox.width-px(a,12),a->track.bbox.y+a->track.bbox.height/2);
    XSetLineAttributes(a->display,gc,1,LineSolid,CapButt,JoinRound);
    for(int i=0;i<GATES;i++) {
        Spot spot=&a->park.spots[i]; int size=a->plane_size, x=spot->x,y=spot->y;
        XSetForeground(a->display,gc,i==a->selected?accent:muted);
        XSetLineAttributes(a->display,gc,i==a->selected?px(a,2):1,LineSolid,CapButt,JoinRound);
        XDrawRectangle(a->display,window,gc,x-px(a,4),y-px(a,4),size+px(a,8),size+px(a,8));
        if(!spot->empty) {
            /* A transparent, scale-aware aircraft silhouette, with no bitmap tile. */
            static const int shape[][2]={{12,0},{15,9},{24,16},{24,19},{15,15},{15,21},{19,24},{19,25},{12,23},{5,25},{5,24},{9,21},{9,15},{0,19},{0,16},{9,9}};
            XPoint points[XtNumber(shape)];
            for(unsigned k=0;k<XtNumber(shape);k++){points[k].x=x+shape[k][0]*size/25;points[k].y=y+shape[k][1]*size/25;}
            XSetForeground(a->display,gc,AirportResources.spot_foreground); XFillPolygon(a->display,window,gc,points,XtNumber(points),Nonconvex,CoordModeOrigin);
        }
        char label[48];snprintf(label,sizeof(label),"%c%d%s",a->code,i+1,spot->empty?" · Empty":"");
        DrawText(a,label,x-px(a,24),y+size+px(a,8),size+px(a,48),text_color);
    }
    XSetLineAttributes(a->display,gc,1,LineSolid,CapButt,JoinRound);
    Report();
}
void AirportRefresh(Airport a)
{
    if(a && a->state==open && XtIsRealized(a->airport))DrawAirport(a,XtWindow(a->airport),AirportResources.track_background,AirportResources.track_foreground);
}
static void ConfigureAirport(Widget w, XtPointer client, XtPointer data)
{
    Airport a=client; Dimension width,height; (void)data;
    XtVaGetValues(w,XmNwidth,&width,XmNheight,&height,NULL);
    int margin=px(a,20),cell=(int)width/COLUMNS;
    a->plane_size=px(a,34); if(a->plane_size>cell-px(a,16))a->plane_size=cell-px(a,16);
    if(a->plane_size<8)a->plane_size=8;
    int row_height=a->plane_size+px(a,38);
    a->track.bbox.x=margin; a->track.bbox.y=(int)height/2-px(a,12);
    a->track.bbox.width=width>2*margin?width-2*margin:1;
    a->track.bbox.height=px(a,24);
    for(int i=0;i<GATES;i++) {
        a->park.spots[i].x=cell*(i%COLUMNS)+(cell-a->plane_size)/2;
        a->park.spots[i].y=i<COLUMNS?px(a,12):(int)height-row_height+px(a,4);
    }
    a->state=open; AirportRefresh(a);
}
static void ShowAirport(Widget w, XtPointer client, XtPointer data)
{ (void)w;(void)data;AirportRefresh(client); }
int AirportAcceptPlane(Airport a, long number)
{
    if(a->state!=open)return -1;
    for(int i=0;i<GATES;i++)if(a->park.spots[i].empty && !a->park.spots[i].reserved) {
        a->park.spots[i].empty=False;a->park.spots[i].number=number;
        char msg[128];snprintf(msg,sizeof(msg),"Flight %ld arrived at gate %c%d",number,a->code,i+1);
        AirportSetStatus(a,False,"Landed safely");Paging(a,msg);AirportRefresh(a);return i;
    }
    AirportSetStatus(a,False,"Full — no free gates");Paging(a,"No free gates. Fly a plane to the other airport first.");return -1;
}
Boolean AirportSendSelected(Airport a)
{
    if(a->active_drag || a->peer->active_drag){Paging(a,"Finish or cancel the current drag before sending another plane.");return False;}
    int i=a->selected;
    if(i<0 || i>=GATES || a->park.spots[i].empty){Paging(a,"Select an occupied gate, then choose Fly or press Enter.");return False;}
    long number=a->park.spots[i].number;
    if(AirportAcceptPlane(a->peer,number)<0)return False;
    a->park.spots[i].empty=True;a->park.spots[i].number=0;
    AirportSetStatus(a,True,"Landed at destination");
    char msg[128];snprintf(msg,sizeof(msg),"Flight %ld flew to Airport %c",number,a->peer->code);Paging(a,msg);AirportRefresh(a);return True;
}
static void Fly(Widget w, XtPointer client, XtPointer data)
{ (void)w;(void)data;(void)AirportSendSelected(client); }
static int Hit(Airport a, int x, int y)
{
    for(int i=0;i<GATES;i++) {
        Spot p=&a->park.spots[i];int pad=px(a,6);
        if(x>=p->x-pad && x<=p->x+a->plane_size+pad && y>=p->y-pad && y<=p->y+a->plane_size+px(a,28))return i;
    }return -1;
}
static void AirportInput(Widget w, XtPointer client, XEvent *event, Boolean *dispatch)
{
    Airport a=client;(void)dispatch;
    if(event->type==ButtonPress && (event->xbutton.button==Button1 || event->xbutton.button==Button2)) {
        a->selected=Hit(a,event->xbutton.x,event->xbutton.y);a->pressed=False;
        XmProcessTraversal(w,XmTRAVERSE_CURRENT);AirportRefresh(a);
        if(a->selected<0){Paging(a,"Select a plane at a gate. Drop incoming planes anywhere in this airport.");return;}
        if(a->park.spots[a->selected].empty){Paging(a,"This gate is empty. Choose a gate with a plane.");return;}
        if(event->xbutton.button==Button2)TakeOff(w,a,&event->xbutton,a->selected);
        else {a->press=event->xbutton;a->pressed=True;Paging(a,"Plane selected. Drag to the other airport, or choose Fly / press Enter.");}
    } else if(event->type==MotionNotify && a->pressed && (event->xmotion.state&Button1Mask)) {
        if(abs(event->xmotion.x-a->press.x)+abs(event->xmotion.y-a->press.y)>=px(a,6)) {
            a->pressed=False;a->press.time=event->xmotion.time;TakeOff(w,a,&a->press,a->selected);
        }
    } else if(event->type==ButtonRelease)a->pressed=False;
    else if(event->type==KeyPress) {
        KeySym key=XLookupKeysym(&event->xkey,0);
        if(key==XK_Return || key==XK_KP_Enter || key==XK_space)(void)AirportSendSelected(a);
        else if(key==XK_Left || key==XK_Right || key==XK_Up || key==XK_Down || key==XK_Home) {
            int delta=key==XK_Left?-1:key==XK_Right?1:key==XK_Up?-COLUMNS:COLUMNS;
            a->selected=key==XK_Home?0:(a->selected+delta+GATES)%GATES;AirportRefresh(a);
        }
    }
}
static void Reset(Widget w, XtPointer client, XtPointer data)
{
    (void)w;(void)client;(void)data;
    if(airports[0]->active_drag || airports[1]->active_drag){Paging(airports[0],"Finish or cancel the current flight before resetting.");return;}
    for(int k=0;k<2;k++)for(int i=0;i<GATES;i++)if(airports[k]->park.spots[i].reserved){Paging(airports[k],"Wait for the incoming transfer before resetting.");return;}
    XmListDeleteAllItems(airports[0]->msg_area);
    for(int k=0;k<2;k++) {
        Airport a=airports[k];a->selected=0;a->pressed=False;
        for(int i=0;i<GATES;i++){a->park.spots[i].empty=i>=COLUMNS;a->park.spots[i].reserved=False;a->park.spots[i].number=i<COLUMNS?((long)getpid()%1000000L)*100L+(k+1)*10+i+1:0;}
        AirportSetStatus(a,True,"Ready");AirportSetStatus(a,False,"Ready");AirportRefresh(a);
    }
    Paging(airports[0],"Ready: drag a plane to Airport B. Or select a plane and choose Fly.");
}
static void Help(Widget w, XtPointer client, XtPointer data)
{
    (void)w;(void)client;(void)data;
    if(!help_dialog) {
        XmString message=XmStringCreateLocalized("Fly between the two airports\n\nDrag a plane with the left mouse button and release over the other airport.\nMiddle-button dragging also works. Escape cancels a flight.\n\nFor keyboard or click operation: select an occupied gate, then choose Fly\nor press Enter. Arrow keys move the gate selection.\n\nEach airport has eight gates. A full airport rejects arrivals.\nReset gates restores four planes at each airport. Resizing preserves flights.\nThe flight log records departures, arrivals, cancellations, and full airports.");
        help_dialog=XmCreateInformationDialog(app_shell,"flightInstructions",NULL,0);
        XtAddCallback(help_dialog,XmNunmapCallback,ReportChanged,NULL);
        XmString title=XmStringCreateLocalized("How to fly");
        XtVaSetValues(help_dialog,XmNdialogTitle,title,XmNmessageString,message,NULL);XmStringFree(title);XmStringFree(message);
        XtUnmanageChild(XmMessageBoxGetChild(help_dialog,XmDIALOG_CANCEL_BUTTON));
        XtUnmanageChild(XmMessageBoxGetChild(help_dialog,XmDIALOG_HELP_BUTTON));
    }XtManageChild(help_dialog);Report();
}
static void Close(Widget w, XtPointer client, XtPointer data)
{ (void)w;(void)client;(void)data;for(int k=0;k<2;k++)if(airports[k]->active_drag)XmDragCancel(airports[k]->active_drag);XtAppSetExitFlag(app); }
static Widget Label(Widget parent,const char *name,const char *text,XmAppearanceRole role)
{
    XmString s=XmStringCreateLocalized((char *)text);
    Widget w=XtVaCreateManagedWidget(name,xmLabelWidgetClass,parent,XmNlabelString,s,
                XmNalignment,XmALIGNMENT_BEGINNING,XmNappearanceRole,role,NULL);XmStringFree(s);return w;
}
static Widget Button(Widget parent,const char *name,const char *text,XtCallbackProc callback,XtPointer client)
{
    XmString s=XmStringCreateLocalized((char *)text);
    Widget w=XtVaCreateManagedWidget(name,xmPushButtonWidgetClass,parent,XmNlabelString,s,NULL);XmStringFree(s);
    XtAddCallback(w,XmNactivateCallback,callback,client);return w;
}
static Airport CreateAirport(Widget parent, Display *display, int index,double scale)
{
    Airport a=(Airport)XtCalloc(1,sizeof(AirportRec));airports[index]=a;
    a->display=display;a->context=app;a->shell=app_shell;a->scale=scale;a->code='A'+index;a->name=index?"Airport B":"Airport A";
    a->ocean=RootWindowOfScreen(XtScreen(app_shell));a->screen_width=WidthOfScreen(XtScreen(app_shell));a->screen_height=HeightOfScreen(XtScreen(app_shell));
    a->park.spot_count=GATES;a->park.plane_count=COLUMNS;a->park.spots=(Spot)XtCalloc(GATES,sizeof(SpotRec));
    a->main=XtVaCreateManagedWidget(index?"airportB":"airportA",xmFormWidgetClass,parent,
        XmNleftAttachment,index?XmATTACH_POSITION:XmATTACH_FORM,XmNleftPosition,51,
        XmNrightAttachment,index?XmATTACH_FORM:XmATTACH_POSITION,XmNrightPosition,49,
        XmNtopAttachment,XmATTACH_FORM,XmNbottomAttachment,XmATTACH_FORM,NULL);
    a->title=Label(a->main,"airportTitle",a->name,XmAPPEARANCE_ROLE_SECTION);
    XtVaSetValues(a->title,XmNleftAttachment,XmATTACH_FORM,XmNtopAttachment,XmATTACH_FORM,NULL);
    char text[80];snprintf(text,sizeof(text),"Fly selected plane to Airport %c",index?'A':'B');
    a->fly_button=Button(a->main,"flySelected",text,Fly,a);
    XtVaSetValues(a->fly_button,XmNleftAttachment,XmATTACH_FORM,XmNbottomAttachment,XmATTACH_FORM,NULL);
    a->incoming=Label(a->main,"incomingStatus","Incoming: Ready",XmAPPEARANCE_ROLE_SECONDARY);
    XtVaSetValues(a->incoming,XmNleftAttachment,XmATTACH_FORM,XmNbottomAttachment,XmATTACH_WIDGET,XmNbottomWidget,a->fly_button,XmNbottomOffset,px(a,6),NULL);
    a->outgoing=Label(a->main,"outgoingStatus","Outgoing: Ready",XmAPPEARANCE_ROLE_SECONDARY);
    XtVaSetValues(a->outgoing,XmNleftAttachment,XmATTACH_FORM,XmNbottomAttachment,XmATTACH_WIDGET,XmNbottomWidget,a->incoming,XmNbottomOffset,px(a,4),NULL);
    a->airport=XtVaCreateManagedWidget("gates",xmDrawingAreaWidgetClass,a->main,
        XmNtopAttachment,XmATTACH_WIDGET,XmNtopWidget,a->title,XmNtopOffset,px(a,10),
        XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,
        XmNbottomAttachment,XmATTACH_WIDGET,XmNbottomWidget,a->outgoing,XmNbottomOffset,px(a,10),
        XmNnavigationType,XmTAB_GROUP,XmNtraversalOn,True,XmNwidth,px(a,440),XmNheight,px(a,260),NULL);
    XtAddCallback(a->airport,XmNexposeCallback,ShowAirport,a);XtAddCallback(a->airport,XmNresizeCallback,ConfigureAirport,a);
    XtAddEventHandler(a->airport,ButtonPressMask|ButtonReleaseMask|Button1MotionMask|KeyPressMask,False,AirportInput,a);
    return a;
}
static void Report(void)
{
    if(!report_path || !airports[0] || !airports[1] || !airports[1]->airport || !XtIsRealized(airports[1]->airport))return;
    FILE *f=fopen(report_path,"w");if(!f)return;
    XmAppearanceProfile profile;XmAppearanceDensity density;double scale=1;
    XmGetAppearanceProfile(airports[0]->display,&profile);XmGetAppearanceDensity(airports[0]->display,&density);XmGetWidgetScale(airports[0]->airport,&scale);
    fprintf(f,"{\"pid\":%ld,\"profile\":%d,\"density\":%d,\"scale\":%.17g,\"shell\":%lu,\"airports\":[",(long)getpid(),profile,density,scale,XtWindow(app_shell));
    for(int k=0;k<2;k++) {
        Airport a=airports[k];Position x,y;Dimension width,height;XtTranslateCoords(a->airport,0,0,&x,&y);XtVaGetValues(a->airport,XmNwidth,&width,XmNheight,&height,NULL);
        fprintf(f,"%s{\"window\":%lu,\"x\":%d,\"y\":%d,\"width\":%u,\"height\":%u,\"selected\":%d,\"dragging\":%s,\"gates\":[",k?",":"",XtWindow(a->airport),x,y,width,height,a->selected,a->active_drag?"true":"false");
        for(int i=0;i<GATES;i++)fprintf(f,"%s{\"number\":%ld,\"empty\":%s,\"reserved\":%s,\"x\":%d,\"y\":%d,\"size\":%d}",i?",":"",a->park.spots[i].number,a->park.spots[i].empty?"true":"false",a->park.spots[i].reserved?"true":"false",a->park.spots[i].x,a->park.spots[i].y,a->plane_size);
        fprintf(f,"]}");
    }
    fprintf(f,"],\"buttons\":{");
    Widget buttons[]={reset_button,help_button,close_button,airports[0]->fly_button,airports[1]->fly_button};
    const char *names[]={"reset","help","close","flyA","flyB"};
    for(unsigned i=0;i<XtNumber(buttons);i++){Position x,y;Dimension width,height;XtVaGetValues(buttons[i],XmNwidth,&width,XmNheight,&height,NULL);XtTranslateCoords(buttons[i],width/2,height/2,&x,&y);fprintf(f,"%s\"%s\":[%d,%d]",i?",":"",names[i],x,y);}
    if(help_dialog && XtIsManaged(help_dialog)) {Widget ok=XmMessageBoxGetChild(help_dialog,XmDIALOG_OK_BUTTON);Position x,y;Dimension width,height;XtVaGetValues(ok,XmNwidth,&width,XmNheight,&height,NULL);XtTranslateCoords(ok,width/2,height/2,&x,&y);fprintf(f,",\"helpOk\":[%d,%d]",x,y);}
    fprintf(f,"},\"help_visible\":%s,\"message\":\"",help_dialog && XtIsManaged(help_dialog)?"true":"false");for(const unsigned char *p=(unsigned char *)last_message;*p;p++){if(*p=='"'||*p=='\\')fputc('\\',f);if(*p>=32)fputc(*p,f);}fprintf(f,"\"}\n");fclose(f);
}
int main(int argc,char **argv)
{
    report_path=getenv("XM_AIRPORT_STATE_REPORT");
    for(int i=1;i<argc;i++)if(!strcmp(argv[i],"--state-report") && i+1<argc){report_path=argv[i+1];memmove(&argv[i],&argv[i+2],(argc-i-1)*sizeof(*argv));argc-=2;i--;}
    XtSetLanguageProc(NULL,NULL,NULL);
    String fallback[]={"*dragInitiatorProtocolStyle: XmDRAG_DYNAMIC","*dragReceiverProtocolStyle: XmDRAG_DYNAMIC",NULL};
    app_shell=XtVaOpenApplication(&app,"XmdAirport",NULL,0,&argc,argv,fallback,sessionShellWidgetClass,XmNtitle,"Airport — Drag and Drop",XmNdeleteResponse,XmDO_NOTHING,NULL);
    Display *display=XtDisplay(app_shell);double scale=1;XmGetWidgetScale(app_shell,&scale);
    if(!isfinite(scale) || scale<=0 || scale>SHRT_MAX/1060.0){fprintf(stderr,"Airport scale exceeds representable drawing geometry\n");return 2;}
    FlightAtom=XmInternAtom(display,"FLIGHT",False);DeleteAtom=XmInternAtom(display,"DELETE",False);
    Widget viewport=XtVaCreateManagedWidget("viewport",xmScrolledWindowWidgetClass,app_shell,XmNscrollingPolicy,XmAUTOMATIC,XmNshadowThickness,0,NULL);
    Widget outer=XtVaCreateManagedWidget("airportDemo",xmFormWidgetClass,viewport,XmNwidth,(int)(1040*scale),XmNheight,(int)(680*scale),XmNresizePolicy,XmRESIZE_NONE,XmNmarginWidth,(int)(16*scale),XmNmarginHeight,(int)(16*scale),NULL);
    Widget heading=Label(outer,"heading","Airport · Drag and Drop",XmAPPEARANCE_ROLE_PAGE_TITLE);
    XtVaSetValues(heading,XmNtopAttachment,XmATTACH_FORM,XmNleftAttachment,XmATTACH_FORM,NULL);
    Widget instructions=Label(outer,"instructions","Drag a plane to the other airport. Or select a plane and choose Fly / press Enter.",XmAPPEARANCE_ROLE_NORMAL);
    XtVaSetValues(instructions,XmNtopAttachment,XmATTACH_WIDGET,XmNtopWidget,heading,XmNtopOffset,(int)(8*scale),XmNleftAttachment,XmATTACH_FORM,NULL);
    Widget toolbar=XtVaCreateManagedWidget("actions",xmRowColumnWidgetClass,outer,XmNorientation,XmHORIZONTAL,XmNpacking,XmPACK_TIGHT,XmNspacing,(int)(8*scale),XmNtopAttachment,XmATTACH_WIDGET,XmNtopWidget,instructions,XmNtopOffset,(int)(10*scale),XmNleftAttachment,XmATTACH_FORM,NULL);
    reset_button=Button(toolbar,"resetGates","Reset gates",Reset,NULL);help_button=Button(toolbar,"howToFly","How to fly",Help,NULL);close_button=Button(toolbar,"closeAirport","Close",Close,NULL);
    Widget log_title=Label(outer,"logTitle","Flight log",XmAPPEARANCE_ROLE_SECTION);
    Widget log=XmCreateScrolledList(outer,"flightLog",NULL,0);XtVaSetValues(log,XmNvisibleItemCount,5,NULL);XtManageChild(log);
    XtVaSetValues(XtParent(log),XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,XmNbottomAttachment,XmATTACH_FORM,NULL);
    XtVaSetValues(log_title,XmNleftAttachment,XmATTACH_FORM,XmNbottomAttachment,XmATTACH_WIDGET,XmNbottomWidget,XtParent(log),XmNbottomOffset,(int)(6*scale),NULL);
    Widget ports=XtVaCreateManagedWidget("airports",xmFormWidgetClass,outer,XmNtopAttachment,XmATTACH_WIDGET,XmNtopWidget,toolbar,XmNtopOffset,(int)(16*scale),XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,XmNbottomAttachment,XmATTACH_WIDGET,XmNbottomWidget,log_title,XmNbottomOffset,(int)(16*scale),NULL);
    for(int k=0;k<2;k++){Airport a=CreateAirport(ports,display,k,scale);a->msg_area=log;}
    airports[0]->peer=airports[1];airports[1]->peer=airports[0];
    int sw=WidthOfScreen(XtScreen(app_shell)),sh=HeightOfScreen(XtScreen(app_shell));
    int width=(int)(1060*scale),height=(int)(704*scale);if(width>sw*9/10)width=sw*9/10;if(height>sh*9/10)height=sh*9/10;
    String geometry=NULL;int gx,gy;unsigned int gw,gh;int specified=0;
    XtVaGetValues(app_shell,XtNgeometry,&geometry,NULL);
    if(geometry)specified=XParseGeometry(geometry,&gx,&gy,&gw,&gh);
    if(specified&WidthValue)width=gw>SHRT_MAX?SHRT_MAX:(int)gw;
    if(specified&HeightValue)height=gh>SHRT_MAX?SHRT_MAX:(int)gh;
    XtVaSetValues(app_shell,XmNwidth,width,XmNheight,height,XmNminWidth,width<640*scale?width:(int)(640*scale),XmNminHeight,height<480*scale?height:(int)(480*scale),NULL);
    if(!(specified&XValue))XtVaSetValues(app_shell,XmNx,(sw-width)/2,NULL);
    if(!(specified&YValue))XtVaSetValues(app_shell,XmNy,(sh-height)/2,NULL);
    Atom wm_delete=XmInternAtom(display,"WM_DELETE_WINDOW",False);XmAddWMProtocolCallback(app_shell,wm_delete,Close,NULL);
    XtRealizeWidget(app_shell);
    Pixel foreground,background,muted,accent;XtVaGetValues(airports[0]->airport,XmNforeground,&foreground,XmNbackground,&background,XmNbottomShadowColor,&muted,XmNhighlightColor,&accent,NULL);
    XtResource specs[XtNumber(AirportResourcesSpecs)];memcpy(specs,AirportResourcesSpecs,sizeof(specs));
    for(unsigned i=0;i<XtNumber(specs);i++)if(!strcmp(specs[i].resource_type,XmRPixel)) {
        Pixel color=strstr(specs[i].resource_name,"Background")?background:foreground;
        if(!strcmp(specs[i].resource_name,"trackBackground"))color=muted;
        if(!strcmp(specs[i].resource_name,"dragUnderBackground"))color=accent;
        if(!strcmp(specs[i].resource_name,"dragUnderForeground"))color=background;
        specs[i].default_type=XmRImmediate;specs[i].default_addr=(XtPointer)(uintptr_t)color;
    }
    XtGetApplicationResources(app_shell,&AirportResources,specs,XtNumber(specs),NULL,0);
    for(int k=0;k<2;k++) {
        Airport a=airports[k];a->park.gc=XCreateGC(display,XtWindow(a->airport),0,NULL);a->track.gc=a->park.gc;
        /* Drag cursors remain monochrome, but scale with the selected display setting. */
        int size=px(a,30);Pixmap icon=XCreatePixmap(display,XtWindow(a->airport),size,size,1);GC gc=XCreateGC(display,icon,0,NULL);
        XSetForeground(display,gc,0);XFillRectangle(display,icon,gc,0,0,size,size);XSetForeground(display,gc,1);
        XPoint points[]={{size/2,0},{size*3/5,size*2/5},{size-1,size*3/5},{size-1,size*4/5},{size*3/5,size*3/5},{size*3/5,size*4/5},{size*4/5,size-1},{size/2,size*9/10},{size/5,size-1},{size*2/5,size*4/5},{size*2/5,size*3/5},{0,size*4/5},{0,size*3/5},{size*2/5,size*2/5}};
        XFillPolygon(display,icon,gc,points,XtNumber(points),Nonconvex,CoordModeOrigin);XFreeGC(display,gc);
        a->drag_bitmap=icon;
        a->dragIcon=XtVaCreateWidget("planeCursor",xmDragIconObjectClass,a->airport,XmNpixmap,icon,XmNmask,icon,XmNdepth,1,XmNwidth,size,XmNheight,size,NULL);
        ConfigureAirport(a->airport,a,NULL);MakeAirportDropSite(a->airport,a);
    }
    Reset(NULL,NULL,NULL);Report();XtAppMainLoop(app);report_path=NULL;
    for(int k=0;k<2;k++)airports[k]->state=closed;
    XtDestroyWidget(app_shell);
    for(int k=0;k<2;k++){XFreeGC(display,airports[k]->park.gc);XFreePixmap(display,airports[k]->drag_bitmap);}
    XtCloseDisplay(display);
    for(int k=0;k<2;k++){XtFree((char *)airports[k]->park.spots);XtFree((char *)airports[k]);}
    XtDestroyApplicationContext(app);return 0;
}
