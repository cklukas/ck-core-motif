/* Link owns natural geometry; applications must not repair it after creation. */
#include <Xm/XmAll.h>
#include <X11/Xutil.h>
#include <Xm/Link.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x); exit(1); } } while (0)
static void drain(XtAppContext app, Display *d) {
    XSync(d,False); while(XtAppPending(app)) XtAppProcessEvent(app,XtIMAll); XSync(d,False);
}
static void natural(Widget link) {
    Dimension width,height,tw,th; XmString label; XmRenderTable font;
    XtWidgetGeometry preferred={0}, request={0};
    XtVaGetValues(link,XmNwidth,&width,XmNheight,&height,XmNlinkLabel,&label,XmNrenderTable,&font,NULL);
    CHECK(label && font);
    XmStringExtent(font,label,&tw,&th);
    XtQueryGeometry(link,NULL,&preferred);
    CHECK((preferred.request_mode & (CWWidth|CWHeight)) == (CWWidth|CWHeight));
    if(width!=preferred.width || height!=preferred.height)
        fprintf(stderr,"actual=%ux%u preferred=%ux%u text=%ux%u\n",width,height,preferred.width,preferred.height,tw,th);
    CHECK(width==preferred.width && height==preferred.height);
    CHECK(width>=tw && height>=th);
    {
        double scale; Pixel foreground; Boolean visited; CHECK(XmGetWidgetScale(link,&scale));
        XtVaGetValues(link,XtIsSensitive(link)?XmNforeground:XmNbottomShadowColor,&foreground,
            XmNlinkVisited,&visited,NULL);
        int underline_y=((int)height+th)/2+(int)(scale+0.5);
        XImage *image=XGetImage(XtDisplay(link),XtWindow(link),0,0,width,height,AllPlanes,ZPixmap);
        CHECK(image && underline_y<(int)height);
        CHECK(XGetPixel(image,width/2,underline_y)==foreground);
        if (visited) {
            int second=underline_y+2*(int)(scale+0.5);
            if (second>=height || XGetPixel(image,width/2,second)!=foreground)
                fprintf(stderr,"visited scale=%g size=%ux%u text=%ux%u underline=%d second=%d actual=%lu expected=%lu\n",scale,width,height,tw,th,underline_y,second,second<height?XGetPixel(image,width/2,second):0,foreground);
            CHECK(second<height && XGetPixel(image,width/2,second)==foreground);
        }
        XDestroyImage(image);
    }
    request.request_mode=CWWidth|CWHeight; request.width=preferred.width;request.height=preferred.height;
    CHECK(XtQueryGeometry(link,&request,&preferred)==XtGeometryYes);
}
int main(int argc,char **argv) {
    XtAppContext app; Display *d;unsigned cases=0;
    const double scales[]={1,1.5,2,3};
    XtToolkitInitialize();app=XtCreateApplicationContext();
    d=XtOpenDisplay(app,NULL,"linkGeometry","LinkGeometry",NULL,0,&argc,argv);CHECK(d);
    for(unsigned manager=0;manager<2;++manager)
    for(unsigned dark=0;dark<2;++dark) for(unsigned compact=0;compact<2;++compact)
    for(unsigned scale=0;scale<4;++scale) {
        Widget shell,board,link; Dimension width,height,after_width,after_height;
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmApplyAppearanceDensity(d,compact?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        shell=XtAppCreateShell("linkGeometry","LinkGeometry",applicationShellWidgetClass,d,NULL,0);
        XtVaSetValues(shell,XmNallowShellResize,True,NULL);
        if (manager)
            board=XtVaCreateManagedWidget("board",xmRowColumnWidgetClass,shell,
                XmNorientation,XmVERTICAL,XmNpacking,XmPACK_TIGHT,XmNadjustLast,False,NULL);
        else
            board=XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,
                XmNwidth,1400,XmNheight,700,XmNresizePolicy,XmRESIZE_NONE,NULL);
        XmString text=XmStringCreateLocalized("Open documentation");
        link=XtVaCreateManagedWidget("link",xmLinkWidgetClass,board,XmNlinkLabel,text,NULL);
        XmStringFree(text);XtRealizeWidget(shell);drain(app,d);natural(link);
        XtVaSetValues(link,XmNlinkVisited,True,NULL);drain(app,d);natural(link);
        XtSetSensitive(link,False);drain(app,d);natural(link);
        XtSetSensitive(link,True);XtVaSetValues(link,XmNlinkVisited,False,NULL);drain(app,d);natural(link);
        CHECK(XmApplyAppearanceDensity(d,compact?XmAPPEARANCE_DENSITY_COMFORTABLE:XmAPPEARANCE_DENSITY_COMPACT));drain(app,d);natural(link);
        CHECK(XmApplyAppearanceDensity(d,compact?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));drain(app,d);natural(link);
        XtVaGetValues(link,XmNwidth,&width,XmNheight,&height,NULL);
        /* Equal-valued assignments still transfer each dimension to the app. */
        XtVaSetValues(link,XmNwidth,width,XmNheight,height,NULL);
        text=XmStringCreateLocalized("Open documentation and examples\nAdditional details");
        XtVaSetValues(link,XmNlinkLabel,text,NULL);XmStringFree(text);drain(app,d);
        XtVaGetValues(link,XmNwidth,&after_width,XmNheight,&after_height,NULL);
        CHECK(width==after_width && height==after_height);
        XtVaSetValues(link,XmNwidth,0,XmNheight,0,NULL);drain(app,d);natural(link);
        XtVaGetValues(link,XmNwidth,&after_width,XmNheight,&after_height,NULL);
        CHECK(after_width>width && after_height>height);
        CHECK(XmSetDisplayScaleOverride(d,scales[(scale+1)%4]));drain(app,d);natural(link);
        XtVaSetValues(link,XmNwidth,777,XmNheight,111,NULL);
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));drain(app,d);
        XtVaGetValues(link,XmNwidth,&width,XmNheight,&height,NULL);CHECK(width==777 && height==111);
        XtVaSetValues(link,XmNwidth,0,XmNheight,0,NULL);drain(app,d);natural(link);
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(app,d);natural(link);
        XtDestroyWidget(shell);drain(app,d);++cases;
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS Link natural geometry: %u configurations\n",cases);return 0;
}
