#define _POSIX_C_SOURCE 200809L
#include <Xm/Accessible.h>
#include <Xm/BulletinB.h>
#include <Xm/Label.h>
#include <Xm/TeachingTip.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static unsigned checks, dismissals;
#define CHECK(c) do { if (!(c)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #c); return EXIT_FAILURE; } ++checks; } while (0)

static void dismissed(Widget widget, XtPointer closure, XtPointer data) {
    (void)widget; (void)closure; (void)data; ++dismissals;
}

static void wait_for_timeout(XtAppContext app) {
    struct timespec delay = {0, 60000000L};
    nanosleep(&delay, NULL);
    while (XtAppPending(app) & XtIMXEvent)
        XtAppProcessEvent(app, XtIMXEvent);
    while (XtAppPending(app) & XtIMTimer)
        XtAppProcessEvent(app, XtIMTimer);
}

int main(int argc, char **argv) {
    XtAppContext app;
    Widget shell, board, owner, tip;
    XmString title, text;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId id;
    Position x, y;
    Dimension width, height;
    (void)argv;

    shell = XtVaAppInitialize(&app, "TeachingTipTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 320, XmNheight, 120, NULL);
    board = XmCreateBulletinBoard(shell, (char *)"board", NULL, 0);
    XtManageChild(board);
    owner = XmCreateLabel(board, (char *)"owner", NULL, 0);
    XtVaSetValues(owner, XmNx, 290, XmNy, 20, XmNwidth, 25, XmNheight, 24,
                  NULL);
    XtManageChild(owner);
    title = XmStringCreateLocalized((char *)"Getting started");
    text = XmStringCreateLocalized((char *)"Use the toolbar to begin.");
    tip = XmCreateTeachingTip(board, (char *)"tip", NULL, 0);
    XtVaSetValues(tip, XmNteachingTipTitle, title, XmNteachingTipText, text,
                  XmNteachingTipDelay, 40, XmNteachingTipOffsetY, 4, NULL);
    XmStringFree(title);
    XmStringFree(text);
    XtAddCallback(tip, XmNteachingTipDismissCallback, dismissed, NULL);
    XtRealizeWidget(shell);
    XmTeachingTipShow(tip);
    CHECK(!XmTeachingTipIsVisible(tip));
    wait_for_timeout(app);
    CHECK(XmTeachingTipIsVisible(tip));
    XtVaGetValues(owner, XmNx, &x, XmNy, &y, XmNwidth, &width,
                  XmNheight, &height, NULL);
    CHECK(x == 290 && y == 20 && width == 25 && height == 24);
    accessibility = XmAccessibilityCreate(shell, "TeachingTipTest");
    CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, tip);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.role == XmACCESSIBLE_PANEL && info.name != NULL);
    CHECK(info.name[0] == 'G');
    XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);
    XmTeachingTipHide(tip);
    CHECK(!XmTeachingTipIsVisible(tip) && dismissals == 1);
    XtVaSetValues(tip, XmNteachingTipDelay, 0, NULL);
    XmTeachingTipShow(tip);
    CHECK(XmTeachingTipIsVisible(tip));
    XtUnmanageChild(board);
    XSync(XtDisplay(shell), False);
    while (XtAppPending(app) & XtIMXEvent)
        XtAppProcessEvent(app, XtIMXEvent);
    CHECK(!XmTeachingTipIsVisible(tip));
    XtDestroyWidget(tip);
    XtDestroyWidget(shell);
    XtCloseDisplay(XtDisplay(shell));
    XtDestroyApplicationContext(app);
    printf("PASS teaching-tip: %u checks\n", checks);
    return EXIT_SUCCESS;
}
