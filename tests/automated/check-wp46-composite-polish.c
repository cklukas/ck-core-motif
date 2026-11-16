/* WP-46 regression test: composite appearance, natural sizing, and context-menu entries. */
#include "native-window.h"
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Paned.h>
#include <Xm/TabBox.h>
#include <Xm/TabStack.h>
#include <Xm/BreadcrumbBar.h>
#include <Xm/DropHighlight.h>
#include <Xm/InfoBar.h>
#include <Xm/DataGrid.h>
#include <Xm/TreeView.h>
#include <Xm/Sidebar.h>
#include <Xm/CommandPalette.h>
#include <X11/keysym.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(cond) \
  do { \
    if (!(cond)) { \
      fprintf(stderr, "FAIL WP-46 composite polish line %d: %s\n", __LINE__, #cond); \
      exit(1); \
    } \
  } while (0)

static void
pump(XtAppContext app, Display *dpy)
{
  for (unsigned i = 0; i < 4; ++i) {
    XSync(dpy, False);
    while (XtAppPending(app))
      XtAppProcessEvent(app, XtIMAll);
  }
}

static unsigned int context_activated = 0;

static void
on_context_activate(Widget w, XtPointer client_data, XtPointer call_data)
{
  (void)w;
  (void)call_data;
  if (client_data != NULL)
    context_activated += (unsigned int)(uintptr_t)client_data;
}

int
main(int argc, char **argv)
{
  XtAppContext app;
  Widget shell = XtVaAppInitialize(&app, "Wp46CompositePolish", NULL, 0,
                                   &argc, argv, NULL, NULL);
  Display *dpy = XtDisplay(shell);
  CHECK(XmSetAppearanceProfile(dpy, XmAPPEARANCE_CLASSIC));
  CHECK(XmSetAppearanceDensity(dpy, XmAPPEARANCE_DENSITY_COMFORTABLE));

  Widget host = XtVaCreateManagedWidget("host", xmBulletinBoardWidgetClass,
                                        shell, XmNwidth, 1280, XmNheight, 900,
                                        NULL);

  /* 1. PanedWindow and Paned sash + density + live profile transitions */
  Widget pw = XtVaCreateManagedWidget("pw", xmPanedWindowWidgetClass, host,
                                      XmNx, 10, XmNy, 10, NULL);
  XmString top_str = XmStringCreateLocalized((char *)"Top pane label");
  XmString bot_str = XmStringCreateLocalized((char *)"Bottom pane label");
  XtVaCreateManagedWidget("pwTop", xmLabelWidgetClass, pw,
                          XmNlabelString, top_str, NULL);
  XtVaCreateManagedWidget("pwBottom", xmLabelWidgetClass, pw,
                          XmNlabelString, bot_str, NULL);

  Widget paned = XtVaCreateManagedWidget("paned", xmPanedWidgetClass, host,
                                         XmNx, 260, XmNy, 10, NULL);
  XtVaCreateManagedWidget("pTop", xmLabelWidgetClass, paned,
                          XmNlabelString, top_str, NULL);
  XtVaCreateManagedWidget("pBottom", xmLabelWidgetClass, paned,
                          XmNlabelString, bot_str, NULL);
  XmStringFree(top_str);
  XmStringFree(bot_str);

  /* 2. SpinBox with explicit width and ComboBox */
  Widget spin = XtVaCreateManagedWidget("spin", xmSpinBoxWidgetClass, host,
                                        XmNx, 520, XmNy, 10, XmNwidth, 180, NULL);
  XtVaCreateManagedWidget("spinField", xmTextFieldWidgetClass, spin,
                          XmNspinBoxChildType, XmNUMERIC, XmNminimumValue, 0,
                          XmNmaximumValue, 50, XmNposition, 5, XmNcolumns, 6,
                          NULL);

  XmString cb_items[2];
  cb_items[0] = XmStringCreateLocalized((char *)"Alpha");
  cb_items[1] = XmStringCreateLocalized((char *)"Beta");
  Widget combo = XtVaCreateManagedWidget("combo", xmComboBoxWidgetClass, host,
                                         XmNx, 520, XmNy, 70,
                                         XmNitems, cb_items, XmNitemCount, 2,
                                         XmNvisibleItemCount, 2, NULL);
  XmStringFree(cb_items[0]);
  XmStringFree(cb_items[1]);

  /* 3. TabBox and TabStack */
  XmTabbedStackList tabs = XmTabbedStackListCreate();
  const char *tab_names[3] = {"Overview", "Details", "Activity"};
  for (int i = 0; i < 3; ++i) {
    XmString ts = XmStringCreateLocalized((char *)tab_names[i]);
    XmTabbedStackListSimpleAppend(tabs, ts);
    XmStringFree(ts);
  }
  Widget tabbox = XtVaCreateManagedWidget("tabbox", xmTabBoxWidgetClass, host,
                                          XmNx, 10, XmNy, 160,
                                          XmNtabList, tabs, NULL);
  XmTabbedStackListFree(tabs);

  Widget tabstack = XtVaCreateManagedWidget("tabstack", xmTabStackWidgetClass,
                                            host, XmNx, 10, XmNy, 240, NULL);
  for (int i = 0; i < 2; ++i) {
    XmString tl = XmStringCreateLocalized((char *)tab_names[i]);
    XmString pl = XmStringCreateLocalized((char *)"Page body text");
    XtVaCreateManagedWidget(tab_names[i], xmLabelWidgetClass, tabstack,
                            XmNtabLabelString, tl, XmNlabelString, pl, NULL);
    XmStringFree(tl);
    XmStringFree(pl);
  }

  /* 4. Natural sizing without caller-supplied XmNwidth / XmNheight */
  XmBreadcrumbSegment bsegs[3] = {
    {"Home", (XtPointer)"home"},
    {"Workspace", (XtPointer)"ws"},
    {"Settings", (XtPointer)"settings"}
  };
  Widget bc = XtVaCreateManagedWidget("bc", xmBreadcrumbBarWidgetClass, host,
                                      XmNx, 10, XmNy, 380, NULL);
  XmBreadcrumbBarSetSegments(bc, bsegs, 3);

  XmString dh_label = XmStringCreateLocalized((char *)"Drop archive here");
  Widget dh = XtVaCreateManagedWidget("dh", xmDropHighlightWidgetClass, host,
                                      XmNx, 10, XmNy, 440,
                                      XmNdropHighlightLabel, dh_label, NULL);
  XmStringFree(dh_label);
  XmDropHighlightSetState(dh, XmDROP_HIGHLIGHT_VALID);

  Widget ib = XtVaCreateManagedWidget("ib", xmInfoBarWidgetClass, host,
                                      XmNx, 10, XmNy, 540,
                                      XmNinfoMessage, (char *)"Configuration saved",
                                      XmNinfoActionLabel, (char *)"Revert",
                                      NULL);

  static const char *const dg_row0[] = {"Alpha", "Ready"};
  static const char *const dg_row1[] = {"Beta", "Queued"};
  static const XmDataGridColumn dg_cols[] = {
    {(XtPointer)"name", "Name", False, 0},
    {(XtPointer)"state", "State", False, 0}
  };
  static const XmDataGridRow dg_rows[] = {
    {(XtPointer)"r0", dg_row0},
    {(XtPointer)"r1", dg_row1}
  };
  Widget dg = XtVaCreateManagedWidget("dg", xmDataGridWidgetClass, host,
                                      XmNx, 400, XmNy, 380, NULL);
  CHECK(XmDataGridSetModel(dg, dg_cols, 2, dg_rows, 2));

  static const XmTreeViewNode tv_nodes[] = {
    {(XtPointer)"root", NULL, "Project Root", True, True},
    {(XtPointer)"child", (XtPointer)"root", "Submodule", False, False}
  };
  Widget tv = XtVaCreateManagedWidget("tv", xmTreeViewWidgetClass, host,
                                      XmNx, 400, XmNy, 560, NULL);
  XmTreeViewSetNodes(tv, tv_nodes, 2);

  /* 5. Opt-in Icon-and-Shortcut Context-Menu Entries */
  Pixmap menu_icon = XCreatePixmap(dpy, DefaultRootWindow(dpy), 16, 16,
                                   DefaultDepth(dpy, DefaultScreen(dpy)));
  Widget popup = XmCreatePopupMenu(host, (char *)"contextPopup", NULL, 0);
  CHECK(popup != NULL);
  XmString open_lbl = XmStringCreateLocalized((char *)"Open Document");
  XmString open_acc = XmStringCreateLocalized((char *)"Ctrl+O");
  XmString save_lbl = XmStringCreateLocalized((char *)"Save Copy");
  XmString save_acc = XmStringCreateLocalized((char *)"Ctrl+S");
  XmContextMenuEntry entries[2] = {
    {"openEntry", open_lbl, open_acc, "<Key>o", XK_O, menu_icon, False, True,
     on_context_activate, (XtPointer)(uintptr_t)3},
    {"saveEntry", save_lbl, save_acc, "<Key>s", XK_S, None, True, False,
     on_context_activate, (XtPointer)(uintptr_t)7}
  };
  CHECK(XmRowColumnSetContextMenuEntries(popup, entries, 2));
  XmStringFree(open_lbl);
  XmStringFree(open_acc);
  XmStringFree(save_lbl);
  XmStringFree(save_acc);

  Widget open_btn = XtNameToWidget(popup, "openEntry");
  Widget save_btn = XtNameToWidget(popup, "saveEntry");
  CHECK(open_btn != NULL);
  CHECK(save_btn != NULL);
  unsigned char open_type = 0, save_type = 0;
  Pixmap got_pix = None;
  Dimension pad = 0;
  XtVaGetValues(open_btn, XmNlabelType, &open_type, XmNlabelPixmap, &got_pix,
                XmNpixmapTextPadding, &pad, NULL);
  XtVaGetValues(save_btn, XmNlabelType, &save_type, NULL);
  CHECK(open_type == XmPIXMAP_AND_STRING);
  CHECK(got_pix == menu_icon);
  CHECK(pad == 8);
  CHECK(save_type == XmSTRING);
  CHECK(!XtIsSensitive(save_btn));
  XtCallCallbacks(open_btn, XmNactivateCallback, NULL);
  CHECK(context_activated == 3);

  XtRealizeWidget(shell);
  NativeWaitForViewable(host);
  pump(app, dpy);

  /* Verify natural dimensions are non-trivial without explicit XmNwidth/XmNheight */
  Dimension bc_w = 0, bc_h = 0, dh_w = 0, dh_h = 0, ib_w = 0, ib_h = 0;
  Dimension dg_w = 0, dg_h = 0, tv_w = 0, tv_h = 0, cb_w = 0, cb_h = 0;
  Dimension tb_w = 0, tb_h = 0, ts_w = 0, ts_h = 0;
  XtVaGetValues(bc, XmNwidth, &bc_w, XmNheight, &bc_h, NULL);
  XtVaGetValues(dh, XmNwidth, &dh_w, XmNheight, &dh_h, NULL);
  XtVaGetValues(ib, XmNwidth, &ib_w, XmNheight, &ib_h, NULL);
  XtVaGetValues(dg, XmNwidth, &dg_w, XmNheight, &dg_h, NULL);
  XtVaGetValues(tv, XmNwidth, &tv_w, XmNheight, &tv_h, NULL);
  XtVaGetValues(combo, XmNwidth, &cb_w, XmNheight, &cb_h, NULL);
  XtVaGetValues(tabbox, XmNwidth, &tb_w, XmNheight, &tb_h, NULL);
  XtVaGetValues(tabstack, XmNwidth, &ts_w, XmNheight, &ts_h, NULL);
  CHECK(bc_w >= 180 && bc_h >= 24);
  CHECK(dh_w >= 240 && dh_h >= 64);
  CHECK(ib_w >= 200 && ib_h >= 28);
  CHECK(dg_w >= 200 && dg_h >= 80);
  CHECK(tv_w >= 80 && tv_h >= 36);
  CHECK(cb_w >= 40 && cb_h >= 20);
  CHECK(tb_w >= 80 && tb_h >= 20);
  CHECK(ts_w >= 80 && ts_h >= 40);

  /* Switch profile and density live and verify PanedWindow/Paned sash + SpinBox */
  CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_MODERN_LIGHT));
  pump(app, dpy);
  Dimension pw_sh = 99, paned_sh = 99, pw_sash_sh = 0, paned_sash_sh = 0;
  Dimension pw_sw = 0, paned_sw = 0, spin_w = 0;
  XtVaGetValues(pw, XmNshadowThickness, &pw_sh,
                XmNsashShadowThickness, &pw_sash_sh,
                XmNsashWidth, &pw_sw, NULL);
  XtVaGetValues(paned, XmNshadowThickness, &paned_sh,
                XmNsashShadowThickness, &paned_sash_sh,
                XmNsashWidth, &paned_sw, NULL);
  XtVaGetValues(spin, XmNwidth, &spin_w, NULL);
  CHECK(pw_sh == 0 && paned_sh == 0);
  CHECK(pw_sash_sh == 1 && paned_sash_sh == 1);
  CHECK(pw_sw == 12 && paned_sw == 12);
  CHECK(spin_w == 180);

  CHECK(XmApplyAppearanceDensity(dpy, XmAPPEARANCE_DENSITY_COMPACT));
  pump(app, dpy);
  XtVaGetValues(pw, XmNsashWidth, &pw_sw, NULL);
  XtVaGetValues(paned, XmNsashWidth, &paned_sw, NULL);
  XtVaGetValues(spin, XmNwidth, &spin_w, NULL);
  CHECK(pw_sw == 10 && paned_sw == 10);
  CHECK(spin_w == 180);

  CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_MODERN_DARK));
  pump(app, dpy);
  CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_HIGH_CONTRAST));
  pump(app, dpy);
  XtVaGetValues(pw, XmNsashShadowThickness, &pw_sash_sh, NULL);
  XtVaGetValues(paned, XmNsashShadowThickness, &paned_sash_sh, NULL);
  CHECK(pw_sash_sh == 2 && paned_sash_sh == 2);

  CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_CLASSIC));
  CHECK(XmApplyAppearanceDensity(dpy, XmAPPEARANCE_DENSITY_COMFORTABLE));
  pump(app, dpy);

  XFreePixmap(dpy, menu_icon);
  XtDestroyWidget(shell);
  XtCloseDisplay(dpy);
  XtDestroyApplicationContext(app);
  printf("PASS WP-46 composite polish, natural sizing, and context-menu entries\n");
  return 0;
}
