/* Click focus for interactive primitives and existing Motif buttons. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/XmP.h>
#include <Xm/VendorSEP.h>
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s, control=%s profile=%d\n",__LINE__,#c,name,profile); exit(1); } } while(0)
static int focus_destroyed, parent_activations;
static void parent_activated(Widget w, XtPointer closure, XtPointer call) {
    (void)w; (void)closure; (void)call; ++parent_activations;
}
static void destroy_on_focus(Widget owner, XtPointer closure, XtPointer call) {
    XmFocusMovedCallbackStruct *focus=(XmFocusMovedCallbackStruct *)call;
    (void)owner;
    if (focus && focus->new_focus==(Widget)closure) {
        ++focus_destroyed; XtDestroyWidget((Widget)closure); focus->cont=False;
    }
}
static void pump(XtAppContext app, Display *d) {
    for(int n=0;n<3;++n) { XSync(d,False); while(XtAppPending(app)) XtAppProcessEvent(app,XtIMAll); }
}
static void click(Widget w,unsigned button,int x,int y) {
    XEvent e={0}; e.xbutton.type=ButtonPress; e.xbutton.display=XtDisplay(w);
    e.xbutton.window=XtWindow(w); e.xbutton.button=button; e.xbutton.x=x; e.xbutton.y=y;
    XtDispatchEvent(&e);
}
int main(int argc,char **argv) {
    XtAppContext app; Display *d;
    XtToolkitInitialize(); app=XtCreateApplicationContext();
    d=XtOpenDisplay(app,NULL,"clickFocus","ClickFocus",NULL,0,&argc,argv);
    if(!d) return 1;
    WidgetClass classes[]={xmPushButtonWidgetClass,xmToggleButtonWidgetClass,
      xmArrowButtonWidgetClass,xmDrawnButtonWidgetClass,xmLinkWidgetClass,
      xmCalendarWidgetClass,xmDataGridWidgetClass,xmBreadcrumbBarWidgetClass,
      xmTimePickerWidgetClass,xmInfoBarWidgetClass,xmDatePickerWidgetClass,
      xmSearchBoxWidgetClass,xmIconButtonWidgetClass,xmToggleSwitchWidgetClass,
      xmRatingControlWidgetClass,xmChipWidgetClass};
    const char *names[]={"push","toggle","arrow","drawn","link","calendar","grid","breadcrumb","time","info","date","search","iconButton","switch","rating","chip"};
    for(int profile=0;profile<4;++profile) for(unsigned c=0;c<XtNumber(classes);++c) {
        const char *name=names[c]; CHECK(XmApplyAppearanceProfile(d,(XmAppearanceProfile)profile));
        Widget shell=XtAppCreateShell("clickFocus","ClickFocus",applicationShellWidgetClass,d,NULL,0);
        Widget form=XtVaCreateManagedWidget("form",xmFormWidgetClass,shell,XmNwidth,480,XmNheight,320,NULL);
        Widget other=XtVaCreateManagedWidget("other",xmPushButtonWidgetClass,form,XmNnavigationType,XmTAB_GROUP,NULL);
        XtVaSetValues(form,XmNdefaultButton,other,NULL);
        XtAddCallback(other,XmNactivateCallback,parent_activated,NULL);
        Widget w=XtVaCreateManagedWidget(name,classes[c],form,XmNtopAttachment,XmATTACH_WIDGET,
          XmNtopWidget,other,XmNwidth,360,XmNheight,220,XmNnavigationType,XmTAB_GROUP,NULL);
        XtRealizeWidget(shell); XtMapWidget(shell); pump(app,d);
        CHECK(XmProcessTraversal(other,XmTRAVERSE_CURRENT)); pump(app,d);
        CHECK(XmGetFocusWidget(shell)==other);
        if (classes[c]==xmSearchBoxWidgetClass) XmTextFieldSetString(w,"query");
        click(w,Button1,classes[c]==xmSearchBoxWidgetClass?345:10,
              classes[c]==xmSearchBoxWidgetClass?110:10);
        pump(app,d); CHECK(XmGetFocusWidget(shell)==w);
        if (classes[c]==xmDataGridWidgetClass) {
            int row_id=1,col_ids[2]={2,3};
            XmDataGridColumn columns[2]={{&col_ids[0],"A",False,80},{&col_ids[1],"B",False,80}};
            const char *values[]={"a","b"};XmDataGridRow rows[1]={{&row_id,values}};
            XtPointer selected_row=NULL,selected_col=NULL;
            CHECK(XmDataGridSetModel(w,columns,2,rows,1));
            CHECK(XmDataGridSetSelectedCell(w,&row_id,&col_ids[0],False));
            XEvent right={0};right.xkey.type=KeyPress;right.xkey.display=d;right.xkey.window=XtWindow(w);
            right.xkey.keycode=XKeysymToKeycode(d,XK_Right);XtDispatchEvent(&right);pump(app,d);
            CHECK(XmDataGridGetSelectedCell(w,&selected_row,&selected_col));CHECK(selected_col==&col_ids[1]);
        }
        if (classes[c]==xmTimePickerWidgetClass) {
            XmTimePickerValue before={12,10,0},after;
            CHECK(XmTimePickerSetValue(w,&before,False));
            XEvent up={0};up.xkey.type=KeyPress;up.xkey.display=d;up.xkey.window=XtWindow(w);
            up.xkey.keycode=XKeysymToKeycode(d,XK_Up);XtDispatchEvent(&up);pump(app,d);
            CHECK(XmTimePickerGetValue(w,&after));CHECK(after.hour==13 && after.minute==10);
        }
        if (classes[c]==xmLinkWidgetClass || classes[c]==xmCalendarWidgetClass ||
            classes[c]==xmDataGridWidgetClass || classes[c]==xmBreadcrumbBarWidgetClass ||
            classes[c]==xmTimePickerWidgetClass || classes[c]==xmInfoBarWidgetClass ||
            classes[c]==xmRatingControlWidgetClass || classes[c]==xmChipWidgetClass) {
            XEvent arrow={0}; arrow.xkey.type=KeyPress; arrow.xkey.display=d;
            arrow.xkey.window=XtWindow(w);arrow.xkey.keycode=XKeysymToKeycode(d,XK_Left);
            XtDispatchEvent(&arrow);pump(app,d);CHECK(XmGetFocusWidget(shell)==w);
            XEvent enter={0};enter.xkey.type=KeyPress;enter.xkey.display=d;
            enter.xkey.window=XtWindow(w);enter.xkey.keycode=XKeysymToKeycode(d,XK_Return);
            parent_activations=0;XtDispatchEvent(&enter);pump(app,d);
            if (classes[c]!=xmInfoBarWidgetClass && classes[c]!=xmRatingControlWidgetClass)
                CHECK(parent_activations==0);
        }
        XEvent e={0}; e.xkey.type=KeyPress;e.xkey.display=d;e.xkey.window=XtWindow(w);
        e.xkey.keycode=XKeysymToKeycode(d,XK_Tab);XtDispatchEvent(&e);pump(app,d);
        CHECK(XmGetFocusWidget(shell)==other);
        if (classes[c]==xmLinkWidgetClass || classes[c]==xmDataGridWidgetClass ||
            classes[c]==xmCalendarWidgetClass || classes[c]==xmBreadcrumbBarWidgetClass ||
            classes[c]==xmTimePickerWidgetClass || classes[c]==xmInfoBarWidgetClass) {
            CHECK(XmProcessTraversal(other,XmTRAVERSE_CURRENT)); pump(app,d);
            click(w,Button3,10,10); pump(app,d); CHECK(XmGetFocusWidget(shell)==other);
            XtVaSetValues(w,XmNtraversalOn,False,NULL);
            CHECK(XmProcessTraversal(other,XmTRAVERSE_CURRENT)); pump(app,d);
            click(w,Button1,10,10); pump(app,d); CHECK(XmGetFocusWidget(shell)==other);
            XtVaSetValues(w,XmNtraversalOn,True,NULL);
            XtSetSensitive(w,False);pump(app,d);
            click(w,Button1,10,10);pump(app,d);CHECK(XmGetFocusWidget(shell)==other);
        }
        XtDestroyWidget(shell);
    }
    {
      WidgetClass gadgets[]={xmPushButtonGadgetClass,xmToggleButtonGadgetClass,xmArrowButtonGadgetClass};
      const char *gadget_names[]={"pushGadget","toggleGadget","arrowGadget"};
      for(int profile=0;profile<4;++profile) for(unsigned i=0;i<XtNumber(gadgets);++i) {
        const char *name=gadget_names[i]; CHECK(XmApplyAppearanceProfile(d,(XmAppearanceProfile)profile));
        Widget shell=XtAppCreateShell("clickFocus","ClickFocus",applicationShellWidgetClass,d,NULL,0);
        Widget form=XtVaCreateManagedWidget("form",xmFormWidgetClass,shell,XmNwidth,400,XmNheight,180,NULL);
        Widget other=XtVaCreateManagedWidget("other",xmPushButtonWidgetClass,form,XmNnavigationType,XmTAB_GROUP,NULL);
        Widget item=XtVaCreateManagedWidget(name,gadgets[i],form,XmNtopAttachment,XmATTACH_WIDGET,
            XmNtopWidget,other,XmNwidth,100,XmNheight,60,XmNnavigationType,XmTAB_GROUP,NULL);
        XtRealizeWidget(shell); XtMapWidget(shell); pump(app,d);
        CHECK(XmProcessTraversal(other,XmTRAVERSE_CURRENT)); pump(app,d);
        Position x,y; XtVaGetValues(item,XmNx,&x,XmNy,&y,NULL);
        click(form,Button1,x+20,y+20); pump(app,d); CHECK(XmGetFocusWidget(shell)==item);
        XEvent e={0};e.xkey.type=KeyPress;e.xkey.display=d;e.xkey.window=XtWindow(form);
        e.xkey.keycode=XKeysymToKeycode(d,XK_Tab);XtDispatchEvent(&e);pump(app,d);
        CHECK(XmGetFocusWidget(shell)!=item);
        XtDestroyWidget(shell);
      }
    }
    {
      int profile=0; const char *name="destroyOnFocus";
      Widget shell=XtAppCreateShell("clickFocus","ClickFocus",applicationShellWidgetClass,d,NULL,0);
      Widget form=XtVaCreateManagedWidget("form",xmFormWidgetClass,shell,XmNwidth,400,XmNheight,180,NULL);
      Widget other=XtVaCreateManagedWidget("other",xmPushButtonWidgetClass,form,XmNnavigationType,XmTAB_GROUP,NULL);
      Widget link=XtVaCreateManagedWidget("link",xmLinkWidgetClass,form,XmNtopAttachment,XmATTACH_WIDGET,
        XmNtopWidget,other,XmNnavigationType,XmTAB_GROUP,NULL);
      XtRealizeWidget(shell);XtMapWidget(shell);pump(app,d);
      CHECK(XmProcessTraversal(other,XmTRAVERSE_CURRENT));pump(app,d);
      focus_destroyed=0; XmeAddFocusChangeCallback(shell,destroy_on_focus,link);
      click(link,Button1,5,5);pump(app,d);CHECK(focus_destroyed==1);
      XmeRemoveFocusChangeCallback(shell,destroy_on_focus,link);
      XtDestroyWidget(shell);
    }
    XtCloseDisplay(d);XtDestroyApplicationContext(app);
    puts("PASS interactive primitive click focus");return 0;
}
