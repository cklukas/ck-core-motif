/* Accessibility mutation admission follows native modal scope. LGPL-2.0-or-later. */
#include <X11/Shell.h>
#include <Xm/Accessible.h>
#include <Xm/Form.h>
#include <Xm/List.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBar.h>
#include <Xm/XmP.h>
#include <Xm/VendorSEP.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  Widget shell, body, button, focus_target, list, scroll;
  XmAccessibleId native, item, semantic, child;
  unsigned clicks, actions, focuses, values, selections;
  double value;
  Boolean selected;
  int query_fault;
  Boolean snapshot_modal;
} Pane;
static XtAppContext app;
static Display *display;
static XmAccessibility *bridge;
static unsigned checks, failures;
static const char *phase;
static Widget callback_modal;
static void check(int good, const char *detail) {
  ++checks;
  if (!good) { ++failures; fprintf(stderr,"FAIL %s: %s\n",phase,detail); }
}
static void settle(void) {
  unsigned budget=10000;
  XSync(display,False);
  while (XtAppPending(app)&&budget--) XtAppProcessEvent(app,XtIMAll);
  XSync(display,False);
}
static void clicked(Widget w,XtPointer data,XtPointer call) {
  (void)w;(void)call;++((Pane*)data)->clicks;
}
static Boolean query(XtPointer data,XmAccessibleInfo *info) {
  Pane *pane=(Pane*)data;
  int fault=pane->query_fault;pane->query_fault=0;
  if(fault==1) XtManageChild(callback_modal);
  else if(fault==2) XmAccessibilityRemove(bridge,pane->semantic);
  else if(fault==3) XtDestroyWidget(pane->shell);
  info->role=XmACCESSIBLE_LIST;
  info->name=XtNewString("Semantic control");
  info->states=XmACCESSIBLE_STATE(7)|XmACCESSIBLE_STATE(8)|XmACCESSIBLE_STATE(24);
  info->action_count=1;info->has_selection=True;info->has_value=True;
  info->minimum_value=0;info->maximum_value=100;info->current_value=pane->value;info->minimum_increment=1;
  return True;
}
static Boolean action(XtPointer data,unsigned index) {
  if(index) return False;
  ++((Pane*)data)->actions;return True;
}
static Boolean item_query(XtPointer data,XmAccessibleInfo *info) {
  (void)data;info->role=XmACCESSIBLE_LIST_ITEM;info->name=XtNewString("Semantic child");return True;
}
static Boolean focus(XtPointer data) {++((Pane*)data)->focuses;return True;}
static Boolean value(XtPointer data,double next) {
  Pane *pane=(Pane*)data;++pane->values;pane->value=next;return True;
}
static int count(XtPointer data) {
  Pane *pane=(Pane*)data;
  if(pane->snapshot_modal) {pane->snapshot_modal=False;XtManageChild(callback_modal);}
  return pane->selected?1:0;
}
static XmAccessibleId selected(XtPointer data,int index) {
  Pane *pane=(Pane*)data;return pane->selected&&!index?pane->child:0;
}
static Boolean child_selected(XtPointer data,XmAccessibleId child) {
  Pane *pane=(Pane*)data;return pane->selected&&child==pane->child;
}
static Boolean selection(XtPointer data,XmAccessibleSelectionOperation op,XmAccessibleId child) {
  Pane *pane=(Pane*)data;
  if(op==XmACCESSIBLE_CLEAR_SELECTION&&!child) {
    ++pane->selections;pane->selected=False;XmAccessibilityChanged(bridge,pane->semantic);return True;
  }
  if(op!=XmACCESSIBLE_SELECT_CHILD||child!=pane->child)return False;
  ++pane->selections;pane->selected=True;XmAccessibilityChanged(bridge,pane->semantic);return True;
}
static void contents(Pane *pane) {
  XmString strings[2];
  Widget row=XtVaCreateManagedWidget("row",xmRowColumnWidgetClass,pane->body,NULL);
  pane->button=XtVaCreateManagedWidget("activate",xmPushButtonWidgetClass,row,NULL);
  pane->focus_target=XtVaCreateManagedWidget("focus-target",xmPushButtonWidgetClass,row,NULL);
  XtAddCallback(pane->button,XmNactivateCallback,clicked,pane);
  strings[0]=XmStringCreateLocalized("first");strings[1]=XmStringCreateLocalized("second");
  pane->list=XtVaCreateManagedWidget("list",xmListWidgetClass,row,XmNitems,strings,XmNitemCount,2,XmNvisibleItemCount,2,XmNselectionPolicy,XmMULTIPLE_SELECT,NULL);
  XmStringFree(strings[0]);XmStringFree(strings[1]);
  pane->scroll=XtVaCreateManagedWidget("value",xmScrollBarWidgetClass,row,XmNminimum,0,XmNmaximum,100,XmNsliderSize,1,XmNvalue,0,XmNeditable,True,NULL);
}
static void top(Pane *pane,const char *name,int x) {
  memset(pane,0,sizeof(*pane));
  pane->shell=XtVaAppCreateShell(name,"AccessibleModal",applicationShellWidgetClass,display,XmNx,x,XmNy,20,NULL);
  pane->body=XtVaCreateManagedWidget("body",xmFormWidgetClass,pane->shell,NULL);contents(pane);XtRealizeWidget(pane->shell);settle();
}
static void dialog(Pane *pane,Widget parent,const char *name,unsigned char style) {
  Arg args[2];memset(pane,0,sizeof(*pane));
  XtSetArg(args[0],XmNdialogStyle,style);XtSetArg(args[1],XmNautoUnmanage,False);
  pane->body=XmCreateFormDialog(parent,(char*)name,args,2);pane->shell=XtParent(pane->body);contents(pane);XtManageChild(pane->body);settle();
}
static void attach(Pane *pane) {
  XmAccessibleProvider provider={0},child={0};
  check(XmAccessibilityAttachShell(bridge,pane->shell),"attach shell");XmAccessibilityRefresh(bridge);
  pane->native=XmAccessibilityWidget(bridge,pane->button);
  provider.query=query;provider.action=action;provider.focus=focus;provider.set_value=value;
  provider.selection_count=count;provider.selected_child=selected;provider.child_selected=child_selected;provider.change_selection=selection;
  pane->semantic=XmAccessibilityAdd(bridge,pane->native,&provider,pane);
  child.query=item_query;
  pane->child=XmAccessibilityAdd(bridge,pane->semantic,&child,pane);
  XmListSelectPos(pane->list,1,False);
  pane->item=XmAccessibilitySelectedChild(bridge,XmAccessibilityWidget(bridge,pane->list),0);
  XmListDeselectAllItems(pane->list);
  check(pane->native&&pane->semantic&&pane->child&&pane->item,"live native/provider/list identities");
}
static void input(Pane *pane,int allowed,const char *name) {
  unsigned clicks=pane->clicks,actions=pane->actions,focuses=pane->focuses,values=pane->values,selections=pane->selections;
  int current,size,inc,page;XmAccessibleInfo info;
  XmAccessibleId list=XmAccessibilityWidget(bridge,pane->list),scroll=XmAccessibilityWidget(bridge,pane->scroll);
  phase=name;pane->selected=False;pane->value=0;XmListDeselectAllItems(pane->list);XmScrollBarSetValues(pane->scroll,0,1,1,10,False);
  check(XmAccessibilityQuery(bridge,pane->semantic,&info),"read-only query remains available");XmAccessibleInfoClear(&info);
  check(XmAccessibilityAction(bridge,pane->native,0)==allowed&&pane->clicks==clicks+(unsigned)allowed,"native activation and callback");
  check(XmAccessibilityAction(bridge,pane->semantic,0)==allowed&&pane->actions==actions+(unsigned)allowed,"semantic activation and callback");
  check(XmAccessibilityFocus(bridge,pane->semantic)==allowed&&pane->focuses==focuses+(unsigned)allowed,"semantic focus and callback");
  check(XmAccessibilitySetValue(bridge,pane->semantic,3)==allowed&&pane->values==values+(unsigned)allowed&&pane->value==(allowed?3:0),"semantic value and callback");
  check(XmAccessibilityChangeSelection(bridge,pane->semantic,XmACCESSIBLE_SELECT_CHILD,pane->child)==allowed&&pane->selections==selections+(unsigned)allowed&&pane->selected==allowed,"semantic selection and callback");
  check(XmAccessibilityAction(bridge,pane->item,0)==allowed,"native list item activation");
  check(XmAccessibilitySelectionCount(bridge,list)==allowed,"native item activation selection state");
  XmListDeselectAllItems(pane->list);
  check(XmAccessibilityChangeSelection(bridge,list,XmACCESSIBLE_SELECT_CHILD,pane->item)==allowed,"native selection mutation");
  check(XmAccessibilitySelectionCount(bridge,list)==allowed,"native selection exact state");
  check(XmAccessibilitySetValue(bridge,scroll,4)==allowed,"native scalar mutation");
  XmScrollBarGetValues(pane->scroll,&current,&size,&inc,&page);check(current==(allowed?4:0),"native scalar exact state");
}
typedef struct {XmAccessibility *owner;Widget target;unsigned calls;Boolean retire_bridge;} FocusLifetime;
static void focus_lifetime(Widget widget,XtPointer data,XtPointer call) {
  FocusLifetime *state=(FocusLifetime*)data;XmFocusMovedCallbackStruct *moved=(XmFocusMovedCallbackStruct*)call;
  (void)widget;
  if(moved->new_focus!=state->target) return;
  ++state->calls;moved->cont=False;
  if(state->retire_bridge) {XmAccessibilityDestroy(state->owner);state->owner=NULL;}
  else XtDestroyWidget(state->target);
}
static void native_focus_lifetime(Boolean retire_bridge) {
  Pane pane;FocusLifetime state={0};
  XmAccessibleId target;XmAccessibleInfo info;
  phase=retire_bridge?"native-focus-retires-bridge":"native-focus-destroys-target";
  top(&pane,"focus-lifetime",600);
  state.target=pane.focus_target;
  settle();state.owner=XmAccessibilityCreate(pane.shell,"Native focus lifetime");state.retire_bridge=retire_bridge;
  check(state.owner!=NULL&&XmProcessTraversal(pane.button,XmTRAVERSE_CURRENT),"native initial focus established");
  target=XmAccessibilityWidget(state.owner,state.target);check(target!=0,"native focus target identity");
  XmeAddFocusChangeCallback(pane.shell,focus_lifetime,&state);
  check(!XmAccessibilityFocus(state.owner,target)&&state.calls==1,"native focus callback retires target/bridge without post-callback notification");
  XmeRemoveFocusChangeCallback(pane.shell,focus_lifetime,&state);
  if(state.owner) {
    check(!XmAccessibilityQuery(state.owner,target,&info),"destroyed native focus identity unavailable");
    XmAccessibilityDestroy(state.owner);
  }
  settle();XtDestroyWidget(pane.shell);settle();
}
int main(int argc,char **argv) {
  Pane first,other,primary,nested,full,victim;
  phase="setup";
  if(argc!=2||strcmp(argv[1],"--isolated-display"))return 2;
  argc=1;XtToolkitInitialize();app=XtCreateApplicationContext();display=XtOpenDisplay(app,NULL,"accessible-modality","AccessibleModal",NULL,0,&argc,argv);if(!display)return 2;
  top(&first,"first",20);top(&other,"independent",400);bridge=XmAccessibilityCreate(first.shell,"Modal admission fixture");if(!bridge)return 2;
  attach(&first);attach(&other);input(&first,1,"baseline-first");input(&other,1,"baseline-other");
  dialog(&primary,first.body,"primary",XmDIALOG_PRIMARY_APPLICATION_MODAL);attach(&primary);
  input(&first,0,"primary-blocked-parent");input(&other,1,"primary-independent");input(&primary,1,"primary-dialog");
  dialog(&nested,primary.body,"nested",XmDIALOG_PRIMARY_APPLICATION_MODAL);attach(&nested);
  input(&first,0,"nested-ancestor");input(&primary,0,"nested-parent");input(&other,1,"nested-independent");input(&nested,1,"nested-dialog");
  XtDestroyWidget(nested.shell);settle();input(&primary,1,"nested-destroy-restores-parent");input(&first,0,"nested-destroy-retains-outer");
  XtUnmanageChild(primary.body);settle();input(&first,1,"primary-popdown-restores-parent");
  dialog(&full,other.body,"full",XmDIALOG_FULL_APPLICATION_MODAL);attach(&full);
  input(&first,0,"full-blocks-independent");input(&other,0,"full-blocks-parent");input(&full,1,"full-dialog");
  XtDestroyWidget(full.shell);settle();input(&first,1,"full-destroy-restores-first");input(&other,1,"full-destroy-restores-other");
  XtAddGrab(other.body,True,False);
  input(&first,0,"direct-Xt-exclusive-blocks-other");input(&other,1,"direct-Xt-exclusive-owner");
  XtRemoveGrab(other.body);input(&first,1,"direct-Xt-remove-restores");
  phase="insensitive-provider-native-ancestor";XtSetSensitive(first.button,False);
  {unsigned actions=first.actions,focuses=first.focuses,values=first.values,selections=first.selections;
   check(!XmAccessibilityAction(bridge,first.semantic,0)&&first.actions==actions,"insensitive native owner blocks provider action");
   check(!XmAccessibilityFocus(bridge,first.semantic)&&first.focuses==focuses,"insensitive native owner blocks provider focus");
   check(!XmAccessibilitySetValue(bridge,first.semantic,7)&&first.values==values,"insensitive native owner blocks provider value");
   check(!XmAccessibilityChangeSelection(bridge,first.semantic,XmACCESSIBLE_SELECT_CHILD,first.child)&&first.selections==selections,"insensitive native owner blocks provider selection");}
  XtSetSensitive(first.button,True);
  phase="reentrant-query-opens-primary";callback_modal=primary.body;
  first.query_fault=1;
  {unsigned calls=first.values;double previous=first.value;
   check(!XmAccessibilitySetValue(bridge,first.semantic,7)&&first.values==calls&&first.value==previous,"modal opened during value query refuses callback");}
  XtUnmanageChild(primary.body);settle();
  phase="reentrant-selection-snapshot-opens-primary";first.snapshot_modal=True;first.selected=False;
  {unsigned calls=first.selections;
   check(!XmAccessibilityChangeSelection(bridge,first.semantic,XmACCESSIBLE_SELECT_CHILD,first.child)&&first.selections==calls&&!first.selected,"modal opened during selection snapshot refuses callback");}
  XtUnmanageChild(primary.body);settle();
  phase="reentrant-query-removes-target";first.query_fault=2;
  {unsigned calls=first.values;
   check(!XmAccessibilitySetValue(bridge,first.semantic,8)&&first.values==calls,"removed provider never receives setter callback");}
  phase="reentrant-query-destroys-owner";top(&victim,"victim",600);attach(&victim);victim.query_fault=3;
  {unsigned calls=victim.values;
   check(!XmAccessibilitySetValue(bridge,victim.semantic,9)&&victim.values==calls,"destroyed owner never receives setter callback");}
  settle();
  phase="unanchored-semantic-mutations";
  {Pane unanchored={0};XmAccessibleProvider provider={0};XmAccessibleInfo info;
   provider.query=query;provider.action=action;provider.focus=focus;provider.set_value=value;
   provider.selection_count=count;provider.selected_child=selected;provider.child_selected=child_selected;provider.change_selection=selection;
   unanchored.semantic=XmAccessibilityAdd(bridge,XmAccessibilityRoot(bridge),&provider,&unanchored);
   check(unanchored.semantic&&XmAccessibilityQuery(bridge,unanchored.semantic,&info),"unanchored semantic query remains available");XmAccessibleInfoClear(&info);
   check(!XmAccessibilityAction(bridge,unanchored.semantic,0)&&!unanchored.actions,"unanchored action has no implicit first-window owner");
   check(!XmAccessibilityFocus(bridge,unanchored.semantic)&&!unanchored.focuses,"unanchored focus rejected");
   check(!XmAccessibilitySetValue(bridge,unanchored.semantic,1)&&!unanchored.values,"unanchored value rejected");
   check(!XmAccessibilityChangeSelection(bridge,unanchored.semantic,XmACCESSIBLE_CLEAR_SELECTION,0)&&!unanchored.selections,"unanchored selection rejected");
   XmAccessibilityRemove(bridge,unanchored.semantic);}
  native_focus_lifetime(False);native_focus_lifetime(True);
  XmAccessibilityDestroy(bridge);XtDestroyWidget(primary.shell);XtDestroyWidget(first.shell);XtDestroyWidget(other.shell);settle();XtCloseDisplay(display);XtDestroyApplicationContext(app);
  printf("{\"checks\":%u,\"failures\":%u}\n",checks,failures);return failures?1:0;
}
