/* Real pointer-wheel dispatch through descendants and nested scroll frames. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/extensions/XTest.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static XtAppContext app;
static Display *display;
static unsigned callbacks, custom_calls, destroyed;
static void settle(void) {
    for (int pass=0;pass<5;++pass) {
        XSync(display,False);
        unsigned guard=0;
        while(XtAppPending(app)) { CHECK(++guard<10000); XtAppProcessEvent(app,XtIMAll); }
    }
}
static void wheel(Widget target, unsigned button, KeySym modifier) {
    Position x,y;
    XtTranslateCoords(target,5,5,&x,&y);
    CHECK(XTestFakeMotionEvent(display,DefaultScreen(display),x,y,CurrentTime));
    if(modifier) CHECK(XTestFakeKeyEvent(display,XKeysymToKeycode(display,modifier),True,CurrentTime));
    CHECK(XTestFakeButtonEvent(display,button,True,CurrentTime));
    CHECK(XTestFakeButtonEvent(display,button,False,CurrentTime));
    if(modifier) CHECK(XTestFakeKeyEvent(display,XKeysymToKeycode(display,modifier),False,CurrentTime));
    settle();
}
static int value(Widget bar) { int v; XtVaGetValues(bar,XmNvalue,&v,NULL); return v; }
static void reset(Widget bar) { int min; XtVaGetValues(bar,XmNminimum,&min,NULL); XmScrollBarSetValues(bar,min,0,0,0,True); settle(); callbacks=0; }
static void changed(Widget w,XtPointer c,XtPointer data) { (void)w;(void)c;(void)data;++callbacks; }
static void custom(Widget w,XEvent *e,String *p,Cardinal *n) { (void)w;(void)e;(void)p;(void)n;++custom_calls; }
static void clicked(Widget w,XtPointer c,XtPointer data) { (void)w;(void)c;(void)data;++custom_calls; }
static void retire(Widget w,XtPointer c,XtPointer data) { (void)w;(void)data;++destroyed;XtDestroyWidget((Widget)c); }
int main(int argc,char **argv) {
    const double scales[]={1,1.5,2}; unsigned cases=0;
    XtActionsRec actions[]={{"CustomWheel",custom}};
    XtToolkitInitialize();app=XtCreateApplicationContext();
    display=XtOpenDisplay(app,NULL,"wheel","Wheel",NULL,0,&argc,argv);CHECK(display);
    XtAppAddActions(app,actions,XtNumber(actions));
    for(unsigned profile=0;profile<3;++profile) for(unsigned sc=0;sc<3;++sc) {
        CHECK(XmApplyAppearanceProfile(display,(XmAppearanceProfile)profile));
        CHECK(XmSetDisplayScaleOverride(display,scales[sc]));
        Widget shell=XtAppCreateShell("wheel","Wheel",applicationShellWidgetClass,display,NULL,0);
        Widget sw=XtVaCreateManagedWidget("scroll",xmScrolledWindowWidgetClass,shell,XmNscrollingPolicy,XmAUTOMATIC,XmNwidth,640,XmNheight,480,NULL);
        Widget canvas=XtVaCreateManagedWidget("canvas",xmBulletinBoardWidgetClass,sw,XmNwidth,1600,XmNheight,1400,XmNresizePolicy,XmRESIZE_NONE,NULL);
        Widget label=XtVaCreateManagedWidget("label",xmLabelWidgetClass,canvas,XmNx,25,XmNy,25,XmNwidth,160,XmNheight,70,NULL);
        Widget v,h,clip; XtVaGetValues(sw,XmNverticalScrollBar,&v,XmNhorizontalScrollBar,&h,XmNclipWindow,&clip,NULL); CHECK(v&&h);
        XtAddCallback(v,XmNvalueChangedCallback,changed,NULL); XtAddCallback(h,XmNvalueChangedCallback,changed,NULL);
        XtRealizeWidget(shell);settle();
        int step; XtVaGetValues(v,XmNincrement,&step,NULL);reset(v);
        wheel(label,5,NoSymbol);CHECK(value(v)==3*step && callbacks==1);
        reset(v);wheel(canvas,5,NoSymbol);CHECK(value(v)==3*step && callbacks==1);
        reset(v);wheel(label,4,NoSymbol);CHECK(value(v)==0 && callbacks==0);
        reset(h);wheel(label,5,XK_Shift_L);CHECK(value(h)>0 && value(v)==0 && callbacks==1);
        reset(h);wheel(label,7,NoSymbol);CHECK(value(h)>0 && callbacks==1);
        reset(h);wheel(label,6,NoSymbol);CHECK(value(h)==0 && callbacks==0);
        reset(v);wheel(label,5,XK_Control_L);CHECK(value(v)==0 && callbacks==0);
        XtSetSensitive(sw,False);wheel(label,5,NoSymbol);CHECK(value(v)==0);XtSetSensitive(sw,True);
        XtVaSetValues(v,XmNeditable,False,NULL);wheel(label,5,NoSymbol);CHECK(value(v)==0);XtVaSetValues(v,XmNeditable,True,NULL);
        Widget late=XtVaCreateManagedWidget("late",xmLabelWidgetClass,canvas,XmNx,220,XmNy,25,XmNwidth,120,XmNheight,70,NULL);settle();
        wheel(late,5,NoSymbol);CHECK(value(v)==3*step);reset(v);
        Widget override=XtVaCreateManagedWidget("override",xmLabelWidgetClass,canvas,XmNx,360,XmNy,25,XmNwidth,160,XmNheight,70,
            XtNtranslations,XtParseTranslationTable("<Btn5Down>,<Btn5Up>:CustomWheel()"),NULL);settle();
        custom_calls=0;wheel(override,5,NoSymbol);CHECK(custom_calls==1 && value(v)==0);
        Widget press_override=XtVaCreateManagedWidget("pressOverride",xmLabelWidgetClass,canvas,XmNx,360,XmNy,100,XmNwidth,160,XmNheight,50,
            XtNtranslations,XtParseTranslationTable("<Btn5Down>:CustomWheel()"),NULL);settle();
        custom_calls=0;wheel(press_override,5,NoSymbol);CHECK(custom_calls==1 && value(v)==0);
        Widget button=XtVaCreateManagedWidget("button",xmPushButtonWidgetClass,canvas,XmNx,360,XmNy,160,XmNwidth,160,XmNheight,50,NULL);
        XtAddCallback(button,XmNactivateCallback,clicked,NULL);settle();
        custom_calls=0;wheel(button,1,NoSymbol);CHECK(custom_calls==1 && value(v)==0);
        Widget inner=XtVaCreateManagedWidget("inner",xmScrolledWindowWidgetClass,canvas,XmNx,35,XmNy,160,XmNwidth,240,XmNheight,200,XmNscrollingPolicy,XmAUTOMATIC,NULL);
        Widget content=XtVaCreateManagedWidget("content",xmBulletinBoardWidgetClass,inner,XmNwidth,900,XmNheight,800,XmNresizePolicy,XmRESIZE_NONE,NULL);
        Widget child=XtVaCreateManagedWidget("innerLabel",xmLabelWidgetClass,content,XmNx,15,XmNy,15,XmNwidth,140,XmNheight,60,NULL);
        Widget iv; XtVaGetValues(inner,XmNverticalScrollBar,&iv,NULL);settle();
        wheel(child,5,NoSymbol);CHECK(value(iv)>0 && value(v)==0);
        int imax,isize;Widget iclip;XtVaGetValues(iv,XmNmaximum,&imax,XmNsliderSize,&isize,NULL);
        XtVaGetValues(inner,XmNclipWindow,&iclip,NULL);
        XmScrollBarSetValues(iv,imax-isize,0,0,0,True);settle();
        wheel(iclip,5,NoSymbol);CHECK(value(iv)==imax-isize && value(v)==0);
        XtDestroyWidget(inner);settle();
        Widget app_frame=XtVaCreateManagedWidget("appFrame",xmScrolledWindowWidgetClass,canvas,
            XmNx,35,XmNy,160,XmNwidth,240,XmNheight,200,XmNscrollingPolicy,XmAPPLICATION_DEFINED,NULL);
        Widget app_content=XtVaCreateManagedWidget("appContent",xmBulletinBoardWidgetClass,app_frame,
            XmNwidth,300,XmNheight,250,NULL);settle();
        wheel(app_content,5,NoSymbol);CHECK(value(v)==0);
        XtDestroyWidget(app_frame);settle();
        Widget list=XmCreateScrolledList(canvas,"list",NULL,0);Widget frame=XtParent(list);
        XtVaSetValues(frame,XmNx,35,XmNy,160,XmNwidth,260,XmNheight,200,NULL);
        for(int i=0;i<30;++i) { char text[32];snprintf(text,sizeof(text),"Row %d",i);XmString str=XmStringCreateLocalized(text);XmListAddItem(list,str,0);XmStringFree(str); }
        XtManageChild(list);settle();int before,after;XtVaGetValues(list,XmNtopItemPosition,&before,NULL);
        wheel(list,5,NoSymbol);XtVaGetValues(list,XmNtopItemPosition,&after,NULL);CHECK(after==before+3 && value(v)==0);
        XtDestroyWidget(frame);settle();
        Widget text=XmCreateScrolledText(canvas,"text",NULL,0);frame=XtParent(text);
        XtVaSetValues(frame,XmNx,35,XmNy,160,XmNwidth,260,XmNheight,200,NULL);
        XtVaSetValues(text,XmNeditMode,XmMULTI_LINE_EDIT,NULL);
        XmTextSetString(text,"01\n02\n03\n04\n05\n06\n07\n08\n09\n10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n20\n");XtManageChild(text);settle();
        XmTextPosition top=XmTextGetTopCharacter(text);wheel(text,5,NoSymbol);CHECK(XmTextGetTopCharacter(text)==top+9 && value(v)==0);
        XtDestroyWidget(frame);settle();
        int maximum,size;XtVaGetValues(v,XmNmaximum,&maximum,XmNsliderSize,&size,NULL);
        XmScrollBarSetValues(v,maximum-size,0,0,0,True);settle();callbacks=0;
        wheel(clip,5,NoSymbol);CHECK(value(v)==maximum-size && callbacks==0);
        int hstep;XtVaGetValues(h,XmNincrement,&hstep,NULL);
        XtVaSetValues(h,XmNprocessingDirection,XmMAX_ON_LEFT,NULL);
        XmScrollBarSetValues(h,100,0,0,0,True);settle();callbacks=0;
        wheel(clip,6,NoSymbol);CHECK(value(h)==100+3*hstep && callbacks==1);
        XtVaSetValues(h,XmNprocessingDirection,XmMAX_ON_RIGHT,NULL);reset(h);
        XtVaSetValues(v,XmNprocessingDirection,XmMAX_ON_TOP,NULL);
        XmScrollBarSetValues(v,100,0,0,0,True);settle();callbacks=0;
        wheel(clip,4,NoSymbol);CHECK(value(v)==100+3*step && callbacks==1);
        XtVaSetValues(v,XmNprocessingDirection,XmMAX_ON_BOTTOM,NULL);
        reset(v);XtAddCallback(v,XmNvalueChangedCallback,retire,sw);destroyed=0;
        wheel(label,5,NoSymbol);CHECK(destroyed==1);
        XtDestroyWidget(shell);settle();++cases;
    }
    XtCloseDisplay(display);XtDestroyApplicationContext(app);
    printf("PASS ScrolledWindow wheel: %u native theme/scale scenarios\n",cases);return 0;
}
