/* Code-role typography must scale and remain monospaced without warnings. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x); return 1; } } while(0)
static unsigned warnings;
static void warning(String message) { ++warnings; fprintf(stderr,"Xt warning: %s\n",message); }
static int height(Widget w) {
    XmRenderTable font=NULL; int result=0;
    XtVaGetValues(w,XmNrenderTable,&font,NULL);
    if(font) XmRenderTableGetDefaultFontExtents(font,&result,NULL,NULL);
    return result;
}
static Dimension width(Widget w,const char *value) {
    XmRenderTable font=NULL;Dimension result=0,h=0;
    XmString text=XmStringCreateLocalized((char *)value);
    XtVaGetValues(w,XmNrenderTable,&font,NULL);
    if(font) XmStringExtent(font,text,&result,&h);
    XmStringFree(text);return result;
}
int main(int argc,char **argv) {
    XtAppContext app; Display *display;
    const double scales[]={1.0,1.5,2.0,3.0};
    unsigned cases=0;
    Boolean fallback_only=argc>1 && !strcmp(argv[1],"--fallback-only");
    XtToolkitInitialize();app=XtCreateApplicationContext();
    XtAppSetWarningHandler(app,warning);
    display=XtOpenDisplay(app,NULL,"codeFonts","CodeFonts",NULL,0,&argc,argv);CHECK(display);
    for(unsigned dark=0;dark<2;++dark) for(unsigned density=0;density<2;++density) {
        CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_CLASSIC));
        CHECK(XmSetDisplayScaleOverride(display,1.0));
        Widget shell=XtAppCreateShell("codeFonts","CodeFonts",applicationShellWidgetClass,display,NULL,0);
        Widget row=XtVaCreateManagedWidget("row",xmRowColumnWidgetClass,shell,NULL);
        Widget label=XtVaCreateManagedWidget("code",xmLabelWidgetClass,row,
            XmNappearanceRole,XmAPPEARANCE_ROLE_CODE,NULL);
        int classic_height=height(label),previous=0;
        CHECK(XmApplyAppearanceDensity(display,density?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
        CHECK(XmApplyAppearanceProfile(display,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        XtRealizeWidget(shell);
        for(unsigned i=0;i<XtNumber(scales);++i) {
            CHECK(XmSetDisplayScaleOverride(display,scales[i]));
            int actual=height(label);
            CHECK(actual>0 && (fallback_only || actual>previous));
            CHECK(width(label,"iiii")>0 && width(label,"iiii")==width(label,"WWWW"));
            Widget fresh=XtVaCreateManagedWidget("fresh",xmLabelWidgetClass,row,
                XmNappearanceRole,XmAPPEARANCE_ROLE_CODE,NULL);
            CHECK(height(fresh)==actual && width(fresh,"WWWW")==width(label,"WWWW"));
            XtDestroyWidget(fresh);
            previous=actual;++cases;
        }
        CHECK(XmSetDisplayScaleOverride(display,1.0));
        CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_CLASSIC));
        CHECK(height(label)==classic_height);
        XtDestroyWidget(shell);
    }
    CHECK(warnings==0);
    XtCloseDisplay(display);XtDestroyApplicationContext(app);
    printf("PASS code role fonts: %u scaled live/fresh cases\n",cases);return 0;
}
