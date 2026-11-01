/* Native Avatar image crop, circular clip, bitmap and colour refresh checks. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/Avatar.h>
#include <Xm/Appearance.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while (0)
static void drain(XtAppContext app, Display *d) {
    unsigned n=0; XSync(d,False);
    while(XtAppPending(app)){CHECK(++n<10000);XtAppProcessEvent(app,XtIMAll);XSync(d,False);}
}
static unsigned long pixel(Display*d,Widget w,int x,int y) {
    XImage *im=XGetImage(d,XtWindow(w),x,y,1,1,AllPlanes,ZPixmap);CHECK(im);
    unsigned long p=XGetPixel(im,0,0);XDestroyImage(im);return p;
}
int main(int argc,char **argv) {
    XtAppContext app; Widget shell=XtVaAppInitialize(&app,"AvatarPixels",NULL,0,&argc,argv,NULL,NULL);
    Display *d=XtDisplay(shell); unsigned cases=0;
    Pixel white=WhitePixelOfScreen(XtScreen(shell)),black=BlackPixelOfScreen(XtScreen(shell));
    Widget board=XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,XmNwidth,160,XmNheight,160,NULL);
    Widget avatar=XmVaCreateManagedAvatar(board,"avatar",XmNx,10,XmNy,10,XmNwidth,40,XmNheight,40,
        XmNbackground,white,XmNforeground,black,NULL);
    Pixmap colour=XCreatePixmap(d,RootWindowOfScreen(XtScreen(shell)),100,100,DefaultDepthOfScreen(XtScreen(shell)));
    GC gc=XCreateGC(d,colour,0,NULL);
    XSetForeground(d,gc,white);XFillRectangle(d,colour,gc,0,0,100,100);
    XSetForeground(d,gc,black);XFillRectangle(d,colour,gc,30,30,40,40);
    char bits[128]; for(unsigned i=0;i<sizeof(bits);++i)bits[i]=(char)255;
    Pixmap bitmap=XCreateBitmapFromData(d,colour,bits,32,32);
    XtRealizeWidget(shell);drain(app,d);
    for(unsigned profile=0;profile<3;++profile) {
        CHECK(XmApplyAppearanceProfile(d,profile==0?XmAPPEARANCE_CLASSIC:profile==1?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));
        XtVaSetValues(avatar,XmNbackground,white,XmNforeground,black,XmNavatarPixmap,colour,NULL);drain(app,d);
        CHECK(pixel(d,avatar,0,0)==white); /* Images have the same circular boundary as initials. */
        CHECK(pixel(d,avatar,20,20)==black); /* Crop from the image centre, not its top-left. */
        ++cases;
        XtVaSetValues(avatar,XmNavatarPixmap,bitmap,NULL);drain(app,d);
        CHECK(pixel(d,avatar,20,20)==black);CHECK(pixel(d,avatar,0,0)==white);++cases;
        XtVaSetValues(avatar,XmNforeground,white,XmNbackground,black,NULL);drain(app,d);
        CHECK(pixel(d,avatar,20,20)==white);CHECK(pixel(d,avatar,0,0)==black);++cases;
        XtVaSetValues(avatar,XmNavatarPixmap,XmUNSPECIFIED_PIXMAP,NULL);drain(app,d);
        const unsigned sizes[]={1,2,3,4,7,8,31,32,63,64};
        for(unsigned i=0;i<XtNumber(sizes);++i) {
            unsigned edge=sizes[i];XtVaSetValues(avatar,XmNwidth,edge,XmNheight,edge,NULL);drain(app,d);
            XImage *im=XGetImage(d,XtWindow(avatar),0,0,edge,edge,AllPlanes,ZPixmap);CHECK(im);
            for(unsigned y=0;y<edge;++y)for(unsigned x=0;x<edge;++x) {
                CHECK(XGetPixel(im,x,y)==XGetPixel(im,edge-1-x,y));
                CHECK(XGetPixel(im,x,y)==XGetPixel(im,x,edge-1-y));
            }
            XDestroyImage(im);++cases;
        }
        XtVaSetValues(avatar,XmNwidth,40,XmNheight,40,NULL);drain(app,d);
        XmString long_text=XmStringCreateLocalized("WWWWWWWWWWWWWW\nWWWWWWWWWWWWWW\nWWWWWWWWWWWWWW");
        XtVaSetValues(avatar,XmNavatarInitials,long_text,NULL);XmStringFree(long_text);drain(app,d);
        CHECK(pixel(d,avatar,0,0)==black);CHECK(pixel(d,avatar,39,0)==black);
        CHECK(pixel(d,avatar,0,39)==black);CHECK(pixel(d,avatar,39,39)==black);++cases;
        /* Replacing and clearing the owned compound string must be safe. */
        XtVaSetValues(avatar,XmNavatarInitials,NULL,XmNwidth,64,XmNheight,40,
                      XmNavatarPixmap,bitmap,NULL);drain(app,d);
        CHECK(pixel(d,avatar,32,20)==white);CHECK(pixel(d,avatar,0,20)==black);
        CHECK(pixel(d,avatar,63,20)==black);++cases;
        XtVaSetValues(avatar,XmNwidth,40,XmNheight,40,NULL);drain(app,d);
    }
    XtDestroyWidget(shell);XFreeGC(d,gc);XFreePixmap(d,colour);XFreePixmap(d,bitmap);
    XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS avatar rendering: %u cases\n",cases);return 0;
}
