/* Live semantic tree and cache regression through the real AT-SPI transport. */
#include <Xm/Accessible.h>
#include <Xm/DrawingA.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

typedef struct {
  XtAppContext app;
  Widget shell;
  XmAccessibility *bridge;
  XmAccessibleId canvas, parent;
  unsigned int attempts;
} Fixture;
static Boolean query(XtPointer data, XmAccessibleInfo *info) {
  (void)data;
  info->role = XmACCESSIBLE_LABEL;
  info->name = XtNewString("Semantic item");
  return True;
}
static void command(XtPointer data, int *fd, XtInputId *input) {
  Fixture *f = data;
  char line[64];
  unsigned int count, i;
  XmAccessibleProvider provider = {0};
  (void)fd; (void)input;
  if (!fgets(line, sizeof(line), stdin) || !strncmp(line, "quit", 4)) {
    XmAccessibilityDestroy(f->bridge);
    XtDestroyWidget(f->shell);
    exit(0);
  }
  if (!strncmp(line, "clear", 5)) {
    XmAccessibilityRemove(f->bridge, f->parent);
    f->parent = 0;
    puts("{\"cleared\":true}"); fflush(stdout); return;
  }
  count = (unsigned int)strtoul(line, NULL, 10);
  if (f->parent || (count != 1000 && count != 10000)) exit(3);
  provider.query = query;
  f->parent = XmAccessibilityAdd(f->bridge, f->canvas, &provider, NULL);
  for (i = 0; i < count; ++i)
    if (!XmAccessibilityAdd(f->bridge, f->parent, &provider, NULL)) exit(4);
  printf("{\"parent\":%u,\"count\":%u}\n", f->parent, count); fflush(stdout);
}
static void connected(XtPointer data, XtIntervalId *timer) {
  Fixture *f = data;
  (void)timer;
  if (XmAccessibilityGetStatus(f->bridge) == XmACCESSIBILITY_CONNECTED) {
    printf("{\"bus\":\"%s\",\"canvas\":%u}\n", XmAccessibilityBusName(f->bridge), f->canvas);
    fflush(stdout);
    XtAppAddInput(f->app, STDIN_FILENO, (XtPointer)XtInputReadMask, command, f);
  } else if (++f->attempts < 100) XtAppAddTimeOut(f->app, 100, connected, f);
  else exit(2);
}
int main(int argc, char **argv) {
  Fixture f = {0};
  Widget canvas;
  f.shell = XtVaAppInitialize(&f.app, "SemanticTreeBus", NULL, 0, &argc, argv, NULL, NULL);
  canvas = XtVaCreateManagedWidget("canvas", xmDrawingAreaWidgetClass, f.shell,
                                   XmNwidth, 120, XmNheight, 80, NULL);
  XtRealizeWidget(f.shell);
  f.bridge = XmAccessibilityCreate(f.shell, "Semantic tree bus fixture");
  if (!f.bridge) return 1;
  f.canvas = XmAccessibilityWidget(f.bridge, canvas);
  XmAccessibilityConnect(f.bridge);
  XtAppAddTimeOut(f.app, 100, connected, &f);
  XtAppMainLoop(f.app);
  return 1;
}
