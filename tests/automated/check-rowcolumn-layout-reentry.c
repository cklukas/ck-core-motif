#include <stdio.h>
#include <Xm/XmAll.h>
static unsigned queries;
static void resized(Widget child, XtPointer data, XtPointer call)
{
    XtWidgetGeometry preferred={0};
    (void)data; (void)call;
    XtQueryGeometry(XtParent(child),NULL,&preferred);
    ++queries;
}
int main(int argc,char **argv)
{
    XtAppContext app;
    Widget shell=XtVaAppInitialize(&app,"RCReentry",NULL,0,&argc,argv,NULL,NULL);
    Widget form=XtVaCreateManagedWidget("form",xmFormWidgetClass,shell,
        XmNwidth,400,XmNheight,300,NULL);
    Widget row=XtVaCreateManagedWidget("row",xmRowColumnWidgetClass,form,
        XmNtopAttachment,XmATTACH_FORM,XmNbottomAttachment,XmATTACH_FORM,
        XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,NULL);
    for(unsigned i=0;i<3;++i) {
        Widget child=XtVaCreateManagedWidget("area",xmDrawingAreaWidgetClass,row,
            XmNwidth,80,XmNheight,30,NULL);
        XtAddCallback(child,XmNresizeCallback,resized,NULL);
    }
    XtRealizeWidget(shell);
    for(unsigned orientation=0;orientation<2;++orientation)
    for(unsigned packing=0;packing<2;++packing) {
        XtVaSetValues(row,XmNorientation,orientation?XmVERTICAL:XmHORIZONTAL,
            XmNpacking,packing?XmPACK_COLUMN:XmPACK_TIGHT,NULL);
        for(unsigned i=0;i<20;++i) {
            XtVaSetValues(shell,XmNwidth,300+i*3,XmNheight,200+i*4,NULL);
            XSync(XtDisplay(shell),False);
            while(XtAppPending(app)) XtAppProcessEvent(app,XtIMAll);
        }
    }
    if(queries<20) { fprintf(stderr,"FAIL insufficient nested queries: %u\n",queries); return 1; }
    XtDestroyWidget(shell);
    printf("PASS RowColumn layout reentry: %u nested queries\n",queries);
    return 0;
}
