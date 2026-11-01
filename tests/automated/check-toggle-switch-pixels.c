/* Native pixels: symmetry, RTL, state changes, clipping and missing-GC safety. */
#include <Xm/ToggleSwitchP.h>
#include <Xm/DrawingA.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Shell.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if(!(c)) {fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);} } while(0)
static XImage *capture_raw(Widget w) {
    XmToggleSwitchWidget p=(XmToggleSwitchWidget)w;
    XtClass(w)->core_class.expose(w,NULL,NULL);XSync(XtDisplay(w),False);
    XImage *im=XGetImage(XtDisplay(w),XtWindow(w),0,0,p->core.width,p->core.height,AllPlanes,ZPixmap);
    CHECK(im);return im;
}
static XImage *capture(Widget w) {
    ((XmToggleSwitchWidget)w)->primitive.highlighted=False;
    return capture_raw(w);
}
static unsigned difference(XImage *a,XImage *b) {
    unsigned n=0;CHECK(a->width==b->width && a->height==b->height);
    for(int y=0;y<a->height;y++)for(int x=0;x<a->width;x++)
        n+=XGetPixel(a,x,y)!=XGetPixel(b,x,y);
    return n;
}
static void symmetry(Widget w) {
    XmToggleSwitchWidget p=(XmToggleSwitchWidget)w;
    XImage *im=capture(w);int lo=im->height,hi=-1;
    for(int y=0;y<im->height;y++)for(int x=0;x<im->width;x++)
        if(XGetPixel(im,x,y)!=p->core.background_pixel){if(y<lo)lo=y;if(y>hi)hi=y;}
    CHECK(hi>lo);
    for(int y=lo;y<=hi;y++)for(int x=0;x<im->width;x++)
        CHECK(XGetPixel(im,x,y)==XGetPixel(im,x,hi+lo-y));
    XDestroyImage(im);
}
int main(int argc,char **argv) {
    XtAppContext app;Widget shell=XtVaAppInitialize(&app,"SwitchPixels",NULL,0,&argc,argv,NULL,
        XmNwidth,900,XmNheight,600,NULL);
    Display *d=XtDisplay(shell);Widget host=XtVaCreateManagedWidget("host",xmDrawingAreaWidgetClass,shell,NULL);
    XtRealizeWidget(shell);unsigned cases=0;
    for(unsigned dark=0;dark<2;dark++)for(unsigned scale=0;scale<5;scale++) {
        CHECK(XmSetDisplayScaleOverride(d,1.+.25*scale));
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        Widget w=XtVaCreateManagedWidget("switch",xmToggleSwitchWidgetClass,host,NULL);
        XmToggleSwitchWidget p=(XmToggleSwitchWidget)w;
        for(unsigned state=0;state<2;state++) {
            fprintf(stderr,"STATE dark=%u scale=%g state=%u\n",dark,1.+.25*scale,state);
            XmToggleSwitchSetState(w,state,False);symmetry(w);
            XImage *left=capture(w);
            Widget mirrored=XtVaCreateManagedWidget("mirrored",xmToggleSwitchWidgetClass,host,
                XmNy,200,XmNlayoutDirection,XmRIGHT_TO_LEFT,XmNset,state,NULL);
            XImage *right=capture(mirrored);
            CHECK(left->width==right->width && left->height==right->height);
            for(int y=0;y<left->height;y++)for(int x=0;x<left->width;x++)
                CHECK(XGetPixel(left,x,y)==XGetPixel(right,left->width-1-x,y));
            XDestroyImage(left);XDestroyImage(right);symmetry(mirrored);
            XtDestroyWidget(mirrored);
            XtSetSensitive(w,False);symmetry(w);XtSetSensitive(w,True);
            ++cases;
        }
        XmToggleSwitchSetState(w,False,False);
        XImage *normal=capture(w);
        XtCallActionProc(w,"SwitchEnter",NULL,NULL,0);
        XImage *hover=capture(w);CHECK(difference(normal,hover)>0);
        XEvent event={0};event.xbutton.type=ButtonPress;event.xbutton.x=2;event.xbutton.y=2;
        XtCallActionProc(w,"SwitchArm",&event,NULL,0);
        XImage *pressed=capture(w);CHECK(difference(hover,pressed)>0);symmetry(w);
        XtCallActionProc(w,"SwitchLeave",NULL,NULL,0);
        XImage *outside=capture(w);CHECK(difference(normal,outside)==0);
        event.xbutton.type=ButtonRelease;event.xbutton.x=-1;
        XtCallActionProc(w,"SwitchRelease",&event,NULL,0);
        CHECK(!XmToggleSwitchGetState(w));
        xmToggleSwitchClassRec.primitive_class.border_highlight(w);
        XImage *focus=capture_raw(w);CHECK(difference(normal,focus)>0);
        xmToggleSwitchClassRec.primitive_class.border_unhighlight(w);
        XImage *unfocused=capture_raw(w);CHECK(difference(normal,unfocused)==0);
        XDestroyImage(normal);XDestroyImage(hover);XDestroyImage(pressed);
        XDestroyImage(outside);XDestroyImage(focus);XDestroyImage(unfocused);
        GC saved=p->toggle_switch.track_gc;p->toggle_switch.track_gc=NULL;
        XtClass(w)->core_class.expose(w,NULL,NULL);p->toggle_switch.track_gc=saved;
        XtVaSetValues(w,XmNwidth,1,XmNheight,1,NULL);
        XImage *tiny=capture(w);CHECK(tiny->width==1 && tiny->height==1);XDestroyImage(tiny);
        XtDestroyWidget(w);
    }
    XtDestroyWidget(shell);XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS switch pixels: %u theme/scale/state cases, RTL, disabled, hover, press, focus, tiny and missing GC\n",cases);return 0;
}
