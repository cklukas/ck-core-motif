/* Native scrolling, hit testing and selection visibility of TabStack. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/extensions/XTest.h>
#include <Xm/XmAll.h>
#include <Xm/TabBoxP.h>
#include <Xm/Appearance.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static unsigned notifications;
static void selected(Widget w,XtPointer c,XtPointer v){(void)w;(void)c;(void)v;++notifications;}
static void drain(XtAppContext a,Display*d){unsigned n=0;XSync(d,False);while(XtAppPending(a)){CHECK(++n<10000);XtAppProcessEvent(a,XtIMAll);XSync(d,False);}}
static int visible(Widget box,int index){
 Widget canvas=XtNameToWidget(box,"canvas");Boolean horizontal=XmTabBox_orientation(box)==XmHORIZONTAL;
 for(int p=0;p<(horizontal?XtWidth(canvas):XtHeight(canvas));p++)
  if(XmTabBoxXYToIndex(box,XtX(canvas)+(horizontal?p:XtWidth(canvas)/2),XtY(canvas)+(horizontal?XtHeight(canvas)/2:p))==index){
   int x=horizontal?p:XtWidth(canvas)/2,y=horizontal?XtHeight(canvas)/2:p;
   Widget stack=XtParent(box),page=XmTabStackIndexToWidget(stack,index);
   CHECK(XmTabStackXYToWidget(canvas,x,y)==page);
   CHECK(XmTabStackXYToWidget(stack,XtX(box)+XtX(canvas)+x,XtY(box)+XtY(canvas)+y)==page);
   return 1;
  }
 return 0;
}
static void click(Widget w,XtAppContext a,Display*d){
 Position x,y;XtTranslateCoords(w,XtWidth(w)/2,XtHeight(w)/2,&x,&y);
 XTestFakeMotionEvent(d,-1,x,y,CurrentTime);XTestFakeButtonEvent(d,1,True,CurrentTime);XTestFakeButtonEvent(d,1,False,CurrentTime);drain(a,d);
}
static void key(Widget canvas,const char*action,XtAppContext a,Display*d){
 XEvent event={0};event.xkey.type=KeyPress;event.xkey.display=d;event.xkey.window=XtWindow(canvas);
 XtCallActionProc(canvas,(char*)action,&event,NULL,0);drain(a,d);
}
static void press(KeySym symbol,XtAppContext a,Display*d){
 KeyCode code=XKeysymToKeycode(d,symbol);CHECK(code);
 CHECK(XTestFakeKeyEvent(d,code,True,CurrentTime));
 CHECK(XTestFakeKeyEvent(d,code,False,CurrentTime));drain(a,d);
}
static void edge(Widget canvas,const char*which,XtAppContext a,Display*d){
 (void)canvas;press(which[0]=='f'?XK_Home:XK_End,a,d);
}
static void repaint(Widget canvas,Display*d){
 unsigned w=XtWidth(canvas),h=XtHeight(canvas);
 XtClass(canvas)->core_class.expose(canvas,NULL,NULL);XSync(d,False);
 XImage*before=XGetImage(d,XtWindow(canvas),0,0,w,h,AllPlanes,ZPixmap);CHECK(before);
 XRectangle stripe={0,h/2,w,1};Region region=XCreateRegion();XUnionRectWithRegion(&stripe,region,region);
 XClearArea(d,XtWindow(canvas),0,h/2,w,1,False);
 XtClass(canvas)->core_class.expose(canvas,NULL,region);XSync(d,False);
 XImage*after=XGetImage(d,XtWindow(canvas),0,0,w,h,AllPlanes,ZPixmap);CHECK(after);
 for(unsigned y=0;y<h;y++)for(unsigned x=0;x<w;x++)CHECK(XGetPixel(before,x,y)==XGetPixel(after,x,y));
 XDestroyImage(before);XDestroyImage(after);XDestroyRegion(region);
}
int main(int argc,char**argv){
 XtAppContext app;Display*d;unsigned cases=0;
 XmAppearanceProfile profiles[]={XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT,XmAPPEARANCE_MODERN_DARK,XmAPPEARANCE_HIGH_CONTRAST};
 unsigned char sides[]={XmTABS_ON_TOP,XmTABS_ON_BOTTOM,XmTABS_ON_LEFT,XmTABS_ON_RIGHT};
 double scales[]={1,1.5,2};
 XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"scrolltabs","ScrollTabs",NULL,0,&argc,argv);CHECK(d);
 for(unsigned p=0;p<4;p++)for(unsigned scale=0;scale<3;scale++)for(unsigned side=0;side<4;side++)for(unsigned rtl=0;rtl<2;rtl++){
  printf("CASE profile%u scale%g side%u rtl%u\n",p,scales[scale],side,rtl);fflush(stdout);
  CHECK(XmApplyAppearanceProfile(d,profiles[p]));CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
  Widget shell=XtAppCreateShell("scrolltabs","ScrollTabs",applicationShellWidgetClass,d,NULL,0);
  XtVaSetValues(shell,XtNwidth,320,XtNheight,180,NULL);
  Widget stack=XtVaCreateManagedWidget("stack",xmTabStackWidgetClass,shell,XmNtabMode,XmTABS_SCROLLED,
    XmNtabSide,sides[side],XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);
  Widget pages[9];
  for(int i=0;i<9;i++){
   char label[100];snprintf(label,sizeof(label),"Section %d configuration",i+1);XmString text=XmStringCreateLocalized(label);
   pages[i]=XtVaCreateManagedWidget("page",xmLabelWidgetClass,stack,XmNlabelString,text,XmNtabLabelString,text,NULL);XmStringFree(text);
  }
  XtAddCallback(stack,XmNtabSelectedCallback,selected,NULL);XtRealizeWidget(shell);drain(app,d);
  Widget box=XtNameToWidget(stack,"tabBox"),canvas=XtNameToWidget(box,"canvas");CHECK(box&&canvas);
  Widget left=XtNameToWidget(box,"leftArrow"),right=XtNameToWidget(box,"rightArrow");CHECK(left&&right);
  CHECK(XtIsManaged(left)&&XtIsManaged(right));CHECK(visible(box,0));
  Boolean reverse=rtl && side<2;Widget forward=reverse?left:right,back=reverse?right:left;
  CHECK(!XtIsSensitive(back)&&XtIsSensitive(forward));
  unsigned before=notifications;click(forward,app,d);
  CHECK(XmTabBox__scroll_x(box)>0);CHECK(notifications==before);CHECK(XmTabStackGetSelectedTab(stack)==pages[0]);
  CHECK(XmTabBoxXYToIndex(box,XtX(forward)+XtWidth(forward)/2,XtY(forward)+XtHeight(forward)/2)==-1);
  for(int placement=0;placement<3;placement++){
   XtVaSetValues(stack,XmNtabArrowPlacement,placement,NULL);drain(app,d);
   XmTabStackSelectTab(pages[8],True);drain(app,d);CHECK(visible(box,8));CHECK(XmTabStackGetSelectedTab(stack)==pages[8]);
   int label_extent=side<2?XmTabBox__wanted(box)[8].width:XmTabBox__wanted(box)[8].height;
   int viewport=side<2?XtWidth(canvas):XtHeight(canvas);
   if(label_extent<=viewport)CHECK(!XtIsSensitive(forward));
   XmTabStackSelectTab(pages[0],False);drain(app,d);CHECK(visible(box,0));CHECK(XmTabBox__scroll_x(box)==0);
  }
  key(canvas,"XmTabBoxTraversePrevious",app,d);CHECK(XmTabStackGetSelectedTab(stack)==pages[8]);CHECK(visible(box,8));
  key(canvas,"XmTabBoxTraverseNext",app,d);CHECK(XmTabStackGetSelectedTab(stack)==pages[0]);CHECK(visible(box,0));
  XSetInputFocus(d,XtWindow(shell),RevertToParent,CurrentTime);drain(app,d);
  CHECK(XmProcessTraversal(canvas,XmTRAVERSE_CURRENT));drain(app,d);
  press(side<2?(rtl?XK_Left:XK_Right):XK_Down,app,d);
  CHECK(XmTabStackGetSelectedTab(stack)==pages[1]);CHECK(visible(box,1));
  edge(canvas,"last",app,d);CHECK(XmTabStackGetSelectedTab(stack)==pages[8]);CHECK(visible(box,8));repaint(canvas,d);
  XtSetSensitive(pages[0],False);XtSetSensitive(pages[8],False);drain(app,d);
  edge(canvas,"first",app,d);CHECK(XmTabStackGetSelectedTab(stack)==pages[1]);
  edge(canvas,"last",app,d);CHECK(XmTabStackGetSelectedTab(stack)==pages[7]);
  XtSetSensitive(pages[0],True);XtSetSensitive(pages[8],True);drain(app,d);
  XmTabStackSelectTab(pages[0],False);drain(app,d);
  XtVaSetValues(stack,XmNtabAutoSelect,False,NULL);drain(app,d);before=notifications;
  key(canvas,"XmTabBoxTraversePrevious",app,d);CHECK(XmTabStackGetSelectedTab(stack)==pages[0]);CHECK(visible(box,8));CHECK(notifications==before);
  press(XK_space,app,d);CHECK(XmTabStackGetSelectedTab(stack)==pages[8]);CHECK(notifications==before+1);
  XtUnmanageChildren(pages+1,7);drain(app,d);
  CHECK(XmTabStackGetSelectedTab(stack)==pages[8]);CHECK(visible(box,1));
  XtVaSetValues(shell,XtNwidth,1600,XtNheight,1000,NULL);drain(app,d);
  CHECK(!XtIsManaged(left)&&!XtIsManaged(right));CHECK(XmTabBox__scroll_x(box)==0);
  XtVaSetValues(shell,XtNwidth,320,XtNheight,180,NULL);drain(app,d);CHECK(visible(box,1));
  XtVaSetValues(shell,XtNwidth,1,XtNheight,1,NULL);drain(app,d);CHECK(XtWidth(canvas)>0&&XtHeight(canvas)>0);
  XmTabStackSelectTab(pages[0],False);drain(app,d);
  XtDestroyWidget(shell);drain(app,d);cases++;
 }
 for(unsigned dark=0;dark<2;dark++){
  XmAppearanceProfile profile=dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT;
  CHECK(XmApplyAppearanceProfile(d,profile));
  Widget shell=XtAppCreateShell("defaults","Defaults",applicationShellWidgetClass,d,NULL,0);
  Widget row=XtVaCreateManagedWidget("row",xmRowColumnWidgetClass,shell,NULL);
  Widget normal=XtVaCreateManagedWidget("normal",xmTabStackWidgetClass,row,NULL);
  Widget explicit=XtVaCreateManagedWidget("explicit",xmTabStackWidgetClass,row,XmNtabMode,XmTABS_BASIC,NULL);
  Widget direct=XtVaCreateManagedWidget("direct",xmTabBoxWidgetClass,row,NULL);
  XmTabMode mode;XtVaGetValues(normal,XmNtabMode,&mode,NULL);CHECK(mode==XmTABS_SCROLLED);
  XtVaGetValues(explicit,XmNtabMode,&mode,NULL);CHECK(mode==XmTABS_BASIC);
  XtVaGetValues(direct,XmNtabMode,&mode,NULL);CHECK(mode==XmTABS_SCROLLED);
  XtVaSetValues(direct,XmNtabMode,XmTABS_SCROLLED,NULL);
  CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));
  XtVaGetValues(normal,XmNtabMode,&mode,NULL);CHECK(mode==XmTABS_BASIC);
  XtVaGetValues(explicit,XmNtabMode,&mode,NULL);CHECK(mode==XmTABS_BASIC);
  XtVaGetValues(direct,XmNtabMode,&mode,NULL);CHECK(mode==XmTABS_SCROLLED);
  CHECK(XmApplyAppearanceProfile(d,profile));
  XtVaGetValues(normal,XmNtabMode,&mode,NULL);CHECK(mode==XmTABS_SCROLLED);
  XtVaGetValues(explicit,XmNtabMode,&mode,NULL);CHECK(mode==XmTABS_BASIC);
  XtDestroyWidget(shell);drain(app,d);
 }
 XtCloseDisplay(d);XtDestroyApplicationContext(app);printf("PASS tab scrolling: %u native scenarios\n",cases);return 0;
}
