/* Disabled glyphs must use one readable color, without modern embossing. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/LabelP.h>
#include <Xm/LabelGP.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static void drain(XtAppContext app,Display *d) {
    XSync(d,False);while(XtAppPending(app)) XtAppProcessEvent(app,XtIMAll);XSync(d,False);
}
static Pixel color(Widget w,const char *name) {
    XColor exact,screen;Colormap map;
    XtVaGetValues(XtIsWidget(w)?w:XtParent(w),XmNcolormap,&map,NULL);
    CHECK(XAllocNamedColor(XtDisplayOfObject(w),map,name,&screen,&exact));return screen.pixel;
}
static unsigned channel(unsigned long pixel,unsigned long mask) {
    unsigned shift=0;
    if(!mask)return 0;
    while(!(mask&1)) {mask>>=1;++shift;}
    return (unsigned)(((pixel>>shift)&mask)*255/mask);
}
static void glyphs(Widget w,Boolean accelerator,Boolean embossed,Boolean blue) {
    XRectangle rect;XImage *image;unsigned purple=0,ink=0,blue_ink=0;
    if(XtIsWidget(w)) {
        XmLabelWidget label=(XmLabelWidget)w;
        rect=accelerator?label->label.acc_TextRect:label->label.TextRect;
    } else {
        XmLabelGadget label=(XmLabelGadget)w;
        rect=accelerator?LabG_AccTextRect(label):LabG_TextRect(label);
    }
    CHECK(rect.width>0 && rect.height>0);
    XSync(XtDisplayOfObject(w),False);
    image=XGetImage(XtDisplayOfObject(w),XtWindowOfObject(w),
        (XtIsWidget(w)?0:XtX(w))+rect.x,(XtIsWidget(w)?0:XtY(w))+rect.y,
        rect.width+1,rect.height+1,AllPlanes,ZPixmap);CHECK(image);
    for(unsigned y=0;y<(unsigned)image->height;++y) for(unsigned x=0;x<(unsigned)image->width;++x) {
        unsigned long pixel=XGetPixel(image,x,y);
        unsigned r=channel(pixel,image->red_mask),g=channel(pixel,image->green_mask),b=channel(pixel,image->blue_mask);
        if(r>g+20 && b>g+20)++purple;
        if(r<240 || g<240 || b<240)++ink;
        if(b>r+20 && b>g+20)++blue_ink;
    }
    if((purple>0)!=embossed)fprintf(stderr,"%s purple=%u expected embossed=%d\n",XtName(w),purple,embossed);
    CHECK((purple>0)==embossed);CHECK(ink>0);if(blue)CHECK(blue_ink>0);
    XDestroyImage(image);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *d;unsigned cases=0;
    WidgetClass classes[]={xmLabelWidgetClass,xmLabelGadgetClass,xmPushButtonWidgetClass,
        xmPushButtonGadgetClass,xmToggleButtonWidgetClass,xmToggleButtonGadgetClass};
    const double scales[]={1,1.5,2};
    XtToolkitInitialize();app=XtCreateApplicationContext();
    d=XtOpenDisplay(app,NULL,"disabledLabels","DisabledLabels",NULL,0,&argc,argv);CHECK(d);
    for(unsigned scale=0;scale<3;++scale) for(unsigned dark=0;dark<2;++dark)
    for(unsigned cls=0;cls<XtNumber(classes);++cls) for(unsigned mnemonic=0;mnemonic<2;++mnemonic) {
        Widget shell,board,w;XmString text=XmStringCreateLocalized("Unavailable WWW");Pixel expected,actual;
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        shell=XtAppCreateShell("labels","DisabledLabels",applicationShellWidgetClass,d,NULL,0);
        board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,700,XmNheight,300,NULL);
        w=XtVaCreateManagedWidget("label",classes[cls],board,XmNlabelString,text,
            XmNmnemonic,mnemonic?XK_U:XK_VoidSymbol,XmNsensitive,False,
            XmNbackground,WhitePixelOfScreen(XtScreen(board)),XmNforeground,BlackPixelOfScreen(XtScreen(board)),
            XmNtopShadowColor,color(board,"magenta"),XmNbottomShadowColor,color(board,"#606060"),NULL);
        XmStringFree(text);
        XtVaGetValues(w,XmNinsensitiveForeground,&actual,NULL);
        CHECK(actual==color(w,dark?"#9aa0a6":"#747b84"));
        XtRealizeWidget(shell);drain(app,d);glyphs(w,False,False,False);
        expected=color(w,"#0033cc");XtVaSetValues(w,XmNinsensitiveForeground,expected,NULL);drain(app,d);glyphs(w,False,False,True);
        XtVaGetValues(w,XmNinsensitiveForeground,&actual,NULL);CHECK(actual==expected);
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(app,d);glyphs(w,False,True,True);
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));drain(app,d);glyphs(w,False,False,True);
        XtVaGetValues(w,XmNinsensitiveForeground,&actual,NULL);CHECK(actual==expected);
        XtVaSetValues(w,XmNinsensitiveForeground,XmUNSPECIFIED_PIXEL,NULL);drain(app,d);
        XtVaSetValues(w,XmNbottomShadowColor,expected,NULL);drain(app,d);glyphs(w,False,False,True);
        XtDestroyWidget(shell);drain(app,d);++cases;
    }
    for(unsigned dark=0;dark<2;++dark) for(unsigned gadget=0;gadget<2;++gadget) {
        Widget shell,board,menu,w; XmString text,accel;
        CHECK(XmSetDisplayScaleOverride(d,1));
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        shell=XtAppCreateShell("menus","DisabledLabels",applicationShellWidgetClass,d,NULL,0);
        board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,500,XmNheight,250,NULL);
        menu=XmCreatePopupMenu(board,"menu",NULL,0);
        text=XmStringCreateLocalized("Unavailable");accel=XmStringCreateLocalized("Ctrl+U");
        w=XtVaCreateManagedWidget("menuLabel",gadget?xmPushButtonGadgetClass:xmPushButtonWidgetClass,menu,
            XmNlabelString,text,XmNacceleratorText,accel,XmNmnemonic,XK_U,XmNsensitive,False,
            XmNbackground,WhitePixelOfScreen(XtScreen(board)),XmNforeground,BlackPixelOfScreen(XtScreen(board)),
            XmNinsensitiveForeground,color(board,"#0033cc"),XmNtopShadowColor,color(board,"magenta"),NULL);
        XmStringFree(text);XmStringFree(accel);
        XtRealizeWidget(shell);XtManageChild(menu);drain(app,d);
        glyphs(w,False,False,True);glyphs(w,True,False,True);
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(app,d);
        glyphs(w,False,True,True);glyphs(w,True,True,True);
        XtDestroyWidget(shell);drain(app,d);++cases;
    }
    printf("PASS disabled labels: %u cases\n",cases);XtCloseDisplay(d);XtDestroyApplicationContext(app);return 0;
}
