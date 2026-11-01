/* Native hit testing and keyboard order for arbitrary flat tree input. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <X11/keysym.h>
#include <stdio.h>
#include <stdlib.h>
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL %d: %s\n", __LINE__, #c); exit(1); } } while (0)
static int ids[6];
static void key(Widget tree, KeySym symbol) {
    XEvent event = {0};
    event.xkey.type = KeyPress; event.xkey.display = XtDisplay(tree);
    event.xkey.window = XtWindow(tree); event.xkey.keycode = XKeysymToKeycode(XtDisplay(tree), symbol);
    XtDispatchEvent(&event);
}
static void verify(Widget tree, const unsigned *order, unsigned count) {
    Dimension height;
    XtVaGetValues(tree, XmNtreeViewRowHeight, &height, NULL);
    XmTreeViewSetTopRow(tree, 0);
    for (unsigned row = 0; row < count; ++row) {
        XEvent event = {0};
        event.xbutton.type = ButtonPress; event.xbutton.display = XtDisplay(tree);
        event.xbutton.window = XtWindow(tree); event.xbutton.button = Button1;
        event.xbutton.x = 150; event.xbutton.y = row * height + height / 2;
        XtDispatchEvent(&event);
        CHECK(XmTreeViewGetSelectedId(tree) == &ids[order[row]]);
    }
    XmTreeViewSetSelectedId(tree, &ids[order[0]], False);
    for (unsigned row = 1; row < count; ++row) {
        key(tree, XK_Down); CHECK(XmTreeViewGetSelectedId(tree) == &ids[order[row]]);
    }
    for (unsigned row = count - 1; row > 0; --row) {
        key(tree, XK_Up); CHECK(XmTreeViewGetSelectedId(tree) == &ids[order[row - 1]]);
    }
}
int main(int argc, char **argv) {
    XtAppContext app; Display *display; unsigned cases = 0;
    const unsigned expected[] = {0,2,5,3,1,4}, collapsed[] = {0,1,4};
    const unsigned permutations[][6] = {{0,1,2,3,4,5},{5,2,3,0,4,1}};
    XmTreeViewNode nodes[] = {
        {&ids[0],NULL,"First root",True,True}, {&ids[1],NULL,"Second root",True,True},
        {&ids[2],&ids[0],"First child",True,True}, {&ids[3],&ids[0],"Second child",False,False},
        {&ids[4],&ids[1],"Other branch",False,False}, {&ids[5],&ids[2],"Grandchild",False,False}};
    XtToolkitInitialize(); app = XtCreateApplicationContext();
    display = XtOpenDisplay(app,NULL,"treeOrder","TreeOrder",NULL,0,&argc,argv); CHECK(display);
    for (unsigned profile = 0; profile < 4; ++profile)
    for (unsigned rtl = 0; rtl < 2; ++rtl)
    for (unsigned permutation = 0; permutation < 2; ++permutation) {
        Widget shell, tree; XmTreeViewNode input[6];
        CHECK(XmApplyAppearanceProfile(display,(XmAppearanceProfile)profile));
        shell = XtAppCreateShell("treeOrder","TreeOrder",applicationShellWidgetClass,display,NULL,0);
        tree = XtVaCreateManagedWidget("tree",xmTreeViewWidgetClass,shell,
            XmNwidth,300,XmNheight,240,XmNtreeViewVisibleRows,8,XmNtreeViewRtl,rtl,NULL);
        for (unsigned i = 0; i < 6; ++i) input[i] = nodes[permutations[permutation][i]];
        XmTreeViewSetNodes(tree,input,6); XtRealizeWidget(shell); XSync(display,False);
        verify(tree,expected,XtNumber(expected));
        CHECK(XmTreeViewSetExpanded(tree,&ids[0],False,False));
        verify(tree,collapsed,XtNumber(collapsed));
        CHECK(XmTreeViewSetExpanded(tree,&ids[0],True,False));
        verify(tree,expected,XtNumber(expected));
        XtDestroyWidget(shell); ++cases;
    }
    XtCloseDisplay(display); XtDestroyApplicationContext(app);
    printf("PASS TreeView order: %u cases\n",cases); return 0;
}
