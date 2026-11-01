/* Disabling a native list cancels its pending gesture, including via parents. */
#include <Xm/XmAll.h>
#include <Xm/ListP.h>
#include <Xm/Appearance.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do{if(!(x)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static unsigned notifications;
static void notified(Widget w,XtPointer closure,XtPointer data){(void)w;(void)closure;(void)data;++notifications;}
static void button(Widget w,int type){
 XEvent e;Position x,y;Dimension width,height;
 CHECK(XmListPosToBounds(w,1,&x,&y,&width,&height));memset(&e,0,sizeof(e));
 e.xbutton.type=type;e.xbutton.display=XtDisplay(w);e.xbutton.window=XtWindow(w);
 e.xbutton.root=RootWindowOfScreen(XtScreen(w));e.xbutton.same_screen=True;e.xbutton.button=Button1;
 e.xbutton.x=x+width/2;e.xbutton.y=y+height/2;e.xbutton.time=type==ButtonPress?1000:1010;
 e.xbutton.state=type==ButtonRelease?Button1Mask:0;XtDispatchEvent(&e);
}
static void drain(XtAppContext app,Display*d){XSync(d,False);while(XtAppPending(app))XtAppProcessEvent(app,XtIMAll);XSync(d,False);}
int main(int argc,char **argv){
 XtAppContext app;Display*d;unsigned cases=0;
 unsigned char policies[]={XmSINGLE_SELECT,XmMULTIPLE_SELECT,XmBROWSE_SELECT,XmEXTENDED_SELECT};
 const char *callbacks[]={XmNsingleSelectionCallback,XmNmultipleSelectionCallback,XmNbrowseSelectionCallback,XmNextendedSelectionCallback};
 XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"listCancel","ListCancel",NULL,0,&argc,argv);CHECK(d);
 for(unsigned theme=0;theme<3;++theme)for(unsigned policy=0;policy<XtNumber(policies);++policy)for(unsigned parent=0;parent<2;++parent){
  Widget shell,row,list,target;XmString item=XmStringCreateLocalized("Select this item");unsigned before;
  CHECK(XmApplyAppearanceProfile(d,theme==0?XmAPPEARANCE_CLASSIC:theme==1?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));
  shell=XtAppCreateShell("listCancel","ListCancel",applicationShellWidgetClass,d,NULL,0);
  row=XtVaCreateManagedWidget("row",xmRowColumnWidgetClass,shell,NULL);
  list=XtVaCreateManagedWidget("list",xmListWidgetClass,row,XmNitems,&item,XmNitemCount,1,
    XmNselectionPolicy,policies[policy],XmNvisibleItemCount,2,NULL);XmStringFree(item);
  XtAddCallback(list,callbacks[policy],notified,NULL);XtAddCallback(list,XmNdefaultActionCallback,notified,NULL);
  XtRealizeWidget(shell);drain(app,d);target=parent?row:list;notifications=0;
  button(list,ButtonPress);before=notifications;
  CHECK(((XmListWidget)list)->list.Event!=0);
  XtSetSensitive(target,False);XtSetSensitive(target,True);
  CHECK(((XmListWidget)list)->list.Event==0);
  CHECK(((XmListWidget)list)->list.DragID==0 && ((XmListWidget)list)->list.drag_start_timer==0);
  button(list,ButtonRelease);CHECK(notifications==before);
  XmListDeselectAllItems(list);button(list,ButtonPress);button(list,ButtonRelease);CHECK(notifications>before);
  XtDestroyWidget(shell);drain(app,d);++cases;
 }
 XtCloseDisplay(d);XtDestroyApplicationContext(app);printf("PASS list sensitivity: %u configurations\n",cases);return 0;
}
