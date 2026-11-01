/* Native visible-pixel acceptance for modern Avatar surfaces and lifecycle. */
#include <X11/IntrinsicP.h>
#include <X11/CompositeP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/Avatar.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include "native-window.h"
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while (0)
static void tick(XtPointer data, XtIntervalId *id) { *(Boolean *)data=True; (void)id; }
static void settle(XtAppContext app, Display *display) {
    for (unsigned i=0;i<3;++i) {
        Boolean done=False; XtAppAddTimeOut(app,20,tick,&done);
        while(!done) XtAppProcessEvent(app,XtIMAll);
        XSync(display,False);
        while(XtAppPending(app)) XtAppProcessEvent(app,XtIMAll);
    }
}
static unsigned long pixel(Display *d, Widget w, int x, int y) {
    XImage *im=XGetImage(d,XtWindow(w),x,y,1,1,AllPlanes,ZPixmap); CHECK(im);
    unsigned long p=XGetPixel(im,0,0);XDestroyImage(im);return p;
}
int main(int argc,char **argv) {
    XtAppContext app; Widget shell=XtVaAppInitialize(&app,"ModernAvatar",NULL,0,&argc,argv,NULL,NULL);
    Display *d=XtDisplay(shell); unsigned cases=0;
    Widget board=XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,
        XmNwidth,500,XmNheight,400,XmNresizePolicy,XmRESIZE_NONE,NULL);
    XtRealizeWidget(shell);NativeWaitForViewable(board);
    const double scales[]={1,1.5,2};
    for(unsigned dark=0;dark<2;++dark)for(unsigned scale=0;scale<3;++scale) {
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMFORTABLE));
        Widget avatar=XmVaCreateManagedAvatar(board,"avatar",XmNx,13,XmNy,17,NULL);
        NativeWaitForViewable(avatar);settle(app,d);
        Pixel bg,parent;Dimension width,height;XmRenderTable font;
        XtVaGetValues(avatar,XmNbackground,&bg,XmNwidth,&width,XmNheight,&height,XmNrenderTable,&font,NULL);
        XtVaGetValues(board,XmNbackground,&parent,NULL);
        CHECK(width==(Dimension)(48*scales[scale]) && height==width);
        CHECK(bg!=parent);CHECK(pixel(d,avatar,0,0)==parent);
        CHECK(pixel(d,avatar,width/2,height/2)==bg);
        XImage *image=XGetImage(d,XtWindow(avatar),0,0,width,height,AllPlanes,ZPixmap);CHECK(image);
        unsigned partial=0;
        for(unsigned y=0;y<height;++y)for(unsigned x=0;x<width;++x) {
            unsigned long p=XGetPixel(image,x,y);
            CHECK(p==XGetPixel(image,width-1-x,y));CHECK(p==XGetPixel(image,x,height-1-y));
            partial+=p!=bg && p!=parent;
        }
        XDestroyImage(image);CHECK(partial>16);++cases;
        XmString initials=XmStringCreateLocalized("CK");Dimension tw,th;
        XmStringExtent(font,initials,&tw,&th);CHECK(tw>=20*scales[scale] && th>=18*scales[scale]);
        XtVaSetValues(avatar,XmNavatarInitials,initials,NULL);XmStringFree(initials);settle(app,d);
        image=XGetImage(d,XtWindow(avatar),width/4,height/4,width/2,height/2,AllPlanes,ZPixmap);CHECK(image);
        unsigned ink=0;for(unsigned y=0;y<height/2;++y)for(unsigned x=0;x<width/2;++x) ink+=XGetPixel(image,x,y)!=bg;
        XDestroyImage(image);CHECK(ink>30);++cases;
        /* Parent changes must repaint the outside without overriding the face. */
        Pixel changed=dark?0x101820:0xe1e4e8;
        XtVaSetValues(board,XmNbackground,changed,NULL);settle(app,d);
        CHECK(pixel(d,avatar,0,0)==changed);++cases;
        Pixmap tile=XCreatePixmap(d,XtWindow(board),2,2,DefaultDepthOfScreen(XtScreen(board)));
        GC gc=XCreateGC(d,tile,0,NULL);XSetForeground(d,gc,0x657483);XFillRectangle(d,tile,gc,0,0,2,2);
        XSetForeground(d,gc,0xabc9de);XDrawPoint(d,tile,gc,1,1);
        XtVaSetValues(board,XmNbackgroundPixmap,tile,NULL);settle(app,d);
        Position px,py;Dimension bw;XtVaGetValues(avatar,XmNx,&px,XmNy,&py,XmNborderWidth,&bw,NULL);
        CHECK(pixel(d,avatar,0,0)==(((px+bw)%2 && (py+bw)%2)?0xabc9deUL:0x657483UL));
        CHECK(pixel(d,avatar,width-1,0)==((((px+bw+width-1)%2) && (py+bw)%2)?0xabc9deUL:0x657483UL));++cases;
        XtVaSetValues(board,XmNbackgroundPixmap,XmUNSPECIFIED_PIXMAP,NULL);settle(app,d);
        XFreeGC(d,gc);XFreePixmap(d,tile);
        Widget inherited=XtVaCreateManagedWidget("inherited",compositeWidgetClass,board,
            XmNx,150,XmNy,150,XmNwidth,150,XmNheight,150,
            XmNbackground,(Pixel)0xa1a2a3,XmNbackgroundPixmap,ParentRelative,NULL);
        Widget child=XmVaCreateManagedAvatar(inherited,"inheritedAvatar",NULL);
        NativeWaitForViewable(child);settle(app,d);
        CHECK(pixel(d,child,0,0)==changed);++cases;
        XtDestroyWidget(inherited);
        XtVaSetValues(avatar,XmNbackground,(Pixel)0x123456,XmNforeground,(Pixel)0xeeeeee,NULL);settle(app,d);
        CHECK(pixel(d,avatar,0,0)==0x123456);++cases;
        CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMPACT));settle(app,d);
        XtVaGetValues(avatar,XmNwidth,&width,NULL);CHECK(width==(Dimension)(40*scales[scale]));
        CHECK(pixel(d,avatar,0,0)==0x123456);++cases;
        XtDestroyWidget(avatar);settle(app,d);
    }
    /* Recycled server pixmap IDs must not retain a stale Xft text target. */
    CHECK(XmSetDisplayScaleOverride(d,1));CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_LIGHT));
    for(unsigned i=0;i<100;++i) {
        XmString initials=XmStringCreateLocalized("CK");
        Widget avatar=XmVaCreateManagedAvatar(board,"recreated",XmNavatarInitials,initials,NULL);
        XmStringFree(initials);NativeWaitForViewable(avatar);settle(app,d);
        CHECK(pixel(d,avatar,0,0)==pixel(d,board,100,100));
        XtDestroyWidget(avatar);++cases;
    }
    XtDestroyWidget(shell);XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS modern Avatar: %u surface, scale, font, parent and lifecycle cases\n",cases);return 0;
}
