/* Dispatch native events through Xt: preview, cancellation, RTL and lifetime. */
#include <Xm/RatingControlP.h>
#include <Xm/DrawingA.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Shell.h>
#include <X11/keysym.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static unsigned long pixels(Widget w) {
 XtClass(w)->core_class.expose(w,NULL,NULL);XSync(XtDisplay(w),False);
 XImage *im=XGetImage(XtDisplay(w),XtWindow(w),0,0,w->core.width,w->core.height,AllPlanes,ZPixmap);CHECK(im);
 unsigned long hash=5381;
 for(int y=0;y<im->height;y++)for(int x=0;x<im->width;x++)hash=hash*33+XGetPixel(im,x,y);
 XDestroyImage(im);return hash;
}
static unsigned callbacks,destroyed,after_destroy;
static void changed(Widget w,XtPointer data,XtPointer call){(void)w;(void)data;CHECK(call);++callbacks;}
static void destroy(Widget w,XtPointer data,XtPointer call){(void)data;(void)call;++destroyed;XtDestroyWidget(w);}
static void forbidden(Widget w,XtPointer data,XtPointer call){(void)w;(void)data;(void)call;++after_destroy;}
static void send(Widget w,int type,int x,int y,unsigned button,KeySym key){
 XEvent e={0};e.xany.type=type;e.xany.display=XtDisplay(w);e.xany.window=XtWindow(w);e.xany.send_event=True;
 if(type==KeyPress){e.xkey.keycode=XKeysymToKeycode(XtDisplay(w),key);e.xkey.same_screen=True;}
 else if(type==MotionNotify){e.xmotion.x=x;e.xmotion.y=y;e.xmotion.same_screen=True;}
 else {e.xbutton.x=x;e.xbutton.y=y;e.xbutton.button=button;e.xbutton.same_screen=True;}
 XtDispatchEvent(&e);
}
int main(int argc,char **argv){
 XtAppContext app;Widget shell=XtVaAppInitialize(&app,"RatingInteraction",NULL,0,&argc,argv,NULL,XmNwidth,1200,XmNheight,800,NULL);
 Display*d=XtDisplay(shell);Widget host=XtVaCreateManagedWidget("host",xmDrawingAreaWidgetClass,shell,NULL);XtRealizeWidget(shell);
 unsigned cases=0;
 for(unsigned dark=0;dark<2;dark++)for(unsigned scale=1;scale<4;scale++)for(unsigned rtl=0;rtl<2;rtl++){
  CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));CHECK(XmSetDisplayScaleOverride(d,scale));
  Widget w=XtVaCreateManagedWidget("rating",xmRatingControlWidgetClass,host,XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,XmNratingValue,2,NULL);
  XmRatingControlWidget p=(XmRatingControlWidget)w;XtAddCallback(w,XmNratingValueChangedCallback,changed,NULL);callbacks=0;
  int x=w->core.width/10,y=w->core.height/2,first=rtl?5:1;
  unsigned long normal=pixels(w);
  send(w,MotionNotify,x,y,0,0);CHECK(p->rating.hover_value==first && p->rating.value==2 && callbacks==0);
  CHECK(pixels(w)!=normal);
  send(w,ButtonPress,x,y,Button1,0);CHECK(p->rating.value==2 && callbacks==0);
  send(w,ButtonRelease,x,y,Button1,0);CHECK(p->rating.value==first && callbacks==1 && !p->rating.armed);
  send(w,ButtonPress,x,y,Button1,0);send(w,ButtonRelease,x,y,Button1,0);CHECK(p->rating.value==0 && callbacks==2);
  send(w,ButtonPress,x,y,Button3,0);send(w,ButtonRelease,x,y,Button3,0);CHECK(callbacks==2);
  send(w,ButtonRelease,x,y,Button1,0);CHECK(callbacks==2);
  send(w,ButtonPress,x,y,Button1,0);send(w,ButtonRelease,-1,y,Button1,0);CHECK(callbacks==2 && p->rating.value==0);
  send(w,ButtonPress,x,y,Button1,0);XtSetSensitive(w,False);XtSetSensitive(w,True);send(w,ButtonRelease,x,y,Button1,0);CHECK(callbacks==2);
  send(w,ButtonPress,x,y,Button1,0);XtVaSetValues(w,XmNratingReadonly,True,NULL);XtVaSetValues(w,XmNratingReadonly,False,NULL);send(w,ButtonRelease,x,y,Button1,0);CHECK(callbacks==2);
  XmRatingControlSetValue(w,2,False);send(w,KeyPress,0,0,0,XK_Right);CHECK(p->rating.value==(rtl?1:3));
  send(w,KeyPress,0,0,0,XK_End);CHECK(p->rating.value==5);unsigned before=callbacks;
  send(w,KeyPress,0,0,0,XK_Up);CHECK(p->rating.value==5 && callbacks==before);
  XtVaSetValues(w,XmNratingAllowClear,False,NULL);send(w,KeyPress,0,0,0,XK_Delete);CHECK(p->rating.value==5 && callbacks==before);
  XtVaSetValues(w,XmNratingReadonly,True,NULL);send(w,KeyPress,0,0,0,XK_Home);CHECK(p->rating.value==5);
  XtVaSetValues(w,XmNratingReadonly,False,NULL);
  xmRatingControlClassRec.primitive_class.border_unhighlight(w);normal=pixels(w);
  xmRatingControlClassRec.primitive_class.border_highlight(w);CHECK(pixels(w)!=normal);
  xmRatingControlClassRec.primitive_class.border_unhighlight(w);CHECK(pixels(w)==normal);
  XtDestroyWidget(w);++cases;
 }
 Widget w=XtVaCreateManagedWidget("doomed",xmRatingControlWidgetClass,host,NULL);
 XtAddCallback(w,XmNratingValueChangedCallback,destroy,NULL);XtAddCallback(w,XmNratingValueChangedCallback,forbidden,NULL);
 send(w,ButtonPress,w->core.width/10,w->core.height/2,Button1,0);
 send(w,ButtonRelease,w->core.width/10,w->core.height/2,Button1,0);
 CHECK(destroyed==1 && after_destroy==0);
 XtDestroyWidget(shell);XtCloseDisplay(d);XtDestroyApplicationContext(app);
 printf("PASS rating interaction: %u theme/scale/RTL event sequences and callback destruction\n",cases);return 0;
}
