/* Installed-consumer proof for the deterministic display-scale override. */
#include <Xm/Display.h>
#include <Xm/Appearance.h>
#include <Xm/ArrowB.h>
#include <Xm/ArrowBG.h>
#include <Xm/BulletinB.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/Command.h>
#include <Xm/FileSB.h>
#include <Xm/Label.h>
#include <Xm/Notebook.h>
#include <Xm/Paned.h>
#include <Xm/PanedW.h>
#include <Xm/ProgressRing.h>
#include <Xm/RowColumn.h>
#include <Xm/Scale.h>
#include <Xm/ScrollBar.h>
#include <Xm/SpinB.h>
#include <Xm/TearOffBP.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <X11/Intrinsic.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <math.h>
#include <stdio.h>
#include <string.h>

static int
fail(const char *message)
{
    fprintf(stderr, "display-scale override: %s\n", message);
    return 1;
}

static Dimension
scaled_dimension(Dimension baseline, double scale)
{
    return (Dimension)(baseline * scale + 0.5);
}

static Dimension
dimension(Widget widget, const char *resource)
{
    Dimension result = 0;
    XtVaGetValues(widget, resource, &result, NULL);
    return result;
}

static Position
left(Widget widget)
{
    Position result = 0;
    XtVaGetValues(widget, XmNx, &result, NULL);
    return result;
}

static unsigned int
cascade_size(Display *display, Widget widget)
{
    Pixmap pixmap;
    Window root;
    int x, y;
    unsigned int width, height, border, depth;
    XtVaGetValues(widget, XmNcascadePixmap, &pixmap, NULL);
    if (pixmap == XmUNSPECIFIED_PIXMAP ||
        !XGetGeometry(display, pixmap, &root, &x, &y, &width, &height,
                      &border, &depth))
        return 0;
    return width == height ? width : 0;
}

static unsigned int
tearoff_dash_length(Widget tearoff)
{
    Display *display = XtDisplay(tearoff);
    XtAppContext context = XtWidgetToApplicationContext(tearoff);
    unsigned int width = dimension(tearoff, XmNwidth);
    unsigned int height = dimension(tearoff, XmNheight);
    unsigned int length = 0;
    XImage *image;
    unsigned long pixel;

    if (!XtIsRealized(tearoff) || width < 25 || height < 3)
        return 0;
    XClearArea(display, XtWindow(tearoff), 0, 0, 0, 0, True);
    XSync(display, False);
    while (XtAppPending(context) & XtIMXEvent)
        XtAppProcessEvent(context, XtIMXEvent);
    image = XGetImage(display, XtWindow(tearoff), 0, height / 2 - 1,
                      25, 1, AllPlanes, ZPixmap);
    if (image == NULL)
        return 0;
    pixel = XGetPixel(image, 0, 0);
    while (length < 25 && XGetPixel(image, length, 0) == pixel)
        ++length;
    XDestroyImage(image);
    return length;
}

