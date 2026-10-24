/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Ancestor geometry changes must publish descendant SHOWING transitions. */
#include <Xm/Accessible.h>
#include <Xm/DrawingA.h>
#include <Xm/Label.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  XtAppContext app;
  Widget shell, clip, work, target;
  XmAccessibility *bridge;
  XmAccessibleId target_id;
  unsigned int attempts;
} Fixture;
static void require(int condition, const char *message) {
  if (!condition) { fprintf(stderr, "%s\n", message); exit(3); }
}
static void geometry(Fixture *f, int expected_x) {
  Window child;
  int x = 0, y = 0;
  XWindowAttributes attributes;
  XSync(XtDisplay(f->shell), False);
  require(XTranslateCoordinates(XtDisplay(f->shell), XtWindow(f->target),
      XtWindow(f->clip), 0, 0, &x, &y, &child), "target translation unavailable");
  require(x == expected_x && y == 20, "actual ancestor movement differs from fixture");
  require(XGetWindowAttributes(XtDisplay(f->target), XtWindow(f->target),
                               &attributes) && attributes.map_state == IsViewable,
          "target must remain natively viewable while clipped");
}
static void command(XtPointer data, int *fd, XtInputId *input) {
  Fixture *f = data;
  char line[64];
  int reveal;
  (void)fd; (void)input;
  if (!fgets(line, sizeof(line), stdin) || !strcmp(line, "stop\n")) {
    XtAppSetExitFlag(f->app);
    return;
  }
  require(!strcmp(line, "reveal\n") || !strcmp(line, "hide\n"), "unknown command");
  reveal = !strcmp(line, "reveal\n");
  /* Establish fixture-owned clipping geometry directly, independently of
   * DrawingArea geometry-request admission. Only the ancestor moves; native
   * ConfigureNotify must drive the semantic update without an explicit Changed. */
  XtMoveWidget(f->work, reveal ? -150 : 0, 0);
  geometry(f, reveal ? 30 : 180);
  printf("{\"done\":true,\"target_x_in_clip\":%d}\n", reveal ? 30 : 180);
  fflush(stdout);
}
static void connected(XtPointer data, XtIntervalId *timer) {
  Fixture *f = data;
  (void)timer;
  if (XmAccessibilityGetStatus(f->bridge) == XmACCESSIBILITY_CONNECTED) {
    geometry(f, 180);
    printf("{\"bus\":\"%s\",\"target\":%u,\"pid\":%ld,"
           "\"application\":\"Native clipping events\",\"frame\":\"Clipping event fixture\"}\n",
           XmAccessibilityBusName(f->bridge), f->target_id, (long)getpid());
    fflush(stdout);
    XtAppAddInput(f->app, STDIN_FILENO, (XtPointer)XtInputReadMask, command, f);
  } else if (++f->attempts < 100)
    XtAppAddTimeOut(f->app, 100, connected, f);
  else { fputs("private accessibility transport did not connect\n", stderr); exit(2); }
}
static Widget area(const char *name, Widget parent, int x, int y, int width, int height) {
  return XtVaCreateManagedWidget(name, xmDrawingAreaWidgetClass, parent,
      XmNx, x, XmNy, y, XmNwidth, width, XmNheight, height,
      XmNborderWidth, 0, XmNmarginWidth, 0, XmNmarginHeight, 0,
      XmNresizePolicy, XmRESIZE_NONE, NULL);
}
int main(int argc, char **argv) {
  Fixture f = {0};
  Widget root;
  Display *display;
  require(getenv("AT_SPI_BUS_ADDRESS") && *getenv("AT_SPI_BUS_ADDRESS"),
          "an explicit private AT_SPI_BUS_ADDRESS is required");
  f.shell = XtVaAppInitialize(&f.app, "NativeClippingEvents", NULL, 0,
      &argc, argv, NULL, XmNtitle, "Clipping event fixture", NULL);
  display = XtDisplay(f.shell);
  root = area("root", f.shell, 0, 0, 320, 160);
  f.clip = area("clip", root, 20, 20, 100, 100);
  f.work = area("work", f.clip, 0, 0, 300, 100);
  f.target = XtVaCreateManagedWidget("target", xmLabelWidgetClass, f.work,
      XmNx, 180, XmNy, 20, XmNwidth, 40, XmNheight, 20,
      XmNborderWidth, 0, XmNrecomputeSize, False, NULL);
  XtRealizeWidget(f.shell);
  f.bridge = XmAccessibilityCreate(f.shell, "Native clipping events");
  require(f.bridge != NULL, "accessibility bridge creation failed");
  f.target_id = XmAccessibilityWidget(f.bridge, f.target);
  require(f.target_id != XmACCESSIBLE_NONE, "target semantic identity missing");
  XmAccessibilityConnect(f.bridge);
  XtAppAddTimeOut(f.app, 100, connected, &f);
  XtAppMainLoop(f.app);
  XmAccessibilityDestroy(f.bridge);
  XtDestroyWidget(f.shell);
  XtCloseDisplay(display);
  XtDestroyApplicationContext(f.app);
  return 0;
}
