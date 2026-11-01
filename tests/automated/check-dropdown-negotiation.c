/* Refused geometry and partial query replies must never become X requests. */
#include <X11/IntrinsicP.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL line %d: %s\n",__LINE__,#c); exit(1); } } while (0)
static unsigned refusals;
static XtGeometryResult deny(Widget w,XtWidgetGeometry *request,XtWidgetGeometry *reply) {
    (void)w;(void)request;(void)reply; ++refusals; return XtGeometryNo;
}
static XtGeometryResult width_only(Widget w,XtWidgetGeometry *request,XtWidgetGeometry *reply) {
    (void)request;
    memset(reply,0xa5,sizeof(*reply));
    reply->request_mode=CWWidth;reply->width=w->core.width;
    return XtGeometryAlmost;
}
static void settle(XtAppContext app,Display *d) {
    for(unsigned i=0;i<10000;++i) { XSync(d,False);if(!XtAppPending(app))return;XtAppProcessEvent(app,XtIMAll); }
    CHECK(0);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *d;unsigned cases=0;
    XmAppearanceProfile profiles[]={XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT,XmAPPEARANCE_MODERN_DARK};
    XtToolkitInitialize();app=XtCreateApplicationContext();
    d=XtOpenDisplay(app,NULL,"dropdownGeometry","DropdownGeometry",NULL,0,&argc,argv);CHECK(d);
    for(unsigned p=0;p<3;++p)for(unsigned scale=0;scale<2;++scale) {
        CHECK(XmSetDisplayScaleOverride(d,scale?1.5:1));CHECK(XmApplyAppearanceProfile(d,profiles[p]));
        Widget shell=XtAppCreateShell("dropdownGeometry","DropdownGeometry",applicationShellWidgetClass,d,NULL,0);
        Widget board=XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,XmNwidth,600,XmNheight,300,NULL);
        Widget drop=XtVaCreateManagedWidget("drop",xmDropDownWidgetClass,board,XmNshowLabel,True,NULL);
        XtRealizeWidget(shell);settle(app,d);
        Widget label=XmDropDownGetLabel(drop);CHECK(label);
        XtGeometryHandler original_query=XtClass(label)->core_class.query_geometry;
        XtClass(label)->core_class.query_geometry=width_only;
        XtWidgetGeometry preferred={0};XtQueryGeometry(drop,NULL,&preferred);
        CHECK(preferred.height>0 && preferred.height<300);
        CHECK(preferred.width>0 && preferred.width<600);
        XtClass(label)->core_class.query_geometry=original_query;
        CompositeWidgetClass parent_class=(CompositeWidgetClass)XtClass(board);
        XtGeometryHandler original_manager=parent_class->composite_class.geometry_manager;
        parent_class->composite_class.geometry_manager=deny;
        Dimension width=label->core.width,height=label->core.height;
        for(unsigned query=0;query<2;++query)for(unsigned both=0;both<2;++both) {
            XtWidgetGeometry request={0},reply;
            request.request_mode=CWHeight|(both?CWWidth:0)|(query?XtCWQueryOnly:0);
            request.height=1234;request.width=width+100;
            memset(&reply,0xa5,sizeof(reply));
            CHECK(((CompositeWidgetClass)XtClass(drop))->composite_class.geometry_manager(label,&request,&reply)==XtGeometryNo);
            CHECK(label->core.width==width && label->core.height==height);
            CHECK(reply.request_mode==0);++cases;
        }
        parent_class->composite_class.geometry_manager=original_manager;
        XtDestroyWidget(shell);settle(app,d);
    }
    CHECK(refusals==cases);
    /* FontSelector embeds DropDown and exposed the live-transaction failure. */
    for(unsigned cycle=0;cycle<12;++cycle) {
        Widget shell=XtAppCreateShell("fontLifecycle","FontLifecycle",applicationShellWidgetClass,d,NULL,0);
        XtVaCreateManagedWidget("selector",xmFontSelectorWidgetClass,shell,NULL);
        XtRealizeWidget(shell);settle(app,d);
        for(unsigned step=0;step<4;++step) {
            XmAppearanceProfile profile=step==1?XmAPPEARANCE_MODERN_DARK:step==2?XmAPPEARANCE_CLASSIC:XmAPPEARANCE_MODERN_LIGHT;
            CHECK(XmApplyAppearanceProfile(d,profile));
            CHECK(XmApplyAppearanceDensity(d,step%2?XmAPPEARANCE_DENSITY_COMPACT:XmAPPEARANCE_DENSITY_COMFORTABLE));
            CHECK(XmSetDisplayScaleOverride(d,step==1?1.5:step==2?2:1));settle(app,d);
        }
        XtDestroyWidget(shell);settle(app,d);
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS DropDown negotiation: %u refusals and 48 FontSelector transitions\n",cases);return 0;
}
