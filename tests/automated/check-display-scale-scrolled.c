/* Verify live scaling of toolkit-owned ScrolledWindow dimensions. */
#include <Xm/Display.h>
#include <Xm/ScrolledW.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <stdio.h>

static int
fail(const char *message)
{
    fprintf(stderr, "display-scale scrolled window: %s\n", message);
    return 1;
}

static Dimension
read_width(Widget widget)
{
    Dimension width;
    XtVaGetValues(widget, XmNwidth, &width, NULL);
    return width;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, ordinary, explicit_shell, explicit, scaled_shell, scaled;
    Display *display;
    Dimension ordinary_width, explicit_width, scaled_width;
    XtWidgetGeometry preferred;
    Arg automatic_args[1];

    shell = XtVaAppInitialize(&app, "DisplayScaleScrolled", NULL, 0,
                              &argc, argv, NULL, NULL);
    display = XtDisplay(shell);
    XtVaSetValues(shell, XmNwidth, (Dimension)100,
                  XmNheight, (Dimension)100, NULL);
    if (!XmSetDisplayScaleOverride(display, 1.0))
        return fail("initial scale override failed");
    XtSetArg(automatic_args[0], XmNscrollingPolicy, XmAUTOMATIC);
    ordinary = XmCreateScrolledWindow(shell, "ordinary", automatic_args, 1);
    XtManageChild(ordinary);
    XtRealizeWidget(shell);
    ordinary_width = read_width(ordinary);

    explicit_shell = XtVaCreatePopupShell("explicitShell",
                                          topLevelShellWidgetClass, shell,
                                          NULL);
    explicit = XmCreateScrolledWindow(explicit_shell, "explicit",
                                      automatic_args, 1);
    XtVaSetValues(explicit, XmNwidth, (Dimension)140,
                  XmNheight, (Dimension)120, NULL);
    XtManageChild(explicit);
    XtRealizeWidget(explicit_shell);
    explicit_width = read_width(explicit);
    if (ordinary_width != 100 || explicit_width != 140) {
        fprintf(stderr, "initial widths: ordinary=%u explicit=%u\n",
                ordinary_width, explicit_width);
        return fail("initial default or explicit width was unexpected");
    }

    if (!XmSetDisplayScaleOverride(display, 2.0))
        return fail("live scale override failed");
    XSync(display, False);
    ordinary_width = read_width(ordinary);
    explicit_width = read_width(explicit);
    if (ordinary_width != 200 || explicit_width != 140) {
        fprintf(stderr, "live widths: ordinary=%u explicit=%u\n",
                ordinary_width, explicit_width);
        return fail("live default reflow changed the wrong width");
    }

    scaled_shell = XtVaCreatePopupShell("scaledShell",
                                        topLevelShellWidgetClass, shell,
                                        NULL);
    scaled = XmCreateScrolledWindow(scaled_shell, "scaled",
                                    automatic_args, 1);
    XtManageChild(scaled);
    XtRealizeWidget(scaled_shell);
    scaled_width = read_width(scaled);
    XtQueryGeometry(scaled, NULL, &preferred);
    if (!(preferred.request_mode & CWWidth) || preferred.width != 200) {
        fprintf(stderr, "scaled width=%u preferred=%u mode=%lu\n",
                scaled_width, preferred.width,
                (unsigned long)preferred.request_mode);
        return fail("construction did not use the active scale");
    }

    if (!XmSetDisplayScaleOverride(display, 1.0))
        return fail("scale reset failed");
    XSync(display, False);
    if (read_width(ordinary) != 100 || read_width(explicit) != 140)
        return fail("default reflow was not reversible");
    XtDestroyWidget(scaled_shell);
    XtDestroyWidget(explicit_shell);
    XtDestroyWidget(shell);
    puts("PASS WP-05.2 display-scale scrolled window");
    return 0;
}
