/* Native acceptance using the gallery factory and real cascade state handlers. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <Xm/CascadeBP.h>
#include <Xm/CascadeBGP.h>
#include "native-window.h"
extern Widget gallery_create_component_example(Widget,WidgetClass,const char *,XtCallbackProc,XtPointer);
#define CHECK(c) do{if(!(c)){fprintf(stderr,"FAIL %d kind=%u dark=%u scale=%u: %s\n",__LINE__,kind,dark,scale,#c);exit(1);}}while(0)
static unsigned kind,dark,scale,cases,activated;
static void tick(XtPointer p,XtIntervalId *id){*(Boolean*)p=True;(void)id;}
static void settle(XtAppContext a,Display*d){for(unsigned i=0;i<3;++i){Boolean done=False;XtAppAddTimeOut(a,20,tick,&done);while(!done)XtAppProcessEvent(a,XtIMAll);XSync(d,False);while(XtAppPending(a))XtAppProcessEvent(a,XtIMAll);}}
static void called(Widget w,XtPointer a,XtPointer b){(void)w;(void)a;(void)b;++activated;}
static unsigned long pixel(Widget w,int x,int y){Widget window=XtIsWidget(w)?w:XtParent(w);Position px=0,py=0;if(!XtIsWidget(w))XtVaGetValues(w,XmNx,&px,XmNy,&py,NULL);XImage*im=XGetImage(XtDisplay(window),XtWindow(window),px+x,py+y,1,1,AllPlanes,ZPixmap);CHECK(im);unsigned long p=XGetPixel(im,0,0);XDestroyImage(im);return p;}
static void arm(Widget w,Boolean armed){if(XtIsWidget(w)){XmPrimitiveWidgetClass c=(XmPrimitiveWidgetClass)XtClass(w);(armed?c->primitive_class.border_highlight:c->primitive_class.border_unhighlight)(w);}else{XmGadgetClass c=(XmGadgetClass)XtClass(w);(armed?c->gadget_class.border_highlight:c->gadget_class.border_unhighlight)(w);}}
int main(int argc,char**argv){XtAppContext a;Widget shell=XtVaAppInitialize(&a,"CascadeAppearance",NULL,0,&argc,argv,NULL,NULL);Display*d=XtDisplay(shell);Widget host=XtVaCreateManagedWidget("host",xmDrawingAreaWidgetClass,shell,XmNwidth,900,XmNheight,600,XmNresizePolicy,XmRESIZE_NONE,NULL);XtRealizeWidget(shell);NativeWaitForViewable(host);const double scales[]={1,1.5,2};
for(kind=0;kind<2;++kind)for(dark=0;dark<2;++dark)for(scale=0;scale<3;++scale){CHECK(XmSetDisplayScaleOverride(d,scales[scale]));CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMFORTABLE));Widget button=gallery_create_component_example(host,kind?xmCascadeButtonGadgetClass:xmCascadeButtonWidgetClass,"cascade",called,NULL);Widget bar=XtParent(button),menu=NULL;unsigned char type;XtVaGetValues(bar,XmNrowColumnType,&type,NULL);CHECK(XmIsRowColumn(bar)&&type==XmMENU_BAR);XtVaGetValues(button,XmNsubMenuId,&menu,NULL);CHECK(menu);NativeWaitForViewable(bar);if(XtIsWidget(button))NativeWaitForViewable(button);settle(a,d);
Dimension width,height,mw,mh,shadow,tw,th;Pixel bg,fg;XmRenderTable font=NULL;XmString text=XmStringCreateLocalized("File");XtVaGetValues(button,XmNwidth,&width,XmNheight,&height,XmNmarginWidth,&mw,XmNmarginHeight,&mh,XmNshadowThickness,&shadow,XmNbackground,&bg,XmNforeground,&fg,XmNrenderTable,&font,NULL);CHECK(font);XmStringExtent(font,text,&tw,&th);XmStringFree(text);CHECK(tw>=24*scales[scale]&&th>=18*scales[scale]);CHECK(mw==(Dimension)(12*scales[scale])&&mh==(Dimension)(6*scales[scale]));CHECK(shadow==0);CHECK(width>=tw+2*mw&&height>=th+2*mh);CHECK(pixel(button,width/2,5)==bg);++cases;
arm(button,True);settle(a,d);Pixel selected=pixel(button,width/2,5);CHECK(selected!=bg);CHECK(selected==(dark?0x3c4043:0xdbeafe));++cases;
/* Full expose and state transitions retain a single flat selection surface. */
XtExposeProc expose=XtIsWidget(button)?XtClass(button)->core_class.expose:((RectObjClass)XtClass(button))->rect_class.expose;expose(button,NULL,NULL);settle(a,d);CHECK(pixel(button,width/2,5)==selected);arm(button,False);settle(a,d);CHECK(pixel(button,width/2,5)==bg);++cases;
CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMPACT));settle(a,d);XtVaGetValues(button,XmNmarginWidth,&mw,XmNmarginHeight,&mh,NULL);CHECK(mw==(Dimension)(8*scales[scale])&&mh==(Dimension)(3*scales[scale]+0.5));++cases;
/* Explicit colors and metrics stay authoritative across palette changes. */
XtVaSetValues(button,XmNbackground,(Pixel)0x234567,XmNforeground,(Pixel)0xfedcba,XmNmarginWidth,7,XmNshadowThickness,2,NULL);CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));settle(a,d);XtVaGetValues(button,XmNbackground,&bg,XmNforeground,&fg,XmNmarginWidth,&mw,XmNshadowThickness,&shadow,NULL);CHECK(bg==0x234567&&fg==0xfedcba&&mw==7&&shadow==2);++cases;
/* Toolkit's submenu callbacks are still connected to every real menu action. */
WidgetList items;Cardinal count;XtVaGetValues(menu,XmNchildren,&items,XmNnumChildren,&count,NULL);CHECK(count==3);for(Cardinal i=0;i<count;++i)XtCallCallbacks(items[i],XmNactivateCallback,NULL);CHECK(activated==(kind*6+dark*3+scale+1)*3);++cases;
XtDestroyWidget(bar);settle(a,d);
}
/* Returning to Classic restores the original font and geometry defaults. */
CHECK(XmSetDisplayScaleOverride(d,1));
for(kind=0;kind<2;++kind){CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));Widget b=gallery_create_component_example(host,kind?xmCascadeButtonGadgetClass:xmCascadeButtonWidgetClass,"classic",NULL,NULL);Dimension old_shadow,old_margin,old_tw,old_th,tw,th,shadow,margin;XmRenderTable font;XmString text=XmStringCreateLocalized("File");XtVaGetValues(b,XmNshadowThickness,&old_shadow,XmNmarginWidth,&old_margin,XmNrenderTable,&font,NULL);XmStringExtent(font,text,&old_tw,&old_th);CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_LIGHT));CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));settle(a,d);XtVaGetValues(b,XmNshadowThickness,&shadow,XmNmarginWidth,&margin,XmNrenderTable,&font,NULL);XmStringExtent(font,text,&tw,&th);CHECK(shadow==old_shadow&&margin==old_margin&&tw==old_tw&&th==old_th);XmStringFree(text);XtDestroyWidget(XtParent(b));++cases;}
CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMPACT));
/* Revisit the preview repeatedly, including recycled gadget addresses. */
CHECK(XmSetDisplayScaleOverride(d,1));CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_LIGHT));for(unsigned i=0;i<40;++i){Widget b=gallery_create_component_example(host,xmCascadeButtonGadgetClass,"revisited",NULL,NULL);settle(a,d);Pixel bg;Dimension mw;XtVaGetValues(b,XmNbackground,&bg,XmNmarginWidth,&mw,NULL);CHECK(bg==0xf1f2f4&&mw==8);arm(b,True);settle(a,d);arm(b,False);XtDestroyWidget(XtParent(b));++cases;}
XtDestroyWidget(shell);XtCloseDisplay(d);XtDestroyApplicationContext(a);printf("PASS Cascade appearance: %u native cases (widget/gadget, palette, scale, state, ownership, callbacks and revisits)\n",cases);return 0;}
