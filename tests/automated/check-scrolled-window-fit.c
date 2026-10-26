/* A child that fills the visible axis must not introduce the other scrollbar. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(x) do {if(!(x)){fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#x);exit(1);}}while(0)
static void drain(XtAppContext app,Display *display) {
    XSync(display,False);
    while(XtAppPending(app))XtAppProcessEvent(app,XtIMAll);
    XSync(display,False);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *display,*anchor;
    unsigned scales[]={100,150,200,300};
    XmAppearanceProfile profiles[]={XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT,
        XmAPPEARANCE_MODERN_DARK,XmAPPEARANCE_HIGH_CONTRAST};
    unsigned cases=0;
    XtToolkitInitialize();app=XtCreateApplicationContext();
    /* Keep Xvfb alive while independent display lifetimes reset toolkit state. */
    anchor=XOpenDisplay(NULL);CHECK(anchor);
    for(unsigned p=0;p<4;++p)for(unsigned s=0;s<4;++s)
    for(unsigned axis=0;axis<2;++axis) {
        Widget shell,scroll,work,clip;Dimension width,height,after_width,after_height;
        display=XtOpenDisplay(app,NULL,"fitTest","FitTest",NULL,0,&argc,argv);CHECK(display);
        CHECK(XmSetAppearanceProfile(display,profiles[p]));
        CHECK(XmSetDisplayScaleOverride(display,scales[s]/100.0));
        shell=XtAppCreateShell("fitTest","FitTest",applicationShellWidgetClass,display,NULL,0);
        scroll=XtVaCreateManagedWidget("scroll",xmScrolledWindowWidgetClass,shell,
            XmNscrollingPolicy,XmAUTOMATIC,XmNscrollBarDisplayPolicy,XmAS_NEEDED,
            XmNwidth,300,XmNheight,240,NULL);
        work=XtVaCreateManagedWidget("work",xmDrawingAreaWidgetClass,scroll,
            XmNwidth,axis?1000:1,XmNheight,axis?1:1000,NULL);
        XtRealizeWidget(shell);drain(app,display);
        XtVaGetValues(scroll,XmNclipWindow,&clip,NULL);
        XtVaGetValues(clip,XmNwidth,&width,XmNheight,&height,NULL);
        for(unsigned repeat=0;repeat<5;++repeat) {
            XtVaSetValues(work,axis?XmNheight:XmNwidth,axis?height:width,NULL);
            drain(app,display);
            XtVaGetValues(clip,XmNwidth,&after_width,XmNheight,&after_height,NULL);
            if(after_width!=width || after_height!=height)
                fprintf(stderr,"profile=%u scale=%u axis=%u viewport %ux%u -> %ux%u\n",p,scales[s],axis,width,height,after_width,after_height);
            CHECK(width==after_width && height==after_height);
        }
        XtDestroyWidget(shell);drain(app,display);XtCloseDisplay(display);++cases;
    }
    XCloseDisplay(anchor);XtDestroyApplicationContext(app);
    printf("PASS stable scrolled viewport fit: %u cases\n",cases);return 0;
}
