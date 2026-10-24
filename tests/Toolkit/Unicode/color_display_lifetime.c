/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* One display's ColorObj lookup and teardown cannot depend on a peer. */
#include <Xm/ColorObjP.h>
#include <Xm/Display.h>
#include <X11/Shell.h>
#include <X11/StringDefs.h>
#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>
#include <string.h>

static unsigned int assertions;
static void require(int condition, const char *message)
{
  ++assertions;
  if (!condition) {
    fprintf(stderr, "%s\n", message);
    exit(1);
  }
}
static void destroyed(Widget widget, XtPointer data, XtPointer call)
{
  (void)widget;
  (void)call;
  ++*(unsigned int *)data;
}
static struct {
  Display *display;
  Widget *created;
} watched[2];

/* Xt 1.3.1 does not report parentless XtAppCreateShell objects through either
 * XtNcreateHook or XtNshells. Observe the public creation call in this test
 * executable, forwarding every argument and recording its genuine result. */
Widget XtAppCreateShell(const char *name, const char *class_name,
                       WidgetClass widget_class, Display *display,
                       ArgList args, Cardinal count)
{
  typedef Widget (*CreateShell)(const char *, const char *, WidgetClass,
                               Display *, ArgList, Cardinal);
  static CreateShell original;
  Widget result;
  unsigned int index;
  if (!original) {
    void *symbol = dlsym(RTLD_NEXT, "XtAppCreateShell");
    _Static_assert(sizeof(symbol) == sizeof(original), "native function pointer width");
    require(symbol != NULL, "resolve native XtAppCreateShell");
    memcpy(&original, &symbol, sizeof(original));
  }
  result = original(name, class_name, widget_class, display, args, count);
  if (result && XmIsColorObj(result)) {
    for (index = 0; index < 2; ++index) if (watched[index].display == display) {
      require(watched[index].created && !*watched[index].created,
              "display created more than one ColorObj");
      *watched[index].created = result;
    }
  }
  return result;
}
static void lookup(Widget object)
{
  Boolean found;
  found = XmeGetColorObjData(XtScreen(object), NULL, NULL, 0,
                            NULL, NULL, NULL, NULL, NULL);
  require(found, "live display ColorObj lookup depended on a closed peer");
}
static void palette(Widget object, int identity)
{
  XmColorObj color = (XmColorObj)object;
  int screen, entry;
  /* Seed the existing palette model so this ownership test does not depend on
   * an optional CDE color server. Pixels are actual black/white screen pixels. */
  if (!color->color_obj.colors)
    color->color_obj.colors = (Colors *)XtCalloc(color->color_obj.numScreens,
                                                sizeof(Colors));
  if (!color->color_obj.colorUse)
    color->color_obj.colorUse = (int *)XtCalloc(color->color_obj.numScreens,
                                               sizeof(int));
  for (screen = 0; screen < color->color_obj.numScreens; ++screen) {
    Screen *native = ScreenOfDisplay(XtDisplay(object), screen);
    Pixel foreground = identity ? WhitePixelOfScreen(native) : BlackPixelOfScreen(native);
    Pixel background = identity ? BlackPixelOfScreen(native) : WhitePixelOfScreen(native);
    color->color_obj.colorUse[screen] = identity ? XmCO_HIGH_COLOR : XmCO_MEDIUM_COLOR;
    for (entry = 0; entry < XmCO_NUM_COLORS; ++entry) {
      XmPixelSet *value = &color->color_obj.colors[screen][entry];
      value->fg = foreground; value->bg = background;
      value->ts = foreground; value->bs = background; value->sc = foreground;
    }
  }
  color->color_obj.colorIsRunning = True;
  color->color_obj.useColorObj = True;
  color->color_obj.useMask = identity != 0;
  color->color_obj.useMultiColorIcons = identity == 0;
  color->color_obj.useIconFileCache = identity != 0;
}
static void icon_policy(Widget object)
{
  XmColorObj color = (XmColorObj)object;
  Boolean mask, multi, cache;
  require(XmeGetIconControlInfo(XtScreen(object), &mask, &multi, &cache) &&
              mask == color->color_obj.useMask &&
              multi == color->color_obj.useMultiColorIcons &&
              cache == color->color_obj.useIconFileCache,
          "screen icon policy came from another display");
}
static void default_policy(Widget object)
{
  XmColorObj color = (XmColorObj)object;
  XmPixelSet values[XmCO_NUM_COLORS];
  int usage = -1, screen = XScreenNumberOfScreen(XtScreen(object));
  require(XmeUseColorObj(), "first live default ColorObj became unavailable");
  require(XmeGetPixelData(screen, &usage, values, NULL, NULL, NULL, NULL) &&
              usage == color->color_obj.colorUse[screen] &&
              values[0].bg == color->color_obj.colors[screen][0].bg,
          "default color query did not use the first live display");
  require(!XmeGetPixelData(-1, &usage, values, NULL, NULL, NULL, NULL) &&
              !XmeGetPixelData(ScreenCount(XtDisplay(object)), &usage, values,
                               NULL, NULL, NULL, NULL),
          "default color query accepted an out-of-range screen");
  require(!XmeGetColorObjData(XtScreen(object), NULL, values,
                              XmCO_NUM_COLORS + 1, NULL, NULL, NULL, NULL, NULL),
          "color query exceeded the palette extent");
}

