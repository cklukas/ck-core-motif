/* Default-action emphasis must not move modern labels or resize controls. */
#include <X11/IntrinsicP.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <Xm/LabelP.h>
#include <Xm/LabelGP.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);} }while(0)
static void drain(XtAppContext a,Display*d){XSync(d,False);while(XtAppPending(a))XtAppProcessEvent(a,XtIMAll);XSync(d,False);}
typedef struct { Dimension w,h,highlight,left,right,top,bottom; XRectangle text; } Geometry;
static Geometry geometry(Widget w){Geometry g={0};
 XtVaGetValues(w,XmNwidth,&g.w,XmNheight,&g.h,XmNhighlightThickness,&g.highlight,
 XmNmarginLeft,&g.left,XmNmarginRight,&g.right,XmNmarginTop,&g.top,XmNmarginBottom,&g.bottom,NULL);
 g.text=XtIsWidget(w)?((XmLabelWidget)w)->label.TextRect:LabG_TextRect(w);return g;}
static void same(Widget w,Geometry expected){Geometry g=geometry(w);
 if(g.w!=expected.w||g.h!=expected.h)fprintf(stderr,"%s: %ux%u expected %ux%u\n",XtName(w),g.w,g.h,expected.w,expected.h);
 CHECK(g.w==expected.w&&g.h==expected.h);CHECK(g.highlight==expected.highlight);
 CHECK(g.left==expected.left&&g.right==expected.right&&g.top==expected.top&&g.bottom==expected.bottom);
 CHECK(g.text.x==expected.text.x&&g.text.y==expected.text.y&&g.text.width==expected.text.width&&g.text.height==expected.text.height);}
int main(int argc,char**argv){XtAppContext app;Display*d;unsigned count=0;const double scales[]={1,1.5,2};
 XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"defaultGeometry","DefaultGeometry",NULL,0,&argc,argv);CHECK(d);
 for(unsigned scale=0;scale<3;++scale)for(unsigned dark=0;dark<2;++dark)for(unsigned gadget=0;gadget<2;++gadget)for(unsigned fixed=0;fixed<2;++fixed){
  XmAppearanceProfile profile=dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT;Widget shell,board,w;XmString label;Geometry before;
  CHECK(XmSetDisplayScaleOverride(d,scales[scale]));CHECK(XmApplyAppearanceProfile(d,profile));
  shell=XtAppCreateShell("defaultGeometry","DefaultGeometry",applicationShellWidgetClass,d,NULL,0);
  board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,800,XmNheight,500,NULL);
  label=XmStringCreateLocalized("Save changes");w=XtVaCreateManagedWidget("button",gadget?xmPushButtonGadgetClass:xmPushButtonWidgetClass,board,XmNlabelString,label,XmNwidth,fixed?240:0,XmNheight,fixed?80:0,NULL);XmStringFree(label);
  XtRealizeWidget(shell);drain(app,d);before=geometry(w);
  {
   Widget initial; XmString text=XmStringCreateLocalized("Save changes");
   initial=XtVaCreateManagedWidget("initialDefault",gadget?xmPushButtonGadgetClass:xmPushButtonWidgetClass,board,
      XmNlabelString,text,XmNwidth,fixed?240:0,XmNheight,fixed?80:0,
      XmNshowAsDefault,1,XmNdefaultButtonShadowThickness,4,XmNy,200,NULL);
   XmStringFree(text);drain(app,d);same(initial,before);XtDestroyWidget(initial);
  }
  XtVaSetValues(board,XmNdefaultButton,w,NULL);drain(app,d);same(w,before);
  XtVaSetValues(board,XmNdefaultButton,NULL,NULL);drain(app,d);same(w,before);
  XtVaSetValues(w,XmNshowAsDefault,1,NULL);drain(app,d);same(w,before);
  XtVaSetValues(w,XmNshowAsDefault,0,NULL);drain(app,d);same(w,before);
  XtVaSetValues(w,XmNdefaultButtonShadowThickness,4,XmNshowAsDefault,1,NULL);drain(app,d);same(w,before);
  for(unsigned cycle=0;cycle<3;++cycle){CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(app,d);CHECK(XmApplyAppearanceProfile(d,profile));drain(app,d);same(w,before);}
  XtVaSetValues(w,XmNshowAsDefault,0,XmNdefaultButtonShadowThickness,0,NULL);drain(app,d);same(w,before);
  XtDestroyWidget(shell);drain(app,d);++count;
 }
 for(unsigned gadget=0;gadget<2;++gadget)for(unsigned dark=0;dark<2;++dark){
  Widget shell,board,w,reference;XmString text;Geometry expected;
  CHECK(XmSetDisplayScaleOverride(d,1));CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));
  shell=XtAppCreateShell("classicFirst","DefaultGeometry",applicationShellWidgetClass,d,NULL,0);
  board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,800,XmNheight,500,NULL);
  text=XmStringCreateLocalized("Save changes");
  w=XtVaCreateManagedWidget("classicDefault",gadget?xmPushButtonGadgetClass:xmPushButtonWidgetClass,board,
    XmNlabelString,text,XmNshowAsDefault,1,XmNdefaultButtonShadowThickness,4,NULL);
  XtRealizeWidget(shell);drain(app,d);
  CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));drain(app,d);
  reference=XtVaCreateManagedWidget("modernReference",gadget?xmPushButtonGadgetClass:xmPushButtonWidgetClass,board,
    XmNlabelString,text,XmNy,200,NULL);XmStringFree(text);drain(app,d);
  expected=geometry(reference);same(w,expected);
  XtDestroyWidget(shell);drain(app,d);++count;
 }
 printf("PASS default button geometry: %u cases\n",count);XtCloseDisplay(d);XtDestroyApplicationContext(app);return 0;
}
