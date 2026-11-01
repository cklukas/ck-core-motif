/* Explicit geometry must survive deferred appearance/font application. */
#include <X11/IntrinsicP.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static void size(Widget w,Dimension *width,Dimension *height) {
    XtVaGetValues(w,XmNwidth,width,XmNheight,height,NULL);
}
int main(int argc,char **argv) {
    XtAppContext app; Display *display; unsigned cases=0;
    const double scales[]={1,1.5,2,3};
    XtToolkitInitialize(); app=XtCreateApplicationContext();
    display=XtOpenDisplay(app,NULL,"appearanceDimensions","AppearanceDimensions",NULL,0,&argc,argv); CHECK(display);
    {
        XrmDatabase database=XrmGetDatabase(display);
        XrmPutStringResource(&database,"*fromDatabase.width","333");
        XrmPutStringResource(&database,"*fromDatabase.height","87");
        XrmPutStringResource(&database,"*naturalDatabase.width","0");
        XrmSetDatabase(display,database);
    }
    for(unsigned multi=0;multi<2;++multi) for(unsigned scale=0;scale<XtNumber(scales);++scale)
    for(unsigned mode=0;mode<4;++mode) {
        Widget shell,form,field; Arg args[4]; Cardinal count=0;
        Dimension width,height,initial_width,initial_height;
        CHECK(XmSetDisplayScaleOverride(display,scales[scale]));
        CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_MODERN_LIGHT));
        shell=XtAppCreateShell("appearanceDimensions","AppearanceDimensions",applicationShellWidgetClass,display,NULL,0);
        form=XtVaCreateManagedWidget("form",xmDrawingAreaWidgetClass,shell,XmNresizePolicy,XmRESIZE_NONE,XmNwidth,1600,XmNheight,1000,NULL);
        XtSetArg(args[count],XmNcolumns,12); ++count;
        if(mode&1) {XtSetArg(args[count],XmNwidth,300); ++count;}
        if(mode&2) {XtSetArg(args[count],XmNheight,90); ++count;}
        field=XtCreateManagedWidget("field",multi?xmTextWidgetClass:xmTextFieldWidgetClass,form,args,count);
        size(field,&width,&height); initial_width=width; initial_height=height;
        if(mode&1) CHECK(width==300);
        if(mode&2) CHECK(height==90);
        CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_MODERN_DARK));
        CHECK(XmSetDisplayScaleOverride(display,scales[scale]*2));
        size(field,&width,&height);
        if(mode&1) CHECK(width==300); else CHECK(width>initial_width);
        if(mode&2) CHECK(height==90); else CHECK(height>initial_height);
        /* Equal-valued setters still claim ownership, parent allocations do not
         * replace the constraint with an obsolete requested size. */
        XtVaSetValues(field,XmNwidth,width,XmNheight,height,NULL);
        XtResizeWidget(field,420,110,0);
        size(field,&width,&height); CHECK(width==420 && height==110);
        CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_MODERN_LIGHT));
        size(field,&width,&height); CHECK(width==420 && height==110);
        XtVaSetValues(field,XmNwidth,0,NULL);
        CHECK(XmSetDisplayScaleOverride(display,scales[scale]));
        size(field,&width,&height); CHECK(width>0 && width!=420 && height==110);
        XtDestroyWidget(shell); ++cases;
    }
    {
        for(unsigned multi=0;multi<2;++multi) {
            Widget shell,form,field,natural; Dimension width,height,initial_width;
            CHECK(XmSetDisplayScaleOverride(display,1));
            CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_MODERN_LIGHT));
            shell=XtAppCreateShell("appearanceDimensions","AppearanceDimensions",applicationShellWidgetClass,display,NULL,0);
            form=XtVaCreateManagedWidget("form",xmDrawingAreaWidgetClass,shell,XmNresizePolicy,XmRESIZE_NONE,XmNwidth,1600,XmNheight,1000,NULL);
            field=XtVaCreateManagedWidget("fromDatabase",multi?xmTextWidgetClass:xmTextFieldWidgetClass,form,XmNcolumns,12,NULL);
            natural=XtVaCreateManagedWidget("naturalDatabase",multi?xmTextWidgetClass:xmTextFieldWidgetClass,form,XmNcolumns,12,NULL);
            size(field,&width,&height); CHECK(width==333 && height==87);
            size(natural,&initial_width,&height);
            CHECK(XmSetDisplayScaleOverride(display,2));
            size(field,&width,&height); CHECK(width==333 && height==87);
            size(natural,&width,&height); CHECK(width>initial_width);
            XtDestroyWidget(shell); ++cases;
        }
    }
    {
        WidgetClass classes[]={xmLabelWidgetClass,xmLabelGadgetClass,
            xmPushButtonWidgetClass,xmPushButtonGadgetClass,xmDrawnButtonWidgetClass};
        for(unsigned cls=0;cls<XtNumber(classes);++cls) for(unsigned profile=0;profile<4;++profile)
        for(unsigned mode=1;mode<4;++mode) {
            Widget shell,board,w;Dimension width,height;Arg args[3];Cardinal count=0;
            XmString text=XmStringCreateLocalized("A longer replacement label");
            CHECK(XmSetDisplayScaleOverride(display,1));
            CHECK(XmApplyAppearanceProfile(display,(XmAppearanceProfile)profile));
            shell=XtAppCreateShell("labelDimensions","LabelDimensions",applicationShellWidgetClass,display,NULL,0);
            board=XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,
                XmNresizePolicy,XmRESIZE_NONE,XmNwidth,1200,XmNheight,600,NULL);
            if(mode&1) {XtSetArg(args[count],XmNwidth,300);++count;}
            if(mode&2) {XtSetArg(args[count],XmNheight,90);++count;}
            w=XtCreateManagedWidget("label",classes[cls],board,args,count);
            size(w,&width,&height);CHECK(!(mode&1)||width==300);CHECK(!(mode&2)||height==90);
            XtSetArg(args[count],XmNlabelString,text);++count;
            XtSetValues(w,args,count);XmStringFree(text);
            size(w,&width,&height);CHECK(!(mode&1)||width==300);CHECK(!(mode&2)||height==90);
            CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_MODERN_DARK));
            CHECK(XmSetDisplayScaleOverride(display,2));
            size(w,&width,&height);CHECK(!(mode&1)||width==300);CHECK(!(mode&2)||height==90);
            XtVaSetValues(w,XmNwidth,0,XmNheight,0,NULL);
            size(w,&width,&height);CHECK(width>0&&height>0);CHECK(width!=300&&height!=90);
            XtDestroyWidget(shell);++cases;
        }
    }
    XtCloseDisplay(display); XtDestroyApplicationContext(app);
    printf("PASS appearance dimensions: %u cases\n",cases); return 0;
}
