/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Qualification for historical-widget live DPI/scale reflow and sub-paragraph
 * incremental HarfBuzz/Xft shaping in XmText. */
#define _POSIX_C_SOURCE 200809L
#include <Xm/Appearance.h>
#include <Xm/ArrowB.h>
#include <Xm/CascadeB.h>
#include <Xm/Command.h>
#include <Xm/Display.h>
#include <Xm/DrawingA.h>
#include <Xm/FileSB.h>
#include <Xm/Form.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/MainW.h>
#include <Xm/MessageB.h>
#include <Xm/Notebook.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBar.h>
#include <Xm/ScrolledW.h>
#include <Xm/SelectioB.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/TextP.h>
#include <Xm/TextOutP.h>
#include <Xm/TextLayout.h>
#include <X11/Intrinsic.h>
#include <X11/Shell.h>
#include <X11/Xutil.h>

#include <locale.h>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define CHECK(cond) do { \
    if (!(cond)) { \
        fprintf(stderr, "FAIL %s:%d: %s\n", __FILE__, __LINE__, #cond); \
        return 1; \
    } \
} while (0)

static void
drain(XtAppContext app, Display *dpy)
{
    for (int i = 0; i < 4; ++i) {
        struct timespec ts = {0, 5000000L};
        XSync(dpy, False);
        while (XtAppPending(app))
            XtAppProcessEvent(app, XtIMAll);
        nanosleep(&ts, NULL);
    }
    XSync(dpy, False);
}

static Dimension
dim_of(Widget w, const char *res)
{
    Dimension v = 0;
    XtVaGetValues(w, res, &v, NULL);
    return v;
}

static Position
pos_y(Widget w)
{
    Position v = 0;
    XtVaGetValues(w, XmNy, &v, NULL);
    return v;
}

