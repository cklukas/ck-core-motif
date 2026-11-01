/* Native Divider measurement, line thickness and live layout checks. */
#include <Xm/XmAll.h>
#include <Xm/Divider.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Xutil.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1); } } while(0)
static void drain(XtAppContext app,Display *d) {
    for(unsigned n=0;n<10000;++n) { XSync(d,False);if(!XtAppPending(app))return;XtAppProcessEvent(app,XtIMAll); } CHECK(0);
}
static void natural(Widget w) {
    Dimension width,height,tw=0,th=0;XmString label;XmRenderTable font;XtWidgetGeometry preferred={0},request={0};
    XtVaGetValues(w,XmNwidth,&width,XmNheight,&height,XmNdividerLabel,&label,XmNrenderTable,&font,NULL);
    XmStringExtent(font,label,&tw,&th);XtQueryGeometry(w,NULL,&preferred);
    if(width!=preferred.width || height!=preferred.height || width<tw || height<th)
        fprintf(stderr,"divider actual=%ux%u preferred=%ux%u text=%ux%u\n",width,height,preferred.width,preferred.height,tw,th);
    CHECK(width==preferred.width && height==preferred.height && width>=tw && height>=th);
    request.request_mode=CWWidth|CWHeight;request.width=width;request.height=height;
    CHECK(XtQueryGeometry(w,&request,&preferred)==XtGeometryYes);
}
static void stroke(Widget w,unsigned thickness) {
    Dimension width,height;Pixel bg,fg;XtVaGetValues(w,XmNwidth,&width,XmNheight,&height,XmNbackground,&bg,XmNforeground,&fg,NULL);
    XImage *im=XGetImage(XtDisplay(w),XtWindow(w),0,0,width,height,AllPlanes,ZPixmap);CHECK(im);
    unsigned first=(height-thickness)/2;
    for(unsigned y=0;y<height;++y)CHECK(XGetPixel(im,0,y)==(y>=first && y<first+thickness?fg:bg));
    XDestroyImage(im);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *d;unsigned cases=0;const double scales[]={1,1.5,2,3};
    XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"dividerGeometry","DividerGeometry",NULL,0,&argc,argv);CHECK(d);
    for(unsigned manager=0;manager<2;++manager)for(unsigned dark=0;dark<2;++dark)
    for(unsigned scale=0;scale<4;++scale)for(unsigned rtl=0;rtl<2;++rtl) {
        CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMFORTABLE));
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        Widget shell=XtAppCreateShell("dividerGeometry","DividerGeometry",applicationShellWidgetClass,d,NULL,0);
        XtVaSetValues(shell,XmNallowShellResize,True,NULL);
        Widget board=manager?XtVaCreateManagedWidget("board",xmRowColumnWidgetClass,shell,XmNorientation,XmVERTICAL,XmNpacking,XmPACK_TIGHT,XmNadjustLast,False,NULL):XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,XmNwidth,1600,XmNheight,900,XmNresizePolicy,XmRESIZE_NONE,NULL);
        XmString label=XmStringCreateLocalized("Presentation\nSection details");
        Widget divider=XmVaCreateManagedDivider(board,"divider",XmNdividerLabel,label,XmNdividerThickness,5,
            XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);
        Widget bounds=XmVaCreateDivider(board,"bounds",XmNdividerLabel,label,
            XmNdividerSpacing,USHRT_MAX,XmNdividerThickness,USHRT_MAX,NULL);
        XtWidgetGeometry bounded={0};XtQueryGeometry(bounds,NULL,&bounded);
        CHECK(bounded.width==USHRT_MAX && bounded.height==USHRT_MAX);XtDestroyWidget(bounds);
        XmStringFree(label);XtRealizeWidget(shell);drain(app,d);natural(divider);
        Dimension spacing;XtVaGetValues(divider,XmNdividerSpacing,&spacing,NULL);CHECK(spacing==(Dimension)(8*scales[scale]));
        Pixel black=BlackPixelOfScreen(XtScreen(divider)),white=WhitePixelOfScreen(XtScreen(divider));
        XtVaSetValues(divider,XmNforeground,black,XmNbottomShadowColor,black,XmNbackground,white,NULL);drain(app,d);stroke(divider,5);
        XtVaSetValues(divider,XmNforeground,white,XmNbottomShadowColor,white,XmNbackground,black,NULL);drain(app,d);stroke(divider,5);
        CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMPACT));drain(app,d);natural(divider);
        XtVaGetValues(divider,XmNdividerSpacing,&spacing,NULL);CHECK(spacing==(Dimension)(6*scales[scale]));
        CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMFORTABLE));drain(app,d);natural(divider);
        XtVaSetValues(divider,XmNwidth,180,XmNheight,70,NULL);drain(app,d);
        label=XmStringCreateLocalized("A much longer section title\nSecond line\nThird line");
        XtVaSetValues(divider,XmNdividerLabel,label,NULL);XmStringFree(label);drain(app,d);
        Dimension width,height;XtVaGetValues(divider,XmNwidth,&width,XmNheight,&height,NULL);CHECK(width==180 && height==70);
        XtVaSetValues(divider,XmNwidth,0,XmNheight,0,NULL);drain(app,d);natural(divider);
        XtVaSetValues(divider,XmNdividerSpacing,13,NULL);drain(app,d);natural(divider);
        CHECK(XmSetDisplayScaleOverride(d,scales[(scale+1)%4]));drain(app,d);natural(divider);
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(app,d);natural(divider);
        XtVaGetValues(divider,XmNdividerSpacing,&spacing,NULL);CHECK(spacing==13);
        XtVaSetValues(divider,XmNdividerLabel,NULL,NULL);drain(app,d);natural(divider);stroke(divider,5);
        XtVaSetValues(divider,XmNdividerOrientation,XmVERTICAL,NULL);drain(app,d);
        XtWidgetGeometry pref={0};XtQueryGeometry(divider,NULL,&pref);
        XtVaGetValues(divider,XmNwidth,&width,XmNheight,&height,NULL);CHECK(width==pref.width && height==pref.height);
        for(unsigned size=1;size<8;++size){XtVaSetValues(divider,XmNwidth,size,XmNheight,size,NULL);drain(app,d);}
        label=XmStringCreateLocalized("Wide text in a tiny allocation");
        XtVaSetValues(divider,XmNdividerOrientation,XmHORIZONTAL,XmNdividerLabel,label,NULL);
        XmStringFree(label);drain(app,d);
        for(unsigned size=1;size<8;++size){XtVaSetValues(divider,XmNwidth,size,XmNheight,size,NULL);drain(app,d);}
        XtDestroyWidget(shell);drain(app,d);++cases;
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS divider geometry: %u configurations\n",cases);return 0;
}
