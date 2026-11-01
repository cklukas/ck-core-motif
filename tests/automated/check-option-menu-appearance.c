/* Inspect the actual OptionMenu disclosure across direction, scale and states. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/CascadeBGP.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static unsigned cases;
static void drain(XtAppContext a,Display *d) {XSync(d,False);while(XtAppPending(a))XtAppProcessEvent(a,XtIMAll);XSync(d,False);}
static void inspect(Widget w,Boolean enabled,Boolean armed) {
    XmCascadeButtonGadget c=(XmCascadeButtonGadget)w;
    int x=c->rectangle.x+CBG_Cascade_x(c),y=c->rectangle.y+CBG_Cascade_y(c);
    int width=CBG_Cascade_width(c),height=CBG_Cascade_height(c);
    Pixel bg,fg,border;XtVaGetValues(w,XmNbackground,&bg,XmNforeground,&fg,XmNbottomShadowColor,&border,NULL);
    Pixel ink=enabled?(armed?bg:fg):border,fill=enabled&&armed?fg:bg;
    CHECK(width>=7 && height>=7);
    CHECK(CBG_Cascade_x(c)>=0 && CBG_Cascade_x(c)+width<=c->rectangle.width);
    CHECK(CBG_Cascade_y(c)>=0 && CBG_Cascade_y(c)+height<=c->rectangle.height);
    XImage *im=XGetImage(XtDisplay(w),XtWindowOfObject(w),x,y,width,height,AllPlanes,ZPixmap);CHECK(im);
    /* Armed glyph and the outside rounded corners share the background
     * color. Exclude only boundary-connected background from the glyph mask. */
    unsigned char *outside=calloc((size_t)width*height,1);CHECK(outside);
    if(enabled && armed) {
        for(int yy=0;yy<height;++yy)for(int xx=0;xx<width;++xx)
            if((xx==0 || yy==0 || xx==width-1 || yy==height-1) && XGetPixel(im,xx,yy)==ink)
                outside[yy*width+xx]=1;
        Boolean changed;
        do {
            changed=False;
            for(int yy=1;yy<height-1;++yy)for(int xx=1;xx<width-1;++xx) {
                int n=yy*width+xx;
                if(!outside[n] && XGetPixel(im,xx,yy)==ink &&
                   (outside[n-1] || outside[n+1] || outside[n-width] || outside[n+width]))
                    {outside[n]=1;changed=True;}
            }
        } while(changed);
    }
    int left=width,right=-1,top=height,bottom=-1,count=0;
    for(int yy=0;yy<height;++yy)for(int xx=0;xx<width;++xx){
        Pixel p=XGetPixel(im,xx,yy);CHECK(p==ink || p==fill || p==bg);
        if(p==ink && !outside[yy*width+xx]){if(xx<left)left=xx;if(xx>right)right=xx;if(yy<top)top=yy;if(yy>bottom)bottom=yy;++count;}
    }
    CHECK(count>0 && count<width*height/2);
    if (!(left>0 && right<width-1 && top>0 && bottom<height-1)) fprintf(stderr,"indicator %dx%d bounds %d,%d..%d,%d enabled=%d armed=%d bg=%lu ink=%lu fill=%lu\n",width,height,left,top,right,bottom,enabled,armed,bg,ink,fill);
    CHECK(left>0 && right<width-1 && top>0 && bottom<height-1);
    /* A downward chevron has separated upper arms and a joined lower tip. */
    int middle=(left+right)/2;
    CHECK(XGetPixel(im,middle,top)!=ink);
    CHECK(XGetPixel(im,middle,bottom)==ink);
    free(outside);XDestroyImage(im);++cases;
}
int main(int argc,char **argv) {
    XtAppContext a;Display*d;double scales[]={1,1.5,2};
    XtToolkitInitialize();a=XtCreateApplicationContext();d=XtOpenDisplay(a,NULL,"optionVisual","OptionVisual",NULL,0,&argc,argv);CHECK(d);
    for(unsigned dark=0;dark<2;++dark)for(unsigned scale=0;scale<3;++scale)for(unsigned rtl=0;rtl<2;++rtl){
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        Widget shell=XtAppCreateShell("optionVisual","OptionVisual",applicationShellWidgetClass,d,NULL,0);
        Widget board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,NULL);
        Widget menu=XmCreatePulldownMenu(board,"menu",NULL,0);
        XtVaCreateManagedWidget("Sunday",xmPushButtonGadgetClass,menu,NULL);
        Arg args[2];XtSetArg(args[0],XmNsubMenuId,menu);XtSetArg(args[1],XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT);
        Widget option=XmCreateOptionMenu(board,"option",args,2);XtManageChild(option);
        Widget button=XmOptionButtonGadget(option);CHECK(button);
        XtRealizeWidget(shell);drain(a,d);
        XmCascadeButtonGadget c=(XmCascadeButtonGadget)button;
        for(unsigned enabled=0;enabled<2;++enabled)for(unsigned armed=0;armed<2;++armed){
            XtSetSensitive(option,enabled);drain(a,d);CBG_SetArmed(c,armed);
            ((RectObjClass)XtClass(button))->rect_class.expose(button,NULL,NULL);XSync(d,False);
            inspect(button,enabled,armed);
        }
        CBG_SetArmed(c,False);XtSetSensitive(option,True);drain(a,d);
        /* A caller-supplied icon must not be replaced by the default chevron. */
        Pixmap custom=XCreatePixmap(d,XtWindow(shell),11,9,DefaultDepth(d,DefaultScreen(d)));
        GC gc=XCreateGC(d,custom,0,NULL);XSetForeground(d,gc,WhitePixel(d,DefaultScreen(d)));XFillRectangle(d,custom,gc,0,0,11,9);
        XtVaSetValues(button,XmNcascadePixmap,custom,NULL);drain(a,d);
        ((RectObjClass)XtClass(button))->rect_class.expose(button,NULL,NULL);XSync(d,False);
        CHECK(CBG_Cascade_width(c)==11 && CBG_Cascade_height(c)==9);
        XImage *im=XGetImage(d,XtWindowOfObject(button),c->rectangle.x+CBG_Cascade_x(c),c->rectangle.y+CBG_Cascade_y(c),11,9,AllPlanes,ZPixmap);CHECK(im);
        for(int y=0;y<9;++y)for(int x=0;x<11;++x)CHECK(XGetPixel(im,x,y)==WhitePixel(d,DefaultScreen(d)));
        XDestroyImage(im);++cases;XtDestroyWidget(shell);XFreeGC(d,gc);XFreePixmap(d,custom);drain(a,d);
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(a);printf("PASS option menu appearance: %u cases\n",cases);return 0;
}
