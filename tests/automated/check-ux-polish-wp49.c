/*
 * WP-49 regression test: modern hover affordances, XmNfocusHighlightPolicy
 * (focus-visible keyboard vs pointer focus), and standardized mouse-wheel
 * scrolling with nested ScrolledWindow boundary forwarding.
 */
#include "native-window.h"
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/DataGrid.h>
#include <Xm/TreeView.h>
#include <X11/keysym.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(cond) \
  do { \
    if (!(cond)) { \
      fprintf(stderr, "FAIL WP-49 UX polish line %d: %s\n", __LINE__, #cond); \
      exit(1); \
    } \
  } while (0)

static void
pump(XtAppContext app, Display *dpy)
{
  for (unsigned i = 0; i < 6; ++i) {
    XSync(dpy, False);
    while (XtAppPending(app))
      XtAppProcessEvent(app, XtIMAll);
  }
}

static uint64_t
widget_pixel_hash(Widget w, int x0, int y0, int rw, int rh)
{
  Display *dpy = XtDisplayOfObject(w);
  Window win = XtWindowOfObject(w);
  int ww = (int)XtWidth(w);
  int wh = (int)XtHeight(w);
  if (rw <= 0 || x0 + rw > ww)
    rw = ww - x0;
  if (rh <= 0 || y0 + rh > wh)
    rh = wh - y0;
  CHECK(win != None && rw > 0 && rh > 0);
  XSync(dpy, False);
  XImage *img = XGetImage(dpy, win, x0, y0, (unsigned int)rw, (unsigned int)rh,
                          AllPlanes, ZPixmap);
  CHECK(img != NULL);
  uint64_t hash = 1469598103934665603ULL;
  for (int y = 0; y < rh; ++y) {
    for (int x = 0; x < rw; ++x) {
      unsigned long p = XGetPixel(img, x, y);
      hash ^= (uint64_t)p;
      hash *= 1099511628211ULL;
    }
  }
  XDestroyImage(img);
  return hash;
}

static void
dispatch_crossing(Widget w, int type, int x, int y)
{
  Display *dpy = XtDisplayOfObject(w);
  Window win = XtWindowOfObject(w);
  XEvent ev;
  memset(&ev, 0, sizeof(ev));
  ev.xcrossing.type = type;
  ev.xcrossing.display = dpy;
  ev.xcrossing.window = win;
  ev.xcrossing.root = DefaultRootWindow(dpy);
  ev.xcrossing.subwindow = None;
  ev.xcrossing.time = CurrentTime;
  ev.xcrossing.x = x;
  ev.xcrossing.y = y;
  ev.xcrossing.x_root = x;
  ev.xcrossing.y_root = y;
  ev.xcrossing.mode = NotifyNormal;
  ev.xcrossing.detail = NotifyAncestor;
  ev.xcrossing.same_screen = True;
  ev.xcrossing.focus = True;
  XtDispatchEvent(&ev);
}

static void
dispatch_motion(Widget w, int x, int y)
{
  Display *dpy = XtDisplayOfObject(w);
  Window win = XtWindowOfObject(w);
  XEvent ev;
  memset(&ev, 0, sizeof(ev));
  ev.xmotion.type = MotionNotify;
  ev.xmotion.display = dpy;
  ev.xmotion.window = win;
  ev.xmotion.root = DefaultRootWindow(dpy);
  ev.xmotion.subwindow = None;
  ev.xmotion.time = CurrentTime;
  ev.xmotion.x = x;
  ev.xmotion.y = y;
  ev.xmotion.x_root = x;
  ev.xmotion.y_root = y;
  ev.xmotion.is_hint = NotifyNormal;
  ev.xmotion.same_screen = True;
  XtDispatchEvent(&ev);
}

static void
dispatch_unmap(Widget w)
{
  Display *dpy = XtDisplayOfObject(w);
  Window win = XtWindowOfObject(w);
  XEvent ev;
  memset(&ev, 0, sizeof(ev));
  ev.xunmap.type = UnmapNotify;
  ev.xunmap.display = dpy;
  ev.xunmap.event = win;
  ev.xunmap.window = win;
  ev.xunmap.from_configure = False;
  XtDispatchEvent(&ev);
}

static Time
next_event_time(void)
{
  static Time t = 1000;
  t += 400;
  return t;
}

static void
dispatch_button(Widget w, unsigned int button, int x, int y)
{
  Display *dpy = XtDisplayOfObject(w);
  Window win = XtWindowOfObject(w);
  XEvent ev;
  memset(&ev, 0, sizeof(ev));
  ev.xbutton.type = ButtonPress;
  ev.xbutton.display = dpy;
  ev.xbutton.window = win;
  ev.xbutton.root = DefaultRootWindow(dpy);
  ev.xbutton.subwindow = None;
  ev.xbutton.time = next_event_time();
  ev.xbutton.x = x;
  ev.xbutton.y = y;
  ev.xbutton.x_root = x;
  ev.xbutton.y_root = y;
  ev.xbutton.button = button;
  ev.xbutton.same_screen = True;
  XtDispatchEvent(&ev);

  ev.xbutton.type = ButtonRelease;
  ev.xbutton.time += 10;
  ev.xbutton.state = (Button1Mask << (button - 1));
  XtDispatchEvent(&ev);
}

static void
dispatch_key(Widget w, KeySym keysym)
{
  Display *dpy = XtDisplayOfObject(w);
  Window win = XtWindowOfObject(w);
  KeyCode kc = XKeysymToKeycode(dpy, keysym);
  CHECK(kc != 0);
  XEvent ev;
  memset(&ev, 0, sizeof(ev));
  ev.xkey.type = KeyPress;
  ev.xkey.display = dpy;
  ev.xkey.window = win;
  ev.xkey.root = DefaultRootWindow(dpy);
  ev.xkey.subwindow = None;
  ev.xkey.time = next_event_time();
  ev.xkey.x = 4;
  ev.xkey.y = 4;
  ev.xkey.keycode = kc;
  ev.xkey.same_screen = True;
  XtDispatchEvent(&ev);

  ev.xkey.type = KeyRelease;
  ev.xkey.time = next_event_time();
  XtDispatchEvent(&ev);
}

static void
verify_hover_cycle(XtAppContext app, Display *dpy, Widget w,
                   int hx, int hy, Boolean expect_change)
{
  dispatch_crossing(w, LeaveNotify, -5, -5);
  pump(app, dpy);
  uint64_t base = widget_pixel_hash(w, 0, 0, 0, 0);

  dispatch_crossing(w, EnterNotify, hx, hy);
  dispatch_motion(w, hx, hy);
  pump(app, dpy);
  uint64_t hovered = widget_pixel_hash(w, 0, 0, 0, 0);

  if (expect_change) {
    CHECK(hovered != base);
  } else {
    CHECK(hovered == base);
  }

  dispatch_crossing(w, LeaveNotify, -5, -5);
  pump(app, dpy);
  uint64_t left = widget_pixel_hash(w, 0, 0, 0, 0);
  CHECK(left == base);

  if (expect_change) {
    dispatch_crossing(w, EnterNotify, hx, hy);
    dispatch_motion(w, hx, hy);
    pump(app, dpy);
    CHECK(widget_pixel_hash(w, 0, 0, 0, 0) == hovered);

    dispatch_unmap(w);
    pump(app, dpy);
    CHECK(widget_pixel_hash(w, 0, 0, 0, 0) == base);
  }
}

int
main(int argc, char **argv)
{
  XtAppContext app;
  Widget shell = XtVaAppInitialize(&app, "Wp49UxPolish", NULL, 0,
                                   &argc, argv, NULL, NULL);
  Display *dpy = XtDisplay(shell);
  CHECK(XmSetAppearanceProfile(dpy, XmAPPEARANCE_MODERN_LIGHT));
  CHECK(XmSetAppearanceDensity(dpy, XmAPPEARANCE_DENSITY_COMFORTABLE));

  Widget root_form = XtVaCreateManagedWidget(
      "rootForm", xmBulletinBoardWidgetClass, shell,
      XmNwidth, 1280, XmNheight, 860, NULL);

  /* 1. Controls for hover & focus-visible tests */
  XmString btn_s = XmStringCreateLocalized((char *)"Primary Action");
  Widget push_btn = XtVaCreateManagedWidget(
      "pushBtn", xmPushButtonWidgetClass, root_form,
      XmNx, 20, XmNy, 20, XmNwidth, 150, XmNheight, 36,
      XmNnavigationType, XmTAB_GROUP,
      XmNlabelString, btn_s, NULL);
  XmStringFree(btn_s);

  XmString focus_s = XmStringCreateLocalized((char *)"Secondary Action");
  Widget focus_btn = XtVaCreateManagedWidget(
      "focusBtn", xmPushButtonWidgetClass, root_form,
      XmNx, 190, XmNy, 20, XmNwidth, 150, XmNheight, 36,
      XmNnavigationType, XmTAB_GROUP,
      XmNlabelString, focus_s, NULL);
  XmStringFree(focus_s);

  XmString tog_s = XmStringCreateLocalized((char *)"Enable Hover");
  Widget toggle_btn = XtVaCreateManagedWidget(
      "toggleBtn", xmToggleButtonWidgetClass, root_form,
      XmNx, 360, XmNy, 20, XmNwidth, 150, XmNheight, 36,
      XmNlabelString, tog_s, NULL);
  XmStringFree(tog_s);

  XmString cb_items[3];
  cb_items[0] = XmStringCreateLocalized((char *)"Option One");
  cb_items[1] = XmStringCreateLocalized((char *)"Option Two");
  cb_items[2] = XmStringCreateLocalized((char *)"Option Three");
  Widget combo = XtVaCreateManagedWidget(
      "combo", xmComboBoxWidgetClass, root_form,
      XmNx, 530, XmNy, 20, XmNwidth, 180,
      XmNitems, cb_items, XmNitemCount, 3, XmNvisibleItemCount, 3, NULL);
  for (int i = 0; i < 3; ++i)
    XmStringFree(cb_items[i]);

  Widget notebook = XtVaCreateManagedWidget(
      "notebook", xmNotebookWidgetClass, root_form,
      XmNx, 20, XmNy, 80, XmNwidth, 320, XmNheight, 140, NULL);
  XmString page1_s = XmStringCreateLocalized((char *)"Notebook Page 1");
  XtVaCreateManagedWidget(
      "page1", xmLabelWidgetClass, notebook,
      XmNnotebookChildType, XmPAGE, XmNpageNumber, 1,
      XmNlabelString, page1_s, NULL);
  XmStringFree(page1_s);
  XmString tab1_s = XmStringCreateLocalized((char *)"Overview Tab");
  Widget nb_tab = XtVaCreateManagedWidget(
      "tab1", xmPushButtonWidgetClass, notebook,
      XmNnotebookChildType, XmMAJOR_TAB, XmNpageNumber, 1,
      XmNlabelString, tab1_s, NULL);
  XmStringFree(tab1_s);

  /* 2. Outer ScrolledWindow with nested scrollable List, Text, TreeView, DataGrid */
  Widget outer_sw = XtVaCreateManagedWidget(
      "outerSw", xmScrolledWindowWidgetClass, root_form,
      XmNx, 20, XmNy, 250, XmNwidth, 1180, XmNheight, 260,
      XmNscrollingPolicy, XmAUTOMATIC, NULL);
  Widget outer_bb = XtVaCreateManagedWidget(
      "outerBb", xmBulletinBoardWidgetClass, outer_sw,
      XmNresizePolicy, XmRESIZE_NONE,
      XmNwidth, 1120, XmNheight, 760, NULL);

  XmString list_items[16];
  char buf[64];
  for (int i = 0; i < 16; ++i) {
    snprintf(buf, sizeof(buf), "List Row Item %02d", i + 1);
    list_items[i] = XmStringCreateLocalized(buf);
  }
  Widget list_w = XtVaCreateManagedWidget(
      "listW", xmListWidgetClass, outer_bb,
      XmNx, 10, XmNy, 10, XmNwidth, 220,
      XmNitems, list_items, XmNitemCount, 16, XmNvisibleItemCount, 5, NULL);
  for (int i = 0; i < 16; ++i)
    XmStringFree(list_items[i]);

  char text_buf[1024];
  size_t off = 0;
  for (int i = 0; i < 20; ++i) {
    off += (size_t)snprintf(text_buf + off, sizeof(text_buf) - off,
                            "Scrollable text line %02d\n", i + 1);
  }
  Widget text_w = XtVaCreateManagedWidget(
      "textW", xmTextWidgetClass, outer_bb,
      XmNx, 250, XmNy, 10, XmNwidth, 240, XmNheight, 110,
      XmNeditMode, XmMULTI_LINE_EDIT, XmNrows, 4,
      XmNvalue, text_buf, NULL);

  XmTreeViewNode tv_nodes[16];
  char tv_labels[16][32];
  for (int i = 0; i < 16; ++i) {
    snprintf(tv_labels[i], sizeof(tv_labels[i]), "Tree Node %02d", i + 1);
    tv_nodes[i].id = (XtPointer)(uintptr_t)(i + 1);
    tv_nodes[i].parent_id = NULL;
    tv_nodes[i].label = tv_labels[i];
    tv_nodes[i].has_children = False;
    tv_nodes[i].expanded = False;
  }
  Widget tv_w = XtVaCreateManagedWidget(
      "tvW", xmTreeViewWidgetClass, outer_bb,
      XmNx, 510, XmNy, 10, XmNwidth, 240, XmNheight, 110, NULL);
  XmTreeViewSetNodes(tv_w, tv_nodes, 16);

  static const XmDataGridColumn dg_cols[] = {
    {(XtPointer)"c0", "Item", False, 120},
    {(XtPointer)"c1", "Status", False, 100}
  };
  const char *dg_vals[16][2];
  char dg_cell0[16][32];
  XmDataGridRow dg_rows[16];
  for (int i = 0; i < 16; ++i) {
    snprintf(dg_cell0[i], sizeof(dg_cell0[i]), "Grid Row %02d", i + 1);
    dg_vals[i][0] = dg_cell0[i];
    dg_vals[i][1] = "Ready";
    dg_rows[i].id = (XtPointer)(uintptr_t)(i + 1);
    dg_rows[i].values = dg_vals[i];
  }
  Widget dg_w = XtVaCreateManagedWidget(
      "dgW", xmDataGridWidgetClass, outer_bb,
      XmNx, 770, XmNy, 10, XmNwidth, 260, XmNheight, 120, NULL);
  CHECK(XmDataGridSetModel(dg_w, dg_cols, 2, dg_rows, 16));

  XtRealizeWidget(shell);
  NativeWaitForViewable(root_form);
  pump(app, dpy);

  /* Part 1: Verify hover feedback in modern-light, modern-dark, and classic */
  int combo_ax = (int)XtWidth(combo) - 12;
  int combo_ay = (int)XtHeight(combo) / 2;

  for (int p = 0; p < 3; ++p) {
    XmAppearanceProfile prof =
        (p == 0) ? XmAPPEARANCE_MODERN_LIGHT :
        (p == 1) ? XmAPPEARANCE_MODERN_DARK :
                   XmAPPEARANCE_CLASSIC;
    Boolean expect_hover = (prof != XmAPPEARANCE_CLASSIC);
    CHECK(XmApplyAppearanceProfile(dpy, prof));
    pump(app, dpy);

    verify_hover_cycle(app, dpy, push_btn, 20, 12, expect_hover);
    verify_hover_cycle(app, dpy, toggle_btn, 20, 12, expect_hover);
    verify_hover_cycle(app, dpy, combo, combo_ax, combo_ay, expect_hover);
    verify_hover_cycle(app, dpy, nb_tab, 16, 10, expect_hover);
    verify_hover_cycle(app, dpy, list_w, 30, 32, expect_hover);
    verify_hover_cycle(app, dpy, tv_w, 30, 36, expect_hover);
    verify_hover_cycle(app, dpy, dg_w, 40, 48, expect_hover);
  }

  /* Restore modern-light for Part 2 & Part 3 */
  CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_MODERN_LIGHT));
  pump(app, dpy);

  /* Part 2: Focus-visible (XmNfocusHighlightPolicy) */
  Widget xm_dpy = XmGetXmDisplay(dpy);
  unsigned char policy = 99;
  XtVaGetValues(xm_dpy, XmNfocusHighlightPolicy, &policy, NULL);
  CHECK(policy == XmFOCUS_HIGHLIGHT_ALWAYS);

  /* Under ALWAYS, pointer click on focus_btn draws the focus ring */
  CHECK(XmProcessTraversal(push_btn, XmTRAVERSE_CURRENT));
  pump(app, dpy);
  CHECK(XmGetFocusWidget(push_btn) == push_btn);
  uint64_t unfocused_hash = widget_pixel_hash(focus_btn, 0, 0, 0, 0);

  dispatch_button(focus_btn, Button1, 20, 12);
  pump(app, dpy);
  CHECK(XmGetFocusWidget(focus_btn) == focus_btn);
  uint64_t always_clicked_hash = widget_pixel_hash(focus_btn, 0, 0, 0, 0);
  CHECK(always_clicked_hash != unfocused_hash);

  /* Switch to XmFOCUS_HIGHLIGHT_KEYBOARD */
  XtVaSetValues(xm_dpy, XmNfocusHighlightPolicy, XmFOCUS_HIGHLIGHT_KEYBOARD, NULL);
  XtVaGetValues(xm_dpy, XmNfocusHighlightPolicy, &policy, NULL);
  CHECK(policy == XmFOCUS_HIGHLIGHT_KEYBOARD);

  /* Traverse back to push_btn via keyboard first */
  dispatch_key(focus_btn, XK_Tab);
  CHECK(XmProcessTraversal(push_btn, XmTRAVERSE_CURRENT));
  pump(app, dpy);
  CHECK(XmGetFocusWidget(push_btn) == push_btn);
  unfocused_hash = widget_pixel_hash(focus_btn, 0, 0, 0, 0);

  /* Pointer click on focus_btn gives it focus WITHOUT drawing focus ring */
  dispatch_button(focus_btn, Button1, 20, 12);
  pump(app, dpy);
  CHECK(XmGetFocusWidget(focus_btn) == focus_btn);
  uint64_t kb_policy_clicked_hash = widget_pixel_hash(focus_btn, 0, 0, 0, 0);
  CHECK(kb_policy_clicked_hash == unfocused_hash);

  /* Pressing Tab on push_btn traverses to focus_btn WITH visible focus ring */
  dispatch_button(push_btn, Button1, 20, 12);
  pump(app, dpy);
  CHECK(XmGetFocusWidget(push_btn) == push_btn);
  dispatch_key(push_btn, XK_Tab);
  pump(app, dpy);
  CHECK(XmGetFocusWidget(focus_btn) == focus_btn);
  uint64_t kb_traversed_hash = widget_pixel_hash(focus_btn, 0, 0, 0, 0);
  CHECK(kb_traversed_hash != unfocused_hash);

  /* Part 3: Standardized wheel scrolling (Button4/Button5) + nested boundary forwarding */
  Widget vbar = NULL;
  XtVaGetValues(outer_sw, XmNverticalScrollBar, &vbar, NULL);
  CHECK(vbar != NULL);

  /* 3a. XmList wheel scrolling + boundary forwarding */
  int vval = 0, vsz = 0, vinc = 0, vpg = 0;
  XmScrollBarGetValues(vbar, &vval, &vsz, &vinc, &vpg);
  XmScrollBarSetValues(vbar, 0, vsz, vinc, vpg, True);
  pump(app, dpy);
  int top_item = 0;
  XtVaGetValues(list_w, XmNtopItemPosition, &top_item, NULL);
  CHECK(top_item == 1);
  dispatch_button(list_w, Button5, 20, 20);
  pump(app, dpy);
  XtVaGetValues(list_w, XmNtopItemPosition, &top_item, NULL);
  CHECK(top_item > 1);
  XmScrollBarGetValues(vbar, &vval, &vsz, &vinc, &vpg);
  CHECK(vval == 0);
  for (int i = 0; i < 12; ++i)
    dispatch_button(list_w, Button5, 20, 20);
  pump(app, dpy);
  XmScrollBarGetValues(vbar, &vval, &vsz, &vinc, &vpg);
  CHECK(vval > 0);
  for (int i = 0; i < 12; ++i)
    dispatch_button(list_w, Button4, 20, 20);
  pump(app, dpy);
  XtVaGetValues(list_w, XmNtopItemPosition, &top_item, NULL);
  CHECK(top_item == 1);

  /* 3b. XmText wheel scrolling + boundary forwarding */
  XmScrollBarGetValues(vbar, &vval, &vsz, &vinc, &vpg);
  XmScrollBarSetValues(vbar, 0, vsz, vinc, vpg, True);
  XmTextSetTopCharacter(text_w, 0);
  pump(app, dpy);
  CHECK(XmTextGetTopCharacter(text_w) == 0);
  dispatch_button(text_w, Button5, 20, 20);
  pump(app, dpy);
  CHECK(XmTextGetTopCharacter(text_w) > 0);
  for (int i = 0; i < 15; ++i)
    dispatch_button(text_w, Button5, 20, 20);
  pump(app, dpy);
  XmScrollBarGetValues(vbar, &vval, &vsz, &vinc, &vpg);
  CHECK(vval > 0);

  /* 3c. XmTreeView multi-line wheel scrolling + boundary forwarding */
  XmScrollBarGetValues(vbar, &vval, &vsz, &vinc, &vpg);
  XmScrollBarSetValues(vbar, 0, vsz, vinc, vpg, True);
  XmTreeViewSetTopRow(tv_w, 0);
  pump(app, dpy);
  CHECK(XmTreeViewGetTopRow(tv_w) == 0);
  dispatch_button(tv_w, Button5, 20, 20);
  pump(app, dpy);
  CHECK(XmTreeViewGetTopRow(tv_w) >= 2);
  XmScrollBarGetValues(vbar, &vval, &vsz, &vinc, &vpg);
  CHECK(vval == 0);
  for (int i = 0; i < 12; ++i)
    dispatch_button(tv_w, Button5, 20, 20);
  pump(app, dpy);
  XmScrollBarGetValues(vbar, &vval, &vsz, &vinc, &vpg);
  CHECK(vval > 0);

  /* 3d. XmDataGrid multi-line wheel scrolling + boundary forwarding */
  XmScrollBarGetValues(vbar, &vval, &vsz, &vinc, &vpg);
  XmScrollBarSetValues(vbar, 0, vsz, vinc, vpg, True);
  XmDataGridSetTopRow(dg_w, 0);
  pump(app, dpy);
  CHECK(XmDataGridGetTopRow(dg_w) == 0);
  dispatch_button(dg_w, Button5, 20, 50);
  pump(app, dpy);
  CHECK(XmDataGridGetTopRow(dg_w) >= 2);
  XmScrollBarGetValues(vbar, &vval, &vsz, &vinc, &vpg);
  CHECK(vval == 0);
  for (int i = 0; i < 12; ++i)
    dispatch_button(dg_w, Button5, 20, 50);
  pump(app, dpy);
  XmScrollBarGetValues(vbar, &vval, &vsz, &vinc, &vpg);
  CHECK(vval > 0);

  XtDestroyWidget(shell);
  XtCloseDisplay(dpy);
  XtDestroyApplicationContext(app);
  printf("PASS WP-49 hover affordances, focus-visible policy, and wheel scrolling\n");
  return 0;
}
