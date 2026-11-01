/* Native activation must survive destruction and retain normal parent dispatch. */
#include <Xm/XmAll.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static unsigned submitted,observers;
static void observe(Widget w,XtPointer data,XtPointer call){(void)w;(void)data;(void)call;++observers;}
static void submit(Widget w,XtPointer data,XtPointer call){(void)w;(void)data;(void)call;++submitted;}
static void gone(Widget w,XtPointer data,XtPointer call){(void)w;(void)call;*(Boolean*)data=True;}
static void activate(Widget w,XtPointer data,XtPointer call){
    unsigned mode=*(unsigned*)data;(void)call;
    if(mode==1)XtDestroyWidget(w);
    if(mode==2)XtDestroyWidget(XtParent(w));
    if(mode==3)XtDestroyWidget(XtParent(XtParent(w)));
}
static void drain(XtAppContext app,Display*d){
    XSync(d,False);for(unsigned n=0;XtAppPending(app);++n){CHECK(n<10000);XtAppProcessEvent(app,XtIMAll);XSync(d,False);}
}
int main(int argc,char **argv){
    XtAppContext app;Display*d;unsigned cases=0;
    XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"activationLifetime","ActivationLifetime",NULL,0,&argc,argv);CHECK(d);
    for(unsigned dispatch=0;dispatch<2;++dispatch)for(unsigned mode=0;mode<4;++mode){
        Widget shell=XtAppCreateShell("activationLifetime","ActivationLifetime",applicationShellWidgetClass,d,NULL,0);
        Widget board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,300,XmNheight,180,NULL);
        Widget field=XtVaCreateManagedWidget("editor",xmTextFieldWidgetClass,board,XmNx,10,XmNy,10,NULL);
        Widget button=XtVaCreateManagedWidget("submit",xmPushButtonWidgetClass,board,XmNx,10,XmNy,70,NULL);
        XtVaSetValues(board,XmNdefaultButton,button,NULL);XtAddCallback(button,XmNactivateCallback,submit,NULL);
        Boolean destroyed=False;XtAddCallback(field,XmNdestroyCallback,gone,&destroyed);
        XtAddCallback(field,XmNactivateCallback,observe,NULL);
        XtAddCallback(field,XmNactivateCallback,activate,&mode);
        XtAddCallback(field,XmNactivateCallback,observe,NULL);
        XtRealizeWidget(shell);drain(app,d);XSetInputFocus(d,XtWindow(shell),RevertToParent,CurrentTime);
        CHECK(XmProcessTraversal(field,XmTRAVERSE_CURRENT));drain(app,d);
        unsigned before=submitted,before_observers=observers;
        if(dispatch){XEvent event={0};event.xkey.type=KeyPress;event.xkey.display=d;event.xkey.window=XtWindow(field);
            event.xkey.root=RootWindowOfScreen(XtScreen(field));event.xkey.same_screen=True;
            event.xkey.keycode=XKeysymToKeycode(d,XK_Return);XtDispatchEvent(&event);
        }else XtCallActionProc(field,"activate",NULL,NULL,0);
        drain(app,d);CHECK(destroyed==(mode!=0));
        CHECK(submitted==before+(mode==0?1:0));
        CHECK(observers==before_observers+(mode==0?2:1));
        if(mode!=3)XtDestroyWidget(shell);
        drain(app,d);++cases;
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(app);printf("PASS textfield activation lifetime: %u cases\n",cases);return 0;
}
