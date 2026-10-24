/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Native SHOWING follows the common intersection of actual ancestor windows. */
#include <Xm/Accessible.h>
#include <Xm/DrawingA.h>
#include <Xm/Disclosure.h>
#include <Xm/Label.h>
#include <Xm/LabelG.h>
#include <Xm/ScrolledW.h>
#include <X11/IntrinsicP.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct Rect { int x, y, width, height; };
static unsigned int assertions, queries;
static void require(int value, const char *message) {
  ++assertions;
  if (!value) { fprintf(stderr, "%s\n", message); exit(1); }
}
static void drain(XtAppContext app, Display *display) {
  XSync(display, False);
  while (XtAppPending(app)) XtAppProcessEvent(app, XtIMAll);
  XSync(display, False);
}
static struct Rect rectangle(Widget widget) {
  struct Rect result;
  Position x = 0, y = 0;
  Dimension width = 0, height = 0;
  XtVaGetValues(widget, XmNx, &x, XmNy, &y, XmNwidth, &width,
                XmNheight, &height, NULL);
  result.width = width; result.height = height;
  if (XtIsWidget(widget)) {
    Window root, child;
    int local_x, local_y;
    unsigned int actual_width, actual_height, border, depth;
    require(XGetGeometry(XtDisplay(widget), XtWindow(widget), &root,
                         &local_x, &local_y, &actual_width, &actual_height,
                         &border, &depth), "native window geometry unavailable");
    require(actual_width == width && actual_height == height,
            "Xt and X dimensions disagree after event drain");
    require(XTranslateCoordinates(XtDisplay(widget), XtWindow(widget), root,
                                  0, 0, &result.x, &result.y, &child),
            "native screen translation failed");
  } else {
    struct Rect parent = rectangle(XtParent(widget));
    result.x = parent.x + x; result.y = parent.y + y;
  }
  return result;
}
static int intersects(struct Rect a, struct Rect b) {
  return a.x < b.x + b.width && b.x < a.x + a.width &&
         a.y < b.y + b.height && b.y < a.y + a.height;
}
static void relative(Widget child, Widget parent, int x, int y,
                     int width, int height) {
  struct Rect a = rectangle(child), b = rectangle(parent);
  if (a.x != b.x + x || a.y != b.y + y ||
      a.width != width || a.height != height) {
    fprintf(stderr,
        "%s relative to %s: actual (%d,%d %dx%d), expected (%d,%d %dx%d); "
        "screen child (%d,%d), parent (%d,%d)\n",
        XtName(child), XtName(parent), a.x - b.x, a.y - b.y,
        a.width, a.height, x, y, width, height,
        a.x, a.y, b.x, b.y);
  }
  require(a.x == b.x + x && a.y == b.y + y &&
              a.width == width && a.height == height,
          "fixture geometry was changed by native layout");
}
static void viewable(Widget widget, int expected) {
  XWindowAttributes attributes;
  require(XGetWindowAttributes(XtDisplayOfObject(widget),
                               XtWindowOfObject(widget), &attributes),
          "native map state unavailable");
  require((attributes.map_state == IsViewable) == expected,
          "unexpected actual native map state");
}
static void state(XmAccessibility *bridge, Widget widget, int showing,
                   const char *message) {
  XmAccessibleInfo info;
  struct Rect actual = rectangle(widget);
  XmAccessibleId id = XmAccessibilityWidget(bridge, widget);
  require(id && XmAccessibilityQuery(bridge, id, &info),
          "native accessible must remain queryable");
  ++queries;
  require((info.states & XmACCESSIBLE_STATE(30)) != 0,
          "managed native object must remain VISIBLE when clipped");
  if (!!(info.states & XmACCESSIBLE_STATE(25)) != showing) {
    Widget current;
    for (current = widget; current; current = XtParent(current)) {
      Position x, y;
      Dimension width, height, border;
      XtVaGetValues(current, XmNx, &x, XmNy, &y, XmNwidth, &width,
                    XmNheight, &height, XmNborderWidth, &border, NULL);
      fprintf(stderr, "%s x=%d y=%d width=%u height=%u border=%u shell=%d managed=%d\n",
              XtName(current), x, y, width, height, border,
              XtIsShell(current), XtIsManaged(current));
      if (XtIsShell(current)) break;
    }
  }
  require(!!(info.states & XmACCESSIBLE_STATE(25)) == showing, message);
  require(info.has_extents && info.screen_x == actual.x &&
              info.screen_y == actual.y && info.width == actual.width &&
              info.height == actual.height,
          "semantic extents must retain actual unclipped native geometry");
  XmAccessibleInfoClear(&info);
}
static Widget area(const char *name, Widget parent, int x, int y,
                    int width, int height) {
  return XtVaCreateManagedWidget(name, xmDrawingAreaWidgetClass, parent,
      XmNx, x, XmNy, y, XmNwidth, width, XmNheight, height,
      XmNborderWidth, 0, XmNmarginWidth, 0, XmNmarginHeight, 0,
      XmNresizePolicy, XmRESIZE_NONE, NULL);
}
static Widget label(const char *name, Widget parent, int x, int y,
                     int width, int height) {
  return XtVaCreateManagedWidget(name, xmLabelWidgetClass, parent,
      XmNx, x, XmNy, y, XmNwidth, width, XmNheight, height,
      XmNborderWidth, 0, XmNrecomputeSize, False, NULL);
}
int main(int argc, char **argv) {
  XtAppContext app;
  Widget shell, root, clip, target, outer, middle, nested;
  Widget gadget_parent, gadget, popup, popup_label, scroll, work, reveal, viewport;
  Widget section, header;
  XmString section_label;
  unsigned int round;
  XmAccessibility *bridge;
  struct Rect before, after;
  require(setlocale(LC_ALL, "") != NULL, "locale unavailable");
  XtSetLanguageProc(NULL, NULL, NULL);
  shell = XtVaAppInitialize(&app, "NativeClipping", NULL, 0, &argc, argv,
                            NULL, XmNtitle, "Native accessibility clipping", NULL);
  root = area("root", shell, 0, 0, 640, 420);
  clip = area("clip", root, 20, 20, 100, 100);
  target = label("target", clip, 120, 20, 40, 20);
  outer = area("outer", root, 200, 20, 100, 100);
  middle = area("middle", outer, 120, 0, 100, 100);
  nested = label("nested", middle, -70, 20, 100, 20);
  gadget_parent = area("gadgetParent", root, 20, 180, 100, 100);
  gadget = XtVaCreateManagedWidget("gadget", xmLabelGadgetClass, gadget_parent,
      XmNx, 10, XmNy, 10, XmNwidth, 40, XmNheight, 20,
      XmNborderWidth, 0, XmNrecomputeSize, False, NULL);
  popup = XtVaCreatePopupShell("popup", overrideShellWidgetClass, clip,
      XtNx, 400, XtNy, 40, XtNwidth, 100, XtNheight, 60,
      XtNborderWidth, 0, NULL);
  popup_label = label("popupLabel", popup, 0, 0, 100, 60);
  scroll = XtVaCreateManagedWidget("scroll", xmScrolledWindowWidgetClass, root,
      XmNx, 400, XmNy, 180, XmNwidth, 140, XmNheight, 110,
      XmNborderWidth, 0, XmNscrollingPolicy, XmAUTOMATIC, NULL);
  work = area("work", scroll, 0, 0, 400, 300);
  reveal = label("reveal", work, 220, 180, 40, 20);
  XtVaSetValues(scroll, XmNworkWindow, work, NULL);
  section_label = XmStringCreateLocalized("Section");
  section = XtVaCreateManagedWidget("section", xmDisclosureWidgetClass, root,
      XmNx, 180, XmNy, 180, XmNwidth, 180, XmNheight, 120,
      XmNlabelString, section_label, NULL);
  XmStringFree(section_label);
  header = XmDisclosureGetHeader(section);
  require(header != NULL, "disclosure header missing");
  XtRealizeWidget(shell);
  XtPopup(popup, XtGrabNone);
  drain(app, XtDisplay(shell));
  bridge = XmAccessibilityCreate(shell, "Native clipping");
  require(bridge != NULL, "accessibility bridge creation failed");
  require(XmAccessibilityAttachShell(bridge, popup), "popup attachment failed");
  XmAccessibilityRefresh(bridge);

  relative(clip, root, 20, 20, 100, 100);
  relative(target, clip, 120, 20, 40, 20);
  viewable(target, 1);
  state(bridge, target, 0, "mapped wholly clipped child must not be SHOWING");
  /* The fixture deliberately establishes clipping geometry, rather than
   * asking the DrawingArea geometry manager to admit an out-of-bounds move. */
  XtMoveWidget(target, 80, 20);
  drain(app, XtDisplay(shell));
  relative(target, clip, 80, 20, 40, 20);
  state(bridge, target, 1, "partially intersecting child must be SHOWING");
  XtMoveWidget(target, 100, 20);
  drain(app, XtDisplay(shell));
  relative(target, clip, 100, 20, 40, 20);
  state(bridge, target, 0, "right edge contact has no visible area");
  XtMoveWidget(target, -40, 20);
  drain(app, XtDisplay(shell));
  relative(target, clip, -40, 20, 40, 20);
  state(bridge, target, 0, "left edge contact has no visible area");
  XtMoveWidget(target, 10, 20);
  drain(app, XtDisplay(shell));
  state(bridge, target, 1, "moving into the viewport must restore SHOWING");

  relative(middle, outer, 120, 0, 100, 100);
  relative(nested, middle, -70, 20, 100, 20);
  viewable(nested, 1);
  require(intersects(rectangle(nested), rectangle(outer)) &&
              intersects(rectangle(nested), rectangle(middle)) &&
              !intersects(rectangle(outer), rectangle(middle)),
          "nested fixture must intersect each clip separately but not jointly");
  state(bridge, nested, 0, "ancestor clips require one common intersection");
  XtMoveWidget(middle, 80, 0);
  drain(app, XtDisplay(shell));
  relative(middle, outer, 80, 0, 100, 100);
  state(bridge, nested, 1, "nested common intersection must restore SHOWING");

  relative(gadget, gadget_parent, 10, 10, 40, 20);
  state(bridge, gadget, 1, "managed gadget must initially be SHOWING");
  XtSetMappedWhenManaged(gadget_parent, False);
  drain(app, XtDisplay(shell));
  require(XtIsManaged(gadget), "gadget must remain managed");
  viewable(gadget_parent, 0);
  state(bridge, gadget, 0, "gadget must obey its native ancestor map state");
  XtSetMappedWhenManaged(gadget_parent, True);
  drain(app, XtDisplay(shell));
  state(bridge, gadget, 1, "remapped gadget ancestor must restore SHOWING");

  require(!intersects(rectangle(popup_label), rectangle(clip)),
          "popup must lie outside its logical owner's clip");
  viewable(popup_label, 1);
  state(bridge, popup_label, 1, "popup must use its own native window ancestry");
  XtSetMappedWhenManaged(clip, False);
  drain(app, XtDisplay(shell));
  viewable(clip, 0);
  viewable(popup_label, 1);
  state(bridge, popup_label, 1, "unmapped logical owner must not clip a popup");
  XtSetMappedWhenManaged(clip, True);
  drain(app, XtDisplay(shell));

  viewport = XtParent(work);
  require(viewport != scroll && XtParent(viewport) == scroll,
          "automatic scrolling must create an actual clipping viewport");
  before = rectangle(work);
  require(!intersects(rectangle(reveal), rectangle(viewport)),
          "scroll target must initially be wholly outside the viewport");
  viewable(reveal, 1);
  state(bridge, reveal, 0, "mapped scrolled-out target must not be SHOWING");
  XmScrollVisible(scroll, reveal, 0, 0);
  drain(app, XtDisplay(shell));
  after = rectangle(work);
  require(after.x != before.x && after.y != before.y &&
              intersects(rectangle(reveal), rectangle(viewport)),
          "native reveal must actually scroll both axes into the viewport");
  state(bridge, reveal, 1, "native scroll reveal must restore SHOWING");

  /* Disclosure owns its supplied string while the header's label getter
   * returns a copy. Repeated semantic reads must preserve both contracts. */
  for (round = 0; round < 10; ++round) {
    Widget named[] = {section, header};
    unsigned int index;
    for (index = 0; index < XtNumber(named); ++index) {
      XmAccessibleInfo info;
      XmAccessibleId id = XmAccessibilityWidget(bridge, named[index]);
      require(id && XmAccessibilityQuery(bridge, id, &info),
              "disclosure and header must remain queryable");
      ++queries;
      require(info.name && !strcmp(info.name, "Section"),
              "repeated disclosure/header query changed the owned label");
      XmAccessibleInfoClear(&info);
    }
  }

  printf("{\"assertions\":%u,\"native_queries\":%u}\n", assertions, queries);
  XmAccessibilityDestroy(bridge);
  XtDestroyWidget(popup);
  {
    Display *display = XtDisplay(shell);
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
  }
  XtDestroyApplicationContext(app);
  return 0;
}
