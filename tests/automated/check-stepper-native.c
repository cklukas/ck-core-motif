/* Native Stepper layout and momentary input across profiles, scale and RTL. */
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/XmAll.h>
#include <Xm/Stepper.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static unsigned changes,after;
static void changed(Widget w,XtPointer closure,XtPointer data){(void)w;(void)closure;XmStepperCallbackStruct *c=data;CHECK(c && c->reason==XmCR_VALUE_CHANGED);++changes;}
static void destroy(Widget w,XtPointer closure,XtPointer data){(void)closure;(void)data;XtDestroyWidget(w);}
static void observer(Widget w,XtPointer closure,XtPointer data){(void)w;(void)closure;(void)data;++after;}
static void tick(XtPointer data,XtIntervalId *id){*(Boolean*)data=True;(void)id;}
static void settle(XtAppContext app,Display*d){Boolean done=False;XtAppAddTimeOut(app,10,tick,&done);for(unsigned i=0;!done;++i){CHECK(i<10000);XtAppProcessEvent(app,XtIMAll);}XSync(d,False);for(unsigned i=0;XtAppPending(app);++i){CHECK(i<10000);XtAppProcessEvent(app,XtIMAll);XSync(d,False);}}
static void button(Widget w,int type,Boolean inside){static Time time=1000;XEvent e={0};Position rx,ry;int x=inside?w->core.width/2:-10,y=inside?w->core.height/2:-10;XtTranslateCoords(w,x,y,&rx,&ry);e.xbutton.type=type;e.xbutton.display=XtDisplay(w);e.xbutton.window=XtWindow(w);e.xbutton.root=RootWindowOfScreen(XtScreen(w));e.xbutton.x=x;e.xbutton.y=y;e.xbutton.x_root=rx;e.xbutton.y_root=ry;e.xbutton.button=Button1;e.xbutton.same_screen=True;e.xbutton.state=type==ButtonRelease?Button1Mask:0;e.xbutton.time=(time+=1000);XtDispatchEvent(&e);}
static void key(Widget w,KeySym sym){XEvent e={0};e.xkey.type=KeyPress;e.xkey.display=XtDisplay(w);e.xkey.window=XtWindow(w);e.xkey.root=RootWindowOfScreen(XtScreen(w));e.xkey.same_screen=True;e.xkey.keycode=XKeysymToKeycode(XtDisplay(w),sym);XtDispatchEvent(&e);}
static void contained(Widget s){const char *names[]={"decrement","value","increment"};for(unsigned i=0;i<3;++i){Widget c=XtNameToWidget(s,(char*)names[i]);CHECK(c);if(!c->core.mapped_when_managed)continue;CHECK(c->core.x>=0 && c->core.y>=0);CHECK(c->core.x+c->core.width<=s->core.width);CHECK(c->core.y+c->core.height<=s->core.height);}}
static void natural(Widget s,Boolean rtl){XtWidgetGeometry g={0};for(unsigned attempt=0;attempt<100;++attempt){XtQueryGeometry(s,NULL,&g);if(s->core.width==g.width && s->core.height==g.height)break;settle(XtWidgetToApplicationContext(s),XtDisplay(s));}if(s->core.width!=g.width || s->core.height!=g.height)fprintf(stderr,"actual %ux%u preferred %ux%u\n",s->core.width,s->core.height,g.width,g.height);CHECK(s->core.width==g.width && s->core.height==g.height);contained(s);Widget dec=XtNameToWidget(s,"decrement"),val=XtNameToWidget(s,"value"),inc=XtNameToWidget(s,"increment");CHECK(rtl?inc->core.x+inc->core.width<=val->core.x:dec->core.x+dec->core.width<=val->core.x);CHECK(rtl?val->core.x+val->core.width<=dec->core.x:val->core.x+val->core.width<=inc->core.x);XmRenderTable font;XtVaGetValues(val,XmNrenderTable,&font,NULL);int low,high;XmStepperGetRange(s,&low,&high);int values[]={low,high,0};for(unsigned i=0;i<3;++i){char text[32];snprintf(text,sizeof(text),"%d",values[i]);XmString string=XmStringCreateLocalized(text);Dimension w,h;XmStringExtent(font,string,&w,&h);XmStringFree(string);CHECK(val->core.width>=w && val->core.height>=h);}}
int main(int argc,char **argv){XtAppContext app;Display*d;unsigned cases=0;const double scales[]={1,1.5,2,3};XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"stepperNative","StepperNative",NULL,0,&argc,argv);CHECK(d);
for(unsigned manager=0;manager<2;++manager)for(unsigned profile=0;profile<3;++profile)for(unsigned scale=0;scale<4;++scale)for(unsigned rtl=0;rtl<2;++rtl){
CHECK(XmSetDisplayScaleOverride(d,scales[scale]));CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMFORTABLE));CHECK(XmApplyAppearanceProfile(d,profile==0?XmAPPEARANCE_CLASSIC:profile==1?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));
Widget shell=XtAppCreateShell("stepperNative","StepperNative",applicationShellWidgetClass,d,NULL,0);XtVaSetValues(shell,XmNallowShellResize,True,NULL);Widget board=manager?XtVaCreateManagedWidget("board",xmRowColumnWidgetClass,shell,XmNorientation,XmVERTICAL,XmNadjustLast,False,NULL):XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,XmNwidth,1400,XmNheight,700,XmNresizePolicy,XmRESIZE_NONE,NULL);Widget s=XmVaCreateManagedStepper(board,"stepper",XmNstepperMinimum,-100,XmNstepperMaximum,100,XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);XtAddCallback(s,XmNstepperValueChangedCallback,changed,NULL);XtRealizeWidget(shell);settle(app,d);natural(s,rtl);
Widget dec=XtNameToWidget(s,"decrement"),inc=XtNameToWidget(s,"increment"),val=XtNameToWidget(s,"value");unsigned before=changes;Dimension width=s->core.width;
button(val,ButtonPress,True);button(val,ButtonRelease,True);CHECK(XmStepperGetValue(s)==0 && changes==before);
button(inc,ButtonPress,True);CHECK(XmStepperGetValue(s)==0);button(inc,ButtonRelease,True);CHECK(XmStepperGetValue(s)==1 && changes==before+1);
button(dec,ButtonPress,True);button(dec,ButtonRelease,False);CHECK(XmStepperGetValue(s)==1);
button(dec,ButtonPress,True);button(dec,ButtonRelease,True);CHECK(XmStepperGetValue(s)==0);
key(inc,XK_Up);CHECK(XmStepperGetValue(s)==1);key(dec,XK_Left);CHECK(XmStepperGetValue(s)==0);settle(app,d);CHECK(s->core.width==width);
XtSetSensitive(s,False);button(inc,ButtonPress,True);button(inc,ButtonRelease,True);key(inc,XK_Up);CHECK(XmStepperGetValue(s)==0);XtSetSensitive(s,True);
button(inc,ButtonPress,True);XtSetSensitive(s,False);XtSetSensitive(s,True);button(inc,ButtonRelease,True);CHECK(XmStepperGetValue(s)==0);
XmStepperSetValue(s,100,False);CHECK(!XtIsSensitive(inc) && XtIsSensitive(dec));XmStepperSetValue(s,-100,False);CHECK(!XtIsSensitive(dec) && XtIsSensitive(inc));XtVaSetValues(s,XmNstepperWrap,True,NULL);CHECK(XtIsSensitive(dec));button(dec,ButtonPress,True);button(dec,ButtonRelease,True);CHECK(XmStepperGetValue(s)==100);
XmStepperSetRange(s,INT_MIN,INT_MAX,False);XtVaSetValues(s,XmNstepperStep,INT_MAX,NULL);XmStepperSetValue(s,INT_MAX,False);XmStepperStep(s,1,False);CHECK(XmStepperGetValue(s)==INT_MIN);XmStepperStep(s,-1,False);CHECK(XmStepperGetValue(s)==INT_MAX);XtVaSetValues(s,XmNstepperStep,1,NULL);settle(app,d);natural(s,rtl);width=s->core.width;XmStepperSetValue(s,0,False);settle(app,d);CHECK(s->core.width==width);
CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMPACT));settle(app,d);natural(s,rtl);CHECK(XmSetDisplayScaleOverride(d,scales[(scale+1)%4]));settle(app,d);natural(s,rtl);
XFontStruct *fixed=XLoadQueryFont(d,"fixed");CHECK(fixed);XmRenderTable custom=XmFontListCreate(fixed,XmFONTLIST_DEFAULT_TAG);CHECK(custom);
XtVaSetValues(val,XmNrenderTable,custom,NULL);settle(app,d);natural(s,rtl);
XmRenderTable parent,actual;XtVaGetValues(s,XmNrenderTable,&parent,NULL);XmRenderTable copy=XmRenderTableCopy(parent,NULL,0);CHECK(copy);XtVaSetValues(s,XmNrenderTable,copy,NULL);XmRenderTableFree(copy);settle(app,d);natural(s,rtl);
XtVaGetValues(val,XmNrenderTable,&actual,NULL);XmString sample=XmStringCreateLocalized("1234567890");Dimension cw,ch,aw,ah;XmStringExtent(custom,sample,&cw,&ch);XmStringExtent(actual,sample,&aw,&ah);CHECK(cw==aw && ch==ah);XmStringFree(sample);
XtVaSetValues(s,XmNwidth,220,NULL);settle(app,d);CHECK(s->core.width==220);contained(s);XmStepperSetRange(s,-10,10,False);settle(app,d);CHECK(s->core.width==220);XtVaSetValues(s,XmNheight,80,NULL);settle(app,d);CHECK(s->core.height==80 && s->core.width==220);XtVaSetValues(s,XmNwidth,0,XmNheight,0,NULL);settle(app,d);natural(s,rtl);
XtVaSetValues(s,XmNwidth,1,XmNheight,1,NULL);settle(app,d);contained(s);XtVaSetValues(s,XmNwidth,0,XmNheight,0,NULL);settle(app,d);
XmStepperSetValue(s,0,False);XtAddCallback(s,XmNstepperValueChangedCallback,destroy,NULL);XtAddCallback(s,XmNstepperValueChangedCallback,observer,NULL);before=after;XmStepperSetValue(s,1,True);CHECK(after==before);settle(app,d);
/* Destruction during native PushButton release also cancels queued layout. */
s=XmVaCreateManagedStepper(board,"destroyStepper",XmNstepperValue,2,NULL);settle(app,d);inc=XtNameToWidget(s,"increment");XtAddCallback(s,XmNstepperValueChangedCallback,destroy,NULL);XtAddCallback(s,XmNstepperValueChangedCallback,observer,NULL);button(inc,ButtonPress,True);button(inc,ButtonRelease,True);settle(app,d);CHECK(after==before);
XtDestroyWidget(shell);settle(app,d);XmRenderTableFree(custom);XFreeFont(d,fixed);++cases;
}
printf("PASS stepper native: %u configurations\n",cases);XtCloseDisplay(d);XtDestroyApplicationContext(app);return 0;}
