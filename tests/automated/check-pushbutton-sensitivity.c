/* Effective sensitivity loss cancels widget and gadget pointer gestures. */
#include <Xm/XmAll.h>
#include <Xm/PushBP.h>
#include <Xm/PushBGP.h>
#include <Xm/Appearance.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static unsigned activations;
static void activated(Widget w,XtPointer closure,XtPointer data){(void)w;(void)closure;(void)data;++activations;}
static void input(Widget w,Boolean press,Boolean multi) {
    XEvent event={0};Position x,y,rx,ry;Dimension width,height;
    XtVaGetValues(w,XmNx,&x,XmNy,&y,XmNwidth,&width,XmNheight,&height,NULL);
    event.xbutton.type=press?ButtonPress:ButtonRelease;
    event.xbutton.display=XtDisplayOfObject(w);event.xbutton.window=XtWindowOfObject(w);
    event.xbutton.button=Button1;event.xbutton.same_screen=True;event.xbutton.time=1000;
    event.xbutton.x=(XtIsWidget(w)?0:x)+width/2;
    event.xbutton.y=(XtIsWidget(w)?0:y)+height/2;
    XtTranslateCoords(XtIsWidget(w)?w:XtParent(w),event.xbutton.x,event.xbutton.y,&rx,&ry);
    event.xbutton.x_root=rx;event.xbutton.y_root=ry;
    if(XtIsWidget(w))XtCallActionProc(w,press?(multi?"MultiArm":"Arm"):(multi?"MultiActivate":"Activate"),&event,NULL,0);
    else xmPushButtonGadgetClassRec.gadget_class.input_dispatch(w,&event,press?(multi?XmMULTI_ARM_EVENT:XmARM_EVENT):(multi?XmMULTI_ACTIVATE_EVENT:XmACTIVATE_EVENT));
}
int main(int argc,char **argv) {
    XtAppContext app;Display *d;unsigned cases=0;
    XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"buttonCancel","ButtonCancel",NULL,0,&argc,argv);CHECK(d);
    for(unsigned profile=0;profile<3;++profile)for(unsigned gadget=0;gadget<2;++gadget)
    for(unsigned ancestor=0;ancestor<2;++ancestor)for(unsigned multi=0;multi<2;++multi) {
        CHECK(XmApplyAppearanceProfile(d,profile==0?XmAPPEARANCE_CLASSIC:profile==1?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));
        Widget shell=XtAppCreateShell("buttonCancel","ButtonCancel",applicationShellWidgetClass,d,NULL,0);
        Widget board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,300,XmNheight,120,NULL);
        Widget w=XtVaCreateManagedWidget("button",gadget?xmPushButtonGadgetClass:xmPushButtonWidgetClass,board,XmNx,20,XmNy,20,XmNmultiClick,XmMULTICLICK_KEEP,NULL);
        XtAddCallback(w,XmNactivateCallback,activated,NULL);XtRealizeWidget(shell);XSync(d,False);
        activations=0;input(w,True,multi);XtSetSensitive(ancestor?board:w,False);XtSetSensitive(ancestor?board:w,True);input(w,False,multi);CHECK(activations==0);
        input(w,True,multi);input(w,False,multi);CHECK(activations==1);
        XtDestroyWidget(shell);++cases;
    }
    printf("PASS pushbutton sensitivity: %u configurations\n",cases);
    XtCloseDisplay(d);XtDestroyApplicationContext(app);return 0;
}
