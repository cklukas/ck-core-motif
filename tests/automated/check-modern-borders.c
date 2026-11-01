/* Shared modern border defaults: flat paint, titles, live changes and overrides. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/FrameP.h>
#include <Xm/LabelGP.h>
#include <Xm/ColorS.h>
#include <Xm/FontS.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static unsigned warnings;
static void warning(String message){++warnings;fprintf(stderr,"warning: %s\n",message);}
static void drain(XtAppContext a,Display*d){XSync(d,False);while(XtAppPending(a))XtAppProcessEvent(a,XtIMAll);XSync(d,False);}
static void border(Widget w,Boolean flat){XmFrameWidget f=(XmFrameWidget)w;Pixel top,bottom;XImage*im;
 XtVaGetValues(w,XmNtopShadowColor,&top,XmNbottomShadowColor,&bottom,NULL);CHECK((top==bottom)==flat);
 im=XGetImage(XtDisplay(w),XtWindow(w),0,0,XtWidth(w),XtHeight(w),AllPlanes,ZPixmap);CHECK(im);
 int x=f->frame.old_shadow_x,y=f->frame.old_shadow_y,width=f->frame.old_width,height=f->frame.old_height;
 CHECK(width>20&&height>20);
 if(flat){CHECK(XGetPixel(im,x,y+height/2)==bottom);CHECK(XGetPixel(im,x+width-1,y+height/2)==bottom);
 CHECK(XGetPixel(im,x+width-8,y)==bottom);CHECK(XGetPixel(im,x+width/2,y+height-1)==bottom);}
 XDestroyImage(im);
}
int main(int argc,char**argv){XtAppContext app;Display*d;unsigned cases=0;const double scales[]={1,1.5,2};
 XtToolkitInitialize();app=XtCreateApplicationContext();XtAppSetWarningHandler(app,warning);
 d=XtOpenDisplay(app,NULL,"borders","Borders",NULL,0,&argc,argv);CHECK(d);
 for(unsigned scale=0;scale<3;++scale)for(unsigned dark=0;dark<2;++dark)for(unsigned titled=0;titled<2;++titled){
  Widget shell,board,frame,selectors[2]={NULL,NULL},preserved[2]={NULL,NULL};Dimension thickness;XmAppearanceProfile profile=dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT;
  CHECK(XmSetDisplayScaleOverride(d,scales[scale]));CHECK(XmApplyAppearanceProfile(d,profile));
  shell=XtAppCreateShell("borders","Borders",applicationShellWidgetClass,d,NULL,0);
  board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,700,XmNheight,500,NULL);
  frame=XtVaCreateManagedWidget("frame",xmFrameWidgetClass,board,XmNwidth,320,XmNheight,200,NULL);
  XtVaCreateManagedWidget("content",xmFormWidgetClass,frame,NULL);
  if(titled){XmString text=XmStringCreateLocalized("Group");XtVaCreateManagedWidget("title",xmLabelWidgetClass,frame,XmNframeChildType,XmFRAME_TITLE_CHILD,XmNlabelString,text,NULL);XmStringFree(text);}
  /* RowColumn shadow changes are legal only for popup/pulldown panes and menu bars. */
  XtVaCreateManagedWidget("rows",xmRowColumnWidgetClass,board,XmNy,250,NULL);
  XmCreateRadioBox(board,"radio",NULL,0);XmCreateOptionMenu(board,"option",NULL,0);XmCreateMenuBar(board,"bar",NULL,0);
  {
   Widget widget=XtVaCreateWidget("toggle",xmToggleButtonWidgetClass,board,NULL);
   Widget gadget=XtVaCreateWidget("toggleGadget",xmToggleButtonGadgetClass,board,NULL);
   Pixel widget_border,gadget_border;
   XtVaGetValues(widget,XmNbottomShadowColor,&widget_border,NULL);
   XtVaGetValues(gadget,XmNbottomShadowColor,&gadget_border,NULL);
   if(widget_border!=gadget_border) fprintf(stderr,"toggle parity scale=%g dark=%u widget=%lu gadget=%lu\n",scales[scale],dark,widget_border,gadget_border);
   if(widget_border!=gadget_border) fprintf(stderr,"cached gadget=%lu\n",LabG_BottomShadowColor(gadget));
   CHECK(widget_border==gadget_border);
   CHECK(gadget_border==LabG_BottomShadowColor(gadget));
   {
    const char *names[]={XmNtopShadowColor,XmNbottomShadowColor,XmNhighlightColor};
    WidgetClass classes[]={xmLabelGadgetClass,xmPushButtonGadgetClass,xmToggleButtonGadgetClass};
    for(unsigned c=0;c<3;++c){
     Widget item=XtVaCreateWidget("explicitGadget",classes[c],board,NULL);
     for(unsigned n=0;n<3;++n){
      Pixel actual,expected=WhitePixelOfScreen(XtScreen(board));
      XtVaSetValues(item,names[n],expected,NULL);
      XtVaGetValues(item,names[n],&actual,NULL);CHECK(actual==expected);
     }
    }
   }
  }
  if(scale==0 && !titled){
   selectors[0]=XtVaCreateWidget("colorSelector",xmColorSelectorWidgetClass,board,NULL);
   selectors[1]=XtVaCreateWidget("fontSelector",xmFontSelectorWidgetClass,board,NULL);
   preserved[0]=XtNameToWidget(selectors[0],"*radioBox");
   preserved[1]=XtNameToWidget(selectors[1],"*encodingOptionMenu");
   for(unsigned i=0;i<2;++i){CHECK(preserved[i]);XtVaSetValues(preserved[i],XmNbottomShadowColor,WhitePixelOfScreen(XtScreen(board)),NULL);}
  }
  {
   Widget icon=XtVaCreateWidget("icon",xmIconGadgetClass,board,NULL);
   Dimension icon_shadow;
   XtVaGetValues(icon,XmNshadowThickness,&icon_shadow,NULL);CHECK(icon_shadow==0);
   XtVaSetValues(icon,XmNshadowThickness,3,NULL);
   CHECK(XmApplyAppearanceProfile(d,profile));
   XtVaGetValues(icon,XmNshadowThickness,&icon_shadow,NULL);CHECK(icon_shadow==3);
  }
  XtRealizeWidget(shell);drain(app,d);XtVaGetValues(frame,XmNshadowThickness,&thickness,NULL);CHECK(thickness==(Dimension)(scales[scale]+0.5));border(frame,True);
  CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(app,d);border(frame,False);
  CHECK(XmApplyAppearanceProfile(d,profile));drain(app,d);border(frame,True);
  XtVaSetValues(frame,XmNshadowThickness,3,NULL);CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));drain(app,d);XtVaGetValues(frame,XmNshadowThickness,&thickness,NULL);CHECK(thickness==3);border(frame,True);
  XtVaSetValues(frame,XmNtopShadowColor,WhitePixelOfScreen(XtScreen(frame)),XmNbottomShadowColor,BlackPixelOfScreen(XtScreen(frame)),NULL);
  CHECK(XmApplyAppearanceProfile(d,profile));drain(app,d);border(frame,False);
  for(unsigned i=0;i<2;++i)if(preserved[i]){
   Pixel value;XtVaGetValues(preserved[i],XmNbottomShadowColor,&value,NULL);
   CHECK(value==WhitePixelOfScreen(XtScreen(board)));
   /* Ordinary application SetValues still propagates composite resources. */
   XtVaSetValues(selectors[i],XmNforeground,BlackPixelOfScreen(XtScreen(board)),NULL);
   XtVaGetValues(preserved[i],XmNforeground,&value,NULL);
   CHECK(value==BlackPixelOfScreen(XtScreen(board)));
  }
  XtDestroyWidget(shell);drain(app,d);++cases;
 }
 CHECK(warnings==0);printf("PASS modern borders: %u cases\n",cases);XtCloseDisplay(d);XtDestroyApplicationContext(app);return 0;
}
