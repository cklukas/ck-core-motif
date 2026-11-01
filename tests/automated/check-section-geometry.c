/* Composite sections must size themselves without gallery repairs. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if(!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1); } } while(0)
static void drain(XtAppContext app,Display *d) {
    for(unsigned i=0;i<10000;++i) { XSync(d,False);if(!XtAppPending(app))return;XtAppProcessEvent(app,XtIMAll); } CHECK(0);
}
static void natural(Widget w) {
    Dimension width,height;XtWidgetGeometry preferred={0};
    XtVaGetValues(w,XmNwidth,&width,XmNheight,&height,NULL);XtQueryGeometry(w,NULL,&preferred);
    if(width<preferred.width || height<preferred.height)
        fprintf(stderr,"%s actual=%ux%u preferred=%ux%u\n",XtName(w),width,height,preferred.width,preferred.height);
    if(width<preferred.width || height<preferred.height) {
        WidgetList kids;Cardinal count;
        XtVaGetValues(w,XmNchildren,&kids,XmNnumChildren,&count,NULL);
        for(Cardinal i=0;i<count;++i) {
            XtWidgetGeometry child={0};Dimension cw,ch;
            XtQueryGeometry(kids[i],NULL,&child);
            XtVaGetValues(kids[i],XmNwidth,&cw,XmNheight,&ch,NULL);
            fprintf(stderr,"child %s actual=%ux%u preferred=%ux%u mode=%lx\n",
                XtName(kids[i]),cw,ch,child.width,child.height,(unsigned long)child.request_mode);
        }
    }
    CHECK(width>=preferred.width && height>=preferred.height);
}
static void section_metrics(Widget section,Display *d) {
    XmAppearanceProfile profile;double scale;Dimension mw,mh,spacing,inner;
    CHECK(XmGetAppearanceProfile(d,&profile));CHECK(XmGetWidgetScale(section,&scale));
    XtVaGetValues(section,XmNformSectionMarginWidth,&mw,XmNformSectionMarginHeight,&mh,
        XmNformSectionSpacing,&spacing,NULL);
    XtVaGetValues(XmFormSectionGetContent(section),XmNspacing,&inner,NULL);
    if(profile==XmAPPEARANCE_CLASSIC) CHECK(mw==6 && mh==4 && spacing==4 && inner==3);
    else {
        CHECK(mw==(Dimension)(12*scale+0.5) && mh==(Dimension)(12*scale+0.5));
        CHECK(spacing==(Dimension)(8*scale+0.5) && inner==(Dimension)(8*scale+0.5));
    }
}
static void contained(Widget w) {
    WidgetList children;Cardinal count;Dimension width,height;
    XtVaGetValues(w,XmNchildren,&children,XmNnumChildren,&count,XmNwidth,&width,XmNheight,&height,NULL);
    for(Cardinal i=0;i<count;++i) {
        Position x,y;Dimension cw,ch,border;Boolean mapped;
        XtVaGetValues(children[i],XmNx,&x,XmNy,&y,XmNwidth,&cw,XmNheight,&ch,XmNborderWidth,&border,XmNmappedWhenManaged,&mapped,NULL);
        if(!XtIsManaged(children[i]) || !mapped)continue;
        if(x<0 || y<0 || (unsigned)x+cw+2*border>width || (unsigned)y+ch+2*border>height)
            fprintf(stderr,"%s child=%s at %d,%d %ux%u in %ux%u\n",XtName(w),XtName(children[i]),x,y,cw,ch,width,height);
        CHECK(x>=0 && y>=0 && (unsigned)x+cw+2*border<=width && (unsigned)y+ch+2*border<=height);
    }
}
int main(int argc,char **argv) {
    XtAppContext app;Display *d;unsigned cases=0;double scales[]={1,1.5,2,3};
    XmAppearanceProfile profiles[]={XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT,XmAPPEARANCE_MODERN_DARK};
    XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"sections","Sections",NULL,0,&argc,argv);CHECK(d);
    for(unsigned kind=0;kind<4;++kind)for(unsigned manager=0;manager<2;++manager)
    for(unsigned profile=0;profile<3;++profile)for(unsigned scale=0;scale<4;++scale) {
        fprintf(stderr,"CASE kind=%u manager=%u profile=%u scale=%g\n",kind,manager,profile,scales[scale]);
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));CHECK(XmApplyAppearanceProfile(d,profiles[profile]));
        Widget shell=XtAppCreateShell("sections","Sections",applicationShellWidgetClass,d,NULL,0);
        XtVaSetValues(shell,XmNallowShellResize,True,NULL);
        /* Leave room for three explicit cells at 300% scale. This fixture
         * tests natural growth; constrained allocations are tested separately. */
        Widget board=manager?XtVaCreateManagedWidget("board",xmRowColumnWidgetClass,shell,XmNorientation,XmVERTICAL,XmNpacking,XmPACK_TIGHT,XmNadjustLast,False,NULL):XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,XmNwidth,2400,XmNheight,1600,XmNresizePolicy,XmRESIZE_NONE,NULL);
        XmString text=XmStringCreateLocalized("Document settings");
        Widget section,content;
        if(kind<2) {
            section=XtVaCreateWidget("section",kind?xmDisclosureWidgetClass:xmFormSectionWidgetClass,board,
                kind?XmNlabelString:XmNformSectionHeader,text,NULL);
            fprintf(stderr,"initial\n");natural(section);XtManageChild(section);
            content=kind?XmDisclosureGetContent(section):XmFormSectionGetContent(section);
            XmStringFree(text);text=XmStringCreateLocalized("Keep a backup copy\nSave changes automatically");
            Widget details=XtVaCreateManagedWidget("details",xmLabelWidgetClass,content,XmNlabelString,text,NULL);
            if (!kind) {
                Dimension before_left,before_right,after_left,after_right;
                XtVaGetValues(details,XmNmarginLeft,&before_left,XmNmarginRight,&before_right,NULL);
                XtVaCreateManagedWidget("option",xmToggleButtonWidgetClass,content,XmNlabelString,text,NULL);
                XtVaGetValues(details,XmNmarginLeft,&after_left,XmNmarginRight,&after_right,NULL);
                CHECK(before_left==after_left && before_right==after_right);
            }
            if(kind)XmDisclosureSetExpanded(section,True,False);
            else XtVaSetValues(section,XmNformSectionFooter,text,NULL);
        } else {
            section=XtVaCreateManagedWidget("section",kind==2?xmIconBoxWidgetClass:xmColumnWidgetClass,board,NULL);
            content=section;
            for(unsigned item=0;item<3;++item) {
                if(kind==2) XtVaCreateManagedWidget("item",xmIconButtonWidgetClass,section,
                    XmNlabelString,text,XmNcellX,item,XmNcellY,0,NULL);
                else XtVaCreateManagedWidget("field",xmTextFieldWidgetClass,section,
                    XmNentryLabelString,text,XmNcolumns,18,XmNvalue,"Quarterly report",NULL);
            }
        }
        XmStringFree(text);
        XtRealizeWidget(shell);drain(app,d);fprintf(stderr,"realized\n");natural(section);contained(section);contained(content);
        if(kind==0)section_metrics(section,d);
        CHECK(XmSetDisplayScaleOverride(d,scales[(scale+1)%4]));
        CHECK(XmApplyAppearanceProfile(d,profiles[(profile+1)%3]));drain(app,d);
        fprintf(stderr,"live change\n");natural(section);contained(section);contained(content);
        if(kind==0) {
            Dimension margin,spacing;
            section_metrics(section,d);
            XtVaSetValues(section,XmNformSectionMarginWidth,17,XmNformSectionSpacing,19,NULL);
            CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_CLASSIC));
            CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_DARK));drain(app,d);
            XtVaGetValues(section,XmNformSectionMarginWidth,&margin,XmNformSectionSpacing,&spacing,NULL);
            CHECK(margin==17 && spacing==19);natural(section);contained(section);
        }
        if(kind==1) {
            Dimension expanded,collapsed;XtVaGetValues(section,XmNheight,&expanded,NULL);
            XmDisclosureSetExpanded(section,False,False);drain(app,d);natural(section);contained(section);
            XtVaGetValues(section,XmNheight,&collapsed,NULL);CHECK(collapsed<expanded);
            XmDisclosureSetExpanded(section,True,False);drain(app,d);natural(section);contained(section);
        }
        XtDestroyWidget(shell);drain(app,d);++cases;
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS section natural geometry: %u configurations\n",cases);return 0;
}
