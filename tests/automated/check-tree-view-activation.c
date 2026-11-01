/* Native selection/activation separation, cancellation and callback lifetime. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while (0)
static int ids[3];
static unsigned activations, selections;
static Boolean destroy_on_activation;
static void activated(Widget widget, XtPointer closure, XtPointer data)
{
    XmTreeViewCallbackStruct *cb = data;
    (void)closure;
    CHECK(cb && cb->reason == XmCR_ACTIVATE && cb->event);
    CHECK(cb->id == &ids[1] || cb->id == &ids[2]);
    CHECK(cb->parent_id == &ids[0] && cb->index > 0 && !cb->expanded);
    ++activations;
    if (destroy_on_activation) XtDestroyWidget(widget);
}
static void selected(Widget widget, XtPointer closure, XtPointer data)
{
    (void)widget; (void)closure; (void)data; ++selections;
}
static void key(Widget tree, KeySym symbol)
{
    XEvent e = {0}; e.xkey.type=KeyPress; e.xkey.display=XtDisplay(tree);
    e.xkey.window=XtWindow(tree); e.xkey.keycode=XKeysymToKeycode(XtDisplay(tree),symbol);
    XtDispatchEvent(&e);
}
static void pointer(Widget tree, int type, int x, int row, unsigned button)
{
    Dimension height;
    XtVaGetValues(tree,XmNtreeViewRowHeight,&height,NULL);
    XEvent e = {0}; e.xbutton.type=type; e.xbutton.display=XtDisplay(tree);
    e.xbutton.window=XtWindow(tree); e.xbutton.button=button;
    e.xbutton.x=x; e.xbutton.y=row*height+height/2;
    XtDispatchEvent(&e);
}
int main(int argc, char **argv)
{
    XtAppContext app; Display *display; unsigned cases=0;
    const double scales[]={1,1.5,2};
    XmTreeViewNode nodes[]={
        {&ids[0],NULL,"Group",True,True},
        {&ids[1],&ids[0],"First page",False,False},
        {&ids[2],&ids[0],"Second page",False,False}};
    XtToolkitInitialize(); app=XtCreateApplicationContext();
    display=XtOpenDisplay(app,NULL,"treeActivation","TreeActivation",NULL,0,&argc,argv); CHECK(display);
    for (unsigned profile=0;profile<4;++profile)
    for (unsigned scale=0;scale<XtNumber(scales);++scale)
    for (unsigned rtl=0;rtl<2;++rtl) {
        Widget shell,tree;
        CHECK(XmApplyAppearanceProfile(display,(XmAppearanceProfile)profile));
        CHECK(XmSetDisplayScaleOverride(display,scales[scale]));
        shell=XtAppCreateShell("treeActivation","TreeActivation",applicationShellWidgetClass,display,NULL,0);
        tree=XtVaCreateManagedWidget("tree",xmTreeViewWidgetClass,shell,
            XmNwidth,500,XmNheight,500,XmNtreeViewRtl,rtl,NULL);
        XtAddCallback(tree,XmNactivateCallback,activated,NULL);
        XtAddCallback(tree,XmNtreeViewSelectionCallback,selected,NULL);
        XmTreeViewSetNodes(tree,nodes,3); XtRealizeWidget(shell);
        activations=selections=0; destroy_on_activation=False;
        XmTreeViewSetSelectedId(tree,&ids[0],False);
        key(tree,XK_Down); CHECK(selections==1 && activations==0);
        key(tree,XK_Return); CHECK(activations==1);
        key(tree,XK_KP_Enter); CHECK(activations==2);
        pointer(tree,ButtonPress,250,1,Button1); CHECK(activations==2);
        pointer(tree,ButtonRelease,250,1,Button1); CHECK(activations==3);
        pointer(tree,ButtonPress,250,2,Button1); CHECK(selections==2 && activations==3);
        pointer(tree,ButtonRelease,250,2,Button1); CHECK(activations==4);
        pointer(tree,ButtonRelease,250,2,Button1); CHECK(activations==4);
        pointer(tree,ButtonPress,250,2,Button1);
        pointer(tree,ButtonRelease,-1,2,Button1); CHECK(activations==4);
        pointer(tree,ButtonPress,250,2,Button1);
        pointer(tree,ButtonRelease,250,1,Button1); CHECK(activations==4);
        pointer(tree,ButtonPress,250,2,Button1); key(tree,XK_Escape);
        pointer(tree,ButtonRelease,250,2,Button1); CHECK(activations==4);
        pointer(tree,ButtonPress,250,2,Button1); XtSetSensitive(tree,False); XtSetSensitive(tree,True);
        pointer(tree,ButtonRelease,250,2,Button1); CHECK(activations==4);
        pointer(tree,ButtonPress,250,2,Button1); XmTreeViewSetNodes(tree,nodes,3);
        pointer(tree,ButtonRelease,250,2,Button1); CHECK(activations==4);
        pointer(tree,ButtonPress,250,1,Button3);
        pointer(tree,ButtonRelease,250,1,Button3); CHECK(activations==4);
        XmTreeViewSetSelectedId(tree,&ids[0],False); key(tree,XK_Return);
        CHECK(!XmTreeViewGetExpanded(tree,&ids[0]) && activations==4);
        key(tree,XK_Return); CHECK(XmTreeViewGetExpanded(tree,&ids[0]) && activations==4);
        XmTreeViewSetSelectedId(tree,&ids[1],False); destroy_on_activation=True;
        key(tree,XK_Return); CHECK(activations==5);
        XtDestroyWidget(shell); ++cases;
    }
    XtCloseDisplay(display); XtDestroyApplicationContext(app);
    printf("PASS TreeView activation: %u profile/scale/direction scenarios\n",cases);
    return 0;
}
