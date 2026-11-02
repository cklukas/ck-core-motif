/* Semantic status colors must follow severity and preserve app overrides. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static Pixel color(Widget w,const char *name){XColor c,e;CHECK(XAllocNamedColor(XtDisplay(w),DefaultColormapOfScreen(XtScreen(w)),name,&c,&e));return c.pixel;}
int main(int argc,char **argv){
 XtAppContext app;Widget shell=XtVaAppInitialize(&app,"StatusColors",NULL,0,&argc,argv,NULL,NULL);Display*d=XtDisplay(shell);
 const char *backgrounds[2][3]={{"#e8f0fe","#fef7e0","#fce8e6"},{"#253b57","#4b3b17","#5c292d"}};
 const char *foregrounds[2][3]={{"#174ea6","#754c00","#a50e0e"},{"#d3e3fd","#fdd663","#f6c7c3"}};
 CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_LIGHT));
 Widget root=XtVaCreateManagedWidget("root",xmRowColumnWidgetClass,shell,NULL);
 Widget bar=XtVaCreateManagedWidget("bar",xmInfoBarWidgetClass,root,XmNinfoMessage,"Status message",XmNwidth,360,XmNheight,48,NULL);
 Widget link=XtVaCreateManagedWidget("link",xmLinkWidgetClass,root,NULL);XtRealizeWidget(shell);
 for(unsigned dark=0;dark<2;dark++){
  CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
  for(unsigned severity=0;severity<3;severity++){
   Pixel bg,fg;XtVaSetValues(bar,XmNinfoSeverity,severity,NULL);XtVaGetValues(bar,XmNbackground,&bg,XmNforeground,&fg,NULL);
   CHECK(bg==color(bar,backgrounds[dark][severity]));CHECK(fg==color(bar,foregrounds[dark][severity]));
  }
  Pixel fg;XtVaGetValues(link,XmNforeground,&fg,NULL);CHECK(fg==color(link,dark?"#8ab4f8":"#1967d2"));
 }
 Pixel explicit_bg=color(bar,"#654321"),explicit_fg=color(bar,"#fedcba"),bg,fg;
 XtVaSetValues(bar,XmNbackground,explicit_bg,XmNforeground,explicit_fg,NULL);
 XtVaSetValues(bar,XmNinfoSeverity,XmINFO_SEVERITY_INFO,NULL);CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_LIGHT));
 XtVaGetValues(bar,XmNbackground,&bg,XmNforeground,&fg,NULL);CHECK(bg==explicit_bg&&fg==explicit_fg);
 XtDestroyWidget(shell);puts("PASS modern status colors: severity, live light/dark, link color and explicit overrides");return 0;
}
