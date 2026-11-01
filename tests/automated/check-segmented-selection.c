/* Selection assignment, reentrant model replacement and callback lifetime. */
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <Xm/RowColumn.h>
#include <Xm/SegmentedControl.h>
#include <Xm/ToggleB.h>
#include <Xm/Appearance.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static unsigned calls,late_calls;static Boolean replaced;
static void counted(Widget w,XtPointer a,XtPointer data){(void)w;(void)a;XmSegmentedControlCallbackStruct *c=data;CHECK(c && c->reason==XmCR_VALUE_CHANGED);++calls;}
static void doomed(Widget w,XtPointer a,XtPointer data){(void)a;(void)data;XtDestroyWidget(w);}
static void late(Widget w,XtPointer a,XtPointer data){(void)w;(void)a;(void)data;++late_calls;}
static void install(Widget w,Boolean disabled_last){XmString text=XmStringCreateLocalized("Choice");XmSegmentedControlItem items[]={{"original-one",text,XmUNSPECIFIED_PIXMAP,True},{"original-two",text,XmUNSPECIFIED_PIXMAP,True},{"original-three",text,XmUNSPECIFIED_PIXMAP,!disabled_last}};CHECK(XmSegmentedControlSetItems(w,items,3));XmStringFree(text);}
static void replace(Widget w,XtPointer a,XtPointer data){(void)a;(void)data;XmString text=XmStringCreateLocalized("Replacement");XmSegmentedControlItem item={"new-model",text,XmUNSPECIFIED_PIXMAP,True};CHECK(XmSegmentedControlSetItems(w,&item,1));XmStringFree(text);replaced=True;}
static void snapshot(Widget w,XtPointer a,XtPointer data){(void)w;(void)a;XmSegmentedControlCallbackStruct *c=data;CHECK(replaced && c->selected_index==1 && c->selected_id && !strcmp(c->selected_id,"original-two"));++late_calls;}
static void drain(XtAppContext app,Display*d){XSync(d,False);unsigned n=0;while(XtAppPending(app)){CHECK(++n<10000);XtAppProcessEvent(app,XtIMAll);XSync(d,False);}}
static void key(Widget w,KeySym symbol){XEvent event={0};event.xkey.type=KeyPress;event.xkey.display=XtDisplay(w);event.xkey.window=XtWindow(w);event.xkey.keycode=XKeysymToKeycode(XtDisplay(w),symbol);event.xkey.same_screen=True;XtDispatchEvent(&event);}
int main(int argc,char **argv){XtAppContext app;XtToolkitInitialize();app=XtCreateApplicationContext();Display*d=XtOpenDisplay(app,NULL,"selection","Selection",NULL,0,&argc,argv);CHECK(d);unsigned cases=0;
for(unsigned profile=0;profile<3;++profile)for(unsigned rtl=0;rtl<2;++rtl)for(unsigned clear=0;clear<2;++clear){CHECK(XmApplyAppearanceProfile(d,profile==0?XmAPPEARANCE_CLASSIC:profile==1?XmAPPEARANCE_MODERN_LIGHT:XmAPPEARANCE_MODERN_DARK));Widget shell=XtAppCreateShell("selection","Selection",applicationShellWidgetClass,d,NULL,0);Widget row=XtVaCreateManagedWidget("row",xmRowColumnWidgetClass,shell,NULL);Widget w=XmVaCreateManagedSegmentedControl(row,"segments",XmNSegmentedAllowClear,clear,XmNSegmentedRtl,rtl,NULL);install(w,True);XtRealizeWidget(shell);drain(app,d);XtAddCallback(w,XmNSegmentedSelectionCallback,counted,NULL);
unsigned before=calls;CHECK(XmSegmentedControlSetSelectedIndex(w,1,True));CHECK(calls==before+1);CHECK(XmSegmentedControlSetSelectedIndex(w,1,True));CHECK(XmSegmentedControlGetSelectedIndex(w)==1 && calls==before+1);CHECK(!XmSegmentedControlSetSelectedIndex(w,2,True));CHECK(!XmSegmentedControlSetSelectedIndex(w,-2,True));XmSegmentedControlClearSelection(w,True);CHECK(calls==before+2);XmSegmentedControlClearSelection(w,True);CHECK(calls==before+2);
Widget child=XtNameToWidget(w,"segment1");CHECK(child);CHECK(XmSegmentedControlSetSelectedIndex(w,1,False));XmToggleButtonSetState(child,False,True);CHECK(XmSegmentedControlGetSelectedIndex(w)==(clear?-1:1));CHECK(XmToggleButtonGetState(child)==!clear);
CHECK(XmSegmentedControlSetSelectedIndex(w,1,False));before=calls;key(child,XK_End);drain(app,d);CHECK(XmSegmentedControlGetSelectedIndex(w)==1 && calls==before);key(child,XK_Home);drain(app,d);CHECK(XmSegmentedControlGetSelectedIndex(w)==0);child=XtNameToWidget(w,"segment0");before=calls;key(child,XK_Home);drain(app,d);CHECK(XmSegmentedControlGetSelectedIndex(w)==0 && calls==before);
XtAddCallback(w,XmNSegmentedSelectionCallback,replace,NULL);XtAddCallback(w,XmNSegmentedSelectionCallback,snapshot,NULL);replaced=False;unsigned observed=late_calls;CHECK(XmSegmentedControlSetSelectedIndex(w,1,True));CHECK(late_calls==observed+1 && XmSegmentedControlGetItemCount(w)==1);XtDestroyWidget(w);drain(app,d);
w=XmVaCreateManagedSegmentedControl(row,"destroyAPI",NULL);install(w,False);XtAddCallback(w,XmNSegmentedSelectionCallback,doomed,NULL);XtAddCallback(w,XmNSegmentedSelectionCallback,late,NULL);observed=late_calls;CHECK(XmSegmentedControlSetSelectedIndex(w,0,True));CHECK(late_calls==observed);drain(app,d);
w=XmVaCreateManagedSegmentedControl(row,"destroyKey",NULL);install(w,False);CHECK(XmSegmentedControlSetSelectedIndex(w,0,False));drain(app,d);XtAddCallback(w,XmNSegmentedSelectionCallback,doomed,NULL);XtAddCallback(w,XmNSegmentedSelectionCallback,late,NULL);key(XtNameToWidget(w,"segment0"),XK_End);drain(app,d);CHECK(late_calls==observed);
w=XmVaCreateManagedSegmentedControl(row,"replaceKey",NULL);install(w,False);CHECK(XmSegmentedControlSetSelectedIndex(w,0,False));XtAddCallback(w,XmNSegmentedSelectionCallback,replace,NULL);drain(app,d);key(XtNameToWidget(w,"segment0"),XK_End);drain(app,d);CHECK(XmSegmentedControlGetItemCount(w)==1);XtDestroyWidget(shell);drain(app,d);++cases;}
XtCloseDisplay(d);XtDestroyApplicationContext(app);printf("PASS segmented selection: %u configurations\n",cases);return 0;}
