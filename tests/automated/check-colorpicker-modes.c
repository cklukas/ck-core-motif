/* Native color modes, conversion vectors, input and colormap ownership. */
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/XmAll.h>
#include <Xm/ColorPickerP.h>
#include <Xm/SegmentedControl.h>
#include <Xm/Accessible.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include "ColorSpaceI.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static unsigned previews,commits;
static void preview(Widget w,XtPointer a,XtPointer b){(void)w;(void)a;(void)b;++previews;}
static void commit_cb(Widget w,XtPointer a,XtPointer b){(void)w;(void)a;(void)b;++commits;}
static void tick(XtPointer data,XtIntervalId *id){(void)id;*(Boolean*)data=True;}
static void settle(XtAppContext app,Display *d){Boolean done=False;XtAppAddTimeOut(app,20,tick,&done);while(!done)XtAppProcessEvent(app,XtIMAll);XSync(d,False);unsigned n=0;while(XtAppPending(app)){CHECK(++n<30000);XtAppProcessEvent(app,XtIMAll);XSync(d,False);}}
static void key(Widget w,KeySym sym){XEvent e={0};e.xkey.type=KeyPress;e.xkey.display=XtDisplay(w);e.xkey.window=XtWindow(w);e.xkey.keycode=XKeysymToKeycode(XtDisplay(w),sym);e.xkey.same_screen=True;XtDispatchEvent(&e);}
static void click(Widget w,int type,int x,int y){static Time t=10000;XEvent e={0};e.xbutton.type=type;e.xbutton.display=XtDisplay(w);e.xbutton.window=XtWindow(w);e.xbutton.root=RootWindowOfScreen(XtScreen(w));Position rx,ry;XtTranslateCoords(w,x,y,&rx,&ry);e.xbutton.x_root=rx;e.xbutton.y_root=ry;e.xbutton.x=x;e.xbutton.y=y;e.xbutton.button=Button1;e.xbutton.time=(t+=100);e.xbutton.same_screen=True;XtDispatchEvent(&e);}
static void contained(Widget w){XmColorPickerWidget p=(XmColorPickerWidget)w;for(unsigned i=0;i<p->composite.num_children;++i){Widget c=p->composite.children[i];if(!XtIsManaged(c)||!c->core.mapped_when_managed)continue;CHECK(c->core.x>=0 && c->core.y>=0);CHECK(c->core.x+c->core.width<=w->core.width && c->core.y+c->core.height<=w->core.height);}}
static void destroyed(Widget w,XtPointer a,XtPointer b){(void)a;(void)b;XtDestroyWidget(w);}
static void vectors(void){
 double hsv[3]={0},lab[3],rgb[3];unsigned cases=0;
 for(unsigned r=0;r<256;r+=17)for(unsigned g=0;g<256;g+=17)for(unsigned b=0;b<256;b+=17){uint32_t v=(r<<24)|(g<<16)|(b<<8)|0x57;color_to_lab(v,lab);CHECK(color_from_lab(lab,rgb));uint32_t next=color_pack(rgb,0x57);CHECK(next==v);color_to_hsv(v,hsv);color_from_hsv(hsv,rgb);CHECK(color_pack(rgb,0x57)==v);++cases;}
 color_to_lab(0xff0000ff,lab);CHECK(fabs(lab[0]-54.29)<.02 && fabs(lab[1]-80.81)<.02 && fabs(lab[2]-69.89)<.02);
 color_to_lab(0xffffffff,lab);CHECK(fabs(lab[0]-100)<.001 && fabs(lab[1])<.001 && fabs(lab[2])<.001);
 double outside[]={50,127,127};CHECK(!color_from_lab(outside,rgb));
 printf("PASS %u sRGB/Lab/HSV round-trip vectors and D50 references\n",cases);
}
int main(int argc,char **argv){vectors();XtAppContext app;XtToolkitInitialize();app=XtCreateApplicationContext();Display *d=XtOpenDisplay(app,NULL,"colorModes","ColorModes",NULL,0,&argc,argv);CHECK(d);unsigned cases=0;const double scales[]={1,1.5,2};
/* Instrumented lifetime runs cover each profile once; ordinary runs retain
 * the complete DPI/RTL pixel and geometry matrix. */
int memory=getenv("CKMOTIF_COLOR_MODES_MEMORY")!=NULL;
for(int profile=0;profile<3;++profile)for(int scale=0;scale<(memory?1:3);++scale)for(int rtl=0;rtl<(memory?1:2);++rtl){CHECK(XmSetDisplayScaleOverride(d,scales[scale]));CHECK(XmApplyAppearanceProfile(d,profile==0?XmAPPEARANCE_CLASSIC:profile==1?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));
 Widget shell=XtAppCreateShell("colorModes","ColorModes",applicationShellWidgetClass,d,NULL,0);Widget board=XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,XmNwidth,1600,XmNheight,1200,XmNresizePolicy,XmRESIZE_NONE,NULL);
 Widget w=XtVaCreateManagedWidget("picker",xmColorPickerWidgetClass,board,XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);XmColorPickerWidget p=(XmColorPickerWidget)w;
 XtAddCallback(w,XmNcolorPickerPreviewCallback,preview,NULL);XtAddCallback(w,XmNcolorPickerValueChangedCallback,commit_cb,NULL);XtRealizeWidget(shell);settle(app,d);CHECK(XmColorPickerGetMode(w)==XmCOLOR_PICKER_HSV);
 WidgetList tabs;Cardinal tab_count;XtVaGetValues(p->color_picker.modes,XmNchildren,&tabs,XmNnumChildren,&tab_count,NULL);CHECK(tab_count==4);
 for(int repeat=0;repeat<2;++repeat){Widget tab=tabs[1];click(tab,ButtonPress,tab->core.width/2,tab->core.height/2);click(tab,ButtonRelease,tab->core.width/2,tab->core.height/2);settle(app,d);CHECK(XmToggleButtonGetState(tab));CHECK(XmSegmentedControlGetSelectedIndex(p->color_picker.modes)==1);}
 XmColorPickerSetValue(w,0x3478d480,False);unsigned pc=previews,cc=commits;
 for(int mode=0;mode<4;++mode){CHECK(XmColorPickerSetMode(w,mode));settle(app,d);CHECK(XmColorPickerGetPreviewValue(w)==0x3478d480 && XmColorPickerGetValue(w)==0x3478d480);CHECK(previews==pc && commits==cc);CHECK(XmSegmentedControlGetSelectedIndex(p->color_picker.modes)==mode);contained(w);}
 CHECK(!XmColorPickerSetMode(w,99));CHECK(!XmColorPickerPreviewHSV(w,NAN,1,1));CHECK(!XmColorPickerPreviewLab(w,50,128,0));
 CHECK(XmColorPickerSetMode(w,XmCOLOR_PICKER_HSV));settle(app,d);CHECK(XmColorPickerPreviewHSV(w,120,1,1));CHECK(XmColorPickerGetPreviewValue(w)==0x00ff0080 && XmColorPickerGetValue(w)==0x3478d480);CHECK(XmColorPickerPreviewHSV(w,240,1,0));CHECK(p->color_picker.hsv[0]==240 && p->color_picker.hsv[1]==1);
 XmScaleSetValue(p->color_picker.channels[3],42);XmScaleCallbackStruct cb={XmCR_VALUE_CHANGED,NULL,42};XtCallCallbacks(p->color_picker.channels[3],XmNvalueChangedCallback,&cb);CHECK(p->color_picker.hsv[0]==240 && p->color_picker.hsv[1]==1);CHECK(XmColorPickerPreviewHSV(w,240,1,1));CHECK(XmColorPickerGetPreviewValue(w)==0x0000ff2a);
 Widget plane=p->color_picker.plane;click(plane,ButtonPress,0,0);click(plane,ButtonRelease,0,0);CHECK(XmColorPickerGetPreviewValue(w)==0xffffff2a);key(plane,XK_Right);CHECK(p->color_picker.hsv[1]>.009);unsigned long before=XmColorPickerGetPreviewValue(w);XtSetSensitive(w,False);key(plane,XK_Left);CHECK(XmColorPickerGetPreviewValue(w)==before);XtSetSensitive(w,True);key(plane,XK_Return);CHECK(XmColorPickerGetValue(w)==before);
 click(plane,ButtonPress,0,0);unsigned long held=XmColorPickerGetPreviewValue(w);XtSetSensitive(w,False);XtSetSensitive(w,True);click(plane,ButtonRelease,plane->core.width-1,plane->core.height-1);CHECK(XmColorPickerGetPreviewValue(w)==held);XmColorPickerRevert(w);
 CHECK(XmColorPickerSetMode(w,XmCOLOR_PICKER_LAB));settle(app,d);CHECK(XmColorPickerPreviewLab(w,50,0,0));CHECK(p->color_picker.gamut);CHECK(XmColorPickerPreviewLab(w,50,127,127));CHECK(!p->color_picker.gamut && p->color_picker.lab[1]==127);settle(app,d);contained(w);CHECK((XmColorPickerGetPreviewValue(w)&255)==42);
 XmAccessibility *a=XmAccessibilityCreate(shell,"Color modes");CHECK(a);XmAccessibleId id=XmAccessibilityWidget(a,p->color_picker.mode_channels[1]);XmAccessibleInfo info;CHECK(XmAccessibilityQuery(a,id,&info));CHECK(info.role==51 && !strcmp(info.name,"a star") && info.minimum_value==-128 && info.maximum_value==127);XmAccessibleInfoClear(&info);CHECK(XmAccessibilitySetValue(a,id,-20.5));CHECK(fabs(p->color_picker.lab[1]+20.5)<1e-6);CHECK(!XmAccessibilitySetValue(a,id,128));XmAccessibilityDestroy(a);
 CHECK(XmColorPickerSetMode(w,XmCOLOR_PICKER_PENCILS));settle(app,d);Widget pencil=p->color_picker.pencils[6];click(pencil,ButtonPress,pencil->core.width/2,pencil->core.height/2);click(pencil,ButtonRelease,pencil->core.width/2,pencil->core.height/2);if(XmColorPickerGetPreviewValue(w)!=0xe33b372a)fprintf(stderr,"pencil preview %08lx bounds %dx%d sensitive%d\n",XmColorPickerGetPreviewValue(w),pencil->core.width,pencil->core.height,XtIsSensitive(pencil));CHECK(XmColorPickerGetPreviewValue(w)==0xe33b372a);a=XmAccessibilityCreate(shell,"Pencils");CHECK(a);id=XmAccessibilityWidget(a,pencil);CHECK(XmAccessibilityQuery(a,id,&info));CHECK(info.role==XmACCESSIBLE_BUTTON && !strcmp(info.name,"Red"));XmAccessibleInfoClear(&info);CHECK(XmAccessibilityAction(a,id,0));XmAccessibilityDestroy(a);key(pencil,XK_Escape);CHECK(XmColorPickerGetPreviewValue(w)==before);contained(w);
 for(int mode=0;mode<4;++mode){XmColorPickerSetMode(w,mode);XtVaSetValues(w,XmNwidth,1,XmNheight,1,NULL);settle(app,d);contained(w);XtVaSetValues(w,XmNwidth,0,XmNheight,0,NULL);settle(app,d);contained(w);}
 XtAddCallback(w,XmNcolorPickerPreviewCallback,destroyed,NULL);CHECK(XmColorPickerPreviewLab(w,20,0,0));settle(app,d);XtDestroyWidget(shell);settle(app,d);++cases;
}
printf("PASS colorpicker modes: %u configurations, four modes each\n",cases);XtCloseDisplay(d);XtDestroyApplicationContext(app);return 0;}
