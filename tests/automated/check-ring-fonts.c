/* Standard font aliases, live appearance/DPI and natural geometry ownership. */
#include <X11/IntrinsicP.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdlib.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static unsigned cases;
static XmFontList fail_copy_source;
static unsigned copy_failures;
XmFontList XmFontListCopy(XmFontList source){
 static XmFontList (*copy)(XmFontList);
 if(source&&source==fail_copy_source){fail_copy_source=NULL;++copy_failures;return NULL;}
 if(!copy)copy=(XmFontList(*)(XmFontList))dlsym(RTLD_NEXT,"XmFontListCopy");
 CHECK(copy);return copy(source);
}
static XmRenderTable table(Widget w){XmRenderTable rt=NULL,alias=NULL;XtVaGetValues(w,XmNrenderTable,&rt,XmNfontList,&alias,NULL);CHECK(rt&&alias==rt);return rt;}
static void metrics(Widget w,Dimension *width,Dimension *height){XmString s=XmStringCreateLocalized("Progress 100%");XmStringExtent(table(w),s,width,height);XmStringFree(s);}
static void samefont(Widget a,Widget b){Dimension aw,ah,bw,bh;metrics(a,&aw,&ah);metrics(b,&bw,&bh);CHECK(aw==bw&&ah==bh);++cases;}
static void drain(XtAppContext app,Display*d){XSync(d,False);while(XtAppPending(app)){XtAppProcessEvent(app,XtIMAll);XSync(d,False);}}
int main(int argc,char **argv){
 XtAppContext app;Widget shell=XtVaAppInitialize(&app,"RingFonts",NULL,0,&argc,argv,NULL,NULL);Display*d=XtDisplay(shell);
 CHECK(XmSetDisplayScaleOverride(d,1));
 Widget board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,1500,XmNheight,1100,NULL);
 Widget reference=XtVaCreateManagedWidget("reference",xmLabelWidgetClass,board,XmNx,700,XmNy,20,NULL);
 WidgetClass classes[]={xmProgressRingWidgetClass,xmSpinnerWidgetClass};
 Widget automatic[2],fixed[2],custom[2],labelled[2];Dimension classic_w[2],classic_h[2];
 XmString short_label=XmStringCreateLocalized("Busy");
 for(unsigned i=0;i<2;++i){
  automatic[i]=XtVaCreateManagedWidget("automatic",classes[i],board,XmNx,20+i*330,XmNy,20,NULL);
  metrics(automatic[i],&classic_w[i],&classic_h[i]);
  XmRenderTable copy=XmRenderTableCopy(table(automatic[i]),NULL,0);CHECK(copy);
  custom[i]=XtVaCreateManagedWidget("custom",classes[i],board,XmNx,20+i*330,XmNy,220,XmNfontList,copy,NULL);
  XmRenderTableFree(copy);
  fixed[i]=XtVaCreateManagedWidget("fixed",classes[i],board,XmNx,20+i*330,XmNy,420,XmNwidth,113,XmNheight,57,NULL);
  labelled[i]=XtVaCreateManagedWidget("labelled",classes[i],board,XmNx,20+i*330,XmNy,620,
   i?XmNspinnerLabelString:XmNprogressLabelString,short_label,NULL);
 }
 XmStringFree(short_label);XtRealizeWidget(shell);drain(app,d);
 const double scales[]={1,1.5,2,1};
 for(unsigned theme=0;theme<2;++theme)for(unsigned scale=0;scale<4;++scale){
  CHECK(XmApplyAppearanceProfile(d,theme?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
  CHECK(XmSetDisplayScaleOverride(d,scales[scale]));drain(app,d);
  for(unsigned i=0;i<2;++i){
   samefont(automatic[i],reference);samefont(fixed[i],reference);samefont(labelled[i],reference);
   Dimension cw,ch;metrics(custom[i],&cw,&ch);CHECK(cw==classic_w[i]&&ch==classic_h[i]);
   CHECK(XtWidth(fixed[i])==113&&XtHeight(fixed[i])==57);
   Widget fresh=XtVaCreateManagedWidget("fresh",classes[i],board,XmNx,800,XmNy,200+i*350,NULL);drain(app,d);
   samefont(fresh,automatic[i]);
   if(XtWidth(fresh)!=XtWidth(automatic[i])||XtHeight(fresh)!=XtHeight(automatic[i]))
    fprintf(stderr,"geometry type=%u scale=%g live=%ux%u fresh=%ux%u\n",i,scales[scale],XtWidth(automatic[i]),XtHeight(automatic[i]),XtWidth(fresh),XtHeight(fresh));
   CHECK(XtWidth(fresh)==XtWidth(automatic[i])&&XtHeight(fresh)==XtHeight(automatic[i]));
   CHECK(XtWidth(automatic[i])==XtHeight(automatic[i]));
   XtDestroyWidget(fresh);++cases;
  }
 }
 /* A larger role changes fonts/geometry through the same resources. */
 XtVaSetValues(reference,XmNappearanceRole,XmAPPEARANCE_ROLE_PAGE_TITLE,NULL);drain(app,d);
 for(unsigned i=0;i<2;++i){
  XtVaSetValues(automatic[i],XmNappearanceRole,XmAPPEARANCE_ROLE_PAGE_TITLE,NULL);drain(app,d);samefont(automatic[i],reference);
  Dimension before=XtWidth(labelled[i]);XmString long_label=XmStringCreateLocalized("A longer operation label that needs more room");
  XtVaSetValues(labelled[i],i?XmNspinnerLabelString:XmNprogressLabelString,long_label,NULL);XmStringFree(long_label);drain(app,d);
  CHECK(XtWidth(labelled[i])>before);
  /* A failed font copy must retain the old owned table and its geometry. */
  XmRenderTable old_table=table(fixed[i]);
  XmRenderTable candidate=XmRenderTableCopy(table(reference),NULL,0);CHECK(candidate);
  unsigned failed_before=copy_failures;fail_copy_source=candidate;
  XtVaSetValues(fixed[i],XmNrenderTable,candidate,NULL);
  CHECK(copy_failures==failed_before+1&&fail_copy_source==NULL);
  CHECK(table(fixed[i])==old_table);
  CHECK(XtWidth(fixed[i])==113&&XtHeight(fixed[i])==57);
  XmRenderTableFree(candidate);++cases;
  /* Both aliases copy caller storage, including replacement and clearing. */
  XmRenderTable copy=XmRenderTableCopy(table(reference),NULL,0);CHECK(copy);
  XtVaSetValues(custom[i],XmNrenderTable,copy,NULL);XmRenderTableFree(copy);samefont(custom[i],reference);
  Dimension override_w,override_h;metrics(custom[i],&override_w,&override_h);
  CHECK(XmSetDisplayScaleOverride(d,1.5));drain(app,d);
  Dimension cw,ch;metrics(custom[i],&cw,&ch);CHECK(cw==override_w&&ch==override_h);
  XtVaSetValues(custom[i],XmNfontList,NULL,NULL);CHECK(table(custom[i]));
  CHECK(XmSetDisplayScaleOverride(d,1));drain(app,d);++cases;
 }
 CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(app,d);
 for(unsigned i=0;i<2;++i){Dimension cw,ch;metrics(labelled[i],&cw,&ch);CHECK(cw==classic_w[i]&&ch==classic_h[i]);}
 XtDestroyWidget(shell);XtCloseDisplay(d);XtDestroyApplicationContext(app);
 printf("PASS ring fonts: %u font/geometry cases\n",cases);return 0;
}
