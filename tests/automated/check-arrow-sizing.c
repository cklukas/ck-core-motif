/* Toolkit-owned arrow targets, independent of gallery geometry overrides. */
#include <X11/IntrinsicP.h>
#include <X11/Xresource.h>
#include <Xm/XmAll.h>
#include <Xm/XmP.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static void drain(XtAppContext app,Display *d) { XSync(d,False);while(XtAppPending(app))XtAppProcessEvent(app,XtIMAll);XSync(d,False); }
static Dimension natural(Widget w) {
    XtWidgetGeometry reply={0};
    XtQueryGeometry(w,NULL,&reply);
    CHECK((reply.request_mode&(CWWidth|CWHeight))==(CWWidth|CWHeight));
    CHECK(reply.width==reply.height && reply.width>0);
    return reply.width;
}
static void size(Widget w,Dimension width,Dimension height) {
    Dimension actual_width,actual_height;
    XtVaGetValues(w,XmNwidth,&actual_width,XmNheight,&actual_height,NULL);
    if(actual_width!=width || actual_height!=height)
        fprintf(stderr,"%s: actual %ux%u expected %ux%u\n",XtName(w),actual_width,actual_height,width,height);
    CHECK(actual_width==width && actual_height==height);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *display;unsigned cases=0;
    const double scales[]={1,1.5,2};
    XtToolkitInitialize();app=XtCreateApplicationContext();
    display=XtOpenDisplay(app,NULL,"arrowSizing","ArrowSizing",NULL,0,&argc,argv);CHECK(display);
    {
        XrmDatabase db=XrmGetDatabase(display);
        XrmPutStringResource(&db,"*databaseArrow.width","71");
        XrmPutStringResource(&db,"*databaseArrow.height","0");
        XrmSetDatabase(display,db);
    }
    for(unsigned profile=0;profile<4;++profile) for(unsigned compact=0;compact<2;++compact)
    for(unsigned scale=0;scale<3;++scale) for(unsigned gadget=0;gadget<2;++gadget) {
        Widget shell,board,automatic,fixed,mixed,database;
        WidgetClass class=gadget?xmArrowButtonGadgetClass:xmArrowButtonWidgetClass;
        Dimension initial;
        CHECK(XmSetDisplayScaleOverride(display,scales[scale]));
        CHECK(XmApplyAppearanceDensity(display,compact?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
        CHECK(XmApplyAppearanceProfile(display,(XmAppearanceProfile)profile));
        shell=XtAppCreateShell("arrowSizing","ArrowSizing",applicationShellWidgetClass,display,NULL,0);
        board=XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,
            XmNwidth,600,XmNheight,400,XmNresizePolicy,XmRESIZE_NONE,NULL);
        automatic=XtVaCreateManagedWidget("automatic",class,board,NULL);
        fixed=XtVaCreateManagedWidget("fixed",class,board,XmNwidth,91,XmNheight,63,NULL);
        mixed=XtVaCreateManagedWidget("mixed",class,board,XmNwidth,91,NULL);
        database=XtVaCreateManagedWidget("databaseArrow",class,board,NULL);
        XtRealizeWidget(shell);drain(app,display);
        initial=natural(automatic);
        if(profile==XmAPPEARANCE_MODERN_LIGHT || profile==XmAPPEARANCE_MODERN_DARK)
            CHECK(initial==(Dimension)((compact?28:36)*scales[scale]+0.5));
        size(automatic,initial,initial);size(fixed,91,63);
        size(mixed,91,natural(mixed));size(database,71,natural(database));
        /* An equal setter still claims ownership of this axis. */
        XtVaSetValues(automatic,XmNwidth,initial,NULL);
        CHECK(XmSetDisplayScaleOverride(display,scales[scale]+0.5));drain(app,display);
        size(automatic,initial,natural(automatic));size(fixed,91,63);
        size(mixed,91,natural(mixed));size(database,71,natural(database));
        XmeConfigureObject(fixed,200,200,99,67,0);
        CHECK(XmApplyAppearanceDensity(display,compact?XmAPPEARANCE_DENSITY_COMFORTABLE:XmAPPEARANCE_DENSITY_COMPACT));
        drain(app,display);size(fixed,99,67);
        XtVaSetValues(fixed,XmNwidth,0,NULL);drain(app,display);
        size(fixed,natural(fixed),67);
        XtVaSetValues(fixed,XmNheight,0,NULL);drain(app,display);
        size(fixed,natural(fixed),natural(fixed));
        XtVaSetValues(fixed,XmNshadowThickness,9,NULL);drain(app,display);
        size(fixed,natural(fixed),natural(fixed));
        CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_MODERN_DARK));drain(app,display);
        size(fixed,natural(fixed),natural(fixed));size(database,71,natural(database));
        CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_CLASSIC));drain(app,display);
        size(fixed,natural(fixed),natural(fixed));size(mixed,91,natural(mixed));
        XtDestroyWidget(shell);drain(app,display);++cases;
    }
    XtCloseDisplay(display);XtDestroyApplicationContext(app);
    printf("PASS arrow sizing: %u cases\n",cases);return 0;
}
