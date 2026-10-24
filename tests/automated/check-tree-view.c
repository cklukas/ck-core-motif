#include <Xm/Accessible.h>
#include <Xm/BulletinB.h>
#include <Xm/TreeView.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned checks, selections, expansions;
static int root_id, child_a_id, child_b_id, leaf_id;
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); return EXIT_FAILURE; } ++checks; } while (0)
static void selected(Widget w, XtPointer client, XtPointer data) { XmTreeViewCallbackStruct *cb = (XmTreeViewCallbackStruct *)data; (void)w; (void)client; if (cb && cb->id) ++selections; }
static void expanded(Widget w, XtPointer client, XtPointer data) { XmTreeViewCallbackStruct *cb = (XmTreeViewCallbackStruct *)data; (void)w; (void)client; if (cb && cb->expanded) ++expansions; }

int main(int argc, char **argv) {
    XtAppContext app; Widget shell, board, view; XmAccessibility *a; XmAccessibleInfo info; XmAccessibleId aid; Cardinal top; int root = 1, child_a = 2, child_b = 3, leaf = 4; XmTreeViewNode nodes[4];
    root_id = root; child_a_id = child_a; child_b_id = child_b; leaf_id = leaf;
    shell = XtVaAppInitialize(&app, "TreeViewTest", NULL, 0, &argc, argv, NULL, XmNwidth, 360, XmNheight, 150, NULL);
    board = XmCreateBulletinBoard(shell, (char *)"board", NULL, 0); XtManageChild(board);
    view = XmCreateTreeView(board, (char *)"view", NULL, 0);
    XtVaSetValues(view, XmNtreeViewVisibleRows, 3, XmNtreeViewRowHeight, 24, XmNtreeViewIndent, 18, XmNwidth, 280, XmNheight, 72, NULL);
    XtAddCallback(view, XmNtreeViewSelectionCallback, selected, NULL); XtAddCallback(view, XmNtreeViewExpansionCallback, expanded, NULL);
    nodes[0].id = &root_id; nodes[0].parent_id = NULL; nodes[0].label = "Root"; nodes[0].has_children = True; nodes[0].expanded = False;
    nodes[1].id = &child_a_id; nodes[1].parent_id = &root_id; nodes[1].label = "Alpha"; nodes[1].has_children = True; nodes[1].expanded = False;
    nodes[2].id = &child_b_id; nodes[2].parent_id = &root_id; nodes[2].label = "Beta"; nodes[2].has_children = False; nodes[2].expanded = False;
    nodes[3].id = &leaf_id; nodes[3].parent_id = &child_a_id; nodes[3].label = "Leaf"; nodes[3].has_children = False; nodes[3].expanded = False;
    XmTreeViewSetNodes(view, nodes, 4); CHECK(XmTreeViewGetNodeCount(view) == 4);
    nodes[3].id = nodes[2].id; XmTreeViewSetNodes(view, nodes, 4); CHECK(XmTreeViewGetNodeCount(view) == 4);
    nodes[3].id = &leaf_id; XtManageChild(view); XtRealizeWidget(shell); XSync(XtDisplay(shell), False);
    CHECK(XmTreeViewGetNodeCount(view) == 4); CHECK(XmTreeViewGetSelectedId(view) == NULL); CHECK(XmTreeViewGetExpanded(view, &root_id) == False);
    CHECK(XmTreeViewSetExpanded(view, &root_id, True, True)); CHECK(XmTreeViewGetExpanded(view, &root_id) == True && expansions == 1);
    XmTreeViewSetSelectedId(view, &child_a_id, True); CHECK(XmTreeViewGetSelectedId(view) == &child_a_id && selections == 1);
    CHECK(XmTreeViewSetExpanded(view, &child_a_id, True, True)); CHECK(XmTreeViewGetExpanded(view, &child_a_id) == True && expansions == 2);
    XmTreeViewSetTopRow(view, 1); top = XmTreeViewGetTopRow(view); CHECK(top == 1); XmTreeViewSetTopRow(view, 99); CHECK(XmTreeViewGetTopRow(view) <= 4);
    a = XmAccessibilityCreate(shell, "TreeViewTest"); CHECK(a != NULL); aid = XmAccessibilityWidget(a, view); CHECK(aid != XmACCESSIBLE_NONE); CHECK(XmAccessibilityQuery(a, aid, &info)); CHECK(info.role == XmACCESSIBLE_TREE && info.has_selection && info.has_value && info.maximum_value == 4); XmAccessibleInfoClear(&info); XmAccessibilityDestroy(a);
    XtDestroyWidget(view); printf("PASS tree view: %u checks\n", checks); return EXIT_SUCCESS;
}
