/* Repeated text-bearing frames must survive buffer swaps and replacement. */
#include "native-window.h"
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <X11/IntrinsicP.h>
#include <X11/Xutil.h>
#include <assert.h>
#include <stdio.h>

static void labels_visible(Widget tree, unsigned frame)
{
    Dimension width, height, row;
    Pixel foreground, background;
    XtVaGetValues(tree, XmNwidth, &width, XmNheight, &height,
        XmNtreeViewRowHeight, &row, XmNforeground, &foreground,
        XmNbackground, &background, NULL);
    assert(foreground != background && row > 6 && height / row >= 4);
    XImage *image = XGetImage(XtDisplay(tree), XtWindow(tree), 0, 0,
                              width, 4 * row, AllPlanes, ZPixmap);
    assert(image);
    for (unsigned i = 0; i < 4; ++i) {
        unsigned ink = 0;
        for (int y = i * row + 3; y < (int)((i + 1) * row) - 3; ++y)
            for (int x = 22; x < (int)width - 20; ++x)
                if (XGetPixel(image, x, y) == foreground) ++ink;
        if (ink < 8) {
            fprintf(stderr, "Buffered text missing: frame=%u row=%u ink=%u fg=%lu bg=%lu\n",
                    frame, i, ink, foreground, background);
            abort();
        }
    }
    XDestroyImage(image);
}
int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell = XtVaAppInitialize(&app, "BufferedText", NULL, 0,
                                     &argc, argv, NULL, NULL);
    assert(XmApplyAppearanceProfile(XtDisplay(shell), XmAPPEARANCE_MODERN_LIGHT));
    Widget host = XtVaCreateManagedWidget("host", xmBulletinBoardWidgetClass,
        shell, XmNwidth, 340, XmNheight, 200, NULL);
    Widget tree = XtVaCreateManagedWidget("tree", xmTreeViewWidgetClass, host,
        XmNwidth, 300, XmNheight, 160, XmNtreeViewRowHeight, 28, NULL);
    int ids[4];
    XmTreeViewNode nodes[4];
    const char *labels[] = {"First visible navigation label", "Second visible navigation label",
                           "Third visible navigation label", "Fourth visible navigation label"};
    for (int i = 0; i < 4; ++i)
        nodes[i] = (XmTreeViewNode){&ids[i], NULL, labels[i], False, False};
    XmTreeViewSetNodes(tree, nodes, 4);
    assert(XmTreeViewGetNodeCount(tree) == 4);
    XtRealizeWidget(shell);
    NativeWaitForViewable(shell);
    for (unsigned frame = 0; frame < 24; ++frame) {
        if (frame % 3 == 0)
            XtVaSetValues(tree, XmNwidth, 300 + 4 * (frame % 2), NULL);
        if (frame % 4 == 0) XmProcessTraversal(tree, XmTRAVERSE_CURRENT);
        XtClass(tree)->core_class.expose(tree, NULL, NULL);
        XSync(XtDisplay(tree), False);
        labels_visible(tree, frame);
    }
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    puts("PASS buffered text: 24 contrasting text frames across swaps, resize and focus");
    return 0;
}