static int
verify_historical_widgets(XtAppContext app, Display *dpy)
{
    XrmDatabase db;
    Widget shell, board;
    Widget mw_shell, mw, menubar, pulldown, cascade, cmd, msg, work_list;
    Widget mw_fixed_shell, mw_fixed, fixed_bar, fixed_work;
    Widget nb, nb_pinned, sb, fsb, msg_box;
    Widget dir_list, file_list, dir_sw, file_sw, dir_vsb = NULL, file_vsb = NULL;
    Dimension mb_h1, mb_h2, cmd_h1, cmd_h2, msg_h1, msg_h2;
    Dimension sb_mw1, sb_mw2, msg_mw1, msg_mw2, fsb_mw1, fsb_mw2;
    Dimension dir_sb_w1, dir_sb_w2, file_sb_w1, file_sb_w2;
    Dimension pinned_tab_h1, pinned_tab_h2;
    Position work_y1, work_y2;

    CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_CLASSIC));
    CHECK(XmSetDisplayScaleOverride(dpy, 1.0));

    db = XrmGetDatabase(dpy);
    XrmPutStringResource(&db, "*dpiMainWin.mainWindowMarginWidth", "6");
    XrmPutStringResource(&db, "*dpiMainWin.mainWindowMarginHeight", "4");
    /*Wait: Xrm resources are app-owned; test default + explicit separately!*/
    XrmPutStringResource(&db, "*fixedSelBox.marginHeight", "13");
    XrmSetDatabase(dpy, db);

    /* 1. XmMainWindow with MenuBar, CommandWindow, MessageWindow, Separators, ScrolledList */
    mw_shell = XtVaAppCreateShell("mwScale", "MwScale", topLevelShellWidgetClass,
                                  dpy, XmNwidth, 520, XmNheight, 420, NULL);
    mw = XtVaCreateManagedWidget("autoMainWin", xmMainWindowWidgetClass, mw_shell,
                                 XmNshowSeparator, True,
                                 XmNwidth, 520, XmNheight, 420, NULL);
    menubar = XmCreateMenuBar(mw, "menuBar", NULL, 0);
    pulldown = XmCreatePulldownMenu(menubar, "fileMenu", NULL, 0);
    XtVaCreateManagedWidget("openItem", xmPushButtonWidgetClass, pulldown, NULL);
    cascade = XtVaCreateManagedWidget("fileCascade", xmCascadeButtonWidgetClass, menubar,
                                      XmNsubMenuId, pulldown, NULL);
    (void)cascade;
    XtManageChild(menubar);

    cmd = XmCreateCommand(mw, "cmdArea", NULL, 0);
    XtManageChild(cmd);

    msg = XmCreateMessageBox(mw, "msgArea", NULL, 0);
    XtManageChild(msg);

    work_list = XmCreateScrolledList(mw, "workList", NULL, 0);
    XtManageChild(work_list);

    XmMainWindowSetAreas(mw, menubar, cmd, NULL, NULL, XtParent(work_list));
    XtVaSetValues(mw, XmNmessageWindow, msg, NULL);

    mw_fixed_shell = XtVaAppCreateShell("mwFixed", "MwScale", topLevelShellWidgetClass,
                                        dpy, XmNwidth, 360, XmNheight, 260, NULL);
    mw_fixed = XtVaCreateManagedWidget("dpiMainWin", xmMainWindowWidgetClass, mw_fixed_shell,
                                       XmNmainWindowMarginWidth, 11,
                                       XmNwidth, 360, XmNheight, 260, NULL);
    fixed_bar = XmCreateMenuBar(mw_fixed, "fBar", NULL, 0);
    XtManageChild(fixed_bar);
    fixed_work = XtVaCreateManagedWidget("fWork", xmDrawingAreaWidgetClass, mw_fixed,
                                         XmNwidth, 200, XmNheight, 120, NULL);
    XmMainWindowSetAreas(mw_fixed, fixed_bar, NULL, NULL, NULL, fixed_work);

    XtRealizeWidget(mw_shell);
    XtRealizeWidget(mw_fixed_shell);

    /* 2. XmNotebook (spiral binding + pinned notebook), XmSelectionBox, XmMessageBox, nested XmFileSelectionBox */
    shell = XtVaAppCreateShell("histScale", "HistScale", topLevelShellWidgetClass,
                               dpy, XmNwidth, 1100, XmNheight, 900, NULL);
    board = XtVaCreateManagedWidget("board", xmBulletinBoardWidgetClass, shell,
                                    XmNresizePolicy, XmRESIZE_NONE,
                                    XmNwidth, 1100, XmNheight, 900, NULL);

    nb = XtVaCreateManagedWidget("spiralNb", xmNotebookWidgetClass, board,
                                 XmNx, 10, XmNy, 10, XmNwidth, 320, XmNheight, 220,
                                 XmNbindingType, XmSPIRAL, NULL);
    for (int p = 1; p <= 2; ++p) {
        XtVaCreateManagedWidget("nbPage", xmFormWidgetClass, nb,
                                XmNnotebookChildType, XmPAGE, XmNpageNumber, p, NULL);
        XtVaCreateManagedWidget("nbTab", xmPushButtonWidgetClass, nb,
                                XmNnotebookChildType, XmMAJOR_TAB, XmNpageNumber, p, NULL);
    }

    Widget pinned_tab = NULL;
    nb_pinned = XtVaCreateManagedWidget("pinnedNb", xmNotebookWidgetClass, board,
                                        XmNx, 350, XmNy, 10, XmNwidth, 320, XmNheight, 220,
                                        XmNbindingWidth, 24, XmNbackPageSize, 8,
                                        XmNmajorTabSpacing, 4, XmNminorTabSpacing, 4,
                                        XmNinnerMarginWidth, 6, XmNinnerMarginHeight, 6,
                                        XmNframeShadowThickness, 2, NULL);
    XtVaCreateManagedWidget("pPage", xmFormWidgetClass, nb_pinned,
                            XmNnotebookChildType, XmPAGE, XmNpageNumber, 1, NULL);
    pinned_tab = XtVaCreateManagedWidget("pTab", xmArrowButtonWidgetClass, nb_pinned,
                                         XmNnotebookChildType, XmMAJOR_TAB, XmNpageNumber, 1,
                                         XmNhighlightThickness, 0, XmNshadowThickness, 0, NULL);

    sb = XtVaCreateManagedWidget("fixedSelBox", xmSelectionBoxWidgetClass, board,
                                 XmNx, 10, XmNy, 250, XmNwidth, 320, XmNheight, 260, NULL);
    msg_box = XtVaCreateManagedWidget("plainMsgBox", xmMessageBoxWidgetClass, board,
                                      XmNx, 350, XmNy, 250, XmNwidth, 300, XmNheight, 180, NULL);
    fsb = XtVaCreateManagedWidget("nestedFsb", xmFileSelectionBoxWidgetClass, board,
                                  XmNx, 680, XmNy, 10, XmNwidth, 400, XmNheight, 360, NULL);

    XtRealizeWidget(shell);
    drain(app, dpy);
    XtResizeWidget(mw, 520, 420, 0);
    XtResizeWidget(fsb, 400, 360, 0);
    drain(app, dpy);

    mb_h1 = dim_of(menubar, XmNheight);
    cmd_h1 = dim_of(cmd, XmNheight);
    msg_h1 = dim_of(msg, XmNheight);
    work_y1 = pos_y(XtParent(work_list));

    sb_mw1 = dim_of(sb, XmNmarginWidth);
    msg_mw1 = dim_of(msg_box, XmNmarginWidth);
    fsb_mw1 = dim_of(fsb, XmNmarginWidth);
    pinned_tab_h1 = dim_of(pinned_tab, XmNheight);

    dir_list = XmFileSelectionBoxGetChild(fsb, XmDIALOG_DIR_LIST);
    file_list = XmFileSelectionBoxGetChild(fsb, XmDIALOG_LIST);
    CHECK(dir_list != NULL && file_list != NULL);
    dir_sw = XtParent(dir_list);
    file_sw = XtParent(file_list);
    XtVaGetValues(dir_sw, XmNverticalScrollBar, &dir_vsb, NULL);
    XtVaGetValues(file_sw, XmNverticalScrollBar, &file_vsb, NULL);
    CHECK(dir_vsb != NULL && file_vsb != NULL);
    dir_sb_w1 = dim_of(dir_vsb, XmNwidth);
    file_sb_w1 = dim_of(file_vsb, XmNwidth);

    CHECK(dim_of(nb, XmNbindingWidth) == 25);
    CHECK(dim_of(nb, XmNframeShadowThickness) == 2);
    CHECK(dim_of(mw_fixed, XmNmainWindowMarginWidth) == 11);

    /* Transition 1x -> 2x */
    CHECK(XmSetDisplayScaleOverride(dpy, 2.0));
    drain(app, dpy);

    mb_h2 = dim_of(menubar, XmNheight);
    cmd_h2 = dim_of(cmd, XmNheight);
    msg_h2 = dim_of(msg, XmNheight);
    work_y2 = pos_y(XtParent(work_list));

    sb_mw2 = dim_of(sb, XmNmarginWidth);
    msg_mw2 = dim_of(msg_box, XmNmarginWidth);
    fsb_mw2 = dim_of(fsb, XmNmarginWidth);
    dir_sb_w2 = dim_of(dir_vsb, XmNwidth);
    file_sb_w2 = dim_of(file_vsb, XmNwidth);
    pinned_tab_h2 = dim_of(pinned_tab, XmNheight);

    /* Verify MainWindow reflowed its CommandWindow, MessageWindow, and WorkWindow */
    CHECK(cmd_h2 > cmd_h1);
    CHECK(msg_h2 > msg_h1);
    CHECK(work_y2 > work_y1);
    CHECK(mb_h2 >= mb_h1);
    CHECK(dim_of(mw_fixed, XmNmainWindowMarginWidth) == 11);

    /* Verify Notebook spiral binding + frameShadowThickness + pinned child tab reflow */
    CHECK(dim_of(nb, XmNbindingWidth) == 50);
    CHECK(dim_of(nb, XmNframeShadowThickness) == 4);
    CHECK(dim_of(nb_pinned, XmNbindingWidth) == 24);
    CHECK(dim_of(nb_pinned, XmNinnerMarginWidth) == 6);
    CHECK(pinned_tab_h2 > pinned_tab_h1);

    /* Verify SelectionBox, MessageBox, and nested FileSelectionBox chrome */
    CHECK(sb_mw2 == (Dimension)(2 * sb_mw1));
    CHECK(dim_of(sb, XmNmarginHeight) == 13);
    CHECK(msg_mw2 == (Dimension)(2 * msg_mw1));
    CHECK(fsb_mw2 == (Dimension)(2 * fsb_mw1));
    CHECK(dir_sb_w2 > dir_sb_w1 && file_sb_w2 > file_sb_w1);
    /* Nested DirList & FileList ScrolledWindows must remain non-overlapping inside fsb */
    CHECK(pos_y(dir_sw) > 0 && dim_of(dir_sw, XmNwidth) > dir_sb_w2);
    CHECK(dim_of(file_sw, XmNwidth) > file_sb_w2);

    /* Transition 2x -> 1x reversible restoration */
    CHECK(XmSetDisplayScaleOverride(dpy, 1.0));
    drain(app, dpy);

    CHECK(dim_of(nb, XmNbindingWidth) == 25);
    CHECK(dim_of(nb, XmNframeShadowThickness) == 2);
    CHECK(dim_of(sb, XmNmarginWidth) == sb_mw1);
    CHECK(dim_of(msg_box, XmNmarginWidth) == msg_mw1);
    CHECK(dim_of(fsb, XmNmarginWidth) == fsb_mw1);
    CHECK(dim_of(dir_vsb, XmNwidth) == dir_sb_w1);
    CHECK(dim_of(file_vsb, XmNwidth) == file_sb_w1);
    CHECK(pos_y(XtParent(work_list)) == work_y1);

    XtDestroyWidget(shell);
    XtDestroyWidget(mw_fixed_shell);
    XtDestroyWidget(mw_shell);
    drain(app, dpy);
    return 0;
}

