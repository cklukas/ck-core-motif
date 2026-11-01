/* NumberBox must provide ordinary caret editing and preserve numeric commits. */
#include <Xm/XmAll.h>
#include <Xm/NumberBox.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1); } } while(0)
static unsigned changes, invalids, observers;
static void observe_callback(Widget w,XtPointer closure,XtPointer data) {
    (void)w;(void)closure;(void)data;++observers;
}
static void changed(Widget widget,XtPointer closure,XtPointer data) {
    XmNumberBoxCallbackStruct *call=data;(void)widget;(void)closure;
    CHECK(call && call->reason==XmCR_VALUE_CHANGED);++changes;
}
static void invalid(Widget widget,XtPointer closure,XtPointer data) {
    XmNumberBoxInvalidCallbackStruct *call=data;(void)widget;(void)closure;
    CHECK(call && call->text && *call->text);++invalids;
}
static void destroy_on_callback(Widget widget,XtPointer closure,XtPointer data) {
    (void)closure;(void)data;XtDestroyWidget(widget);
}
static void destroy_editor_owner(Widget widget,XtPointer closure,XtPointer data) {
    (void)widget;(void)data;XtDestroyWidget((Widget)closure);
}
static void destroyed(Widget widget,XtPointer closure,XtPointer data) {
    (void)widget;(void)data;*(Boolean*)closure=True;
}
static void text_is(Widget editor,const char *expected) {
    char *actual=XmTextFieldGetString(editor);CHECK(actual);
    if(strcmp(actual,expected))fprintf(stderr,"editor text=[%s] expected=[%s]\n",actual,expected);
    CHECK(!strcmp(actual,expected));XtFree(actual);
}
static void drain(XtAppContext app,Display *d) {
    for(unsigned n=0;n<10000;++n){XSync(d,False);if(!XtAppPending(app))return;XtAppProcessEvent(app,XtIMAll);}CHECK(0);
}
static void key_target(Widget target,XtAppContext app,KeySym symbol) {
    CHECK(target && XtIsRealized(target));
    Display *d=XtDisplay(target);XEvent event={0};
    event.xkey.type=KeyPress;event.xkey.display=d;event.xkey.window=XtWindow(target);
    event.xkey.root=RootWindowOfScreen(XtScreen(target));event.xkey.same_screen=True;
    event.xkey.keycode=XKeysymToKeycode(d,symbol);CHECK(event.xkey.keycode);
    XtDispatchEvent(&event);drain(app,d);
}
static void key(Widget shell,XtAppContext app,KeySym symbol) {
    key_target(XmGetFocusWidget(shell),app,symbol);
}
/* Dispatch the native translations with real root coordinates: ArrowButton
 * checks ancestor clipping before admitting an activation. */
