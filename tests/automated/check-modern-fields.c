/* Flat field borders must agree across text renderers and survive editing. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while (0)
static void drain(XtAppContext app,Display *display) {
    XSync(display,False); while(XtAppPending(app)) XtAppProcessEvent(app,XtIMAll); XSync(display,False);
}
static void frame(Widget field,XtAppContext app) {
    Dimension width,height,highlight,shadow; Pixel border; XImage *image;
    drain(app,XtDisplay(field));
    XtVaGetValues(field,XmNwidth,&width,XmNheight,&height,XmNhighlightThickness,&highlight,
        XmNshadowThickness,&shadow,XmNbottomShadowColor,&border,NULL);
    CHECK(shadow>0 && width>2*(highlight+shadow) && height>2*(highlight+shadow));
    image=XGetImage(XtDisplay(field),XtWindow(field),0,0,width,height,AllPlanes,ZPixmap); CHECK(image);
    for(unsigned i=0;i<shadow;++i) {
        CHECK(XGetPixel(image,width/2,highlight+i)==border);
        CHECK(XGetPixel(image,width/2,height-1-highlight-i)==border);
        CHECK(XGetPixel(image,highlight+i,height/2)==border);
        CHECK(XGetPixel(image,width-1-highlight-i,height/2)==border);
    }
    XDestroyImage(image);
}
int main(int argc,char **argv) {
    XtAppContext app; Display *display; unsigned cases=0; const double scales[]={1,1.5,2,3};
    XtToolkitInitialize(); app=XtCreateApplicationContext();
    display=XtOpenDisplay(app,NULL,"modernFields","ModernFields",NULL,0,&argc,argv); CHECK(display);
    for(unsigned dark=0;dark<2;++dark) for(unsigned compact=0;compact<2;++compact)
    for(unsigned scale=0;scale<XtNumber(scales);++scale) for(unsigned multi=0;multi<2;++multi) {
        Widget shell,field; XColor custom,exact; Dimension shadow;

        CHECK(XmSetDisplayScaleOverride(display,scales[scale]));
        CHECK(XmSetAppearanceDensity(display,compact?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
        CHECK(XmApplyAppearanceProfile(display,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        shell=XtAppCreateShell("modernFields","ModernFields",applicationShellWidgetClass,display,NULL,0);
        field=XtVaCreateManagedWidget("field",multi?xmTextWidgetClass:xmTextFieldWidgetClass,shell,
            XmNcolumns,12,XmNvalue,"Editable text",NULL);
        XtRealizeWidget(shell); frame(field,app);
        XSetInputFocus(display,XtWindow(shell),RevertToParent,CurrentTime); drain(app,display);
        CHECK(XmProcessTraversal(field,XmTRAVERSE_CURRENT)); drain(app,display);
        {
            Pixel highlight; Dimension width; XImage *image;
            XtVaGetValues(field,XmNhighlightColor,&highlight,XmNwidth,&width,NULL);
            image=XGetImage(display,XtWindow(field),width/2,0,1,1,AllPlanes,ZPixmap); CHECK(image);
            CHECK(XGetPixel(image,0,0)==highlight); XDestroyImage(image);
        }
        if(multi) XmTextReplace(field,0,8,"Updated"); else XmTextFieldReplace(field,0,8,"Updated");
        frame(field,app); XtSetSensitive(field,False); frame(field,app); XtSetSensitive(field,True);
        CHECK(XAllocNamedColor(display,DefaultColormap(display,DefaultScreen(display)),"#7c597a",&custom,&exact));
        XtVaSetValues(field,XmNshadowThickness,3,XmNbottomShadowColor,custom.pixel,NULL); frame(field,app);
        CHECK(XmApplyAppearanceProfile(display,dark?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));
        frame(field,app); XtVaGetValues(field,XmNshadowThickness,&shadow,NULL); CHECK(shadow==3);
        CHECK(XmApplyAppearanceProfile(display,dark?XmAPPEARANCE_HIGH_CONTRAST:XmAPPEARANCE_CLASSIC));
        drain(app,display);
        {
            Dimension width,height,highlight; Pixel top,bottom; XImage *image;
            XtVaGetValues(field,XmNwidth,&width,XmNheight,&height,XmNhighlightThickness,&highlight,
                XmNtopShadowColor,&top,XmNbottomShadowColor,&bottom,NULL);
            image=XGetImage(display,XtWindow(field),0,0,width,height,AllPlanes,ZPixmap); CHECK(image);
            CHECK(XGetPixel(image,width/2,highlight)==bottom);
            CHECK(XGetPixel(image,width/2,height-1-highlight)==top);
            XDestroyImage(image);
        }
        XtDestroyWidget(shell); drain(app,display); ++cases;
    }
    XtCloseDisplay(display); XtDestroyApplicationContext(app);
    printf("PASS modern fields: %u cases\n",cases); return 0;
}
