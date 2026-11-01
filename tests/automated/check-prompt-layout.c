/* Prompt natural geometry and actual text placement, without host repairs. */
#include <Xm/PromptP.h>
#include <Xm/DrawingA.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/Shell.h>
#include <X11/Xutil.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1); } } while(0)
static void drain(XtAppContext app, Display *d) {
    XSync(d,False);
    for(unsigned n=0;XtAppPending(app);++n) {
        CHECK(n<10000);XtAppProcessEvent(app,XtIMAll);XSync(d,False);
    }
}
static int ink_right(Widget w, unsigned inset) {
    XmPromptWidget p=(XmPromptWidget)w;
    XImage *image;int right=-1;
    XtClass(w)->core_class.expose(w,NULL,NULL);XSync(XtDisplay(w),False);
    image=XGetImage(XtDisplay(w),XtWindow(w),0,0,p->core.width,p->core.height,AllPlanes,ZPixmap);
    CHECK(image);
    for(unsigned y=0;y<p->core.height;++y)for(unsigned x=0;x<p->core.width;++x)
        if(XGetPixel(image,x,y)!=p->core.background_pixel) {
            CHECK(x>=inset && x+inset<p->core.width);
            CHECK(y>=inset && y+inset<p->core.height);
            if((int)x>right)right=(int)x;
        }
    XDestroyImage(image);return right;
}
int main(int argc,char **argv) {
    XtAppContext app;Widget shell=XtVaAppInitialize(&app,"PromptLayout",NULL,0,&argc,argv,NULL,
        XmNwidth,1200,XmNheight,900,NULL);
    Display *d=XtDisplay(shell);unsigned cases=0;
    Widget host=XtVaCreateManagedWidget("host",xmDrawingAreaWidgetClass,shell,NULL);
    XtRealizeWidget(shell);
    for(unsigned dark=0;dark<2;++dark)for(unsigned scale=0;scale<3;++scale)
    for(unsigned rtl=0;rtl<2;++rtl) {
        double factor=1.0+scale*.5;
        CHECK(XmSetDisplayScaleOverride(d,factor));
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        XmString text=XmStringCreateLocalized("Prompt text");
        Widget w=XtVaCreateManagedWidget("prompt",xmPromptWidgetClass,host,
            XmNpromptText,text,XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);
        XmStringFree(text);drain(app,d);
        XmPromptWidget p=(XmPromptWidget)w;Dimension tw,th,natural_width=p->core.width,natural_height=p->core.height;
        XmStringExtent(p->prompt.font,p->prompt.text,&tw,&th);
        unsigned inset=(unsigned)(4*factor+.5),frame=p->primitive.highlight_thickness+p->primitive.shadow_thickness;
        if(frame>inset)inset=frame;
        CHECK(natural_width==tw+2*inset && natural_height==th+2*inset);
        int before=ink_right(w,inset);CHECK(before>=0);
        XtVaSetValues(w,XmNwidth,natural_width+50,NULL);drain(app,d);
        int after=ink_right(w,inset);CHECK(after-before==(rtl?50:0));
        XtVaSetValues(w,XmNwidth,2*inset+8,XmNheight,2*inset+8,NULL);drain(app,d);
        (void)ink_right(w,inset);
        XtVaSetValues(w,XmNwidth,0,XmNheight,0,NULL);drain(app,d);
        CHECK(p->core.width==natural_width && p->core.height==natural_height);
        text=XmStringCreateLocalized("A longer prompt\nwith a second line");
        XtVaSetValues(w,XmNpromptText,text,NULL);XmStringFree(text);drain(app,d);
        XmStringExtent(p->prompt.font,p->prompt.text,&tw,&th);
        CHECK(p->core.width==tw+2*inset && p->core.height==th+2*inset);
        (void)ink_right(w,inset);
        XtVaSetValues(w,XmNwidth,91,XmNheight,37,NULL);drain(app,d);
        XtWidgetGeometry preferred={0};XtQueryGeometry(w,NULL,&preferred);
        CHECK(preferred.width==91 && preferred.height==37);
        XtDestroyWidget(w);drain(app,d);++cases;
    }
    XtDestroyWidget(shell);XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS Prompt layout: %u theme/scale/direction configurations\n",cases);return 0;
}
