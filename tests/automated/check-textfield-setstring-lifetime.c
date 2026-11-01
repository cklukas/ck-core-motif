/* SetString callbacks may destroy the field or an ancestor, immediately or deferred. */
#include <Xm/XmAll.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static unsigned notifications,observers;
static void observe(Widget w,XtPointer data,XtPointer call){(void)w;(void)data;(void)call;++observers;}
static void gone(Widget w,XtPointer data,XtPointer call){(void)w;(void)call;*(Boolean*)data=True;}
static void changed(Widget w,XtPointer data,XtPointer call){
    unsigned mode=*(unsigned*)data;CHECK(((XmAnyCallbackStruct*)call)->reason==XmCR_VALUE_CHANGED);++notifications;
    if(mode==1)XtDestroyWidget(w);
    if(mode==2)XtDestroyWidget(XtParent(w));
    if(mode==3)XtDestroyWidget(XtParent(XtParent(w)));
}
static void set_text(Widget w,Boolean wide){
    if(wide)XmTextFieldSetStringWcs(w,L"42");else XmTextFieldSetString(w,"42");
}
static void input(Widget w,XtPointer data,XEvent*e,Boolean*dispatch){
    if(e->type==KeyPress){*dispatch=False;set_text(w,*(Boolean*)data);}
}
static unsigned mutations,old_calls,new_calls;
static void old_callback(Widget w,XtPointer data,XtPointer call){(void)w;(void)data;(void)call;++old_calls;}
static void new_callback(Widget w,XtPointer data,XtPointer call){(void)w;(void)data;(void)call;++new_calls;}
static void mutate_callbacks(Widget w,XtPointer data,XtPointer call){
    (void)data;(void)call;
    if(++mutations==1){
        XtRemoveCallback(w,XmNvalueChangedCallback,old_callback,NULL);
        XtAddCallback(w,XmNvalueChangedCallback,new_callback,NULL);
    }
}
static void drain(XtAppContext app,Display*d){
    XSync(d,False);for(unsigned n=0;XtAppPending(app);++n){CHECK(n<10000);XtAppProcessEvent(app,XtIMAll);XSync(d,False);}
}
int main(int argc,char **argv){
    XtAppContext app;Display*d;unsigned cases=0;
    XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"setStringLifetime","SetStringLifetime",NULL,0,&argc,argv);CHECK(d);
    for(unsigned dispatch=0;dispatch<2;++dispatch)for(unsigned mode=0;mode<4;++mode)
    for(Boolean wide=False;wide<=True;++wide){
        Widget shell=XtAppCreateShell("setStringLifetime","SetStringLifetime",applicationShellWidgetClass,d,NULL,0);
        Widget board=XtVaCreateManagedWidget("board",xmBulletinBoardWidgetClass,shell,XmNwidth,300,XmNheight,180,NULL);
        Widget field=XtVaCreateManagedWidget("editor",xmTextFieldWidgetClass,board,XmNx,10,XmNy,10,NULL);
        Boolean destroyed=False;XtAddCallback(field,XmNdestroyCallback,gone,&destroyed);
        XtAddCallback(field,XmNvalueChangedCallback,observe,NULL);
        XtAddCallback(field,XmNvalueChangedCallback,changed,&mode);
        XtAddCallback(field,XmNvalueChangedCallback,observe,NULL);
        XtAddEventHandler(field,KeyPressMask,False,input,&wide);
        XtRealizeWidget(shell);drain(app,d);CHECK(XmProcessTraversal(field,XmTRAVERSE_CURRENT));drain(app,d);
        unsigned before=notifications,before_observers=observers;
        if(dispatch){XEvent event={0};event.xkey.type=KeyPress;event.xkey.display=d;event.xkey.window=XtWindow(field);
            event.xkey.root=RootWindowOfScreen(XtScreen(field));event.xkey.same_screen=True;
            event.xkey.keycode=XKeysymToKeycode(d,XK_F8);XtDispatchEvent(&event);
        }else set_text(field,wide);
        drain(app,d);CHECK(destroyed==(mode!=0));CHECK(notifications==before+1);CHECK(observers==before_observers+(mode?1:2));
        if(mode==0){char*text=XmTextFieldGetString(field);CHECK(text && !strcmp(text,"42"));XtFree(text);}
        if(mode!=3)XtDestroyWidget(shell);
        drain(app,d);++cases;
    }
    {
        Widget shell=XtAppCreateShell("mutation","SetStringLifetime",applicationShellWidgetClass,d,NULL,0);
        Widget field=XtVaCreateManagedWidget("editor",xmTextFieldWidgetClass,shell,NULL);
        XtAddCallback(field,XmNvalueChangedCallback,mutate_callbacks,NULL);
        XtAddCallback(field,XmNvalueChangedCallback,old_callback,NULL);
        XtRealizeWidget(shell);drain(app,d);
        XmTextFieldSetString(field,"first");CHECK(mutations==1 && old_calls==1 && new_calls==0);
        XmTextFieldSetString(field,"second");CHECK(mutations==2 && old_calls==1 && new_calls==1);
        XtDestroyWidget(shell);drain(app,d);++cases;
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(app);printf("PASS textfield setstring lifetime: %u cases\n",cases);return 0;
}
