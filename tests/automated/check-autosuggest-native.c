#include <Xm/XmAll.h>
#include <Xm/AutoSuggestBoxP.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/keysym.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if(!(x)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x);exit(1);} }while(0)
static void tick(XtPointer p,XtIntervalId *id){(void)id;*(Boolean*)p=True;}
static void drain(XtAppContext app,Display *d){
 Boolean done=False;XSync(d,False);XtAppAddTimeOut(app,1,tick,&done);
 while(!done || XtAppPending(app)) XtAppProcessEvent(app,XtIMAll);
 XSync(d,False);
}
static void modified_key(Widget w,KeySym symbol,unsigned state){
 XEvent e;memset(&e,0,sizeof(e));e.xkey.type=KeyPress;e.xkey.display=XtDisplay(w);
 e.xkey.window=XtWindow(w);e.xkey.root=RootWindowOfScreen(XtScreen(w));e.xkey.same_screen=True;
 e.xkey.keycode=XKeysymToKeycode(XtDisplay(w),symbol);e.xkey.state=state;XtDispatchEvent(&e);
}
static void key(Widget w,KeySym symbol){modified_key(w,symbol,0);}
static void item_button(Widget list,int type){
 Position x,y;Dimension width,height;XEvent e;
 CHECK(XmListPosToBounds(list,1,&x,&y,&width,&height));
 memset(&e,0,sizeof(e));e.xbutton.display=XtDisplay(list);e.xbutton.window=XtWindow(list);
 e.xbutton.root=RootWindowOfScreen(XtScreen(list));e.xbutton.same_screen=True;
 e.xbutton.button=Button1;e.xbutton.x=x+width/2;e.xbutton.y=y+height/2;
 e.xbutton.type=type;e.xbutton.time=type==ButtonPress?1000:1010;
 e.xbutton.state=type==ButtonRelease?Button1Mask:0;XtDispatchEvent(&e);
}
static void click_item(Widget list){item_button(list,ButtonPress);item_button(list,ButtonRelease);}
static unsigned outside_presses;
static void observe_press(Widget w,XtPointer data,XEvent *event,Boolean *dispatch){
 (void)w;(void)data;(void)dispatch;if(event->type==ButtonPress)++outside_presses;
}
static void press_surface(Widget w){
 XEvent event;memset(&event,0,sizeof(event));event.xbutton.type=ButtonPress;
 event.xbutton.display=XtDisplay(w);event.xbutton.window=XtWindow(w);
 event.xbutton.root=RootWindowOfScreen(XtScreen(w));event.xbutton.same_screen=True;
 event.xbutton.button=Button1;event.xbutton.x=2;event.xbutton.y=2;
 XtDispatchEvent(&event);
}
static unsigned commits,later,destroyed,dismissals;
static void dismissed(Widget w,XtPointer c,XtPointer data){(void)w;(void)c;(void)data;++dismissals;}
static void committed(Widget w,XtPointer c,XtPointer data){
 XmAutoSuggestCallbackStruct *cb=data;(void)w;(void)c;
 CHECK(cb && cb->id && !strcmp(cb->id,"item"));++commits;
}
static void die(Widget w,XtPointer c,XtPointer data){(void)c;(void)data;++destroyed;XtDestroyWidget(w);}
static void observed_destroy(Widget w,XtPointer c,XtPointer data){(void)w;(void)c;(void)data;++destroyed;}
static void unexpected(Widget w,XtPointer c,XtPointer data){(void)w;(void)c;(void)data;++later;}
static void destroy_owner(Widget w,XtPointer c,XtPointer data){(void)w;(void)data;++destroyed;XtDestroyWidget(c);}
static void reject(Widget w,XtPointer c,XtPointer data){(void)w;(void)c;((XmTextVerifyCallbackStruct*)data)->doit=False;}
static void fill(Widget w,unsigned count){
 XmAutoSuggestItem items[30];XmString label=XmStringCreateLocalized("A deliberately long suggestion that must scroll within the anchored list instead of widening the popup");
 CHECK(count<=XtNumber(items));for(unsigned i=0;i<count;++i)items[i]=(XmAutoSuggestItem){"item",label,NULL};
 CHECK(XmAutoSuggestBoxSetSuggestions(w,items,count,XmAutoSuggestBoxGetGeneration(w)));XmStringFree(label);
}
static void bounds(XmAutoSuggestBoxWidget b,Boolean above){
 Dimension highlight;
 Position x,y,px,py;Widget p=b->auto_suggest.popup_shell;XWindowAttributes a;
 CHECK(b->auto_suggest.visible && p && XtIsRealized(p));
 XtTranslateCoords(b->auto_suggest.search,0,0,&x,&y);XtTranslateCoords(p,0,0,&px,&py);
 CHECK(XGetWindowAttributes(XtDisplay(p),XtWindow(p),&a));CHECK(a.map_state==IsViewable);
 CHECK(px>=0 && py>=0 && px+a.width<=WidthOfScreen(XtScreen(p)) && py+a.height<=HeightOfScreen(XtScreen(p)));
 XtVaGetValues(b->auto_suggest.search,XmNhighlightThickness,&highlight,NULL);
 CHECK(a.width==b->auto_suggest.search->core.width-2*highlight);
 CHECK(px==x+highlight);
 if(above)CHECK(py+a.height<=y+highlight);else CHECK(py>=y+b->auto_suggest.search->core.height-highlight);
}
int main(int argc,char **argv){
 XtAppContext app;Display *d;unsigned cases=0;
 XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"suggestNative","SuggestNative",NULL,0,&argc,argv);CHECK(d);
 for(unsigned theme=0;theme<3;++theme)for(unsigned scale=1;scale<=2;++scale)for(unsigned rtl=0;rtl<2;++rtl){
  Widget shell,row,w,search,other,before;XmAutoSuggestBoxWidget b;XtWidgetGeometry pref={0};Dimension width,height;char *text;
  CHECK(XmSetDisplayScaleOverride(d,scale));CHECK(XmApplyAppearanceProfile(d,theme==0?XmAPPEARANCE_CLASSIC:theme==1?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));
  shell=XtVaAppCreateShell("suggestNative","SuggestNative",applicationShellWidgetClass,d,XtNx,50,XtNy,50,NULL);
  row=XtVaCreateManagedWidget("row",xmRowColumnWidgetClass,shell,XmNadjustLast,False,NULL);
  before=XtVaCreateManagedWidget("before",xmPushButtonWidgetClass,row,XmNnavigationType,XmTAB_GROUP,NULL);
  w=XtVaCreateManagedWidget("suggest",xmAutoSuggestBoxWidgetClass,row,XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);
  b=(XmAutoSuggestBoxWidget)w;search=XmAutoSuggestBoxGetSearchField(w);
  XtQueryGeometry(search,NULL,&pref);XtVaGetValues(w,XmNwidth,&width,XmNheight,&height,NULL);
  CHECK(width==pref.width && height==pref.height && height>5);
  XmSearchBoxSetString(search,"query");fill(w,3);CHECK(!XtIsRealized(b->auto_suggest.popup_shell));
  other=XtVaCreateManagedWidget("other",xmPushButtonWidgetClass,row,XmNnavigationType,XmTAB_GROUP,NULL);
  XtRealizeWidget(shell);XSetInputFocus(d,XtWindow(shell),RevertToParent,CurrentTime);drain(app,d);bounds(b,False);
  CHECK(XmIsScrolledWindow(b->auto_suggest.scroller));
  CHECK(!XmAutoSuggestBoxSetSuggestions(w,NULL,0,XmAutoSuggestBoxGetGeneration(w)+1));
  CHECK(XmAutoSuggestBoxGetSuggestionCount(w)==3);
  {XmAutoSuggestItem item={"id",NULL,NULL};CHECK(!XmAutoSuggestBoxSetSuggestions(w,&item,UINT_MAX,XmAutoSuggestBoxGetGeneration(w)));CHECK(XmAutoSuggestBoxGetSuggestionCount(w)==3);}
  fill(w,30);drain(app,d);bounds(b,False);
  {int rows=0;XtVaGetValues(b->auto_suggest.list,XmNvisibleItemCount,&rows,NULL);CHECK(rows<=8 && rows>=1);}
  XSetInputFocus(d,XtWindow(shell),RevertToParent,CurrentTime);
  CHECK(XmProcessTraversal(search,XmTRAVERSE_CURRENT));drain(app,d);
  CHECK(b->auto_suggest.visible);
  CHECK(XmProcessTraversal(b->auto_suggest.list,XmTRAVERSE_CURRENT));drain(app,d);
  CHECK(b->auto_suggest.visible);
  CHECK(XmProcessTraversal(search,XmTRAVERSE_CURRENT));drain(app,d);
  CHECK(b->auto_suggest.visible);
  for(unsigned reverse=0;reverse<2;++reverse){
   key(search,XK_Down);drain(app,d);
   CHECK(XmGetFocusWidget(b->auto_suggest.popup_shell)==b->auto_suggest.list);
   modified_key(b->auto_suggest.list,XK_Tab,reverse?ShiftMask:0);drain(app,d);
   CHECK(!b->auto_suggest.visible);
   CHECK(XmGetFocusWidget(shell)==(reverse?before:other));
   text=XmSearchBoxGetString(search);CHECK(!strcmp(text,"query"));XtFree(text);
   CHECK(XmProcessTraversal(search,XmTRAVERSE_CURRENT));
   XmAutoSuggestBoxShow(w);drain(app,d);CHECK(b->auto_suggest.visible);
  }
  CHECK(XmProcessTraversal(other,XmTRAVERSE_CURRENT));drain(app,d);
  CHECK(!b->auto_suggest.visible);
  CHECK(XmProcessTraversal(search,XmTRAVERSE_CURRENT));
  XmAutoSuggestBoxShow(w);drain(app,d);CHECK(b->auto_suggest.visible);
  {
   /* A late-created inert surface has neither translations nor focus changes.
    * Outside dismissal must preserve delivery of the same native press. */
   Widget surface=XtVaCreateManagedWidget("outside",xmLabelWidgetClass,row,NULL);
   XtAddEventHandler(surface,ButtonPressMask,False,observe_press,NULL);
   outside_presses=0;drain(app,d);
   press_surface(search);CHECK(b->auto_suggest.visible);
   press_surface(surface);drain(app,d);
   CHECK(!b->auto_suggest.visible && outside_presses==1);
   CHECK(XmGetFocusWidget(shell)==search);
   text=XmSearchBoxGetString(search);CHECK(!strcmp(text,"query"));XtFree(text);
   CHECK(!b->auto_suggest.watching_outside && !b->auto_suggest.outside_watches);
   XtDestroyWidget(surface);
   XmAutoSuggestBoxShow(w);drain(app,d);CHECK(b->auto_suggest.visible);
   surface=XtVaCreateManagedWidget("discarded",xmLabelWidgetClass,row,NULL);
   XtDestroyWidget(surface);CHECK(b->auto_suggest.visible);
   Widget extra=XtAppCreateShell("outsideShell","SuggestNative",applicationShellWidgetClass,d,NULL,0);
   surface=XtVaCreateManagedWidget("surface",xmLabelWidgetClass,extra,NULL);
   XtAddEventHandler(surface,ButtonPressMask,False,observe_press,NULL);
   XtRealizeWidget(extra);press_surface(surface);drain(app,d);
   CHECK(!b->auto_suggest.visible && outside_presses==2);
   XtDestroyWidget(extra);
   XmAutoSuggestBoxShow(w);drain(app,d);CHECK(b->auto_suggest.visible);
  }
  key(search,XK_Escape);drain(app,d);CHECK(!b->auto_suggest.visible);
  text=XmSearchBoxGetString(search);CHECK(!strcmp(text,"query"));XtFree(text);
  XmAutoSuggestBoxShow(w);drain(app,d);bounds(b,False);
  XtVaSetValues(shell,XtNy,HeightOfScreen(XtScreen(shell))-100,NULL);drain(app,d);bounds(b,True);
  XtVaSetValues(shell,XtNy,50,NULL);drain(app,d);bounds(b,False);
  XtAddCallback(w,XmNautoSuggestCommittedCallback,committed,NULL);commits=0;
  XtAddCallback(w,XmNautoSuggestDismissedCallback,dismissed,NULL);dismissals=0;
  {
   Widget targets[]={w,row,shell};
   for(unsigned target=0;target<XtNumber(targets);++target){
    item_button(b->auto_suggest.list,ButtonPress);
    XtSetSensitive(targets[target],False);
    XmAutoSuggestBoxCommit(w,0,NULL);CHECK(commits==0);
    XtSetSensitive(targets[target],True);
    XmAutoSuggestBoxCommit(w,0,NULL);CHECK(commits==0);
    XmAutoSuggestBoxShow(w);drain(app,d);CHECK(!b->auto_suggest.visible);
    CHECK(dismissals==target+1);
    XmAutoSuggestBoxShow(w);drain(app,d);CHECK(b->auto_suggest.visible);
    item_button(b->auto_suggest.list,ButtonRelease);CHECK(commits==0);
   }
  }
  XtSetSensitive(w,False);fill(w,3);CHECK(!XtIsSensitive(b->auto_suggest.list));
  XtSetSensitive(w,True);drain(app,d);CHECK(!b->auto_suggest.visible);
  XmAutoSuggestBoxShow(w);drain(app,d);CHECK(b->auto_suggest.visible);
  XtAddCallback(search,XmNmodifyVerifyCallback,reject,NULL);XmAutoSuggestBoxCommit(w,0,NULL);CHECK(commits==0);
  XtRemoveCallback(search,XmNmodifyVerifyCallback,reject,NULL);
  click_item(b->auto_suggest.list);drain(app,d);CHECK(commits==1 && !b->auto_suggest.visible);
  XmSearchBoxSetString(search,"new");CHECK(XmAutoSuggestBoxGetSuggestionCount(w)==0);
  fill(w,0);drain(app,d);CHECK(b->auto_suggest.no_results && !XtIsSensitive(b->auto_suggest.list));
  {char label[]="Nothing found";XtVaSetValues(w,XmNautoSuggestNoResultsLabel,label,NULL);label[0]='X';CHECK(!strcmp(b->auto_suggest.no_results_label,"Nothing found"));}
  XtUnmapWidget(shell);drain(app,d);CHECK(!b->auto_suggest.visible);
  XtDestroyWidget(shell);drain(app,d);++cases;
 }
 /* Destruction must stop both the current list and subsequent notifications. */
 for(unsigned mode=0;mode<11;++mode){
  Widget shell=XtAppCreateShell("lifetime","SuggestNative",applicationShellWidgetClass,d,NULL,0);
  Widget w=XtVaCreateManagedWidget("suggest",xmAutoSuggestBoxWidgetClass,shell,NULL);
  Widget search=XmAutoSuggestBoxGetSearchField(w);XtRealizeWidget(shell);drain(app,d);fill(w,2);drain(app,d);
  const char *resource=mode==0?XmNautoSuggestQueryChangedCallback:(mode==1 || mode==4)?XmNautoSuggestCommittedCallback:XmNautoSuggestDismissedCallback;
  destroyed=later=0;
  if(mode<3 || mode==4 || mode==5 || mode==7 || mode>=9){XtAddCallback(w,resource,die,NULL);XtAddCallback(w,resource,unexpected,NULL);}
  else if(mode==6 || mode==8){XtAddCallback(w,XmNdestroyCallback,observed_destroy,NULL);XtAddCallback(w,XmNautoSuggestDismissedCallback,unexpected,NULL);}
  else XtAddCallback(search,XmNvalueChangedCallback,destroy_owner,w);
  if(mode==0)XmSearchBoxSetString(search,"new");
  else if(mode==2)XmAutoSuggestBoxDismiss(w,NULL);
  else if(mode==5){XtSetSensitive(w,False);drain(app,d);}
  else if(mode==6){XtSetSensitive(w,False);XtDestroyWidget(w);drain(app,d);}
  else if(mode==7 || mode==8){
   XEvent event;memset(&event,0,sizeof(event));event.xfocus.type=FocusOut;
   event.xfocus.display=d;event.xfocus.window=XtWindow(search);
   event.xfocus.mode=NotifyNormal;event.xfocus.detail=NotifyNonlinear;
   XSetInputFocus(d,None,RevertToNone,CurrentTime);XtDispatchEvent(&event);
   CHECK(((XmAutoSuggestBoxWidget)w)->auto_suggest.focus_timer!=0);
   if(mode==8)XtDestroyWidget(w);
   drain(app,d);
  }
  else if(mode>=9)modified_key(((XmAutoSuggestBoxWidget)w)->auto_suggest.list,XK_Tab,mode==10?ShiftMask:0);
  else if(mode==4)click_item(((XmAutoSuggestBoxWidget)w)->auto_suggest.list);
  else XmAutoSuggestBoxCommit(w,0,NULL);
  CHECK(destroyed==1 && later==0);XtDestroyWidget(shell);drain(app,d);
 }
 /* A watched widget may destroy the owner from its earlier destroy callback.
  * Two popup observers may also destroy each other while a press is dispatched. */
 for(unsigned cross=0;cross<2;++cross){
  Widget shell=XtAppCreateShell("outsideLifetime","SuggestNative",applicationShellWidgetClass,d,NULL,0);
  Widget row=XtVaCreateManagedWidget("row",xmRowColumnWidgetClass,shell,NULL);
  Widget owner=XtVaCreateManagedWidget("owner",xmAutoSuggestBoxWidgetClass,row,NULL);
  Widget surface=XtVaCreateManagedWidget("surface",xmLabelWidgetClass,row,NULL);
  Widget second=cross?XtVaCreateManagedWidget("second",xmAutoSuggestBoxWidgetClass,row,NULL):NULL;
  destroyed=later=0;
  XtAddCallback(owner,XmNdestroyCallback,observed_destroy,NULL);
  if(!cross)XtAddCallback(surface,XmNdestroyCallback,destroy_owner,owner);
  XtRealizeWidget(shell);fill(owner,2);
  if(cross){
   fill(second,2);XtAddCallback(second,XmNautoSuggestDismissedCallback,destroy_owner,owner);
   press_surface(surface);drain(app,d);CHECK(destroyed==2);
  }else{XtDestroyWidget(surface);drain(app,d);CHECK(destroyed==2);}
  XtDestroyWidget(shell);drain(app,d);
 }
 XtCloseDisplay(d);XtDestroyApplicationContext(app);printf("PASS autosuggest native: %u configurations and 11 destruction paths\n",cases);return 0;
}
