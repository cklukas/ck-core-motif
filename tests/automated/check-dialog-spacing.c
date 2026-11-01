/* Modern dialog spacing belongs to the toolkit unless overridden by callers. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <X11/Shell.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static unsigned cases;
static void drain(XtAppContext a,Display*d){XSync(d,False);while(XtAppPending(a))XtAppProcessEvent(a,XtIMAll);XSync(d,False);}
static void spacing(Widget w,unsigned margin,unsigned frame){Dimension x,y,s;XtVaGetValues(w,XmNmarginWidth,&x,XmNmarginHeight,&y,XmNshadowThickness,&s,NULL);if(x!=margin||y!=margin||s!=frame)fprintf(stderr,"%s: %u,%u,%u expected%u,%u\n",XtName(w),x,y,s,margin,frame);CHECK(x==margin&&y==margin&&s==frame);cases++;}
int main(int argc,char**argv){
 XtAppContext a;Display*d;double scales[]={1,1.5,2};
 WidgetClass classes[]={xmMessageBoxWidgetClass,xmSelectionBoxWidgetClass,xmCommandWidgetClass,xmFileSelectionBoxWidgetClass};
 XtToolkitInitialize();a=XtCreateApplicationContext();d=XtOpenDisplay(a,NULL,"spacing","Spacing",NULL,0,&argc,argv);CHECK(d);
 for(unsigned dark=0;dark<2;dark++)for(unsigned scale=0;scale<3;scale++)for(unsigned compact=0;compact<2;compact++)for(unsigned type=0;type<4;type++){
  CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));CHECK(XmSetDisplayScaleOverride(d,scales[scale]));CHECK(XmApplyAppearanceDensity(d,compact?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
  Widget shell=XtAppCreateShell("spacing","Spacing",applicationShellWidgetClass,d,NULL,0);
  XtVaSetValues(shell,XtNwidth,1200,XtNheight,1100,NULL);
  Widget dialog=XtVaCreateManagedWidget("dialog",classes[type],shell,NULL);
  Dimension old_x,old_y,old_s;XtVaGetValues(dialog,XmNmarginWidth,&old_x,XmNmarginHeight,&old_y,XmNshadowThickness,&old_s,NULL);
  CHECK(old_x==old_y);XtRealizeWidget(shell);drain(a,d);
  XmAppearanceProfile profile=dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT;
  CHECK(XmApplyAppearanceProfile(d,profile));drain(a,d);
  unsigned margin=(unsigned)((compact?8:16)*scales[scale]+.5);spacing(dialog,margin,0);
  CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(a,d);spacing(dialog,old_x,old_s);
  CHECK(XmApplyAppearanceProfile(d,profile));drain(a,d);spacing(dialog,margin,0);
  XtVaSetValues(dialog,XmNmarginWidth,margin,XmNmarginHeight,margin,XmNshadowThickness,0,NULL);
  CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(a,d);spacing(dialog,margin,0);
  XtVaSetValues(dialog,XmNmarginWidth,7,XmNmarginHeight,7,XmNshadowThickness,3,NULL);
  CHECK(XmApplyAppearanceProfile(d,profile));drain(a,d);spacing(dialog,7,3);
  XtDestroyWidget(shell);drain(a,d);
 }
 XtCloseDisplay(d);XtDestroyApplicationContext(a);printf("PASS dialog spacing: %u ownership cases\n",cases);return 0;
}
