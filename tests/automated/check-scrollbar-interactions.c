/* Pointer/repeat cleanup must survive callback changes and widget teardown. */
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <Xm/XmAll.h>
#include <Xm/ScrollBarP.h>
#include <Xm/Appearance.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
typedef struct {unsigned mode,threshold,calls;Boolean completed,destroyed,timeout;} State;
static void destroyed(Widget w,XtPointer c,XtPointer v){(void)w;(void)v;((State*)c)->destroyed=True;}
static void changed(Widget w,XtPointer c,XtPointer v) {
    State *s=c;CHECK(v);++s->calls;
    if(s->calls==s->threshold) {
        s->completed=True;
        if(s->mode==0) XtSetSensitive(w,False);
        else if(s->mode==1) XtVaSetValues(w,XmNeditable,False,NULL);
        else XtDestroyWidget(w);
    }
}
static void timeout(XtPointer c,XtIntervalId *id){(void)id;((State*)c)->timeout=True;}
static void drain(XtAppContext app,Display *d){XSync(d,False);while(XtAppPending(app))XtAppProcessEvent(app,XtIMAll);XSync(d,False);}
static void pointer(Widget w,const char *action,int type,int x,int y) {
    XEvent e={0};Position rx,ry;XtTranslateCoords(w,x,y,&rx,&ry);
    e.xbutton.type=type;e.xbutton.display=XtDisplay(w);e.xbutton.window=XtWindow(w);
    e.xbutton.button=Button1;e.xbutton.x=x;e.xbutton.y=y;e.xbutton.x_root=rx;e.xbutton.y_root=ry;e.xbutton.same_screen=True;
    XtCallActionProc(w,action,&e,NULL,0);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *d,*probe;unsigned cases=0;
    XmAppearanceProfile profiles[]={XmAPPEARANCE_CLASSIC,XmAPPEARANCE_MODERN_LIGHT,XmAPPEARANCE_MODERN_DARK,XmAPPEARANCE_HIGH_CONTRAST};
    XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"scrollLifetime","ScrollLifetime",NULL,0,&argc,argv);CHECK(d);
    probe=XOpenDisplay(NULL);CHECK(probe);
    for(unsigned p=0;p<4;++p)for(unsigned h=0;h<2;++h)for(unsigned mode=0;mode<3;++mode)for(unsigned timed=0;timed<2;++timed){
        State state={.mode=mode,.threshold=timed?2:1};
        CHECK(XmApplyAppearanceProfile(d,profiles[p]));
        Widget shell=XtAppCreateShell("scrollLifetime","ScrollLifetime",applicationShellWidgetClass,d,NULL,0);
        Widget board=XtVaCreateManagedWidget("board",xmDrawingAreaWidgetClass,shell,XmNwidth,400,XmNheight,400,NULL);
        Widget w=XtVaCreateManagedWidget("scroll",xmScrollBarWidgetClass,board,
            XmNorientation,h?XmHORIZONTAL:XmVERTICAL,XmNwidth,h?300:40,XmNheight,h?40:300,
            XmNshowArrows,XmEACH_SIDE,XmNminimum,0,XmNmaximum,100,XmNsliderSize,20,XmNvalue,40,
            XmNinitialDelay,10,XmNrepeatDelay,10,NULL);
        XtAddCallback(w,XmNvalueChangedCallback,changed,&state);XtAddCallback(w,XmNdestroyCallback,destroyed,&state);
        XtRealizeWidget(shell);drain(app,d);XmScrollBarWidget s=(XmScrollBarWidget)w;
        int x=s->scrollBar.arrow2_x+s->scrollBar.arrow_width/2,y=s->scrollBar.arrow2_y+s->scrollBar.arrow_height/2;
        XtIntervalId watchdog=XtAppAddTimeOut(app,500,timeout,&state);
        pointer(w,"Select",ButtonPress,x,y);
        while(!state.completed && !state.timeout)XtAppProcessEvent(app,XtIMAll);
        CHECK(state.completed && !state.timeout);XtRemoveTimeOut(watchdog);drain(app,d);
        CHECK(state.calls==state.threshold);
        CHECK(state.destroyed==(mode==2));
        if(!state.destroyed){
            CHECK(!s->scrollBar.sliding_on && !s->scrollBar.timer);
            CHECK(!s->scrollBar.arrow1_selected && !s->scrollBar.arrow2_selected);
            CHECK(!(s->scrollBar.flags & KEYBOARD_GRABBED));
            CHECK(!(s->scrollBar.add_flags & INTERACTION_ACTIVE));
            pointer(w,"Release",ButtonRelease,x,y);CHECK(state.calls==state.threshold);
        }
        CHECK(XGrabKeyboard(probe,DefaultRootWindow(probe),False,GrabModeAsync,GrabModeAsync,CurrentTime)==GrabSuccess);
        XUngrabKeyboard(probe,CurrentTime);XSync(probe,False);
        state.timeout=False;XtAppAddTimeOut(app,40,timeout,&state);
        while(!state.timeout)XtAppProcessEvent(app,XtIMAll);
        CHECK(state.calls==state.threshold);
        XtDestroyWidget(shell);drain(app,d);++cases;
    }
    XCloseDisplay(probe);XtCloseDisplay(d);XtDestroyApplicationContext(app);
    printf("PASS scrollbar interactions: %u disable/read-only/destroy cases\n",cases);return 0;
}
