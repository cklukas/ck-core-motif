#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <Xm/TreeViewP.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do{if(!(c)){fprintf(stderr,"FAIL %d: %s\n",__LINE__,#c);exit(1);}}while(0)
static void verify(Widget tree) {
    XmTreeViewWidget view=(XmTreeViewWidget)tree;XtWidgetGeometry preferred={0};
    Dimension width,height,padding;XmString label=XmStringCreateLocalized("Commands and navigation with a longer label");
    XmStringExtent(view->tree_view.render_table,label,&width,&height);XmStringFree(label);
    CHECK(XmGetAppearanceSpacing(XtDisplay(tree),XmAPPEARANCE_SPACING_XS,&padding));
    CHECK(view->tree_view.row_height>=height+2*padding);
    XtQueryGeometry(tree,NULL,&preferred);
    CHECK(preferred.width>=width+view->tree_view.indent+22);
    CHECK(preferred.height==view->tree_view.row_height*view->tree_view.visible_rows);
    CHECK(view->core.height==preferred.height);
}
int main(int argc,char **argv) {
    XtAppContext app;Display *display;Widget shell,tree;int root=1,child=2;
    XmTreeViewNode nodes[]={{&root,NULL,"Root",True,True},{&child,&root,"Commands and navigation with a longer label",False,False}};
    XtToolkitInitialize();app=XtCreateApplicationContext();
    display=XtOpenDisplay(app,NULL,"treeMetrics","TreeMetrics",NULL,0,&argc,argv);CHECK(display);
    for(unsigned scale=1;scale<=3;++scale) {
        CHECK(XmSetDisplayScaleOverride(display,scale));CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_MODERN_LIGHT));
        shell=XtAppCreateShell("treeMetrics","TreeMetrics",applicationShellWidgetClass,display,NULL,0);
        tree=XtVaCreateManagedWidget("tree",xmTreeViewWidgetClass,shell,NULL);
        XmTreeViewSetNodes(tree,nodes,2);verify(tree);
        XmTreeViewSetSelectedId(tree,&child,False);
        XtVaSetValues(tree,XmNtreeViewVisibleRows,1,NULL);
        CHECK(XmTreeViewGetTopRow(tree)==1);
        XmTreeViewSetTopRow(tree,0);XmTreeViewSetSelectedId(tree,&child,False);
        CHECK(XmTreeViewGetTopRow(tree)==1);
        CHECK(XmSetDisplayScaleOverride(display,scale==3?1:scale+1));verify(tree);
        XtVaSetValues(tree,XmNtreeViewRowHeight,7,NULL);
        CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_MODERN_DARK));
        CHECK(((XmTreeViewWidget)tree)->tree_view.row_height==7);
        XtVaSetValues(tree,XmNtreeViewRowHeight,0,NULL);verify(tree);
        XtVaSetValues(tree,XmNheight,37,NULL);
        CHECK(XmApplyAppearanceProfile(display,XmAPPEARANCE_MODERN_LIGHT));
        CHECK(((XmTreeViewWidget)tree)->core.height==37);
        XtVaSetValues(tree,XmNheight,0,NULL);verify(tree);
        /* Parent allocation must not be fed back into preferred rows. */
        {
            XmTreeViewWidget view=(XmTreeViewWidget)tree;
            Cardinal preferred_rows;
            XtVaSetValues(tree,XmNtreeViewVisibleRows,1,NULL);
            preferred_rows=view->tree_view.visible_rows;
            XtResizeWidget(tree,view->core.width,3*view->tree_view.row_height,0);
            XmTreeViewSetTopRow(tree,0);
            XmTreeViewSetSelectedId(tree,&child,False);
            CHECK(XmTreeViewGetTopRow(tree)==0);
            CHECK(view->tree_view.visible_rows==preferred_rows);
            XtResizeWidget(tree,view->core.width,view->tree_view.row_height,0);
            CHECK(XmTreeViewGetTopRow(tree)==1);
            XtResizeWidget(tree,view->core.width,3*view->tree_view.row_height,0);
            CHECK(XmTreeViewGetTopRow(tree)==0);
            CHECK(view->tree_view.visible_rows==preferred_rows);
        }
        XtDestroyWidget(shell);
    }
    XtCloseDisplay(display);XtDestroyApplicationContext(app);puts("PASS TreeView metrics");return 0;
}
