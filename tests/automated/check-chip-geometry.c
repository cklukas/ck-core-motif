/* Measured Chip geometry, live content and explicit-axis ownership. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1); } } while(0)
static unsigned after_destroy;
static void destroy_chip(Widget w, XtPointer closure, XtPointer data) {
    (void)closure; (void)data; XtDestroyWidget(w);
}
static void later_callback(Widget w, XtPointer closure, XtPointer data) {
    (void)w; (void)closure; (void)data; ++after_destroy;
}
static void drain(XtAppContext app,Display *d) {
    for(unsigned n=0;n<10000;++n) { XSync(d,False);if(!XtAppPending(app))return;XtAppProcessEvent(app,XtIMAll); } CHECK(0);
}
static void natural(Widget w,XmString expected) {
    Dimension width,height,tw,th;XmString text;XmRenderTable font;XtWidgetGeometry preferred={0},request={0};
    XtVaGetValues(w,XmNwidth,&width,XmNheight,&height,XmNchipLabel,&text,XmNrenderTable,&font,NULL);
    CHECK(text && font && XmStringCompare(text,expected));
    XmStringExtent(font,text,&tw,&th);XtQueryGeometry(w,NULL,&preferred);
    if(width!=preferred.width || height!=preferred.height || width<tw || height<th)
        fprintf(stderr,"chip actual=%ux%u preferred=%ux%u text=%ux%u\n",width,height,preferred.width,preferred.height,tw,th);
    CHECK(width==preferred.width && height==preferred.height && width>=tw && height>=th);
    request.request_mode=CWWidth|CWHeight;request.width=width;request.height=height;
    CHECK(XtQueryGeometry(w,&request,&preferred)==XtGeometryYes);
}
static void icon_pixel(Widget chip, unsigned icon_width, unsigned icon_height, Pixel color, Boolean rtl, double scale) {
    Dimension width,height,focus;XtVaGetValues(chip,XmNwidth,&width,XmNheight,&height,XmNhighlightThickness,&focus,NULL);
    int inset=focus+1+(int)(10*scale+0.5);
    int x=rtl?(int)width-inset-(int)icon_width:inset;
    int y=((int)height-(int)icon_height)/2;
    XImage *image=XGetImage(XtDisplay(chip),XtWindow(chip),0,0,width,height,AllPlanes,ZPixmap);CHECK(image);
    CHECK(x>=0 && y>=0 && x+icon_width<=width && y+icon_height<=height);
    CHECK(XGetPixel(image,x,y)==color && XGetPixel(image,x+icon_width-1,y+icon_height-1)==color);
    XDestroyImage(image);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *d;unsigned cases=0;const double scales[]={1,1.5,2,3};
    XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"chipGeometry","ChipGeometry",NULL,0,&argc,argv);CHECK(d);
    XColor color,exact;CHECK(XAllocNamedColor(d,DefaultColormap(d,DefaultScreen(d)),"magenta",&color,&exact));
    Pixmap icon=XCreatePixmap(d,DefaultRootWindow(d),23,31,DefaultDepth(d,DefaultScreen(d)));
    GC gc=XCreateGC(d,icon,0,NULL);XSetForeground(d,gc,color.pixel);XFillRectangle(d,icon,gc,0,0,23,31);XFreeGC(d,gc);
    char bits[32];for(unsigned i=0;i<sizeof(bits);++i)bits[i]=(char)0xff;
    Pixmap bitmap=XCreateBitmapFromData(d,DefaultRootWindow(d),bits,16,16);
    for(unsigned manager=0;manager<2;++manager)for(unsigned dark=0;dark<2;++dark)
    for(unsigned scale=0;scale<4;++scale)for(unsigned rtl=0;rtl<2;++rtl) {
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        Widget shell=XtAppCreateShell("chipGeometry","ChipGeometry",applicationShellWidgetClass,d,NULL,0);
        XtVaSetValues(shell,XmNallowShellResize,True,NULL);
        Widget board=manager?XtVaCreateManagedWidget("board",xmRowColumnWidgetClass,shell,XmNorientation,XmVERTICAL,XmNpacking,XmPACK_TIGHT,XmNadjustLast,False,NULL):XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,XmNwidth,1600,XmNheight,900,XmNresizePolicy,XmRESIZE_NONE,NULL);
        XmString text=XmStringCreateLocalized("Filter: active\nMore details");
        Widget chip=XtVaCreateManagedWidget("chip",xmChipWidgetClass,board,XmNchipLabel,text,
            XmNchipRemovable,True,XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);
        XtRealizeWidget(shell);drain(app,d);natural(chip,text);
        CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMPACT));drain(app,d);natural(chip,text);
        CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMFORTABLE));drain(app,d);natural(chip,text);
        XtVaSetValues(chip,XmNchipPixmap,icon,NULL);drain(app,d);natural(chip,text);
        icon_pixel(chip,23,31,color.pixel,rtl,scales[scale]);
        XtVaSetValues(chip,XmNchipPixmap,bitmap,NULL);drain(app,d);natural(chip,text);
        Pixel foreground;XtVaGetValues(chip,XmNforeground,&foreground,NULL);
        icon_pixel(chip,16,16,foreground,rtl,scales[scale]);
        XtVaSetValues(chip,XmNchipPixmap,XmUNSPECIFIED_PIXMAP,NULL);drain(app,d);natural(chip,text);
        Dimension width,height,plain_width,w2,h2;
        XtVaGetValues(chip,XmNwidth,&width,XmNheight,&height,NULL);
        XtVaSetValues(chip,XmNchipRemovable,False,NULL);drain(app,d);natural(chip,text);
        XtVaGetValues(chip,XmNwidth,&plain_width,NULL);CHECK(plain_width<width);
        XtVaSetValues(chip,XmNchipRemovable,True,NULL);drain(app,d);natural(chip,text);
        XtVaGetValues(chip,XmNwidth,&w2,NULL);CHECK(w2==width);
        XtVaSetValues(chip,XmNwidth,width,XmNheight,height,NULL);
        XmStringFree(text);text=XmStringCreateLocalized("A much longer filter label\nSecond line\nThird line");
        XtVaSetValues(chip,XmNchipLabel,text,NULL);drain(app,d);
        XtVaGetValues(chip,XmNwidth,&w2,XmNheight,&h2,NULL);CHECK(w2==width && h2==height);
        XtVaSetValues(chip,XmNwidth,0,XmNheight,0,NULL);drain(app,d);natural(chip,text);
        CHECK(XmSetDisplayScaleOverride(d,scales[(scale+1)%4]));drain(app,d);natural(chip,text);
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(app,d);natural(chip,text);
        XmStringFree(text);XtDestroyWidget(shell);drain(app,d);++cases;
    }
    for (unsigned removal=0; removal<2; ++removal) {
        Widget shell=XtVaAppCreateShell("chipLifetime","ChipLifetime",applicationShellWidgetClass,d,NULL);
        Widget chip=XtVaCreateManagedWidget("chip",xmChipWidgetClass,shell,XmNchipRemovable,True,NULL);
        const char *resource=removal?XmNchipRemoveCallback:XmNchipActivateCallback;
        XtAddCallback(chip,resource,destroy_chip,NULL);
        XtAddCallback(chip,resource,later_callback,NULL);
        XtRealizeWidget(shell);
        if(removal)XmChipRemove(chip,NULL);else XmChipInvoke(chip,NULL);
        drain(app,d);CHECK(after_destroy==0);XtDestroyWidget(shell);drain(app,d);
    }
    XFreePixmap(d,icon);XFreePixmap(d,bitmap);XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS chip natural geometry: %u configurations\n",cases);return 0;
}
