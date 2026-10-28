/* Vertical arrow spacing must depend on height, never on unrelated width. */
#include <X11/IntrinsicP.h>
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/SpinBP.h>
#include <stdio.h>
#include <stdlib.h>

#define CHECK(c) do { if (!(c)) { fprintf(stderr, "FAIL %d: %s\n", __LINE__, #c); exit(1); } } while (0)

static void
drain(XtAppContext app, Display *display)
{
    XSync(display, False);
    while (XtAppPending(app))
        XtAppProcessEvent(app, XtIMAll);
    XSync(display, False);
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Display *display;
    unsigned cases = 0;
    const double scales[] = { 1.0, 1.5, 2.0 };
    const XmAppearanceProfile profiles[] = {
        XmAPPEARANCE_CLASSIC, XmAPPEARANCE_MODERN_LIGHT,
        XmAPPEARANCE_MODERN_DARK, XmAPPEARANCE_HIGH_CONTRAST
    };
    const unsigned char layouts[] = { XmARROWS_BEGINNING, XmARROWS_END };

    XtToolkitInitialize();
    app = XtCreateApplicationContext();
    display = XtOpenDisplay(app, NULL, "spinHeight", "SpinHeight", NULL, 0,
                            &argc, argv);
    CHECK(display != NULL);

    for (unsigned p = 0; p < 4; ++p)
        for (unsigned scale = 0; scale < 3; ++scale)
            for (unsigned layout = 0; layout < 2; ++layout)
                for (unsigned rtl = 0; rtl < 2; ++rtl)
                    for (unsigned extra = 0; extra < 4; ++extra) {
                        CHECK(XmSetDisplayScaleOverride(display, scales[scale]));
                        CHECK(XmApplyAppearanceProfile(display, profiles[p]));
                        Widget shell = XtAppCreateShell(
                            "spinHeight", "SpinHeight", applicationShellWidgetClass,
                            display, NULL, 0);
                        Widget board = XtVaCreateManagedWidget(
                            "board", xmBulletinBoardWidgetClass, shell,
                            XmNwidth, 900, XmNheight, 700, NULL);
                        Widget spin = XtVaCreateManagedWidget(
                            "spin", xmSimpleSpinBoxWidgetClass, board,
                            XmNspinBoxChildType, XmNUMERIC,
                            XmNarrowLayout, layouts[layout], XmNspacing, 6,
                            XmNlayoutDirection,
                            rtl ? XmRIGHT_TO_LEFT : XmLEFT_TO_RIGHT, NULL);
                        XtRealizeWidget(shell);
                        drain(app, display);
                        XmSpinBoxWidget spin_widget = (XmSpinBoxWidget)spin;
                        Dimension arrow = spin_widget->spinBox.arrow_size;
                        Dimension height = 2 * arrow + extra * 4;
                        XtResizeWidget(spin, 400, height, 0);
                        drain(app, display);
                        int up = spin_widget->spinBox.up_arrow_rect.y;
                        int down = spin_widget->spinBox.down_arrow_rect.y;
                        CHECK(up >= 0 && down >= up + (int)arrow);
                        CHECK(down + (int)arrow <= (int)height);
                        XtResizeWidget(spin, 600, height, 0);
                        drain(app, display);
                        CHECK(spin_widget->spinBox.up_arrow_rect.y == up);
                        CHECK(spin_widget->spinBox.down_arrow_rect.y == down);
                        ++cases;
                        XtDestroyWidget(shell);
                        drain(app, display);
                    }

    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    printf("PASS spinbox height: %u constrained cases\n", cases);
    return 0;
}
