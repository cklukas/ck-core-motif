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
 CHECK(g.w==expected.w&&g.h==expected.h);if(g.highlight!=expected.highlight)fprintf(stderr,"%s highlight %u expected %u\n",XtName(w),g.highlight,expected.highlight);CHECK(g.highlight==expected.highlight);
 CHECK(g.left==expected.left&&g.right==expected.right&&g.top==expected.top&&g.bottom==expected.bottom);
 CHECK(g.text.x==expected.text.x&&g.text.y==expected.text.y&&g.text.width==expected.text.width&&g.text.height==expected.text.height);}
static unsigned dialog_actions(XtAppContext app, Display *d){
 unsigned count=0;
 for(unsigned profile=0;profile<3;++profile)for(unsigned compact=0;compact<2;++compact)
 for(unsigned scale=1;scale<=2;++scale)for(unsigned selection=0;selection<2;++selection)
 for(unsigned minimize=0;minimize<2;++minimize)for(unsigned long_label=0;long_label<2;++long_label){
  Widget shell,dialog,button;Dimension width,height,tw,th;XmFontList font;XmString text;
  CHECK(XmApplyAppearanceProfile(d,profile==0?XmAPPEARANCE_CLASSIC:profile==1?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));
  CHECK(XmApplyAppearanceDensity(d,compact?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
  CHECK(XmSetDisplayScaleOverride(d,scale));
  shell=XtAppCreateShell("dialogActions","DefaultGeometry",applicationShellWidgetClass,d,NULL,0);
  dialog=selection?XmCreatePromptDialog(shell,"prompt",NULL,0):XmCreateInformationDialog(shell,"notice",NULL,0);
  button=selection?XmSelectionBoxGetChild(dialog,XmDIALOG_OK_BUTTON):XmMessageBoxGetChild(dialog,XmDIALOG_OK_BUTTON);
  if(!selection){XtUnmanageChild(XmMessageBoxGetChild(dialog,XmDIALOG_CANCEL_BUTTON));XtUnmanageChild(XmMessageBoxGetChild(dialog,XmDIALOG_HELP_BUTTON));}
  text=XmStringCreateLocalized(long_label?"Save all pending document changes":"OK");
  XtVaSetValues(dialog,XmNminimizeButtons,minimize,XmNokLabelString,text,NULL);XmStringFree(text);
  XtManageChild(dialog);drain(app,d);
  XtVaGetValues(button,XmNwidth,&width,XmNheight,&height,XmNfontList,&font,XmNlabelString,&text,NULL);
  XmStringExtent(font,text,&tw,&th);XmStringFree(text);CHECK(width>tw&&height>th);
  {Geometry g=geometry(button);CHECK(abs(g.text.x-((int)width-(int)tw)/2)<=1);}
  if(profile&&!minimize)CHECK(width>=(compact?64:80)*scale);
  if(profile&&minimize&&!long_label&&!selection)CHECK(width<80*scale);
  if(profile&&!long_label){CHECK(th>=12*scale);CHECK(height>=24*scale);}
  /* Long labels retain their natural extent, and a single short action
   * contracts again when the documented natural-size opt-out is enabled. */
  if(profile&&!selection&&!long_label&&!minimize){
   XtVaSetValues(dialog,XmNminimizeButtons,True,NULL);drain(app,d);
   XtVaGetValues(button,XmNwidth,&width,NULL);CHECK(width<80*scale);
   XtVaSetValues(dialog,XmNminimizeButtons,False,NULL);drain(app,d);
   XtVaGetValues(button,XmNwidth,&width,NULL);CHECK(width>=(compact?64:80)*scale);
  }
  XtDestroyWidget(shell);drain(app,d);++count;
 }
 CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMFORTABLE));return count;
}
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
 count+=dialog_actions(app,d);
 printf("PASS default button geometry: %u cases\n",count);XtCloseDisplay(d);XtDestroyApplicationContext(app);return 0;
}
