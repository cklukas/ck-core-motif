#include <stdio.h>
#include <X11/IntrinsicP.h>
#include <Xm/XmAll.h>
#include <Xm/ButtonBox.h>
#define CHECK(x) do { if (!(x)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#x); return 1; } } while (0)
int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell = XtVaAppInitialize(&app,"ButtonBoxTest",NULL,0,&argc,argv,NULL,NULL);
    unsigned count = 0;
    const XmFillOption fills[] = {XmFillNone, XmFillMajor, XmFillMinor, XmFillAll};
    for (unsigned direction=0; direction<2; ++direction)
    for (unsigned equal=0; equal<2; ++equal)
    for (unsigned fill=0; fill<XtNumber(fills); ++fill) {
        Widget form = XtVaCreateManagedWidget("form",xmFormWidgetClass,shell,
            XmNwidth,180,XmNheight,100,NULL);
        Widget box = XtVaCreateManagedWidget("box",xmButtonBoxWidgetClass,form,
            XmNtopAttachment,XmATTACH_FORM,XmNbottomAttachment,XmATTACH_FORM,
            XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,
            XmNorientation,direction?XmVERTICAL:XmHORIZONTAL,
            XmNequalSize,equal,XmNfillOption,fills[fill],NULL);
        Widget child[3];
        for(unsigned i=0;i<3;++i)
            child[i]=XtVaCreateManagedWidget(i==0?"a":i==1?"longer label":"third",
                xmPushButtonWidgetClass,box,NULL);
        XtRealizeWidget(shell);
        for (unsigned i=0;i<3;++i)
        for (unsigned size=0;size<5;++size) {
            XtWidgetGeometry request={0}, reply={0};
            request.request_mode=CWWidth|CWHeight;
            request.width=size ? 15+(size-1)*110 : 0; request.height=size ? 10+(size-1)*25 : 0;
            XtGeometryResult result=XtMakeGeometryRequest(child[i],&request,&reply);
            if(result==XtGeometryAlmost) {
                Dimension w=XtWidth(child[i]),h=XtHeight(child[i]);
                XtWidgetGeometry accepted=reply,ignored={0};
                CHECK(accepted.width>0 && accepted.height>0);
                accepted.request_mode|=XtCWQueryOnly;
                CHECK(XtMakeGeometryRequest(child[i],&accepted,&ignored)==XtGeometryYes);
                CHECK(XtWidth(child[i])==w && XtHeight(child[i])==h);
                accepted.request_mode&=~XtCWQueryOnly;
                CHECK(XtMakeGeometryRequest(child[i],&accepted,&ignored)==XtGeometryYes);
                CHECK(XtWidth(child[i])==accepted.width && XtHeight(child[i])==accepted.height);
                ++count;
            }
        }
        XtDestroyWidget(form);
    }
    CHECK(count>0);
    printf("PASS ButtonBox negotiation: %u accepted compromises\n",count);
    XtDestroyWidget(shell);
    return 0;
}
