/* Public image-cache contract for the shared SVG/bitmap action artwork. */
#include <Xm/Xm.h>
#include <Xm/Display.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#c); exit(1); } } while (0)

static void size(Display *d, Pixmap p, unsigned expected_w, unsigned expected_h)
{
    Window root; int x,y; unsigned w,h,border,depth;
    CHECK(p!=None && p!=XmUNSPECIFIED_PIXMAP);
    CHECK(XGetGeometry(d,p,&root,&x,&y,&w,&h,&border,&depth));
    CHECK(w==expected_w && h==expected_h);
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell=XtVaAppInitialize(&app,"ActionIcons",NULL,0,&argc,argv,NULL,NULL);
    Display *d=XtDisplay(shell); Screen *screen=XtScreen(shell);
    const char *names[]={"action_open","action_document","action_save","action_print",
                         "action_undo","action_picture","action_archive"};
    const double scales[]={1.0,1.5,2.0,3.0};
    unsigned cases=0;
    for(unsigned n=0;n<XtNumber(names);++n) {
        for(unsigned s=0;s<XtNumber(scales);++s)for(unsigned dark=0;dark<2;++dark) {
            Pixel bg=dark?BlackPixelOfScreen(screen):WhitePixelOfScreen(screen);
            Pixel fg=dark?WhitePixelOfScreen(screen):BlackPixelOfScreen(screen);
            CHECK(XmSetDisplayScaleOverride(d,scales[s]));
            Pixmap p=XmGetScaledPixmap(shell,(char *)names[n],fg,bg,DefaultDepthOfScreen(screen),0.0);
            unsigned extent=(unsigned)(24*scales[s]+0.5);
            size(d,p,extent,extent);
            Pixmap again=XmGetScaledPixmap(shell,(char *)names[n],fg,bg,DefaultDepthOfScreen(screen),0.0);
            CHECK(p==again);
            CHECK(XmDestroyPixmap(screen,p));
            /* The second cache reference must still own a usable image. */
            XImage *image=XGetImage(d,again,0,0,extent,extent,AllPlanes,ZPixmap);
            CHECK(image); CHECK(XGetPixel(image,0,0)==bg);
            unsigned ink=0;
            for(unsigned y=0;y<extent;++y)for(unsigned x=0;x<extent;++x)
                ink+=XGetPixel(image,x,y)!=bg;
            CHECK(ink>extent && ink<extent*extent);
            XDestroyImage(image); CHECK(XmDestroyPixmap(screen,again)); ++cases;
        }
        XImage *override=XCreateImage(d,DefaultVisualOfScreen(screen),1,XYBitmap,0,NULL,3,5,8,0);
        CHECK(override); override->data=calloc(override->bytes_per_line,5); CHECK(override->data);
        CHECK(XmInstallImage(override,(char *)names[n]));
        Pixmap custom=XmGetScaledPixmap(shell,(char *)names[n],BlackPixelOfScreen(screen),
                            WhitePixelOfScreen(screen),DefaultDepthOfScreen(screen),2.0);
        size(d,custom,6,10); CHECK(XmDestroyPixmap(screen,custom));
        CHECK(XmUninstallImage(override)); XDestroyImage(override);
        Pixmap builtin=XmGetScaledPixmap(shell,(char *)names[n],BlackPixelOfScreen(screen),
                            WhitePixelOfScreen(screen),DefaultDepthOfScreen(screen),2.0);
        size(d,builtin,48,48); CHECK(XmDestroyPixmap(screen,builtin)); ++cases;
    }
    XtDestroyWidget(shell); XtCloseDisplay(d); XtDestroyApplicationContext(app);
    printf("PASS action icons: %u scaling, background, cache and override cases\n",cases);
    return 0;
}
