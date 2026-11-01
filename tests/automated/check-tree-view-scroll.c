/* Scrolled TreeView: overflow, thumb/wheel synchronization and click focus. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <X11/IntrinsicP.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while(0)
static void pump(XtAppContext app, Display *d) {
    for (int n=0;n<4;++n) { XSync(d,False); while (XtAppPending(app)) XtAppProcessEvent(app,XtIMAll); }
}
static void button(Widget w, unsigned btn, int y) {
    XEvent e={0}; e.xbutton.type=ButtonPress; e.xbutton.display=XtDisplay(w);
    e.xbutton.window=XtWindow(w); e.xbutton.button=btn; e.xbutton.x=50; e.xbutton.y=y;
    XtDispatchEvent(&e);
}
int main(int argc,char **argv) {
    XtAppContext app; Display *d; int ids[80]; XmTreeViewNode nodes[80];
    XtToolkitInitialize(); app=XtCreateApplicationContext();
    d=XtOpenDisplay(app,NULL,"treeScroll","TreeScroll",NULL,0,&argc,argv); CHECK(d);
    CHECK(XmApplyAppearanceProfile(d,XmAPPEARANCE_MODERN_LIGHT));
    Widget shell=XtAppCreateShell("treeScroll","TreeScroll",applicationShellWidgetClass,d,NULL,0);
    Widget form=XtVaCreateManagedWidget("form",xmFormWidgetClass,shell,XmNwidth,400,XmNheight,300,NULL);
    Widget other=XtVaCreateManagedWidget("other",xmPushButtonWidgetClass,form,NULL);
    Widget sc=XtVaCreateManagedWidget("viewport",xmScrolledWindowWidgetClass,form,
      XmNscrollingPolicy,XmAPPLICATION_DEFINED,XmNtopAttachment,XmATTACH_WIDGET,
      XmNtopWidget,other,XmNleftAttachment,XmATTACH_FORM,XmNrightAttachment,XmATTACH_FORM,
      XmNbottomAttachment,XmATTACH_FORM,NULL);
    Widget tree=XtVaCreateManagedWidget("tree",xmTreeViewWidgetClass,sc,NULL);
    for(int i=0;i<80;++i) { ids[i]=i; nodes[i]=(XmTreeViewNode){&ids[i],NULL,"Item",False,False}; }
    XmTreeViewSetNodes(tree,nodes,80); XtRealizeWidget(shell); XtMapWidget(shell); pump(app,d);
    Widget bar=NULL; XtVaGetValues(sc,XmNverticalScrollBar,&bar,NULL); CHECK(bar && XtIsManaged(bar));
    int val,size,inc,page; XmScrollBarGetValues(bar,&val,&size,&inc,&page); CHECK(val==0 && size<80);
    XmScrollBarSetValues(bar,20,size,inc,page,True); pump(app,d); CHECK(XmTreeViewGetTopRow(tree)==20);
    button(tree,Button5,30); pump(app,d); XmScrollBarGetValues(bar,&val,&size,&inc,&page); CHECK(val==21);
    CHECK(XmProcessTraversal(other,XmTRAVERSE_CURRENT)); pump(app,d); CHECK(XmGetFocusWidget(shell)==other);
    button(tree,Button1,20); pump(app,d); CHECK(XmGetFocusWidget(shell)==tree);
    XEvent e={0}; e.xkey.type=KeyPress; e.xkey.display=d; e.xkey.window=XtWindow(tree); e.xkey.keycode=XKeysymToKeycode(d,XK_Tab);
    XtDispatchEvent(&e); pump(app,d); CHECK(XmGetFocusWidget(shell)!=tree);
    XmTreeViewSetNodes(tree,nodes,2); pump(app,d); CHECK(!XtIsManaged(bar));
    XmTreeViewSetNodes(tree,nodes,80); pump(app,d); CHECK(XtIsManaged(bar));
    nodes[0].has_children=True; nodes[0].expanded=False;
    for(int i=1;i<80;++i) nodes[i].parent_id=&ids[0];
    XmTreeViewSetNodes(tree,nodes,80); pump(app,d); CHECK(!XtIsManaged(bar));
    CHECK(XmTreeViewSetExpanded(tree,&ids[0],True,True)); pump(app,d); CHECK(XtIsManaged(bar));
    CHECK(XmTreeViewSetExpanded(tree,&ids[0],False,True)); pump(app,d); CHECK(!XtIsManaged(bar));
    XtDestroyWidget(tree); pump(app,d); XtDestroyWidget(shell); XtCloseDisplay(d); XtDestroyApplicationContext(app);
    puts("PASS TreeView scrolling and click focus"); return 0;
}
