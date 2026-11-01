/* Tab padding follows palette/density/scale without overriding application values. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/TabStack.h>
#include <Xm/TabBox.h>
#include <X11/Shell.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) {fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#c);exit(1);} } while(0)
static unsigned cases;
static void drain(XtAppContext a, Display *d) {
    XSync(d,False); while(XtAppPending(a)) XtAppProcessEvent(a,XtIMAll); XSync(d,False);
}
static void padding(Widget w, unsigned width, unsigned height) {
    Dimension x,y; XtVaGetValues(w,XmNtabMarginWidth,&x,XmNtabMarginHeight,&y,NULL);
    if(x!=width||y!=height)fprintf(stderr,"%s padding %u,%u expected %u,%u\n",XtName(w),x,y,width,height);
    CHECK(x==width && y==height);++cases;
}
int main(int argc,char **argv) {
    XtAppContext a; Display *d; double scales[]={1,1.5,2};
    XtToolkitInitialize();a=XtCreateApplicationContext();d=XtOpenDisplay(a,NULL,"padding","Padding",NULL,0,&argc,argv);CHECK(d);
    for(unsigned dark=0;dark<2;++dark)for(unsigned scale=0;scale<3;++scale)for(unsigned compact=0;compact<2;++compact)for(unsigned startup=0;startup<2;++startup) {
        XmAppearanceProfile profile=dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT;
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmSetAppearanceDensity(d,compact?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
        CHECK(XmApplyAppearanceProfile(d,startup?profile:XmAPPEARANCE_CLASSIC));
        Widget shell=XtAppCreateShell("padding","Padding",applicationShellWidgetClass,d,NULL,0);
        XtVaSetValues(shell,XtNwidth,1000,XtNheight,900,NULL);
        Widget row=XtVaCreateManagedWidget("row",xmRowColumnWidgetClass,shell,NULL);
        Widget stack=XtVaCreateManagedWidget("stack",xmTabStackWidgetClass,row,NULL);
        XmString text=XmStringCreateLocalized("Overview");
        XtVaCreateManagedWidget("page",xmLabelWidgetClass,stack,XmNtabLabelString,text,XmNlabelString,text,NULL);XmStringFree(text);
        Widget tabs=XtNameToWidget(stack,"tabBox");CHECK(tabs);
        Widget standalone=XtVaCreateManagedWidget("standalone",xmTabBoxWidgetClass,row,NULL);
        Widget explicit=XtVaCreateManagedWidget("explicit",xmTabStackWidgetClass,row,XmNtabMarginWidth,19,XmNtabMarginHeight,5,NULL);
        XtRealizeWidget(shell);drain(a,d);
        CHECK(XmApplyAppearanceProfile(d,profile));drain(a,d);
        unsigned w=(unsigned)((compact?8:12)*scales[scale]+.5), h=(unsigned)((compact?3:6)*scales[scale]+.5);
        padding(stack,w,h);padding(tabs,w,h);padding(standalone,w,h);padding(explicit,19,5);
        /* Equal-valued assignments still transfer ownership. */
        XtVaSetValues(standalone,XmNtabMarginWidth,w,XmNtabMarginHeight,h,NULL);
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(a,d);
        padding(stack,3,3);padding(tabs,3,3);padding(standalone,w,h);padding(explicit,19,5);
        CHECK(XmApplyAppearanceProfile(d,profile));drain(a,d);
        padding(stack,w,h);padding(tabs,w,h);padding(standalone,w,h);padding(explicit,19,5);
        CHECK(XmApplyAppearanceDensity(d,compact?XmAPPEARANCE_DENSITY_COMFORTABLE:XmAPPEARANCE_DENSITY_COMPACT));drain(a,d);
        unsigned other_w=(unsigned)((compact?12:8)*scales[scale]+.5);
        unsigned other_h=(unsigned)((compact?6:3)*scales[scale]+.5);
        padding(stack,other_w,other_h);padding(tabs,other_w,other_h);
        padding(standalone,w,h);padding(explicit,19,5);
        CHECK(XmSetDisplayScaleOverride(d,1.25));drain(a,d);
        other_w=(unsigned)((compact?12:8)*1.25+.5);
        other_h=(unsigned)((compact?6:3)*1.25+.5);
        padding(stack,other_w,other_h);padding(tabs,other_w,other_h);
        padding(standalone,w,h);padding(explicit,19,5);
        XtDestroyWidget(shell);drain(a,d);
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(a);printf("PASS tab padding: %u ownership cases\n",cases);return 0;
}
