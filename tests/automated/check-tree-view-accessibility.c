/* Native TreeView semantic activation, admission and callback lifetime. */
#include <Xm/XmAll.h>
#include <Xm/Accessible.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c); exit(1); } } while (0)
static int ids[2];
static unsigned activations, expansions, selections, destruction;
static XmAccessibility *bridge;
static void activated(Widget widget, XtPointer closure, XtPointer data)
{
    XmTreeViewCallbackStruct *cb = data;
    (void)closure;
    CHECK(cb && cb->reason == XmCR_ACTIVATE && cb->event == NULL);
    CHECK(cb->id == &ids[1] && cb->parent_id == &ids[0]);
    CHECK(cb->index == 1 && !cb->expanded);
    ++activations;
    if (destruction == 1) XtDestroyWidget(widget);
    if (destruction == 2) { XmAccessibilityDestroy(bridge); bridge = NULL; }
}
static void expanded(Widget widget, XtPointer closure, XtPointer data)
{
    XmTreeViewCallbackStruct *cb = data;
    (void)widget; (void)closure;
    CHECK(cb && cb->event == NULL && cb->id == &ids[0]);
    ++expansions;
}
static void selected(Widget widget, XtPointer closure, XtPointer data)
{
    (void)widget; (void)closure; (void)data; ++selections;
}
static void actions(XmAccessibleId id, unsigned expected)
{
    XmAccessibleInfo info;
    char *name;
    CHECK(XmAccessibilityQuery(bridge,id,&info));
    CHECK(info.role == XmACCESSIBLE_TREE && info.action_count == expected);
    XmAccessibleInfoClear(&info);
    name = XmAccessibilityActionName(bridge,id,0);
    CHECK(expected ? name && !strcmp(name,"activate") : name == NULL);
    XtFree(name);
    CHECK(!XmAccessibilityAction(bridge,id,1));
}
int main(int argc, char **argv)
{
    XtAppContext app; Display *display; unsigned cases=0;
    const double scales[]={1,1.5,2};
    XmTreeViewNode nodes[]={
        {&ids[0],NULL,"Group",True,True},
        {&ids[1],&ids[0],"Page",False,False}};
    XtToolkitInitialize(); app=XtCreateApplicationContext();
    display=XtOpenDisplay(app,NULL,"treeAccessible","TreeAccessible",NULL,0,&argc,argv); CHECK(display);
    for (unsigned profile=0;profile<4;++profile)
    for (unsigned scale=0;scale<XtNumber(scales);++scale)
    for (unsigned rtl=0;rtl<2;++rtl) {
        Widget shell,tree;
        XmAccessibleId id;
        CHECK(XmApplyAppearanceProfile(display,(XmAppearanceProfile)profile));
        CHECK(XmSetDisplayScaleOverride(display,scales[scale]));
        shell=XtAppCreateShell("treeAccessible","TreeAccessible",applicationShellWidgetClass,display,NULL,0);
        tree=XtVaCreateManagedWidget("tree",xmTreeViewWidgetClass,shell,
            XmNwidth,500,XmNheight,500,XmNtreeViewRtl,rtl,NULL);
        XtAddCallback(tree,XmNactivateCallback,activated,NULL);
        XtAddCallback(tree,XmNtreeViewExpansionCallback,expanded,NULL);
        XtAddCallback(tree,XmNtreeViewSelectionCallback,selected,NULL);
        XtRealizeWidget(shell);
        bridge=XmAccessibilityCreate(shell,"TreeView semantic activation"); CHECK(bridge);
        id=XmAccessibilityWidget(bridge,tree); CHECK(id);
        activations=expansions=selections=destruction=0;
        actions(id,0); CHECK(!XmAccessibilityAction(bridge,id,0));
        XmTreeViewSetNodes(tree,nodes,2);
        XmTreeViewSetSelectedId(tree,&ids[0],False);
        actions(id,1); CHECK(XmAccessibilityAction(bridge,id,0));
        CHECK(!XmTreeViewGetExpanded(tree,&ids[0]) && expansions==1 && activations==0);
        CHECK(XmAccessibilityAction(bridge,id,0));
        CHECK(XmTreeViewGetExpanded(tree,&ids[0]) && expansions==2);
        XmTreeViewSetSelectedId(tree,&ids[1],False);
        actions(id,1); CHECK(XmAccessibilityAction(bridge,id,0));
        CHECK(activations==1 && selections==0);
        CHECK(XmAccessibilityAction(bridge,id,0)); CHECK(activations==2);
        XtSetSensitive(tree,False); actions(id,0);
        CHECK(!XmAccessibilityAction(bridge,id,0) && activations==2);
        XtSetSensitive(tree,True);
        XmTreeViewSetExpanded(tree,&ids[0],False,False);
        actions(id,0); CHECK(!XmAccessibilityAction(bridge,id,0));
        XmTreeViewSetExpanded(tree,&ids[0],True,False);
        actions(id,1);
        destruction=1; CHECK(XmAccessibilityAction(bridge,id,0)); CHECK(activations==3);
        CHECK(!XmAccessibilityAction(bridge,id,0));
        tree=XtVaCreateManagedWidget("replacement",xmTreeViewWidgetClass,shell,NULL);
        XtAddCallback(tree,XmNactivateCallback,activated,NULL);
        XmTreeViewSetNodes(tree,nodes,2); XmTreeViewSetSelectedId(tree,&ids[1],False);
        XmAccessibilityRefresh(bridge); id=XmAccessibilityWidget(bridge,tree); CHECK(id);
        destruction=2; CHECK(XmAccessibilityAction(bridge,id,0));
        CHECK(bridge==NULL && activations==4);
        XtDestroyWidget(shell); ++cases;
    }
    XtCloseDisplay(display); XtDestroyApplicationContext(app);
    printf("PASS TreeView accessibility: %u profile/scale/direction scenarios\n",cases);
    return 0;
}