static void button(Widget arrow, int type, Boolean inside) {
    static Time timestamp=1000;
    Dimension width,height;Position root_x,root_y;
    XtVaGetValues(arrow,XmNwidth,&width,XmNheight,&height,NULL);
    int x=inside?(int)width/2:-10, y=inside?(int)height/2:-10;
    XtTranslateCoords(arrow,(Position)x,(Position)y,&root_x,&root_y);
    XEvent event={0};
    event.xbutton.type=type;event.xbutton.display=XtDisplay(arrow);
    event.xbutton.window=XtWindow(arrow);
    event.xbutton.root=RootWindowOfScreen(XtScreen(arrow));
    event.xbutton.x=x;event.xbutton.y=y;
    event.xbutton.x_root=root_x;event.xbutton.y_root=root_y;
    event.xbutton.button=Button1;event.xbutton.same_screen=True;
    event.xbutton.state=type==ButtonRelease?Button1Mask:0;
    event.xbutton.time=(timestamp+=1000);
    XtDispatchEvent(&event);
}
static void value_at(Widget number,int expected,int line) {
    int actual=XmNumberBoxGetValue(number);
    if(actual!=expected)fprintf(stderr,"line %d: number value=%d expected=%d\n",line,actual,expected);
    CHECK(actual==expected);
}
#define value(number,expected) value_at(number,expected,__LINE__)
int main(int argc,char **argv) {
    XtAppContext app;Display*d;unsigned cases=0;const double scales[]={1,1.5,2,3};
    XtToolkitInitialize();app=XtCreateApplicationContext();
    d=XtOpenDisplay(app,NULL,"numberEditing","NumberEditing",NULL,0,&argc,argv);CHECK(d);
    for(unsigned dark=0;dark<2;++dark)for(unsigned scale=0;scale<4;++scale) {
        CHECK(XmSetDisplayScaleOverride(d,scales[scale]));
        CHECK(XmApplyAppearanceProfile(d,dark?XmAPPEARANCE_MODERN_DARK:XmAPPEARANCE_MODERN_LIGHT));
        Widget shell=XtAppCreateShell("numberEditing","NumberEditing",applicationShellWidgetClass,d,NULL,0);
        Widget board=XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,XmNwidth,800,XmNheight,400,NULL);
        Widget number=XmVaCreateManagedNumberBox(board,"number",XmNnumberMinimum,-1000,
            XmNnumberMaximum,1000,XmNnumberValue,3,XmNwidth,260,XmNheight,80,NULL);
        XtAddCallback(number,XmNnumberValueChangedCallback,changed,NULL);
        XtAddCallback(number,XmNnumberInvalidCallback,invalid,NULL);
        XtRealizeWidget(shell);drain(app,d);XSetInputFocus(d,XtWindow(shell),RevertToParent,CurrentTime);
        CHECK(XmProcessTraversal(number,XmTRAVERSE_CURRENT));drain(app,d);
        unsigned before=changes;
        key(shell,app,XK_Home);key(shell,app,XK_1);value(number,3);CHECK(changes==before);
        key(shell,app,XK_Return);value(number,13);CHECK(changes==before+1);
        key(shell,app,XK_Home);key(shell,app,XK_Right);key(shell,app,XK_BackSpace);
        key(shell,app,XK_Return);value(number,3);
        key(shell,app,XK_End);key(shell,app,XK_5);key(shell,app,XK_Return);value(number,35);
        XmNumberBoxSetRange(number,-10,10,False);XmNumberBoxSetValue(number,3,False);drain(app,d);
        unsigned errors=invalids;key(shell,app,XK_End);key(shell,app,XK_9);key(shell,app,XK_Return);
        value(number,3);CHECK(XmNumberBoxIsInvalid(number) && invalids==errors+1);
        key(shell,app,XK_Escape);CHECK(!XmNumberBoxIsInvalid(number));
        key(shell,app,XK_Up);value(number,4);key(shell,app,XK_Down);value(number,3);
        Widget focused=XmGetFocusWidget(shell);
        XtSetSensitive(number,False);key_target(focused,app,XK_Up);value(number,3);
        XtSetSensitive(number,True);CHECK(XmProcessTraversal(number,XmTRAVERSE_CURRENT));
        XmNumberBoxSetValue(number,9,False);key(shell,app,XK_End);key(shell,app,XK_9);
        key(shell,app,XK_Escape);key(shell,app,XK_Return);value(number,9);
        XmNumberBoxSetRange(number,-1000,1000,False);drain(app,d);
        Widget editor=XmNumberBoxGetTextField(number);CHECK(editor && XmIsTextField(editor));
        XmTextFieldSetString(editor,"123");XmTextFieldSetSelection(editor,0,3,CurrentTime);drain(app,d);
        char *selected=XmTextFieldGetSelection(editor);CHECK(selected && !strcmp(selected,"123"));XtFree(selected);
        CHECK(XmTextFieldCopy(editor,CurrentTime));
        XmTextFieldSetString(editor,"");CHECK(XmTextFieldPaste(editor));drain(app,d);text_is(editor,"123");
        key(shell,app,XK_Return);value(number,123);
        XmTextFieldSetSelection(editor,0,1,CurrentTime);CHECK(XmTextFieldCut(editor,CurrentTime));drain(app,d);text_is(editor,"23");
        key(shell,app,XK_Return);value(number,23);
        XmTextFieldSetInsertionPosition(editor,0);CHECK(XmTextFieldPaste(editor));drain(app,d);text_is(editor,"123");
        key(shell,app,XK_Return);value(number,123);
        Widget increment=XtNameToWidget(number,"increment");
        Widget decrement=XtNameToWidget(number,"decrement");
        CHECK(increment && decrement);
        XmNumberBoxSetValue(number,3,False);before=changes;
        button(increment,ButtonPress,True);value(number,3);CHECK(changes==before);
        button(increment,ButtonRelease,True);value(number,4);CHECK(changes==++before);
        button(decrement,ButtonPress,True);value(number,4);CHECK(changes==before);
        button(decrement,ButtonRelease,True);value(number,3);CHECK(changes==++before);
        button(increment,ButtonPress,True);button(increment,ButtonRelease,False);
        value(number,3);CHECK(changes==before);
        XtSetSensitive(number,False);
        button(increment,ButtonPress,True);button(increment,ButtonRelease,True);
        value(number,3);CHECK(changes==before);
        XtSetSensitive(number,True);
        button(increment,ButtonPress,True);XtSetSensitive(number,False);
        button(increment,ButtonRelease,True);value(number,3);CHECK(changes==before);
        XtSetSensitive(number,True);
        button(increment,ButtonRelease,True);value(number,3);CHECK(changes==before);
        XmNumberBoxSetValue(number,1000,False);
        button(increment,ButtonPress,True);button(increment,ButtonRelease,True);
        value(number,1000);CHECK(changes==before);
        XtVaSetValues(number,XmNnumberWrap,True,NULL);
        button(increment,ButtonPress,True);button(increment,ButtonRelease,True);
        value(number,-1000);CHECK(changes==++before);
        button(decrement,ButtonPress,True);button(decrement,ButtonRelease,True);
        value(number,1000);CHECK(changes==++before);
        const unsigned destruction_cases[]={2,0,1,3};
        for(unsigned destruction_index=0;destruction_index<4;++destruction_index) {
            unsigned invalid_case=destruction_cases[destruction_index];
            Widget doomed=XmVaCreateManagedNumberBox(board,"doomed",XmNnumberValue,3,NULL);
            Boolean gone=False;XtAddCallback(doomed,XmNdestroyCallback,destroyed,&gone);
            const char *callback=invalid_case==1?XmNnumberInvalidCallback:XmNnumberValueChangedCallback;
            unsigned observed=observers;
            XtAddCallback(doomed,callback,observe_callback,NULL);
            XtAddCallback(doomed,callback,destroy_on_callback,NULL);
            XtAddCallback(doomed,callback,observe_callback,NULL);
            CHECK(XmProcessTraversal(doomed,XmTRAVERSE_CURRENT));drain(app,d);
            XmTextFieldSetString(XmNumberBoxGetTextField(doomed),invalid_case==1?"not a number":"4");
            if(invalid_case==3) {
                Widget arrow=XtNameToWidget(doomed,"increment");CHECK(arrow);
                button(arrow,ButtonPress,True);CHECK(!gone);
                button(arrow,ButtonRelease,True);
            }
            else if(invalid_case==2)XtCallActionProc(XmNumberBoxGetTextField(doomed),"activate",NULL,NULL,0);
            else key(shell,app,XK_Return);
            CHECK(gone);CHECK(observers==observed+1);
        }
        for(unsigned operation=0;operation<3;++operation) {
            fprintf(stderr,"editor lifetime: profile=%u scale=%u operation=%u\n",dark,scale,operation);
            Widget doomed=XmVaCreateManagedNumberBox(board,"editorOwner",XmNnumberValue,3,NULL);
            Boolean gone=False;XtAddCallback(doomed,XmNdestroyCallback,destroyed,&gone);
            XtAddCallback(XmNumberBoxGetTextField(doomed),XmNvalueChangedCallback,destroy_editor_owner,doomed);
            if(operation==0)XmNumberBoxSetValue(doomed,4,True);
            else if(operation==1)XmNumberBoxSetRange(doomed,10,20,True);
            else XmNumberBoxStep(doomed,1,False,True);
            CHECK(gone);drain(app,d);
        }
        Widget pending=XmVaCreateManagedNumberBox(board,"pending",NULL);
        XmNumberBoxSetRange(pending,-10000,10000,False);XtDestroyWidget(pending);drain(app,d);
        CHECK(XmNumberBoxGetTextField(NULL)==NULL);
        XtDestroyWidget(shell);drain(app,d);++cases;
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS numberbox editing: %u configurations\n",cases);return 0;
}
