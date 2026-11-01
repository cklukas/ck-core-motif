/* Avatar natural size, appearance ownership and independent explicit axes. */
#include <Xm/XmAll.h>
#include <Xm/Avatar.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1); } } while(0)
static void drain(XtAppContext app,Display *d) {
    for(unsigned n=0;n<10000;++n) { XSync(d,False);if(!XtAppPending(app))return;XtAppProcessEvent(app,XtIMAll); } CHECK(0);
}
static void geometry(Widget w,Dimension expected_width,Dimension expected_height) {
    Dimension width,height;XtWidgetGeometry preferred={0},request={0};
    XtVaGetValues(w,XmNwidth,&width,XmNheight,&height,NULL);
    XtQueryGeometry(w,NULL,&preferred);
    if(width!=expected_width || height!=expected_height)
        fprintf(stderr,"avatar actual=%ux%u expected=%ux%u preferred=%ux%u\n",width,height,expected_width,expected_height,preferred.width,preferred.height);
    CHECK(width==expected_width && height==expected_height);
    CHECK(preferred.width==expected_width && preferred.height==expected_height);
    request.request_mode=CWWidth|CWHeight;request.width=width;request.height=height;
    CHECK(XtQueryGeometry(w,&request,&preferred)==XtGeometryYes);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *d;unsigned cases=0;const double scales[]={1,1.5,2,3};
    XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"avatarGeometry","AvatarGeometry",NULL,0,&argc,argv);CHECK(d);
    for(unsigned manager=0;manager<2;++manager)for(unsigned dark=0;dark<2;++dark)
    for(unsigned scale=0;scale<4;++scale) {
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMFORTABLE));
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        Widget shell=XtAppCreateShell("avatarGeometry","AvatarGeometry",applicationShellWidgetClass,d,NULL,0);
        XtVaSetValues(shell,XmNallowShellResize,True,NULL);
        Widget board=manager?XtVaCreateManagedWidget("board",xmRowColumnWidgetClass,shell,XmNorientation,XmVERTICAL,XmNpacking,XmPACK_TIGHT,XmNadjustLast,False,NULL):XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,XmNwidth,1000,XmNheight,600,XmNresizePolicy,XmRESIZE_NONE,NULL);
        Widget avatar=XmVaCreateManagedAvatar(board,"avatar",NULL);
        XtRealizeWidget(shell);drain(app,d);
        geometry(avatar,(Dimension)(48*scales[scale]),(Dimension)(48*scales[scale]));
        CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMPACT));drain(app,d);
        geometry(avatar,(Dimension)(40*scales[scale]),(Dimension)(40*scales[scale]));
        CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMFORTABLE));drain(app,d);
        geometry(avatar,(Dimension)(48*scales[scale]),(Dimension)(48*scales[scale]));
        CHECK(XmSetDisplayScaleOverride(d,scales[(scale+1)%4]));drain(app,d);
        geometry(avatar,(Dimension)(48*scales[(scale+1)%4]),(Dimension)(48*scales[(scale+1)%4]));
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(app,d);geometry(avatar,32,32);
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));drain(app,d);
        geometry(avatar,(Dimension)(48*scales[(scale+1)%4]),(Dimension)(48*scales[(scale+1)%4]));
        XtVaSetValues(avatar,XmNavatarSize,60,NULL);drain(app,d);geometry(avatar,60,60);
        XtVaSetValues(avatar,XmNwidth,75,NULL);drain(app,d);geometry(avatar,75,60);
        XtVaSetValues(avatar,XmNavatarSize,90,NULL);drain(app,d);geometry(avatar,75,90);
        XtVaSetValues(avatar,XmNheight,50,NULL);drain(app,d);geometry(avatar,75,50);
        CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMPACT));drain(app,d);geometry(avatar,75,50);
        XtVaSetValues(avatar,XmNwidth,0,NULL);drain(app,d);geometry(avatar,90,50);
        XtVaSetValues(avatar,XmNheight,0,NULL);drain(app,d);geometry(avatar,90,90);
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));drain(app,d);geometry(avatar,90,90);
        XtVaSetValues(avatar,XmNavatarSize,0,NULL);drain(app,d);geometry(avatar,1,1);
        XtDestroyWidget(avatar);drain(app,d);
        avatar=XmVaCreateManagedAvatar(board,"explicitAvatar",XmNavatarSize,55,XmNwidth,75,NULL);
        drain(app,d);geometry(avatar,75,55);
        CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_LIGHT));
        CHECK(XmApplyAppearanceDensity(d,XmAPPEARANCE_DENSITY_COMFORTABLE));
        CHECK(XmSetDisplayScaleOverride(d,1));drain(app,d);geometry(avatar,75,55);
        XtDestroyWidget(shell);drain(app,d);++cases;
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS avatar geometry: %u configurations\n",cases);return 0;
}
