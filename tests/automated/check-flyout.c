#include <Xm/BulletinB.h>
#include <Xm/Flyout.h>
#include <Xm/Label.h>
#include <stdio.h>
#include <stdlib.h>

static unsigned checks, dismissals;
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); return EXIT_FAILURE; } ++checks; } while (0)
static void dismissed(Widget widget, XtPointer closure, XtPointer data) {
    (void)widget; (void)closure; (void)data; ++dismissals;
}

int main(int argc, char **argv) {
    XtAppContext app;
    Widget shell, board, owner, flyout;
    Arg args[8]; Cardinal count = 0;
    Position x, y; Dimension width, height;
    shell = XtVaAppInitialize(&app, "FlyoutTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 320, XmNheight, 120, NULL);
    board = XmCreateBulletinBoard(shell, (char *)"board", NULL, 0);
    XtManageChild(board);
    owner = XmCreateLabel(board, (char *)"owner", NULL, 0);
    XtVaSetValues(owner, XmNx, 290, XmNy, 20, XmNwidth, 25, XmNheight, 24,
                  NULL);
    XtManageChild(owner);
    XtSetArg(args[count], XmNflyoutText, (XtArgVal)"anchored help"); ++count;
    XtSetArg(args[count], XmNflyoutOffsetY, (XtArgVal)4); ++count;
    flyout = XmCreateFlyout(board, (char *)"flyout", args, count);
    XtAddCallback(flyout, XmNflyoutDismissCallback, dismissed, NULL);
    XtRealizeWidget(shell);
    XmFlyoutShow(flyout);
    XSync(XtDisplay(shell), False);
    CHECK(XmFlyoutIsVisible(flyout));
    XtVaGetValues(owner, XmNx, &x, XmNy, &y, XmNwidth, &width,
                  XmNheight, &height, NULL);
    CHECK(x == 290 && y == 20 && width == 25 && height == 24);
    XmFlyoutHide(flyout);
    CHECK(!XmFlyoutIsVisible(flyout) && dismissals == 1);
    XmFlyoutShow(flyout);
    CHECK(XmFlyoutIsVisible(flyout));
    XtUnmanageChild(board);
    XSync(XtDisplay(shell), False);
    while (XtAppPending(app) & XtIMXEvent)
        XtAppProcessEvent(app, XtIMXEvent);
    CHECK(!XmFlyoutIsVisible(flyout));
    XtDestroyWidget(flyout);
    printf("PASS flyout: %u checks\n", checks);
    return EXIT_SUCCESS;
}
