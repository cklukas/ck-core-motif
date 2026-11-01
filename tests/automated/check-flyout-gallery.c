/* Exercise the production gallery factory and popup placement, not a clone. */
#include <X11/IntrinsicP.h>
#include <Xm/XmAll.h>
#include <Xm/FlyoutP.h>
#include <Xm/Appearance.h>
#include <stdio.h>
#include <stdlib.h>
static unsigned checks,dismissals;
#define CHECK(c) do {++checks;if(!(c)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#c);exit(1);}}while(0)
extern Widget gallery_create_component_example(Widget,WidgetClass,const char *,XtCallbackProc,XtPointer);
extern void gallery_destroy_component_example(Widget *);
static void dismiss(Widget w,XtPointer p,XtPointer c){(void)w;(void)p;(void)c;++dismissals;}
static void tick(XtPointer p,XtIntervalId *id){(void)id;*(Boolean*)p=True;}
static void settle(XtAppContext app,Display*d){Boolean done=False;XtAppAddTimeOut(app,30,tick,&done);while(!done)XtAppProcessEvent(app,XtIMAll);XSync(d,False);while(XtAppPending(app))XtAppProcessEvent(app,XtIMAll);}
int main(int argc,char **argv){
 XtAppContext app;Widget shell=XtVaAppInitialize(&app,"FlyoutGallery",NULL,0,&argc,argv,NULL,XmNx,200,XmNy,150,NULL);Display*d=XtDisplay(shell);
 Widget board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,900,XmNheight,400,NULL);XtRealizeWidget(shell);
 const double scales[]={1,1.5,2};const XmAppearanceProfile profiles[]={XmAPPEARANCE_MODERN_LIGHT,XmAPPEARANCE_MODERN_DARK,XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT};
 for(unsigned si=0;si<3;si++)for(unsigned compact=0;compact<2;compact++){
  CHECK(XmSetDisplayScaleOverride(d,scales[si]));CHECK(XmApplyAppearanceDensity(d,compact?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
  Widget sample=gallery_create_component_example(board,xmFlyoutWidgetClass,"flyoutExample",NULL,NULL);CHECK(sample!=NULL);CHECK(!XtIsManaged(sample));
  XmFlyoutWidget fly=(XmFlyoutWidget)sample;Widget owner=XtParent(sample),trigger=XtNameToWidget(owner,"showPopup");CHECK(trigger!=NULL);CHECK(fly->flyout.owner==owner);XtAddCallback(sample,XmNflyoutDismissCallback,dismiss,NULL);
  for(unsigned pi=0;pi<4;pi++){
   CHECK(XmApplyAppearanceProfile(d,profiles[pi]));settle(app,d);XtCallCallbacks(trigger,XmNactivateCallback,NULL);settle(app,d);
   CHECK(XmFlyoutIsVisible(sample));CHECK(!XtIsManaged(sample));CHECK(fly->flyout.label!=NULL);
   Dimension ow,oh,pw,ph,mw,mh;Position ax,ay,px,py;XtTranslateCoords(owner,0,0,&ax,&ay);
   XtVaGetValues(owner,XmNwidth,&ow,XmNheight,&oh,NULL);XtVaGetValues(fly->flyout.popup_shell,XmNx,&px,XmNy,&py,XmNwidth,&pw,XmNheight,&ph,NULL);
   CHECK(ow==trigger->core.width);CHECK(oh==trigger->core.height);CHECK(px>=0&&py>=0);CHECK(px+pw<=WidthOfScreen(XtScreen(shell)));CHECK(py+ph<=HeightOfScreen(XtScreen(shell)));
   if(profiles[pi]!=XmAPPEARANCE_CLASSIC){Dimension gap;CHECK(XmGetAppearanceSpacing(d,XmAPPEARANCE_SPACING_SMALL,&gap));CHECK(px==ax);CHECK(py==ay+oh+gap);XtVaGetValues(fly->flyout.label,XmNmarginWidth,&mw,XmNmarginHeight,&mh,NULL);CHECK(mw==(Dimension)((compact?8:12)*scales[si]+.5));CHECK(mh==(Dimension)((compact?6:8)*scales[si]+.5));}
   XmFlyoutHide(sample);CHECK(!XmFlyoutIsVisible(sample));
  }
  /* At the bottom edge, Modern chooses above the owner before clamping. */
  Dimension owner_height;XtVaGetValues(owner,XmNheight,&owner_height,NULL);
  XtVaSetValues(shell,XmNy,HeightOfScreen(XtScreen(shell))-owner_height-20,NULL);
  XtCallCallbacks(trigger,XmNactivateCallback,NULL);settle(app,d);
  Position ay,ax,py;XtTranslateCoords(owner,0,0,&ax,&ay);XtVaGetValues(fly->flyout.popup_shell,XmNy,&py,NULL);CHECK(py<ay);XmFlyoutHide(sample);
  XtVaSetValues(shell,XmNy,150,NULL);gallery_destroy_component_example(&sample);XtDestroyWidget(owner);settle(app,d);
  sample=gallery_create_component_example(board,xmTeachingTipWidgetClass,"teachingExample",NULL,NULL);
  CHECK(sample!=NULL&&!XtIsManaged(sample));owner=XtParent(sample);trigger=XtNameToWidget(owner,"showPopup");CHECK(trigger!=NULL);
  XtCallCallbacks(trigger,XmNactivateCallback,NULL);settle(app,d);CHECK(XmTeachingTipIsVisible(sample));
  XmTeachingTipHide(sample);CHECK(!XmTeachingTipIsVisible(sample));gallery_destroy_component_example(&sample);XtDestroyWidget(owner);settle(app,d);
 }
 CHECK(dismissals==30);XtDestroyWidget(shell);printf("PASS gallery Flyout: %u checks; hidden handle, trigger bounds, modern padding, themes/scales/densities and edge flipping\n",checks);return 0;
}
