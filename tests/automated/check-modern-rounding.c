/* Native pixel checks: reflection symmetry, occupied bounds and real controls. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include "AppearanceDrawI.h"
#include "XmI.h"
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while (0)
static unsigned cases;
static void mirrored(Display *d, Drawable drawable, int x, int y, unsigned w, unsigned h, const char *name)
{
    XImage *im=XGetImage(d,drawable,x,y,w,h,AllPlanes,ZPixmap);
    CHECK(im);
    for(unsigned yy=0;yy<h;++yy) for(unsigned xx=0;xx<w;++xx) {
        unsigned long pixel=XGetPixel(im,xx,yy);
        if(pixel!=XGetPixel(im,w-1-xx,yy) || pixel!=XGetPixel(im,xx,h-1-yy)) {
            fprintf(stderr,"Asymmetric %s %ux%u at %u,%u\n",name,w,h,xx,yy);
            exit(1);
        }
    }
    XDestroyImage(im);++cases;
}
static void drain(XtAppContext app, Display *d)
{
    unsigned n=0;XSync(d,False);
    while(XtAppPending(app)){CHECK(++n<10000);XtAppProcessEvent(app,XtIMAll);XSync(d,False);}
}
int main(int argc,char **argv)
{
    XtAppContext app;Display*d;
    XtToolkitInitialize();app=XtCreateApplicationContext();
    d=XtOpenDisplay(app,NULL,"rounding","Rounding",NULL,0,&argc,argv);CHECK(d);
    Pixmap pix=XCreatePixmap(d,DefaultRootWindow(d),80,80,DefaultDepth(d,DefaultScreen(d)));
    GC gc=XCreateGC(d,pix,0,NULL);
    const int sizes[]={1,2,3,4,7,8,19,63};
    const int radii[]={0,1,2,4,12,99};
    Pixel white=WhitePixel(d,DefaultScreen(d)),black=BlackPixel(d,DefaultScreen(d));
    for(unsigned i=0;i<XtNumber(sizes);++i)for(unsigned j=0;j<XtNumber(sizes);++j)
    for(unsigned r=0;r<XtNumber(radii);++r){
        int w=sizes[i],h=sizes[j];
        XSetForeground(d,gc,white);XFillRectangle(d,pix,gc,0,0,80,80);
        XSetForeground(d,gc,black);
        _XmAppearanceFillRoundedRectangle(d,pix,gc,2,2,w,h,radii[r]);
        mirrored(d,pix,2,2,w,h,"rounded surface");
        XImage *im=XGetImage(d,pix,0,0,80,80,AllPlanes,ZPixmap);CHECK(im);
        int top=0,bottom=0,left=0,right=0;
        for(int y=0;y<80;++y)for(int x=0;x<80;++x)if(XGetPixel(im,x,y)==black){
            CHECK(x>=2 && x<2+w && y>=2 && y<2+h);
            top|=y==2;bottom|=y==h+1;left|=x==2;right|=x==w+1;
        }
        CHECK(top && bottom && left && right);XDestroyImage(im);
    }
    XFreeGC(d,gc);XFreePixmap(d,pix);
    const double scales[]={1.0,1.5,2.0};
    for(unsigned dark=0;dark<2;++dark)for(unsigned scale=0;scale<3;++scale)
    for(unsigned parity=0;parity<4;++parity){
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        Widget shell=XtAppCreateShell("rounding","Rounding",applicationShellWidgetClass,d,NULL,0);
        Widget board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,400,XmNheight,220,NULL);
        WidgetClass classes[]={xmPushButtonWidgetClass,xmPushButtonGadgetClass,xmDrawnButtonWidgetClass};
        Widget controls[3];XmString empty=XmStringCreateLocalized("");
        for(unsigned i=0;i<3;++i)
            controls[i]=XtVaCreateManagedWidget("blank",classes[i],board,XmNx,20+110*i,XmNy,30,
                XmNwidth,70+(parity&1),XmNheight,40+((parity>>1)&1),XmNlabelString,empty,NULL);
        XmStringFree(empty);XtRealizeWidget(shell);drain(app,d);
        for(unsigned i=0;i<3;++i){
            Widget c=controls[i];Dimension focus;XtVaGetValues(c,XmNhighlightThickness,&focus,NULL);
            int x=XtIsWidget(c)?0:XtX(c),y=XtIsWidget(c)?0:XtY(c);
            mirrored(d,XtWindowOfObject(c),x+focus,y+focus,XtWidth(c)-2*focus,XtHeight(c)-2*focus,"button face");
        }
        for(unsigned radio=0;radio<2;++radio){
            XmAppearanceToggle toggle={0};
            toggle.x=4;toggle.y=4;toggle.edge=18+(parity&1);
            toggle.state=XmUNSET;toggle.type=radio?XmONE_OF_MANY_ROUND:XmN_OF_MANY;
            toggle.indicator=XmINDICATOR_CHECK_BOX;toggle.visible=True;toggle.fill=True;
            toggle.background=toggle.unselect=white;
            toggle.foreground=toggle.border=toggle.select=black;
            CHECK(_XmAppearanceDrawToggle(controls[2],&toggle));
            mirrored(d,XtWindow(controls[2]),4,4,toggle.edge,toggle.edge,
                     radio?"radio surface":"checkbox surface");
        }
        XtDestroyWidget(shell);drain(app,d);
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS modern rounding: %u pixel-symmetry cases\n",cases);return 0;
}
