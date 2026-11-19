/* Automated regression for elevated flat-button label backgrounds, etched-in
 * menu bar CascadeButton / CascadeButtonGadget background GC synchronization,
 * live root XA_RESOURCE_MANAGER synchronization, and Motif 1.2 XmGadget
 * visual_change fallback notification. */
#include <X11/IntrinsicP.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <Xm/Appearance.h>
#include <Xm/CascadeBG.h>
#include <Xm/Form.h>
#include <Xm/GadgetP.h>
#include <Xm/LabelGP.h>
#include <Xm/LabelP.h>
#include <Xm/ManagerP.h>
#include <Xm/PushBGP.h>
#include <Xm/PushBP.h>
#include <Xm/RowColumn.h>
#include <Xm/XmAll.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(c) \
    do { \
        if (!(c)) { \
            fprintf(stderr, "FAIL %d: %s\n", __LINE__, #c); \
            exit(1); \
        } \
    } while (0)

typedef struct {
    ObjectPart object;
    RectObjPart rectangle;
    XmGadgetPart gadget;
} LegacyVisualGadgetRec, *LegacyVisualGadget;

typedef struct {
    RectObjClassPart rect_class;
    XmGadgetClassPart gadget_class;
} LegacyVisualGadgetClassRec;

static unsigned int g_visual_change_calls = 0;

static Boolean
legacy_visual_change(Widget child, Widget cur_parent, Widget new_parent)
{
    (void)child;
    if (cur_parent != NULL && new_parent != NULL &&
        (cur_parent->core.background_pixel != new_parent->core.background_pixel ||
         ((XmManagerWidget)cur_parent)->manager.foreground !=
             ((XmManagerWidget)new_parent)->manager.foreground)) {
        ++g_visual_change_calls;
        return True;
    }
    return False;
}

static LegacyVisualGadgetClassRec legacyVisualGadgetClassRec = {
    {
        /* superclass         */ (WidgetClass)&xmGadgetClassRec,
        /* class_name         */ "LegacyVisualGadget",
        /* widget_size        */ sizeof(LegacyVisualGadgetRec),
        /* class_initialize   */ NULL,
        /* class_part_init    */ NULL,
        /* class_inited       */ FALSE,
        /* initialize         */ NULL,
        /* initialize_hook    */ NULL,
        /* realize            */ NULL,
        /* actions            */ NULL,
        /* num_actions        */ 0,
        /* resources          */ NULL,
        /* num_resources      */ 0,
        /* xrm_class          */ NULLQUARK,
        /* compress_motion    */ FALSE,
        /* compress_exposure  */ FALSE,
        /* compress_enterleave*/ FALSE,
        /* visible_interest   */ FALSE,
        /* destroy            */ NULL,
        /* resize             */ NULL,
        /* expose             */ NULL,
        /* set_values         */ NULL,
        /* set_values_hook    */ NULL,
        /* set_values_almost  */ XtInheritSetValuesAlmost,
        /* get_values_hook    */ NULL,
        /* accept_focus       */ NULL,
        /* version            */ XtVersion,
        /* callback_private   */ NULL,
        /* tm_table           */ NULL,
        /* query_geometry     */ NULL,
        /* display_accelerator*/ NULL,
        /* extension          */ NULL,
    },
    {
        /* border_highlight   */ XmInheritBorderHighlight,
        /* border_unhighlight */ XmInheritBorderUnhighlight,
        /* arm_and_activate   */ NULL,
        /* input_dispatch     */ XmInheritInputDispatch,
        /* visual_change      */ legacy_visual_change,
        /* syn_resources      */ NULL,
        /* num_syn_resources  */ 0,
        /* cache_part         */ NULL,
        /* extension          */ NULL,
    },
};

static WidgetClass legacyVisualGadgetClass =
    (WidgetClass)&legacyVisualGadgetClassRec;

static void
drain(XtAppContext app, Display *dpy)
{
    XSync(dpy, False);
    while (XtAppPending(app))
        XtAppProcessEvent(app, XtIMAll);
    XSync(dpy, False);
}

static XImage *
capture(Widget w)
{
    XSync(XtDisplayOfObject(w), False);
    return XGetImage(XtDisplayOfObject(w), XtWindowOfObject(w),
                     XtIsWidget(w) ? 0 : XtX(w),
                     XtIsWidget(w) ? 0 : XtY(w),
                     XtWidth(w), XtHeight(w), AllPlanes, ZPixmap);
}

static void
assert_no_unelevated_textrect(Widget btn, Pixel parent_bg, const char *tag)
{
    Dimension hl = 0, sh = 0;
    XImage *im = capture(btn);
    Pixel face_px;
    int tx, ty, tw, th;

    CHECK(im != NULL);
    XtVaGetValues(btn, XmNhighlightThickness, &hl, XmNshadowThickness, &sh, NULL);
    face_px = XGetPixel(im, (int)hl + (int)sh + 4, (int)XtHeight(btn) / 2);
    CHECK(face_px != parent_bg);

    if (XtIsWidget(btn)) {
        XmLabelWidget lw = (XmLabelWidget)btn;
        tx = lw->label.TextRect.x;
        ty = lw->label.TextRect.y;
        tw = lw->label.TextRect.width;
        th = lw->label.TextRect.height;
    } else {
        XmLabelGadget lg = (XmLabelGadget)btn;
        tx = lg->label.TextRect.x;
        ty = lg->label.TextRect.y;
        tw = lg->label.TextRect.width;
        th = lg->label.TextRect.height;
    }
    CHECK(tw > 8 && th > 6);
    for (int y = ty; y < ty + th && y < im->height; ++y) {
        for (int x = tx; x < tx + tw && x < im->width; ++x) {
            if (x >= 0 && y >= 0 && XGetPixel(im, x, y) == parent_bg) {
                fprintf(stderr,
                        "FAIL %s: TextRect pixel (%d,%d) has un-elevated parent_bg=0x%lx (face=0x%lx)\n",
                        tag, x, y, (unsigned long)parent_bg, (unsigned long)face_px);
                XDestroyImage(im);
                exit(1);
            }
        }
    }
    XDestroyImage(im);
}

static void
assert_menubar_cascade_uniform_bg(Widget mb, Widget cb, Pixel expected_bg,
                                  const char *tag)
{
    XImage *mb_im = capture(mb);
    XImage *cb_im = capture(cb);
    Pixel mb_px, outer_px, inner_px;
    int tx, ty;

    CHECK(mb_im != NULL);
    CHECK(cb_im != NULL);
    mb_px = XGetPixel(mb_im, (int)XtWidth(mb) / 2, (int)XtHeight(mb) / 2);
    CHECK(mb_px == expected_bg);

    outer_px = XGetPixel(cb_im, 2, (int)XtHeight(cb) / 2);
    if (XtIsWidget(cb)) {
        XmLabelWidget lw = (XmLabelWidget)cb;
        tx = lw->label.TextRect.x;
        ty = lw->label.TextRect.y;
    } else {
        XmLabelGadget lg = (XmLabelGadget)cb;
        tx = lg->label.TextRect.x;
        ty = lg->label.TextRect.y;
    }
    inner_px = XGetPixel(cb_im, tx, ty);
    if (outer_px != expected_bg || inner_px != expected_bg) {
        fprintf(stderr,
                "FAIL %s: expected_bg=0x%lx outer_px=0x%lx inner_px=0x%lx\n",
                tag, (unsigned long)expected_bg, (unsigned long)outer_px,
                (unsigned long)inner_px);
        XDestroyImage(mb_im);
        XDestroyImage(cb_im);
        exit(1);
    }
    XDestroyImage(mb_im);
    XDestroyImage(cb_im);
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell = XtVaAppInitialize(&app, "CheckElevatedBtnLabelBg", NULL, 0,
                                     &argc, argv, NULL,
                                     XmNwidth, 420, XmNheight, 320, NULL);
    Display *dpy = XtDisplay(shell);
    Screen *scr = XtScreen(shell);
    Colormap cmap = DefaultColormapOfScreen(scr);
    XrmDatabase db = XrmGetDatabase(dpy);
    if (db == NULL) {
        XrmInitialize();
        db = XrmGetStringDatabase("");
    }
    XrmPutStringResource(&db, "*enableEtchedInMenu", "True");
    XrmPutStringResource(&db, "*ckCoreMotif.elevateFlatButtons", "True");
    XrmPutStringResource(&db, "*appearanceProfile", "modern_dark");
    XrmSetDatabase(dpy, db);
    XtVaSetValues(XmGetXmDisplay(dpy), XmNenableEtchedInMenu, True, NULL);

    Widget form = XtVaCreateManagedWidget("form", xmFormWidgetClass, shell, NULL);
    Arg mb_args[4];
    Cardinal mbn = 0;
    XtSetArg(mb_args[mbn], XmNtopAttachment, XmATTACH_FORM); ++mbn;
    XtSetArg(mb_args[mbn], XmNleftAttachment, XmATTACH_FORM); ++mbn;
    XtSetArg(mb_args[mbn], XmNrightAttachment, XmATTACH_FORM); ++mbn;
    Widget mb = XmCreateMenuBar(form, "menuBar", mb_args, mbn);
    XtManageChild(mb);
    Widget file_pd = XmCreatePulldownMenu(mb, "fileMenu", NULL, 0);
    XtVaCreateManagedWidget("wrapToggle", xmToggleButtonGadgetClass, file_pd, NULL);
    XmString xs_file = XmStringCreateLocalized("File");
    Widget cb_gad = XmCreateCascadeButtonGadget(mb, "File", NULL, 0);
    XtVaSetValues(cb_gad, XmNlabelString, xs_file, XmNsubMenuId, file_pd, NULL);
    XtManageChild(cb_gad);
    XmStringFree(xs_file);
    XmString xs_help = XmStringCreateLocalized("Help");
    Widget cb_wid = XmCreateCascadeButton(mb, "Help", NULL, 0);
    XtVaSetValues(cb_wid, XmNlabelString, xs_help, NULL);
    XtManageChild(cb_wid);
    XmStringFree(xs_help);
    XtVaSetValues(mb, XmNmenuHelpWidget, cb_wid, NULL);

    XmString xs_cancel = XmStringCreateLocalized("Cancel");
    Widget pb_w = XtVaCreateManagedWidget(
        "pbWidget", xmPushButtonWidgetClass, form,
        XmNlabelString, xs_cancel,
        XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, mb, XmNtopOffset, 12,
        XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 16,
        NULL);
    XmString xs_mod = XmStringCreateLocalized("Modify...");
    Widget pb_g = XtVaCreateManagedWidget(
        "pbGadget", xmPushButtonGadgetClass, form,
        XmNlabelString, xs_mod,
        XmNtopAttachment, XmATTACH_WIDGET, XmNtopWidget, pb_w, XmNtopOffset, 16,
        XmNleftAttachment, XmATTACH_FORM, XmNleftOffset, 16,
        NULL);
    XmStringFree(xs_cancel);
    XmStringFree(xs_mod);

    Widget pd = XmCreatePulldownMenu(form, "optPD", NULL, 0);
    XmString xs_item = XmStringCreateLocalized("Comfortable (Standard)");
    XtVaCreateManagedWidget("optItem", xmPushButtonGadgetClass, pd,
                            XmNlabelString, xs_item, NULL);
    XmStringFree(xs_item);
    Arg oargs[4];
    Cardinal on = 0;
    XtSetArg(oargs[on], XmNsubMenuId, pd); ++on;
    XtSetArg(oargs[on], XmNtopAttachment, XmATTACH_WIDGET); ++on;
    XtSetArg(oargs[on], XmNtopWidget, pb_g); ++on;
    XtSetArg(oargs[on], XmNleftAttachment, XmATTACH_FORM); ++on;
    Widget opt = XmCreateOptionMenu(form, "optMenu", oargs, on);
    XtManageChild(opt);
    Widget opt_btn = XmOptionButtonGadget(opt);

    Widget legacy_g = XtVaCreateManagedWidget(
        "legacyGadget", legacyVisualGadgetClass, form,
        XmNx, 280, XmNy, 70, XmNwidth, 40, XmNheight, 24, NULL);
    CHECK(legacy_g != NULL);

    XtRealizeWidget(shell);
    XMapRaised(dpy, XtWindow(shell));
    drain(app, dpy);

    /* Force flat equal-background state on buttons and parent to exercise
     * elevated_button_surface_pixel + Label/LabelG TextRect background match. */
    Pixel parent_bg = 0;
    XtVaGetValues(form, XmNbackground, &parent_bg, NULL);
    XtVaSetValues(pb_w, XmNbackground, parent_bg, NULL);
    XtVaSetValues(pb_g, XmNbackground, parent_bg, NULL);
    XtVaSetValues(opt_btn, XmNbackground, parent_bg, NULL);
    XClearArea(dpy, XtWindow(pb_w), 0, 0, 0, 0, True);
    XClearArea(dpy, XtWindow(form), 0, 0, 0, 0, True);
    XClearArea(dpy, XtWindow(opt), 0, 0, 0, 0, True);
    drain(app, dpy);

    assert_no_unelevated_textrect(pb_w, parent_bg, "PushButton-modern-dark");
    assert_no_unelevated_textrect(pb_g, parent_bg, "PushButtonGadget-modern-dark");
    assert_no_unelevated_textrect(opt_btn, parent_bg, "OptionButtonGadget-modern-dark");

    /* Verify Motif 1.2 XmGadgetClassPart.visual_change fallback on parent color change */
    XColor navy;
    CHECK(XParseColor(dpy, cmap, "#1a2b3c", &navy) && XAllocColor(dpy, cmap, &navy));
    unsigned int before_vc = g_visual_change_calls;
    XtVaSetValues(form, XmNbackground, navy.pixel,
                  XmNforeground, WhitePixelOfScreen(scr), NULL);
    drain(app, dpy);
    CHECK(g_visual_change_calls > before_vc);

    /* Verify live root XA_RESOURCE_MANAGER PropertyNotify updates profile */
    const char *rm_classic =
        "*ckCoreMotif.elevateFlatButtons: True\n"
        "*appearanceProfile: classic\n"
        "*appearanceDensity: compact\n";
    XChangeProperty(dpy, DefaultRootWindow(dpy), XA_RESOURCE_MANAGER, XA_STRING,
                    8, PropModeReplace, (const unsigned char *)rm_classic,
                    (int)strlen(rm_classic));
    drain(app, dpy);
    /* Pump an extra event so any queued WorkProc completes deterministically */
    XChangeProperty(dpy, XtWindow(shell), XA_WM_NAME, XA_STRING, 8,
                    PropModeReplace, (const unsigned char *)"sync", 4);
    drain(app, dpy);

    XmAppearanceProfile cur_prof = XmAPPEARANCE_MODERN_DARK;
    XmAppearanceDensity cur_dens = XmAPPEARANCE_DENSITY_COMFORTABLE;
    CHECK(XmGetAppearanceProfile(dpy, &cur_prof));
    CHECK(XmGetAppearanceDensity(dpy, &cur_dens));
    CHECK(cur_prof == XmAPPEARANCE_CLASSIC);
    CHECK(cur_dens == XmAPPEARANCE_DENSITY_COMPACT);

    /* Verify MenuBar CascadeButtonGadget & CascadeButton have zero stale outer
     * background rectangle after Modern -> Classic transition and palette
     * background updates (with enableEtchedInMenu = True). */
    Pixel mb_bg = 0;
    XtVaGetValues(mb, XmNbackground, &mb_bg, NULL);
    assert_menubar_cascade_uniform_bg(mb, cb_gad, mb_bg, "CascadeBG-after-modern-to-classic");
    assert_menubar_cascade_uniform_bg(mb, cb_wid, mb_bg, "CascadeB-after-modern-to-classic");

    XColor teal, crimson;
    CHECK(XParseColor(dpy, cmap, "#4992a7", &teal) && XAllocColor(dpy, cmap, &teal));
    CHECK(XParseColor(dpy, cmap, "#b34d5d", &crimson) && XAllocColor(dpy, cmap, &crimson));

    /* Parent-only visual propagation onto CascadeButtonGadget + direct widget SetValues */
    XtVaSetValues(mb, XmNbackground, teal.pixel,
                  XmNforeground, WhitePixelOfScreen(scr), NULL);
    XtVaSetValues(cb_wid, XmNbackground, teal.pixel,
                  XmNforeground, WhitePixelOfScreen(scr), NULL);
    XClearArea(dpy, XtWindow(mb), 0, 0, 0, 0, True);
    XClearArea(dpy, XtWindow(cb_wid), 0, 0, 0, 0, True);
    drain(app, dpy);
    assert_menubar_cascade_uniform_bg(mb, cb_gad, teal.pixel, "CascadeBG-teal-visual-change");
    assert_menubar_cascade_uniform_bg(mb, cb_wid, teal.pixel, "CascadeB-teal-setvalues");

    /* Direct SetValues on CascadeButtonGadget + MenuBar */
    XtVaSetValues(mb, XmNbackground, crimson.pixel, NULL);
    XtVaSetValues(cb_gad, XmNbackground, crimson.pixel, NULL);
    XtVaSetValues(cb_wid, XmNbackground, crimson.pixel, NULL);
    XClearArea(dpy, XtWindow(mb), 0, 0, 0, 0, True);
    XClearArea(dpy, XtWindow(cb_wid), 0, 0, 0, 0, True);
    drain(app, dpy);
    assert_menubar_cascade_uniform_bg(mb, cb_gad, crimson.pixel, "CascadeBG-crimson-setvalues");
    assert_menubar_cascade_uniform_bg(mb, cb_wid, crimson.pixel, "CascadeB-crimson-setvalues");

    XtDestroyWidget(shell);
    XtCloseDisplay(dpy);
    XtDestroyApplicationContext(app);
    printf("PASS elevated button label background and live visual sync\n");
    return 0;
}