static void close_order(int victim)
{
  XtAppContext app = XtCreateApplicationContext();
  Display *displays[2];
  Widget shells[2], owners[2], colors[2] = {NULL, NULL};
  unsigned int destroyed_count[2] = {0, 0};
  unsigned int owner_destroyed[2] = {0, 0};
  int argc = 1, index, survivor = 1 - victim;
  char name[] = "color-display-lifetime";
  char *argv[] = {name, NULL};

  for (index = 0; index < 2; ++index) {
    displays[index] = XtOpenDisplay(app, NULL, name, "ColorDisplayLifetime",
                                    NULL, 0, &argc, argv);
    require(displays[index] != NULL, "open independent display connection");
    watched[index].display = displays[index];
    watched[index].created = &colors[index];
    shells[index] = XtVaAppCreateShell(name, "ColorDisplayLifetime",
        applicationShellWidgetClass, displays[index], NULL);
    owners[index] = XmGetXmDisplay(displays[index]);
    require(owners[index] != NULL, "create public XmDisplay owner");
    XtAddCallback(owners[index], XmNdestroyCallback, destroyed,
                  &owner_destroyed[index]);
    require(colors[index] != NULL, "display did not create its ColorObj");
    XtAddCallback(colors[index], XmNdestroyCallback, destroyed,
                  &destroyed_count[index]);
    palette(colors[index], index);
    lookup(colors[index]);
  }
  require(displays[0] != displays[1] && colors[0] != colors[1],
          "display connections must own distinct ColorObjs");
  icon_policy(colors[0]); icon_policy(colors[1]);
  default_policy(colors[0]);

  /* XtCloseDisplay requires shells to be destroyed while the display is live,
   * including the public toolkit XmDisplay owner and its XmScreen children. */
  XtDestroyWidget(shells[victim]);
  if (!owner_destroyed[victim]) XtDestroyWidget(owners[victim]);
  require(owner_destroyed[victim] == 1, "victim XmDisplay must be destroyed once");
  require(destroyed_count[victim] == 1 && destroyed_count[survivor] == 0,
          "destroying one XmDisplay must retire only its own ColorObj");
  XtCloseDisplay(displays[victim]);
  require(XmGetXmDisplay(displays[survivor]) == owners[survivor],
          "surviving display owner was replaced");
  lookup(colors[survivor]);
  icon_policy(colors[survivor]);
  default_policy(colors[survivor]);
  XtDestroyWidget(shells[survivor]);
  if (!owner_destroyed[survivor]) XtDestroyWidget(owners[survivor]);
  require(owner_destroyed[survivor] == 1, "surviving XmDisplay must be destroyed once");
  require(destroyed_count[0] == 1 && destroyed_count[1] == 1,
          "each XmDisplay must destroy its ColorObj exactly once");
  {
    XmPixelSet values[XmCO_NUM_COLORS];
    Boolean mask = False, multi = False, cache = False;
    require(!XmeUseColorObj() &&
                !XmeGetPixelData(0, NULL, values, NULL, NULL, NULL, NULL),
            "default color queries remained available after the last owner");
    require(!XmeGetIconControlInfo(DefaultScreenOfDisplay(displays[survivor]),
                                   &mask, &multi, &cache) && mask && multi && cache,
            "screen color query did not return unavailable defaults after owner destruction");
  }
  XtCloseDisplay(displays[survivor]);
  XtDestroyApplicationContext(app);
  for (index = 0; index < 2; ++index) {
    watched[index].display = NULL;
    watched[index].created = NULL;
  }
}
int main(void)
{
  XtToolkitInitialize();
  XtSetLanguageProc(NULL, NULL, NULL);
  close_order(0);
  close_order(1);
  printf("{\"assertions\":%u,\"close_orders\":2}\n", assertions);
  return 0;
}
