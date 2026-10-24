#define _DEFAULT_SOURCE
#include <Xm/Accessible.h>
#include <Xm/BulletinB.h>
#include <Xm/Spinner.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

static unsigned checks;
static unsigned state_changes;
static unsigned last_phase;

#define CHECK(condition) do { \
    if (!(condition)) { \
        fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, #condition); \
        return EXIT_FAILURE; \
    } \
    ++checks; \
} while (0)

static void state_changed(Widget widget, XtPointer closure, XtPointer data) {
    XmSpinnerCallbackStruct *callback = (XmSpinnerCallbackStruct *)data;
    (void)widget; (void)closure;
    ++state_changes;
    last_phase = callback->phase;
}

static void process_for(XtAppContext app, unsigned milliseconds) {
    unsigned elapsed;
    for (elapsed = 0; elapsed < milliseconds; elapsed += 10) {
        usleep(10000);
        while (XtAppPending(app))
            XtAppProcessEvent(app, XtIMAll);
    }
}

int main(int argc, char **argv) {
    XtAppContext app;
    Widget shell, board, spinner;
    XmAccessibility *accessibility;
    XmAccessibleInfo info;
    XmAccessibleId id;
    XmString label;
    Display *display;
    Dimension width, height;
    unsigned phase;

    shell = XtVaAppInitialize(&app, "SpinnerTest", NULL, 0, &argc, argv,
                              NULL, XmNwidth, 360, XmNheight, 100, NULL);
    board = XmCreateBulletinBoard(shell, (char *)"board", NULL, 0);
    XtManageChild(board);
    label = XmStringCreateLocalized((char *)"Working");
    spinner = XmVaCreateManagedSpinner(
        board, (char *)"spinner", XmNspinnerLabelString, label,
        XmNspinnerActive, True, XmNspinnerInterval, 20,
        XmNwidth, 220, XmNheight, 52, NULL);
    XmStringFree(label);
    XtAddCallback(spinner, XmNspinnerStateChangedCallback, state_changed, NULL);

    XtRealizeWidget(shell);
    display = XtDisplay(shell);
    XSync(display, False);
    XtVaGetValues(spinner, XmNwidth, &width, XmNheight, &height, NULL);
    CHECK(width == 220 && height == 52);
    CHECK(XmSpinnerGetActive(spinner));
    process_for(app, 80);
    phase = XmSpinnerGetPhase(spinner);
    CHECK(phase != 0);
    CHECK(last_phase == phase || state_changes == 0);

    XmSpinnerSetActive(spinner, False, True);
    CHECK(!XmSpinnerGetActive(spinner) && state_changes == 1);
    phase = XmSpinnerGetPhase(spinner);
    process_for(app, 80);
    CHECK(XmSpinnerGetPhase(spinner) == phase);
    XmSpinnerSetActive(spinner, True, True);
    CHECK(XmSpinnerGetActive(spinner) && state_changes == 2);

    accessibility = XmAccessibilityCreate(shell, "SpinnerTest");
    CHECK(accessibility != NULL);
    id = XmAccessibilityWidget(accessibility, spinner);
    CHECK(id != XmACCESSIBLE_NONE);
    CHECK(XmAccessibilityQuery(accessibility, id, &info));
    CHECK(info.role == XmACCESSIBLE_PROGRESS_BAR && !info.has_value);
    CHECK((info.states & XmACCESSIBLE_STATE(32)) != 0);
    XmAccessibleInfoClear(&info);
    XmAccessibilityDestroy(accessibility);

    XtUnmanageChild(spinner);
    phase = XmSpinnerGetPhase(spinner);
    process_for(app, 80);
    CHECK(XmSpinnerGetPhase(spinner) == phase);
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    printf("PASS spinner: %u checks\n", checks);
    return EXIT_SUCCESS;
}
