/* Native pointer/keyboard admission, mirrored focus and removal geometry. */
#include "native-window.h"
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static unsigned activated,removed;
static void activate(Widget w,XtPointer p,XtPointer data){(void)w;(void)p;CHECK(((XmChipCallbackStruct*)data)->reason==XmCR_ACTIVATE);++activated;}
static void remove_chip(Widget w,XtPointer p,XtPointer data){(void)w;(void)p;CHECK(((XmChipCallbackStruct*)data)->reason==XmCHIP_REASON_REMOVE);++removed;}
static void destroy_chip(Widget w,XtPointer p,XtPointer data){(void)p;(void)data;XtDestroyWidget(w);}
static void drain(XtAppContext app,Display *d){for(unsigned i=0;i<10000;++i){XSync(d,False);if(!XtAppPending(app))return;XtAppProcessEvent(app,XtIMAll);}CHECK(0);}
static void button(Widget w,int type,int x,int y,unsigned which){
    XEvent e={0};e.xbutton.type=type;e.xbutton.display=XtDisplay(w);e.xbutton.window=XtWindow(w);
    e.xbutton.x=x;e.xbutton.y=y;e.xbutton.button=which;e.xbutton.same_screen=True;
    XtDispatchEvent(&e);
}
static void key(Widget w,KeySym symbol){XEvent e={0};e.xkey.type=KeyPress;e.xkey.display=XtDisplay(w);e.xkey.window=XtWindow(w);e.xkey.keycode=XKeysymToKeycode(XtDisplay(w),symbol);XtDispatchEvent(&e);}
static void symmetric_focus(Widget w){
    Pixel accent;Dimension width,height;XtVaGetValues(w,XmNhighlightColor,&accent,XmNwidth,&width,XmNheight,&height,NULL);
    XImage *im=XGetImage(XtDisplay(w),XtWindow(w),0,0,width,height,AllPlanes,ZPixmap);CHECK(im);unsigned pixels=0;
    for(unsigned y=0;y<height;++y)for(unsigned x=0;x<width;++x){Boolean yes=XGetPixel(im,x,y)==accent;pixels+=yes;
        CHECK(yes==(XGetPixel(im,width-1-x,y)==accent));CHECK(yes==(XGetPixel(im,x,height-1-y)==accent));}
    CHECK(pixels>0);XDestroyImage(im);
}
int main(int argc,char **argv){
    XtAppContext app;XtToolkitInitialize();app=XtCreateApplicationContext();
    Display *d=XtOpenDisplay(app,NULL,"chipInteraction","ChipInteraction",NULL,0,&argc,argv);CHECK(d);
    const double scales[]={1,1.5,2,3};unsigned cases=0;
    for(unsigned dark=0;dark<2;++dark)for(unsigned s=0;s<4;++s)for(unsigned rtl=0;rtl<2;++rtl){
        CHECK(XmSetDisplayScaleOverride(d,scales[s]));CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        Widget shell=XtAppCreateShell("chipInteraction","ChipInteraction",applicationShellWidgetClass,d,NULL,0);
        Widget board=XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,XmNwidth,1200,XmNheight,500,XmNresizePolicy,XmRESIZE_NONE,NULL);
        XmString text=XmStringCreateLocalized("An active filter");
        Widget chip=XtVaCreateManagedWidget("chip",xmChipWidgetClass,board,XmNchipLabel,text,XmNchipRemovable,True,XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);XmStringFree(text);
        XtAddCallback(chip,XmNchipActivateCallback,activate,NULL);XtAddCallback(chip,XmNchipRemoveCallback,remove_chip,NULL);
        XtRealizeWidget(shell); NativeWaitForViewable(shell);drain(app,d);Dimension width,height;
        XtVaGetValues(chip,XmNwidth,&width,XmNheight,&height,NULL);
        int edge=(int)(12*scales[s]+0.5),body=rtl?width-edge:edge,close=rtl?edge:width-edge,y=height/2;
        unsigned a=activated,r=removed;
        button(chip,ButtonPress,body,y,Button1);drain(app,d);CHECK(activated==a && removed==r);symmetric_focus(chip);
        button(chip,ButtonRelease,body,y,Button1);CHECK(activated==++a && removed==r);
        button(chip,ButtonPress,close,y,Button1);CHECK(removed==r);
        button(chip,ButtonRelease,close,y,Button1);CHECK(removed==++r && activated==a);
        button(chip,ButtonPress,body,y,Button1);button(chip,ButtonRelease,-10,-10,Button1);CHECK(activated==a && removed==r);
        button(chip,ButtonPress,close,y,Button1);button(chip,ButtonRelease,body,y,Button1);CHECK(activated==a && removed==r);
        button(chip,ButtonPress,body,y,Button3);button(chip,ButtonRelease,body,y,Button3);CHECK(activated==a && removed==r);
        key(chip,XK_Return);CHECK(activated==++a);key(chip,XK_Delete);CHECK(removed==++r);
        button(chip,ButtonPress,body,y,Button1);XtSetSensitive(chip,False);button(chip,ButtonRelease,body,y,Button1);
        key(chip,XK_Return);key(chip,XK_Delete);CHECK(activated==a && removed==r);XtSetSensitive(chip,True);
        button(chip,ButtonRelease,body,y,Button1);CHECK(activated==a);
        XtVaSetValues(chip,XmNchipRemovable,False,NULL);key(chip,XK_Delete);CHECK(removed==r);
        XtVaSetValues(chip,XmNchipRemovable,True,NULL);drain(app,d);
        /* Destruction from release callbacks must not leave an armed redraw. */
        XtAddCallback(chip,XmNchipRemoveCallback,destroy_chip,NULL);
        button(chip,ButtonPress,close,y,Button1);button(chip,ButtonRelease,close,y,Button1);CHECK(removed==++r);
        drain(app,d);XtDestroyWidget(shell);drain(app,d);++cases;
    }
    XmChipInvoke(NULL,NULL);XmChipRemove(NULL,NULL);
    XtCloseDisplay(d);XtDestroyApplicationContext(app);printf("PASS chip interaction: %u configurations\n",cases);return 0;
}