static int
check_classic_controls(Widget board, XmAppearanceProfile profile)
{
    Display *display = XtDisplay(board);
    XrmDatabase database;
    const char *sash_names[] = {"*Sash", "*sash"};
    WidgetClass pane_classes[] = {xmPanedWindowWidgetClass, xmPanedWidgetClass};
    Widget panes[2], sashes[2], arrows[2], cascades[2];
    Dimension sash_extent = profile == XmAPPEARANCE_CLASSIC ? 10 : 12;
    Dimension arrow_width[2], arrow_height[2];
    unsigned int cascade_width[2];
    Pixmap application_pixmap;
    Widget popup, submenu[3], custom_cascade, notebook, command, files;
    Widget command_text, filter, explicit_gadget, tearoff, dash;
    Dimension tear_height;
    Dimension command_margin, file_margin;
    Position command_x, filter_x;
    char *input;
    int page;

    if (!XmApplyAppearanceProfile(display, profile) ||
        !XmSetDisplayScaleOverride(display, 1.0))
        return fail("could not select classic geometry test profile");

    /* Equal-valued Xrm resources must remain application-owned. */
    database = XrmGetDatabase(display);
    XrmPutStringResource(&database, "*fixedPaned.sashHeight", "10");
    XrmPutStringResource(&database, "*fixedGadget.width", "41");
    XrmPutStringResource(&database, "*dpiNotebook.minorTabSpacing", "3");
    XrmPutStringResource(&database, "*dpiFiles.marginHeight", "10");
    XrmSetDatabase(display, database);

    for (unsigned int i = 0; i < XtNumber(panes); ++i) {
        panes[i] = XtVaCreateManagedWidget(i ? "fixedPaned" : "dpiPaned",
            pane_classes[i], board, XmNwidth, 260, XmNheight, 180, NULL);
        for (unsigned int j = 0; j < 2; ++j)
            XtVaCreateManagedWidget(j ? "second" : "first", xmLabelWidgetClass,
                panes[i], XmNheight, 60, XmNpaneMinimum, 20, XmNpaneMaximum, 160,
                NULL);
        sashes[i] = XtNameToWidget(panes[i], sash_names[i]);
        if (sashes[i] == NULL || dimension(sashes[i], XmNwidth) != sash_extent ||
            dimension(sashes[i], XmNheight) != (i ? 10 : sash_extent)) {
            fprintf(stderr, "profile=%d pane=%u manager=%ux%u grip=%ux%u\n",
                    profile, i, dimension(panes[i], XmNsashWidth),
                    dimension(panes[i], XmNsashHeight),
                    sashes[i] ? dimension(sashes[i], XmNwidth) : 0,
                    sashes[i] ? dimension(sashes[i], XmNheight) : 0);
            return fail("1x PanedWindow/Paned sash allocation is incorrect");
        }
    }

    arrows[0] = XtVaCreateManagedWidget("dpiArrow", xmArrowButtonWidgetClass,
        board, XmNwidth, 0, XmNheight, 0, XmNhighlightThickness, 0,
        XmNshadowThickness, 0, NULL);
    arrows[1] = XtVaCreateManagedWidget("dpiGadget", xmArrowButtonGadgetClass,
        board, XmNhighlightThickness, 0, XmNshadowThickness, 0, NULL);
    explicit_gadget = XtVaCreateManagedWidget("fixedGadget",
        xmArrowButtonGadgetClass, board, XmNheight, 15,
        XmNhighlightThickness, 0, XmNshadowThickness, 0, NULL);
    for (unsigned int i = 0; i < XtNumber(arrows); ++i) {
        arrow_width[i] = dimension(arrows[i], XmNwidth);
        arrow_height[i] = dimension(arrows[i], XmNheight);
        if (arrow_width[i] != 15 || arrow_height[i] != 15)
            return fail("1x ArrowButton default interior is not 15 pixels");
    }

    notebook = XtVaCreateManagedWidget("dpiNotebook", xmNotebookWidgetClass,
        board, XmNwidth, 280, XmNheight, 180, XmNbindingType, XmSOLID, NULL);
    for (int i = 1; i <= 2; ++i) {
        XtVaCreateManagedWidget("page", xmLabelWidgetClass, notebook,
            XmNnotebookChildType, XmPAGE, XmNpageNumber, i, NULL);
        XtVaCreateManagedWidget("chapter", xmLabelWidgetClass, notebook,
            XmNnotebookChildType, XmMAJOR_TAB, XmNpageNumber, i, NULL);
    }
    XtVaSetValues(notebook, XmNcurrentPageNumber, 2, NULL);
    if (dimension(notebook, XmNbindingWidth) != 25 ||
        dimension(notebook, XmNmajorTabSpacing) != 3)
        return fail("notebook 1x binding or tab space is incorrect");

    command = XtVaCreateManagedWidget("dpiCommand", xmCommandWidgetClass, board,
        XmNwidth, 260, XmNheight, 180, NULL);
    files = XtVaCreateManagedWidget("dpiFiles", xmFileSelectionBoxWidgetClass,
        board, XmNwidth, 480, XmNheight, 360, NULL);
    command_text = XmCommandGetChild(command, XmDIALOG_COMMAND_TEXT);
    filter = XmFileSelectionBoxGetChild(files, XmDIALOG_FILTER_TEXT);
    command_x = left(command_text);
    filter_x = left(filter);
    command_margin = dimension(command, XmNmarginWidth);
    file_margin = dimension(files, XmNmarginWidth);
    XmTextFieldSetString(command_text, "retain command");
    XmTextFieldSetString(filter, "retain filter");

    popup = XmCreatePopupMenu(board, "dpiMenu", NULL, 0);
    XtVaSetValues(popup, XmNtearOffModel, XmTEAR_OFF_ENABLED, NULL);
    tearoff = XmGetTearOffControl(popup);
    if (tearoff == NULL)
        return fail("menu tear-off control could not be created");
    for (unsigned int i = 0; i < XtNumber(submenu); ++i)
        submenu[i] = XmCreatePulldownMenu(popup, "submenu", NULL, 0);
    cascades[0] = XtVaCreateManagedWidget("cascade", xmCascadeButtonWidgetClass,
        popup, XmNsubMenuId, submenu[0], NULL);
    cascades[1] = XtVaCreateManagedWidget("gadget", xmCascadeButtonGadgetClass,
        popup, XmNsubMenuId, submenu[1], NULL);
    application_pixmap = XCreatePixmap(display, DefaultRootWindow(display), 9, 9,
                                       DefaultDepthOfScreen(XtScreen(board)));
    custom_cascade = XtVaCreateManagedWidget("explicitCascade", xmCascadeButtonWidgetClass,
        popup, XmNsubMenuId, submenu[2], XmNcascadePixmap, application_pixmap, NULL);
    dash = XtVaCreateManagedWidget("dash", xmTearOffButtonWidgetClass, board,
        XmNx, 800, XmNy, 20, XmNwidth, 128, XmNheight, 32,
        XmNhighlightThickness, 0, XmNshadowThickness, 2,
        XmNseparatorType, XmSHADOW_ETCHED_OUT_DASH, NULL);
    if (tearoff_dash_length(dash) != 6)
        return fail("tear-off did not render a six-pixel dash at 1x");
    tear_height = dimension(tearoff, XmNheight);
    for (unsigned int i = 0; i < XtNumber(cascades); ++i) {
        cascade_width[i] = cascade_size(display, cascades[i]);
        if (cascade_width[i] == 0)
            return fail("missing toolkit cascade pixmap at 1x");
    }

    /* First expose finishes the Notebook's initial tab-scroller management. */
    XtVaSetValues(notebook, XmNwidth, 280, XmNheight, 180, NULL);
    if (dimension(notebook, XmNwidth) != 280 || dimension(notebook, XmNheight) != 180)
        return fail("notebook fixture could not establish explicit bounds");
    /* Assign the parent allocation after file-list realization and search. */
    XtResizeWidget(files, 480, 360, dimension(files, XmNborderWidth));
    if (dimension(files, XmNwidth) != 480 || dimension(files, XmNheight) != 360)
        return fail("file selection fixture could not establish explicit bounds");
    if (!XmSetDisplayScaleOverride(display, 2.0))
        return fail("2x classic geometry transaction failed");
    XSync(display, False);
    for (unsigned int i = 0; i < XtNumber(panes); ++i) {
        if (dimension(panes[i], XmNsashWidth) != 2 * sash_extent ||
            dimension(sashes[i], XmNwidth) != 2 * sash_extent ||
            dimension(sashes[i], XmNheight) != (i ? 10 : 2 * sash_extent))
            return fail("live sash resize ignored the manager or Xrm ownership");
        if (dimension(arrows[i], XmNwidth) != 30 ||
            dimension(arrows[i], XmNheight) != 30)
            return fail("live ArrowButton/Gadget dimensions did not reach 2x");
        if (cascade_size(display, cascades[i]) <= cascade_width[i])
            return fail("live menu cascade pixmap did not grow");
    }
    if (dimension(explicit_gadget, XmNwidth) != 41 ||
        dimension(explicit_gadget, XmNheight) != 15 ||
        cascade_size(display, custom_cascade) != 9)
        return fail("live scaling changed an explicit arrow dimension or pixmap");
    if (tearoff_dash_length(dash) != 12)
        return fail("tear-off did not render a twelve-pixel dash at 2x");

    if (dimension(notebook, XmNbindingWidth) != 50 ||
        dimension(notebook, XmNbackPageSize) != 16 ||
        dimension(notebook, XmNmajorTabSpacing) != 6 ||
        dimension(notebook, XmNminorTabSpacing) != 3 ||
        dimension(notebook, XmNwidth) != 280 || dimension(notebook, XmNheight) != 180) {
        fprintf(stderr, "notebook binding=%u back=%u tabs=%u/%u size=%ux%u\n",
                dimension(notebook, XmNbindingWidth), dimension(notebook, XmNbackPageSize),
                dimension(notebook, XmNmajorTabSpacing), dimension(notebook, XmNminorTabSpacing),
                dimension(notebook, XmNwidth), dimension(notebook, XmNheight));
        return fail("notebook live metrics or explicit allocation are incorrect");
    }
    XtVaGetValues(notebook, XmNcurrentPageNumber, &page, NULL);
    if (page != 2)
        return fail("notebook lost its current page during scaling");
    if (dimension(command, XmNmarginWidth) != 2 * command_margin ||
        dimension(files, XmNmarginWidth) != 2 * file_margin ||
        dimension(files, XmNmarginHeight) != 10 ||
        dimension(command, XmNwidth) != 260 || dimension(files, XmNwidth) != 480 ||
        left(command_text) <= command_x || left(filter) <= filter_x) {
        fprintf(stderr, "profile=%d command margin=%u (was %u) width=%u x=%d (was %d); "
                "files margins=%u/%u (width was %u) width=%u x=%d (was %d)\n",
                profile, dimension(command, XmNmarginWidth), command_margin,
                dimension(command, XmNwidth), left(command_text), command_x,
                dimension(files, XmNmarginWidth), dimension(files, XmNmarginHeight),
                file_margin, dimension(files, XmNwidth), left(filter), filter_x);
        return fail("selection controls did not reflow within their explicit bounds");
    }
    input = XmTextFieldGetString(command_text);
    if (strcmp(input, "retain command") != 0)
        return fail("command edit lost text during scaling");
    XtFree(input);
    input = XmTextFieldGetString(filter);
    if (strcmp(input, "retain filter") != 0)
        return fail("file filter lost text during scaling");
    XtFree(input);

    Widget new_arrow = XtVaCreateManagedWidget("newArrow", xmArrowButtonWidgetClass,
        board, XmNhighlightThickness, 0, XmNshadowThickness, 0, NULL);
    Widget new_paned = XtVaCreateManagedWidget("newPaned", xmPanedWindowWidgetClass,
        board, NULL);
    if (dimension(new_arrow, XmNwidth) != 30 ||
        dimension(new_paned, XmNsashWidth) != 2 * sash_extent)
        return fail("2x constructors did not use the current DPI");
    XtVaSetValues(explicit_gadget, XmNheight, 0, NULL);
    if (dimension(explicit_gadget, XmNheight) != 30 ||
        dimension(explicit_gadget, XmNwidth) != 41)
        return fail("zero ArrowButtonGadget height did not restore its DPI default");
    if (!XmSetDisplayScaleOverride(display, 1.0))
        return fail("classic geometry reset failed");
    XSync(display, False);
    for (unsigned int i = 0; i < XtNumber(panes); ++i) {
        if (dimension(sashes[i], XmNwidth) != sash_extent ||
            dimension(arrows[i], XmNwidth) != arrow_width[i] ||
            dimension(arrows[i], XmNheight) != arrow_height[i] ||
            cascade_size(display, cascades[i]) != cascade_width[i])
            return fail("reset did not restore the 1x sash or arrow size");
    }
    if (dimension(notebook, XmNbindingWidth) != 25 ||
        left(command_text) != command_x || left(filter) != filter_x ||
        dimension(tearoff, XmNheight) != tear_height ||
        tearoff_dash_length(dash) != 6)
        return fail("reset did not restore notebook, selection or tear-off layout");

    XtDestroyWidget(popup);
    XtDestroyWidget(dash);
    XFreePixmap(display, application_pixmap);
    XtDestroyWidget(new_paned);
    XtDestroyWidget(new_arrow);
    XtDestroyWidget(files);
    XtDestroyWidget(command);
    XtDestroyWidget(notebook);
    XtDestroyWidget(explicit_gadget);
    for (unsigned int i = 0; i < XtNumber(panes); ++i) {
        XtDestroyWidget(arrows[i]);
        XtDestroyWidget(panes[i]);
    }
    return 0;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, board;
    Widget baseline_toggle;
    Widget scaled_toggle;
    Widget explicit_toggle;
    Widget baseline_spin;
    Widget explicit_spin;
    Widget baseline_ring;
    Widget explicit_ring;
    Widget baseline_scroll;
    Widget explicit_scroll;
    Widget baseline_scale;
    Display *display;
    XrmDatabase database;
    double scale;
    Dimension baseline_indicator;
    Dimension live_indicator;
    Dimension scaled_indicator;
    Dimension explicit_indicator;
    Dimension baseline_arrow;
    Dimension live_arrow;
    Dimension explicit_arrow;
    Dimension baseline_ring_height;
    Dimension live_ring_height;
    Dimension explicit_ring_height;
    Dimension scroll_ht;
    Dimension scroll_st;
    Dimension scroll_width;
    Dimension explicit_scroll_width;
    Dimension scale_ht;
    Dimension scale_width;
    const double gallery_scales[] = {1.0, 1.5, 2.0, 3.0};
    XmString empty_label;
    Arg empty_args[1];
    Arg explicit_args[2];

    shell = XtVaAppInitialize(&app, "DisplayScaleOverride", NULL, 0,
                              &argc, argv, NULL, XmNwidth, 1200, XmNheight, 1000, NULL);
    board = XtVaCreateManagedWidget("board", xmBulletinBoardWidgetClass, shell,
        XmNresizePolicy, XmRESIZE_NONE, NULL);
    display = XtDisplay(shell);
    database = XrmGetDatabase(display);

    if (XmGetWidgetScale(NULL, &scale) || XmGetWidgetScale(shell, NULL))
        return fail("effective scale accepted invalid arguments");
    if (XmGetDisplayScaleOverride(display, &scale))
        return fail("override unexpectedly existed before selection");
    if (XmSetDisplayScaleOverride(display, 0.0) ||
        XmSetDisplayScaleOverride(display, -1.0) ||
        XmSetDisplayScaleOverride(display, NAN) ||
        XmSetDisplayScaleOverride(display, INFINITY))
        return fail("invalid override was accepted");
    if (!XmSetDisplayScaleOverride(display, 1.0) ||
        !XmGetDisplayScaleOverride(display, &scale) || scale != 1.0)
        return fail("one-times override was not retained");
    if (XrmGetDatabase(display) != database)
        return fail("override mutated the X resource database");

    empty_label = XmStringCreateLocalized("");
    XtSetArg(empty_args[0], XmNlabelString, empty_label);
    baseline_toggle = XmCreateToggleButton(board, "baseline", empty_args, 1);
    XmStringFree(empty_label);
    XtManageChild(baseline_toggle);

    XtRealizeWidget(shell);
    XtVaGetValues(baseline_toggle, XmNindicatorSize, &baseline_indicator,
                  NULL);
    XmToggleButtonSetState(baseline_toggle, True, False);
    if (!XmToggleButtonGetState(baseline_toggle))
        return fail("toggle state could not be established before reflow");

    if (!XmSetDisplayScaleOverride(display, 2.0) ||
        !XmGetDisplayScaleOverride(display, &scale) || scale != 2.0)
        return fail("live override was not retained");
    if (!XmGetWidgetScale(shell, &scale) || scale != 2.0 ||
        !XmGetWidgetScale(baseline_toggle, &scale) || scale != 2.0)
        return fail("effective widget scale did not follow override");
    XSync(display, False);
    XtVaGetValues(baseline_toggle, XmNindicatorSize, &live_indicator, NULL);
    if (live_indicator <= baseline_indicator)
        return fail("realized default metric did not reflow");
    if (!XmToggleButtonGetState(baseline_toggle))
        return fail("toggle state changed during reflow");

    empty_label = XmStringCreateLocalized("");
    XtSetArg(empty_args[0], XmNlabelString, empty_label);
    scaled_toggle = XmCreateToggleButton(board, "scaled", empty_args, 1);
    XmStringFree(empty_label);
    XtManageChild(scaled_toggle);
    empty_label = XmStringCreateLocalized("");
    XtSetArg(explicit_args[0], XmNlabelString, empty_label);
    XtSetArg(explicit_args[1], XmNindicatorSize, (Dimension)7);
    explicit_toggle = XmCreateToggleButton(board, "explicit", explicit_args, 2);
    XmStringFree(empty_label);
    XtManageChild(explicit_toggle);

    baseline_spin = XmCreateSpinBox(board, "baselineSpin", NULL, 0);
    XtManageChild(baseline_spin);
    baseline_ring = XmCreateProgressRing(board, "baselineRing", NULL, 0);
    XtManageChild(baseline_ring);
    XtRealizeWidget(shell);

    XtVaGetValues(scaled_toggle, XmNindicatorSize, &scaled_indicator, NULL);
    XtVaGetValues(explicit_toggle, XmNindicatorSize, &explicit_indicator, NULL);
    XtVaGetValues(baseline_spin, XmNarrowSize, &baseline_arrow, NULL);
    XtVaGetValues(baseline_ring, XtNheight, &baseline_ring_height, NULL);
    if (baseline_indicator == 0 || live_indicator <= baseline_indicator ||
        scaled_indicator <= baseline_indicator)
        return fail("scaled default metric did not increase");
    if (explicit_indicator != 7)
        return fail("explicit pixel metric was changed by scale override");
    if (baseline_arrow == 0)
        return fail("spin box default metric was not initialized");
    if (baseline_ring_height == 0)
        return fail("progress ring default metric was not initialized");

    if (!XmSetDisplayScaleOverride(display, 1.0))
        return fail("one-times live override was not retained for spin box");
    XSync(display, False);
    XtVaGetValues(baseline_spin, XmNarrowSize, &live_arrow, NULL);
    if (live_arrow >= baseline_arrow)
        return fail("spin box default metric did not reflow down");
    XtVaGetValues(baseline_ring, XtNheight, &live_ring_height, NULL);
    if (live_ring_height >= baseline_ring_height)
        return fail("progress ring default metric did not reflow down");

    explicit_ring = XmCreateProgressRing(board, "explicitRing", NULL, 0);
    XtVaSetValues(explicit_ring, XtNheight, (Dimension)100, NULL);
    XtManageChild(explicit_ring);
    XtVaGetValues(explicit_ring, XtNheight, &explicit_ring_height, NULL);
    if (explicit_ring_height != 100)
        return fail("progress ring explicit height was not retained");

    explicit_spin = XmCreateSpinBox(board, "explicitSpin", NULL, 0);
    XtVaSetValues(explicit_spin, XmNarrowSize, (Dimension)16, NULL);
    XtManageChild(explicit_spin);
    XtVaGetValues(explicit_spin, XmNarrowSize, &explicit_arrow, NULL);
    if (explicit_arrow != 16)
        return fail("spin box explicit pixel metric was not retained");

    baseline_scroll = XmCreateScrollBar(board, "baselineScroll", NULL, 0);
    XtManageChild(baseline_scroll);
    XtVaGetValues(baseline_scroll,
                  XmNhighlightThickness, &scroll_ht,
                  XmNshadowThickness, &scroll_st,
                  XmNwidth, &scroll_width, NULL);
    if (scroll_width != (Dimension)(2 * (scroll_ht + scroll_st) + 11))
        return fail("scrollbar initial classic thickness was unexpected");

    explicit_scroll = XmCreateScrollBar(board, "explicitScroll", NULL, 0);
    XtVaSetValues(explicit_scroll, XmNwidth, (Dimension)33, NULL);
    XtManageChild(explicit_scroll);
    XtVaGetValues(explicit_scroll, XmNwidth, &explicit_scroll_width, NULL);
    if (explicit_scroll_width != 33)
        return fail("scrollbar explicit width was not retained");

    baseline_scale = XmCreateScale(board, "baselineScale", NULL, 0);
    XtManageChild(baseline_scale);
    XtVaGetValues(baseline_scale,
                  XmNhighlightThickness, &scale_ht,
                  XmNwidth, &scale_width, NULL);
    if (scale_width < (Dimension)(2 * scale_ht + 15))
        return fail("scale initial classic minor extent was unexpected");

    if (!XmSetDisplayScaleOverride(display, 2.0))
        return fail("two-times live override was not retained for spin box");
    XSync(display, False);
    XtVaGetValues(explicit_ring, XtNheight, &explicit_ring_height, NULL);
    if (explicit_ring_height != 100)
        return fail("progress ring explicit height changed by scale override");
    XtVaGetValues(explicit_spin, XmNarrowSize, &explicit_arrow, NULL);
    if (explicit_arrow != 16)
        return fail("spin box explicit pixel metric changed by scale override");
    XtVaGetValues(explicit_scroll, XmNwidth, &explicit_scroll_width, NULL);
    if (explicit_scroll_width != 33)
        return fail("scrollbar explicit width changed by scale override");

    for (size_t i = 0; i < XtNumber(gallery_scales); ++i) {
        Dimension indicator, arrow, ring_height, sb_w, sc_w;
        Dimension expected_indicator = scaled_dimension(9, gallery_scales[i]);
        Dimension expected_arrow = scaled_dimension(16, gallery_scales[i]);
        Dimension expected_ring = scaled_dimension(64, gallery_scales[i]) + 16;
        Dimension expected_sb = (Dimension)(2 * (scroll_ht + scroll_st) +
                                            scaled_dimension(11, gallery_scales[i]));
        Dimension expected_sc_min = (Dimension)(2 * scale_ht +
                                                scaled_dimension(15, gallery_scales[i]));
        if (!XmSetDisplayScaleOverride(display, gallery_scales[i]))
            return fail("gallery DPI override was not retained");
        XSync(display, False);
        XtVaGetValues(baseline_toggle, XmNindicatorSize, &indicator, NULL);
        XtVaGetValues(baseline_spin, XmNarrowSize, &arrow, NULL);
        XtVaGetValues(baseline_ring, XtNheight, &ring_height, NULL);
        XtVaGetValues(baseline_scroll, XmNwidth, &sb_w, NULL);
        XtVaGetValues(baseline_scale, XmNwidth, &sc_w, NULL);
        if (indicator != expected_indicator || arrow != expected_arrow ||
            ring_height != expected_ring || sb_w != expected_sb ||
            sc_w < expected_sc_min)
            return fail("gallery DPI matrix produced an unexpected default");
    }

    XtVaSetValues(explicit_scroll, XmNwidth, (Dimension)0, NULL);
    XtVaGetValues(explicit_scroll, XmNwidth, &explicit_scroll_width, NULL);
    if (explicit_scroll_width != (Dimension)(2 * (scroll_ht + scroll_st) +
                                             scaled_dimension(11, 3.0)))
        return fail("scrollbar zero-width reset did not restore scaled default");

    if (!XmSetDisplayScaleOverride(display, 1.0))
        return fail("one-times live override was not retained");
    XSync(display, False);
    XtVaGetValues(baseline_toggle, XmNindicatorSize, &live_indicator, NULL);
    if (live_indicator != baseline_indicator)
        return fail("live scale reflow was not reversible");
    if (!XmClearDisplayScaleOverride(display) ||
        XmGetDisplayScaleOverride(display, &scale))
        return fail("override did not clear");
    XrmPutStringResource(&database, "Xft.dpi", "144");
    XrmSetDatabase(display, database);
    if (!XmGetWidgetScale(shell, &scale) || scale != 1.5)
        return fail("effective scale did not resolve Xft.dpi");
    if (!XmSetDisplayScaleOverride(display, 0.5) ||
        !XmGetWidgetScale(shell, &scale) || scale != 1.0)
        return fail("effective scale did not match minimum default scale");
    if (check_classic_controls(board, XmAPPEARANCE_CLASSIC) ||
        check_classic_controls(board, XmAPPEARANCE_HIGH_CONTRAST))
        return 1;
    XtDestroyWidget(shell);
    puts("PASS WP-05.2 display-scale override");
    return 0;
}