static int
verify_subparagraph_incremental_shaping(XtAppContext app, Display *dpy)
{
    Widget shell, area, edited_wrap, fresh_wrap, edited_nowrap, fresh_nowrap;
    Arg fargs[3];
    XmRendition rend;
    XmFontList flist;
    char *big_text = NULL;
    size_t cap = 98304, len = 0;
    XmTextShapingUsage u0, u1, u2;

    if (!XmTextLayoutAvailable()) {
        puts("SKIP Unicode layout not compiled in");
        return 0;
    }

    shell = XtVaAppCreateShell("shapeScale", "ShapeScale", topLevelShellWidgetClass,
                               dpy, XmNwidth, 900, XmNheight, 700, NULL);
    XtSetArg(fargs[0], XmNfontType, XmFONT_IS_XFT);
    XtSetArg(fargs[1], XmNfontName, "DejaVu Sans");
    XtSetArg(fargs[2], XmNfontSize, 11);
    rend = XmRenditionCreate(shell, XmFONTLIST_DEFAULT_TAG, fargs, 3);
    CHECK(rend != NULL);
    flist = XmRenderTableAddRenditions(NULL, &rend, 1, XmMERGE_REPLACE);
    XmRenditionFree(rend);
    CHECK(flist != NULL);

    area = XtVaCreateManagedWidget("area", xmDrawingAreaWidgetClass, shell,
                                   XmNresizePolicy, XmRESIZE_NONE,
                                   XmNwidth, 900, XmNheight, 700, NULL);
    edited_wrap = XtVaCreateManagedWidget(
        "editedWrap", xmTextWidgetClass, area,
        XmNx, 10, XmNy, 10, XmNwidth, 420, XmNheight, 260,
        XmNfontList, flist, XmNeditMode, XmMULTI_LINE_EDIT,
        XmNwordWrap, True, XmNresizeWidth, False, XmNresizeHeight, False,
        XmNblinkRate, 0, NULL);
    fresh_wrap = XtVaCreateManagedWidget(
        "freshWrap", xmTextWidgetClass, area,
        XmNx, 450, XmNy, 10, XmNwidth, 420, XmNheight, 260,
        XmNfontList, flist, XmNeditMode, XmMULTI_LINE_EDIT,
        XmNwordWrap, True, XmNresizeWidth, False, XmNresizeHeight, False,
        XmNblinkRate, 0, NULL);
    edited_nowrap = XtVaCreateManagedWidget(
        "editedNowrap", xmTextWidgetClass, area,
        XmNx, 10, XmNy, 300, XmNwidth, 420, XmNheight, 140,
        XmNfontList, flist, XmNeditMode, XmMULTI_LINE_EDIT,
        XmNwordWrap, False, XmNresizeWidth, False, XmNresizeHeight, False,
        XmNblinkRate, 0, NULL);
    fresh_nowrap = XtVaCreateManagedWidget(
        "freshNowrap", xmTextWidgetClass, area,
        XmNx, 450, XmNy, 300, XmNwidth, 420, XmNheight, 140,
        XmNfontList, flist, XmNeditMode, XmMULTI_LINE_EDIT,
        XmNwordWrap, False, XmNresizeWidth, False, XmNresizeHeight, False,
        XmNblinkRate, 0, NULL);

    /* Construct a ~18,750-scalar single paragraph (no newlines) */
    big_text = (char *)malloc(cap);
    CHECK(big_text != NULL);
    for (int w = 0; w < 750; ++w) {
        int n = snprintf(big_text + len, cap - len,
                         "office%03d final cafe\xcc\x81 \xe6\x97\xa5\xe6\x9c\xac ", w);
        CHECK(n > 0 && len + (size_t)n < cap);
        len += (size_t)n;
    }
    big_text[len] = '\0';

    XtRealizeWidget(shell);
    drain(app, dpy);

    /* A0. Lazy viewport-horizon wrap on an 18,750-scalar paragraph must NOT shape off-screen tail */
    {
        XftFont *primary = (XftFont *)((XmTextWidget)edited_wrap)->text.output->data->font;
        CHECK(primary != NULL);
        XmTextFontContext *lazy_ctx = XmTextFontContextCreate(
            dpy, DefaultScreen(dpy), primary, NULL);
        CHECK(lazy_ctx != NULL);
        XmTextParagraph *lazy_p = XmTextParagraphCreateWithContext(
            lazy_ctx, big_text, len, 64.0);
        CHECK(lazy_p != NULL);
        size_t lpos = 0;
        for (int vl = 0; vl < 6; ++vl) {
            size_t lend = 0;
            XmTextLayout *ll = XmTextParagraphWrapLine(lazy_p, lpos, 400.0, &lend);
            CHECK(ll != NULL && lend > lpos);
            XmTextLayoutFree(ll);
            lpos = lend;
        }
        XmTextShapingUsage u_lazy;
        CHECK(XmTextFontContextGetShapingUsage(lazy_ctx, &u_lazy));
        CHECK(u_lazy.shaped_scalars > 0 && u_lazy.shaped_scalars < 1600);
        XmTextParagraphFree(lazy_p);
        XmTextFontContextFree(lazy_ctx);
    }

    /* A. Wrapped 18,750-scalar single paragraph: sub-paragraph incremental edit in live XmText */
    XmTextSetString(edited_wrap, big_text);
    drain(app, dpy);
    XmTextPosition total_scalars = XmTextGetLastPosition(edited_wrap);
    CHECK(total_scalars >= 16000);

    CHECK(XmTextFontContextGetShapingUsage(
        ((XmTextWidget)edited_wrap)->text.output->data->unicode_font_context, &u0));
    CHECK(u0.shaped_scalars > 0);

    XmTextReplace(edited_wrap, 120, 126, "updated");
    drain(app, dpy);
    CHECK(XmTextFontContextGetShapingUsage(
        ((XmTextWidget)edited_wrap)->text.output->data->unicode_font_context, &u1));
    size_t wrap_edit_shaped = u1.shaped_scalars - u0.shaped_scalars;
    /* Edit inside 18.7k-scalar wrapped paragraph reuses unchanged sub-paragraph chunks across all ~400 wrapped lines! */
    CHECK(wrap_edit_shaped > 0 && wrap_edit_shaped < 1500);

    XmTextReplace(edited_wrap, 9200, 9206, "center");
    drain(app, dpy);
    XmTextShapingUsage u1b;
    CHECK(XmTextFontContextGetShapingUsage(
        ((XmTextWidget)edited_wrap)->text.output->data->unicode_font_context, &u1b));
    size_t wrap_mid_shaped = u1b.shaped_scalars - u1.shaped_scalars;
    CHECK(wrap_mid_shaped > 0 && wrap_mid_shaped < 1500);

    char *cur_val = XmTextGetString(edited_wrap);
    CHECK(cur_val != NULL);
    XmTextSetString(fresh_wrap, cur_val);
    XtFree(cur_val);
    drain(app, dpy);

    for (XmTextPosition pos = 0, line_idx = 0; line_idx < 6; ++line_idx) {
        XmTextPosition af = 0, al = 0, bf = 0, bl = 0;
        XmTextLayout *la = _XmTextGetLineCapture((XmTextWidget)edited_wrap, pos, &af, &al);
        XmTextLayout *lb = _XmTextGetLineCapture((XmTextWidget)fresh_wrap, pos, &bf, &bl);
        CHECK(la != NULL && lb != NULL);
        CHECK(af == bf && al == bl);
        CHECK(XmTextLayoutGlyphCount(la) == XmTextLayoutGlyphCount(lb));
        CHECK(fabs(XmTextLayoutWidth(la) - XmTextLayoutWidth(lb)) < 0.001);
        pos = al;
        XmTextLayoutFree(la);
        XmTextLayoutFree(lb);
    }

    /* B. Unwrapped 18k-scalar single paragraph: mid-paragraph edit reuses prefix & suffix chunks */
    XmTextSetString(edited_nowrap, big_text);
    drain(app, dpy);
    CHECK(XmTextFontContextGetShapingUsage(
        ((XmTextWidget)edited_nowrap)->text.output->data->unicode_font_context, &u1));

    XmTextReplace(edited_nowrap, 8500, 8505, "midEdit");
    drain(app, dpy);
    CHECK(XmTextFontContextGetShapingUsage(
        ((XmTextWidget)edited_nowrap)->text.output->data->unicode_font_context, &u2));
    size_t nowrap_edit_shaped = u2.shaped_scalars - u1.shaped_scalars;
    /* Mid-paragraph single edit in 18k-scalar unwrapped line must shape <= 2 chunks (< 900 scalars)! */
    CHECK(nowrap_edit_shaped > 0 && nowrap_edit_shaped < 900);

    cur_val = XmTextGetString(edited_nowrap);
    CHECK(cur_val != NULL);
    XmTextSetString(fresh_nowrap, cur_val);
    XtFree(cur_val);
    drain(app, dpy);
    {
        XmTextPosition af = 0, al = 0, bf = 0, bl = 0;
        XmTextLayout *la = _XmTextGetLineCapture((XmTextWidget)edited_nowrap, 0, &af, &al);
        XmTextLayout *lb = _XmTextGetLineCapture((XmTextWidget)fresh_nowrap, 0, &bf, &bl);
        CHECK(la != NULL && lb != NULL);
        CHECK(af == bf && al == bl);
        CHECK(XmTextLayoutGlyphCount(la) == XmTextLayoutGlyphCount(lb));
        CHECK(fabs(XmTextLayoutWidth(la) - XmTextLayoutWidth(lb)) < 0.001);
        double ca = 0.0, cb = 0.0;
        CHECK(XmTextLayoutCaret(la, 8503, 0, &ca) && XmTextLayoutCaret(lb, 8503, 0, &cb));
        CHECK(fabs(ca - cb) < 0.001);
        XmTextLayoutFree(la);
        XmTextLayoutFree(lb);
    }

    /* C. >70,000-scalar single paragraph wrap without 26.6 fixed-point overflow */
    {
        size_t huge_len = 72000;
        char *huge = (char *)malloc(huge_len + 1);
        CHECK(huge != NULL);
        for (size_t i = 0; i < huge_len; ++i)
            huge[i] = ((i % 12) == 11) ? ' ' : (char)('a' + (i % 26));
        huge[huge_len] = '\0';
        XftFont *xft = (XftFont *)((XmTextWidget)edited_wrap)->text.output->data->font;
        CHECK(xft != NULL);
        XmTextParagraph *hp = XmTextParagraphCreate(dpy, xft, huge, huge_len, 64.0);
        CHECK(hp != NULL && XmTextParagraphScalarCount(hp) == huge_len);
        size_t next_s = 0;
        XmTextLayout *hl = XmTextParagraphWrapLine(hp, 0, 300.0, &next_s);
        CHECK(hl != NULL && next_s > 0 && next_s < 200);
        XmTextLayoutFree(hl);
        XmTextParagraphFree(hp);
        free(huge);
    }

    printf("PASS sub-paragraph incremental shaping (total=%ld initial_wrap_shaped=%zu wrap_edit_shaped=%zu nowrap_mid_edit_shaped=%zu)\n",
           (long)total_scalars, u0.shaped_scalars, wrap_edit_shaped, nowrap_edit_shaped);

    free(big_text);
    XtDestroyWidget(shell);
    XmRenderTableFree(flist);
    drain(app, dpy);
    return 0;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget root_shell;
    Display *dpy;

    (void)setlocale(LC_ALL, "C.UTF-8");
    XtSetLanguageProc(NULL, NULL, NULL);
    root_shell = XtVaAppInitialize(&app, "HistoricalScaleShaping", NULL, 0,
                                   &argc, argv, NULL, NULL);
    dpy = XtDisplay(root_shell);

    if (verify_historical_widgets(app, dpy) != 0)
        return 1;
    if (verify_subparagraph_incremental_shaping(app, dpy) != 0)
        return 1;

    XtDestroyWidget(root_shell);
    XtDestroyApplicationContext(app);
    puts("PASS historical-widget live DPI reflow and sub-paragraph incremental shaping");
    return 0;
}
