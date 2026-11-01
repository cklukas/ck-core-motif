/* Native numeric semantics: protocol identity, actions, admission and lifetime. */
#include <Xm/XmAll.h>
#include <Xm/Accessible.h>
#include <Xm/NumberBox.h>
#include <Xm/Stepper.h>
#include <Xm/Appearance.h>
#include <limits.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
extern const char *_XmAccessibilityRoleName(unsigned int);
#define CHECK(c) do {if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static unsigned changes,destruction;
static XmAccessibility *bridge;
static void changed(Widget w,XtPointer closure,XtPointer data) {
    XmAnyCallbackStruct *cb=data;(void)closure;
    CHECK(cb && cb->reason==XmCR_VALUE_CHANGED && cb->event==NULL);++changes;
    if(destruction==1)XtDestroyWidget(w);
    else if(destruction==2){XmAccessibilityDestroy(bridge);bridge=NULL;}
}
static int value(Widget w){return XmIsNumberBox(w)?XmNumberBoxGetValue(w):XmStepperGetValue(w);}
static void set(Widget w,int n){if(XmIsNumberBox(w))XmNumberBoxSetValue(w,n,False);else XmStepperSetValue(w,n,False);}
static void drain(XtAppContext app,Display*d){XSync(d,False);for(unsigned n=0;XtAppPending(app);++n){CHECK(n<10000);XtAppProcessEvent(app,XtIMAll);XSync(d,False);}}
static void query(XmAccessibleId id,int expected,unsigned count) {
    XmAccessibleInfo info;CHECK(XmAccessibilityQuery(bridge,id,&info));
    CHECK(info.role==52 && !strcmp(_XmAccessibilityRoleName(info.role),"spin button"));
    CHECK(info.has_value && info.minimum_value==-10 && info.maximum_value==10);
    CHECK(info.current_value==expected && info.minimum_increment==3 && info.action_count==count);
    CHECK(info.states&XmACCESSIBLE_STATE(7));XmAccessibleInfoClear(&info);
    const char *names[]={"increment","decrement","page-increment","page-decrement"};
    for(unsigned i=0;i<count;++i){char *name=XmAccessibilityActionName(bridge,id,i);CHECK(name && !strcmp(name,names[i]));XtFree(name);}
    CHECK(XmAccessibilityActionName(bridge,id,count)==NULL);
    CHECK(!XmAccessibilityAction(bridge,id,count));
}
int main(int argc,char **argv) {
    XtAppContext app;Display*d;unsigned cases=0;
    /* Protocol numbers are from AT-SPI, independent of toolkit constants. */
    CHECK(XmACCESSIBLE_PROGRESS_BAR==42 && XmACCESSIBLE_TREE==65 && XmACCESSIBLE_CALENDAR==5);
    CHECK(!strcmp(_XmAccessibilityRoleName(42),"progress bar"));
    CHECK(!strcmp(_XmAccessibilityRoleName(65),"tree"));
    CHECK(!strcmp(_XmAccessibilityRoleName(5),"calendar"));
    CHECK(!strcmp(_XmAccessibilityRoleName(999),"unknown"));
    XtToolkitInitialize();app=XtCreateApplicationContext();d=XtOpenDisplay(app,NULL,"numericA11y","NumericA11y",NULL,0,&argc,argv);CHECK(d);
    for(unsigned profile=0;profile<3;++profile)for(unsigned number=0;number<2;++number)
    for(unsigned rtl=0;rtl<2;++rtl)for(unsigned wrap=0;wrap<2;++wrap) {
        CHECK(XmApplyAppearanceProfile(d,profile==0?XmAPPEARANCE_CLASSIC:profile==1?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));
        Widget shell=XtAppCreateShell("numericA11y","NumericA11y",applicationShellWidgetClass,d,NULL,0);
        Widget board=XtVaCreateManagedWidget("board",xmRowColumnWidgetClass,shell,NULL);
        Widget w=number?XmVaCreateManagedNumberBox(board,"quantity",XmNnumberMinimum,-10,XmNnumberMaximum,10,XmNnumberStep,3,XmNnumberPageStep,7,XmNnumberWrap,wrap,XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL):XmVaCreateManagedStepper(board,"quantity",XmNstepperMinimum,-10,XmNstepperMaximum,10,XmNstepperStep,3,XmNstepperWrap,wrap,XmNlayoutDirection,rtl?XmRIGHT_TO_LEFT:XmLEFT_TO_RIGHT,NULL);
        XtAddCallback(w,number?XmNnumberValueChangedCallback:XmNstepperValueChangedCallback,changed,NULL);
        XtRealizeWidget(shell);drain(app,d);bridge=XmAccessibilityCreate(shell,"Numeric controls");CHECK(bridge);
        WidgetClass classes[]={xmProgressBarWidgetClass,xmTreeViewWidgetClass,xmCalendarWidgetClass};
        unsigned roles[]={42,65,5};
        for(unsigned r=0;r<3;++r){Widget probe=XtVaCreateWidget("roleProbe",classes[r],board,NULL);XmAccessibilityRefresh(bridge);XmAccessibleInfo role;CHECK(XmAccessibilityQuery(bridge,XmAccessibilityWidget(bridge,probe),&role));CHECK(role.role==roles[r]);XmAccessibleInfoClear(&role);XtDestroyWidget(probe);}
        XmAccessibleId id=XmAccessibilityWidget(bridge,w);CHECK(id);changes=destruction=0;
        query(id,0,number?4:2);CHECK(XmAccessibilityFocus(bridge,id));
        if(number)CHECK(XmGetFocusWidget(shell)==XmNumberBoxGetTextField(w));
        else CHECK(XmGetFocusWidget(shell)==XtNameToWidget(w,"increment") || XmGetFocusWidget(shell)==XtNameToWidget(w,"decrement"));
        CHECK(XmAccessibilityAction(bridge,id,0));CHECK(value(w)==3 && changes==1);
        CHECK(XmAccessibilityAction(bridge,id,1));CHECK(value(w)==0 && changes==2);
        if(number){CHECK(XmAccessibilityAction(bridge,id,2));CHECK(value(w)==7 && changes==3);CHECK(XmAccessibilityAction(bridge,id,3));CHECK(value(w)==0 && changes==4);}
        CHECK(XmAccessibilitySetValue(bridge,id,9));CHECK(value(w)==9);
        unsigned before=changes;CHECK(XmAccessibilitySetValue(bridge,id,9));CHECK(changes==before);
        CHECK(!XmAccessibilitySetValue(bridge,id,9.5));CHECK(!XmAccessibilitySetValue(bridge,id,NAN));CHECK(!XmAccessibilitySetValue(bridge,id,INFINITY));CHECK(!XmAccessibilitySetValue(bridge,id,11));CHECK(!XmAccessibilitySetValue(bridge,id,-11));CHECK(value(w)==9 && changes==before);
        CHECK(XmAccessibilityAction(bridge,id,0));CHECK(value(w)==(wrap?-10:10));
        set(w,-9);CHECK(XmAccessibilityAction(bridge,id,1));CHECK(value(w)==(wrap?10:-10));
        for(unsigned ancestor=0;ancestor<2;++ancestor){set(w,0);XtSetSensitive(ancestor?board:w,False);before=changes;CHECK(!XmAccessibilityAction(bridge,id,0));CHECK(!XmAccessibilitySetValue(bridge,id,4));CHECK(value(w)==0 && changes==before);query(id,0,number?4:2);XtSetSensitive(ancestor?board:w,True);}
        Widget dialog=XmCreateInformationDialog(shell,"modal",NULL,0);XtVaSetValues(dialog,XmNdialogStyle,XmDIALOG_FULL_APPLICATION_MODAL,NULL);XtManageChild(dialog);drain(app,d);CHECK(!XmAccessibilityAction(bridge,id,0));CHECK(!XmAccessibilitySetValue(bridge,id,4));CHECK(value(w)==0);XtUnmanageChild(dialog);XtDestroyWidget(dialog);drain(app,d);
        CHECK(XmAccessibilitySetValue(bridge,id,4));CHECK(value(w)==4);
        for(unsigned i=0;i<2;++i){Widget child=XtNameToWidget(w,i?"decrement":"increment");XmAccessibleInfo info;CHECK(XmAccessibilityQuery(bridge,XmAccessibilityWidget(bridge,child),&info));CHECK(info.role==XmACCESSIBLE_BUTTON && !strcmp(info.name,i?"Decrease value":"Increase value"));XmAccessibleInfoClear(&info);}
        if(number){Widget editor=XmNumberBoxGetTextField(w);XmTextFieldSetString(editor,"invalid");XtCallActionProc(editor,"activate",NULL,NULL,0);CHECK(XmNumberBoxIsInvalid(w));query(id,4,4);CHECK(XmAccessibilitySetValue(bridge,id,6));CHECK(!XmNumberBoxIsInvalid(w));char *text=XmTextFieldGetString(editor);CHECK(!strcmp(text,"6"));XtFree(text);}
        XtVaSetValues(w,number?XmNnumberMinimum:XmNstepperMinimum,INT_MIN,
            number?XmNnumberMaximum:XmNstepperMaximum,INT_MAX,
            number?XmNnumberStep:XmNstepperStep,INT_MAX,NULL);
        XmAccessibleInfo range;CHECK(XmAccessibilityQuery(bridge,id,&range));
        CHECK(range.minimum_value==INT_MIN && range.maximum_value==INT_MAX && range.minimum_increment==INT_MAX);XmAccessibleInfoClear(&range);
        CHECK(!XmAccessibilitySetValue(bridge,id,(double)INT_MAX+1));CHECK(!XmAccessibilitySetValue(bridge,id,(double)INT_MIN-1));
        CHECK(XmAccessibilitySetValue(bridge,id,INT_MAX));CHECK(XmAccessibilityAction(bridge,id,0));CHECK(value(w)==(wrap?INT_MIN:INT_MAX));
        /* Destroying the native owner during a semantic action invalidates IDs. */
        set(w,0);destruction=1;CHECK(XmAccessibilityAction(bridge,id,0));XmAccessibleInfo gone;CHECK(!XmAccessibilityQuery(bridge,id,&gone));CHECK(!XmAccessibilitySetValue(bridge,id,1));CHECK(!XmAccessibilityAction(bridge,id,0));XmAccessibilityDestroy(bridge);bridge=NULL;destruction=0;
        /* Destroy the bridge during a direct value callback, without touching it afterward. */
        w=number?XmVaCreateManagedNumberBox(board,"replacement",NULL):XmVaCreateManagedStepper(board,"replacement",NULL);
        XtAddCallback(w,number?XmNnumberValueChangedCallback:XmNstepperValueChangedCallback,changed,NULL);bridge=XmAccessibilityCreate(shell,"Replacement");CHECK(bridge);id=XmAccessibilityWidget(bridge,w);destruction=2;CHECK(XmAccessibilitySetValue(bridge,id,1));CHECK(bridge==NULL);destruction=0;
        XtDestroyWidget(shell);drain(app,d);++cases;
    }
    printf("PASS numeric accessibility: %u configurations\n",cases);XtCloseDisplay(d);XtDestroyApplicationContext(app);return 0;
}
