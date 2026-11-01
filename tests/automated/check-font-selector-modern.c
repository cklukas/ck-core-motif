/* Native regression for selected outline previews and live layout transactions. */
#include <X11/IntrinsicP.h>
#include <X11/Xft/Xft.h>
#include <X11/Xutil.h>
#include <strings.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/SegmentedControlP.h>
#include <Xm/FontSP.h>
#include <Xm/DropDownP.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
static unsigned checks, notifications;
#define CHECK(x) do { ++checks; if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(1);} }while(0)
static void changed(Widget w,XtPointer p,XtPointer data){(void)w;(void)p;CHECK(data!=NULL && *(char*)data=='-');++notifications;}
static void tick(XtPointer p,XtIntervalId *id){(void)id;*(Boolean*)p=True;}
static void settle(XtAppContext app,Display *d){Boolean done=False;XtAppAddTimeOut(app,20,tick,&done);while(!done)XtAppProcessEvent(app,XtIMAll);XSync(d,False);while(XtAppPending(app))XtAppProcessEvent(app,XtIMAll);}
static void aligned(Widget dd){Widget label=XmDropDown_label(dd),text=XmDropDown_text(dd);Dimension *a=NULL,*b=NULL;int na=0,nb=0;CHECK(XmWidgetGetBaselines(label,&a,&na));CHECK(XmWidgetGetBaselines(text,&b,&nb));CHECK(na>0&&nb>0);CHECK(abs(label->core.y+a[0]-text->core.y-b[0])<=1);CHECK(text->core.y>=0);CHECK(text->core.y+text->core.height<=dd->core.height);XtFree((char*)a);XtFree((char*)b);}
int main(int argc,char **argv){
 XtAppContext app;Widget shell=XtVaAppInitialize(&app,"FontPreviewTest",NULL,0,&argc,argv,NULL,NULL);Display *d=XtDisplay(shell);
 CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_LIGHT));
 Widget form=XtVaCreateManagedWidget("form",xmFormWidgetClass,shell,XmNwidth,1500,XmNheight,1100,NULL);
 Widget tabs=XmVaCreateManagedSegmentedControl(form,"tabs",XmNtopAttachment,XmATTACH_FORM,XmNleftAttachment,XmATTACH_FORM,NULL);
 XmSegmentedControlItem items[3];char *names[]={"Example","Source","Icons"};
 for(int i=0;i<3;i++)items[i]=(XmSegmentedControlItem){names[i],XmStringCreateLocalized(names[i]),XmUNSPECIFIED_PIXMAP,True};
 XmSegmentedControlSetItems(tabs,items,3);for(int i=0;i<3;i++)XmStringFree(items[i].label);
 Widget fonts=XtVaCreateManagedWidget("fonts",xmFontSelectorWidgetClass,form,XmNy,120,XmNcurrentFont,"-adobe-utopia-medium-r-normal--0-360-0-0-p-0-iso8859-1",NULL);
 XtAddCallback(fonts,XmNvalueChangedCallback,changed,NULL);XtRealizeWidget(shell);settle(app,d);
 const double scales[]={1,1.5,2};const XmAppearanceProfile profiles[]={XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT,XmAPPEARANCE_MODERN_DARK,XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT};
 for(unsigned si=0;si<3;si++)for(unsigned pi=0;pi<5;pi++){
  CHECK(XmSetDisplayScaleOverride(d,scales[si]));CHECK(XmApplyAppearanceProfile(d,profiles[pi]));settle(app,d);CHECK(notifications==0);
  XmSegmentedControlWidget seg=(XmSegmentedControlWidget)tabs;int edge=0;
  for(int i=0;i<3;i++){Widget button=seg->segmented.items[i].button;CHECK(button->core.x==edge);edge+=button->core.width;}
  CHECK(edge==tabs->core.width);
  XmFontSelectorWidget fs=(XmFontSelectorWidget)fonts;XmRenderTable table;XtVaGetValues(XmFontS_text(fs),XmNrenderTable,&table,NULL);XmRendition r=XmRenderTableGetRendition(table,XmFONTLIST_DEFAULT_TAG);CHECK(r!=NULL);XmFontType type;XftFont *font=NULL;Arg args[2];XtSetArg(args[0],XmNfontType,&type);XtSetArg(args[1],XmNxftFont,&font);XmRenditionRetrieve(r,args,2);
  if(profiles[pi]==XmAPPEARANCE_CLASSIC){CHECK(type==XmFONT_IS_FONT);CHECK(XmFontS_old_fontdata(fs)!=NULL);}
  else {CHECK(type==XmFONT_IS_XFT);CHECK(font!=NULL);FcChar8 *family=NULL;FcBool aa=False;double pixels=0;CHECK(FcPatternGetString(font->pattern,FC_FAMILY,0,&family)==FcResultMatch);CHECK(!strcasecmp((char*)family,"Utopia"));CHECK(FcPatternGetBool(font->pattern,FC_ANTIALIAS,0,&aa)==FcResultMatch&&aa);CHECK(FcPatternGetDouble(font->pattern,FC_PIXEL_SIZE,0,&pixels)==FcResultMatch);CHECK(fabs(pixels-48*scales[si])<.1);aligned(XmFontS_family_box(fs));aligned(XmFontS_size_box(fs));CHECK(XmFontS_old_fontdata(fs)==NULL);}
  XmRenditionFree(r);
 }
 /* Verify that changing weight/slant still selects the real outline face. */
 char font_names[4][256];
 const char *faces[]={"medium-r","bold-r","medium-i","bold-i"};
 for(unsigned i=0;i<4;i++) {
  char *name=font_names[i];snprintf(name,sizeof(font_names[i]),"-adobe-utopia-%s-normal--0-360-0-0-p-0-iso8859-1",faces[i]);
  XtVaSetValues(fonts,XmNcurrentFont,name,NULL);settle(app,d);
  XmFontSelectorWidget fs=(XmFontSelectorWidget)fonts;XmRenderTable table;
  XtVaGetValues(XmFontS_text(fs),XmNrenderTable,&table,NULL);
  XmRendition r=XmRenderTableGetRendition(table,XmFONTLIST_DEFAULT_TAG);CHECK(r!=NULL);
  XftFont *font=NULL;Arg arg;XtSetArg(arg,XmNxftFont,&font);XmRenditionRetrieve(r,&arg,1);CHECK(font!=NULL);
  FcChar8 *family=NULL;int weight=0,slant=0;
  CHECK(FcPatternGetString(font->pattern,FC_FAMILY,0,&family)==FcResultMatch);
  CHECK(!strcasecmp((char*)family,"Utopia"));
  CHECK(FcPatternGetInteger(font->pattern,FC_WEIGHT,0,&weight)==FcResultMatch);
  CHECK(FcPatternGetInteger(font->pattern,FC_SLANT,0,&slant)==FcResultMatch);
  CHECK((weight>=FC_WEIGHT_DEMIBOLD)==(i%2==1));CHECK((slant!=FC_SLANT_ROMAN)==(i>=2));
  XmRenditionFree(r);
 }
 Widget text=XmFontS_text((XmFontSelectorWidget)fonts);
 XImage *image=XGetImage(d,XtWindow(text),0,0,text->core.width,text->core.height,AllPlanes,ZPixmap);CHECK(image!=NULL);
 unsigned long colors[16];unsigned count=0;
 for(unsigned y=0;y<text->core.height&&count<16;y++)for(unsigned x=0;x<text->core.width&&count<16;x++){
  unsigned long pixel=XGetPixel(image,x,y);unsigned j=0;while(j<count&&colors[j]!=pixel)j++;
  if(j==count)colors[count++]=pixel;
 }
 CHECK(count>8);XDestroyImage(image);
 XtDestroyWidget(shell);printf("PASS modern FontSelector: %u checks, selected Utopia outlines, live themes/scales, quiet callbacks, baseline alignment, complete segments\n",checks);return 0;
}
