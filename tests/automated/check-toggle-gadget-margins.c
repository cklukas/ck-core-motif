/* Horizontal margins and gadget-relative text coordinates must agree. */
#include <Xm/ToggleBGP.h>
#include <Xm/DrawingA.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Shell.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
int main(int argc,char **argv){
 XtAppContext app;Widget shell=XtVaAppInitialize(&app,"ToggleMargins",NULL,0,&argc,argv,NULL,XmNwidth,1000,XmNheight,800,NULL);
 Display *d=XtDisplay(shell);Widget host=XtVaCreateManagedWidget("host",xmDrawingAreaWidgetClass,shell,NULL);
 XtRealizeWidget(shell);unsigned cases=0;
 for(unsigned theme=0;theme<3;theme++)for(unsigned scale=1;scale<4;scale++)for(unsigned rtl=0;rtl<2;rtl++){
  CHECK(XmApplyAppearanceProfile(d,theme==2?XmAPPEARANCE_CLASSIC:theme?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
  CHECK(XmSetDisplayScaleOverride(d,scale));
  XmString text=XmStringCreateLocalized("Enable notifications");
  Widget w=XtVaCreateManagedWidget("toggle",xmToggleButtonGadgetClass,host,XmNx,37,XmNy,29,
    XmNlabelString,text,XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,
    XmNmarginWidth,12*scale,XmNmarginHeight,3*scale,XmNalignment,XmALIGNMENT_BEGINNING,NULL);
  XmStringFree(text);XmToggleButtonGadget p=(XmToggleButtonGadget)w;
  int inset=p->gadget.highlight_thickness+p->gadget.shadow_thickness+LabG_MarginWidth(p);
  int expected_width=LabG_TextRect(p).width+LabG_MarginLeft(p)+LabG_MarginRight(p)+2*inset;
  CHECK(p->rectangle.width==expected_width);
  int expected_x=rtl?p->rectangle.width-inset-LabG_MarginRight(p)-LabG_TextRect(p).width:inset+LabG_MarginLeft(p);
  CHECK(LabG_TextRect(p).x==expected_x);
  XtVaSetValues(w,XmNmarginWidth,15*scale,NULL);
  inset=p->gadget.highlight_thickness+p->gadget.shadow_thickness+LabG_MarginWidth(p);
  CHECK(p->rectangle.width==LabG_TextRect(p).width+LabG_MarginLeft(p)+LabG_MarginRight(p)+2*inset);
  expected_x=rtl?p->rectangle.width-inset-LabG_MarginRight(p)-LabG_TextRect(p).width:inset+LabG_MarginLeft(p);
  CHECK(LabG_TextRect(p).x==expected_x);
  XtDestroyWidget(w);++cases;
 }
 XtDestroyWidget(shell);XtCloseDisplay(d);XtDestroyApplicationContext(app);
 printf("PASS toggle gadget horizontal margins: %u theme/scale/direction cases\n",cases);return 0;
}
