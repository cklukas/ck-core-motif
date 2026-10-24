/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Installed Xme default-table API; no private cache records or text selections. */
#include <Xm/XmP.h>
#include <Xm/Display.h>
#include <Xm/Label.h>
#include <X11/Shell.h>
#include <X11/Xutil.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>

#define CONNECTIONS 10
typedef struct {
  Display *display;
  Widget shell, native_display, label;
  XmFontList retained;
  unsigned int destroyed;
} Connection;
static unsigned int assertions, x_errors;
static void require(int condition, const char *message) {
  ++assertions;
  if (!condition) { fprintf(stderr, "%s\n", message); exit(1); }
}
static int x_error(Display *display, XErrorEvent *event) {
  char text[256];
  XGetErrorText(display, event->error_code, text, sizeof(text));
  fprintf(stderr, "X error: %s\n", text);
  ++x_errors;
  return 0;
}
static void retired(Widget widget, XtPointer data, XtPointer call) {
  Connection *connection = data;
  (void)widget; (void)call;
  ++connection->destroyed;
  connection->native_display = NULL;
}
static void drain(XtAppContext app, Display *display) {
  XSync(display, False);
  while (XtAppPending(app)) XtAppProcessEvent(app, XtIMAll);
  XSync(display, False);
  require(!x_errors, "default font lifecycle produced an X protocol error");
}
static void paint(Connection *connection) {
  Display *display = connection->display;
  XmString caption = XmStringCreateLocalized("Default table remains live");
  Dimension width, height;
  Window window = XtWindow(connection->label);
  XWindowAttributes attributes;
  XGCValues values;
  GC gc;
  XImage *image;
  int x, y, ink = 0;
  unsigned long white = WhitePixel(display, DefaultScreen(display));
  XRaiseWindow(display, XtWindow(connection->shell));
  XSync(display, False);
  XmStringExtent(connection->retained, caption, &width, &height);
  require(width > 0 && height > 0, "retained default table lost native metrics");
  require(XGetWindowAttributes(display, window, &attributes), "native Label window is absent");
  values.foreground = white;
  gc = XCreateGC(display, window, GCForeground, &values);
  require(gc != NULL, "native font drawing GC unavailable");
  XFillRectangle(display, window, gc, 0, 0, attributes.width, attributes.height);
  XSetForeground(display, gc, BlackPixel(display, DefaultScreen(display)));
  XmStringDraw(display, window, connection->retained, caption, gc, 2, 2,
               attributes.width - 4, XmALIGNMENT_BEGINNING,
               XmSTRING_DIRECTION_L_TO_R, NULL);
  XSync(display, False);
  image = XGetImage(display, window, 0, 0, attributes.width, attributes.height,
                    AllPlanes, ZPixmap);
  require(image != NULL, "native default-font image unavailable");
  for (y = 0; y < attributes.height; ++y)
    for (x = 0; x < attributes.width; ++x)
      if (XGetPixel(image, x, y) != white) ++ink;
  require(ink > 0, "surviving connection did not render native text");
  XDestroyImage(image);
  XFreeGC(display, gc);
  XmStringFree(caption);
  require(!x_errors, "native font readback produced an X protocol error");
}
static void open_connection(Connection *connection, XtAppContext app,
                            const char *display_name, int *argc, char **argv) {
  XmFontList borrowed, again, copy;
  XmString caption;
  unsigned int repeat;
  connection->display = XtOpenDisplay(app, display_name, "defaultCache",
      "DefaultFontCache", NULL, 0, argc, argv);
  require(connection->display != NULL, "independent native display connection unavailable");
  connection->shell = XtVaAppCreateShell("defaultCache", "DefaultFontCache",
      applicationShellWidgetClass, connection->display,
      XmNwidth, 360, XmNheight, 80, NULL);
  connection->native_display = XmGetXmDisplay(connection->display);
  require(connection->native_display != NULL, "native display owner unavailable");
  XtAddCallback(connection->native_display, XmNdestroyCallback, retired, connection);
  borrowed = XmeGetDefaultRenderTable(connection->shell, XmLABEL_FONTLIST);
  require(borrowed != NULL, "default table unavailable");
  for (repeat = 0; repeat < 6; ++repeat) {
    again = XmeGetDefaultRenderTable(connection->shell, XmLABEL_FONTLIST);
    require(again == borrowed, "same display default-table lookup was not cached");
    /* The extension API returns a borrowed table. Release only our explicit
     * public copy; the cache and every other borrower must remain valid. */
    copy = XmFontListCopy(again);
    require(copy != NULL, "default-table public copy unavailable");
    XmFontListFree(copy);
  }
  connection->retained = XmFontListCopy(borrowed);
  require(connection->retained != NULL, "retained default table unavailable");
  caption = XmStringCreateLocalized("Default table remains live");
  connection->label = XtVaCreateManagedWidget("nativeLabel", xmLabelWidgetClass,
      connection->shell, XmNfontList, connection->retained,
      XmNlabelString, caption, NULL);
  XmStringFree(caption);
  XtRealizeWidget(connection->shell);
  drain(app, connection->display);
  paint(connection);
}
static void close_connection(Connection *connection) {
  XmFontListFree(connection->retained);
  connection->retained = NULL;
  XtDestroyWidget(connection->shell);
  connection->shell = NULL;
  /* The last VendorShell normally retires XmDisplay automatically. The
   * callback clears the identity before any optional explicit destruction. */
  if (connection->native_display) XtDestroyWidget(connection->native_display);
  require(connection->destroyed == 1, "native display was not retired exactly once");
  XtCloseDisplay(connection->display);
  connection->display = NULL;
}
int main(int argc, char **argv) {
  XtAppContext app;
  Connection connections[CONNECTIONS] = {{0}}, reopened = {0};
  char *display_name;
  unsigned int index;
  require(setlocale(LC_ALL, "C.UTF-8") != NULL, "UTF-8 locale unavailable");
  XtSetLanguageProc(NULL, NULL, NULL);
  XtToolkitInitialize();
  app = XtCreateApplicationContext();
  XSetErrorHandler(x_error);
  display_name = getenv("DISPLAY");
  require(display_name && *display_name, "DISPLAY is required");
  /* The ninth connection forces array growth beyond the first eight slots. */
  for (index = 0; index < CONNECTIONS; ++index)
    open_connection(&connections[index], app, display_name, &argc, argv);
  for (index = 0; index < CONNECTIONS; index += 2) {
    close_connection(&connections[index]);
    paint(&connections[CONNECTIONS - 1]);
  }
  /* Reopen while other connections remain live, then continue with both
   * survivors. No stale Display key may select a closed connection's table. */
  open_connection(&reopened, app, display_name, &argc, argv);
  paint(&connections[1]);
  close_connection(&reopened);
  for (index = 1; index < CONNECTIONS; index += 2)
    close_connection(&connections[index]);
  /* Empty-cache retirement must also allow a fresh first connection. */
  reopened.destroyed = 0;
  open_connection(&reopened, app, display_name, &argc, argv);
  close_connection(&reopened);
  XtDestroyApplicationContext(app);
  require(!x_errors, "display-cache teardown produced native X errors");
  printf("{\"assertions\":%u,\"concurrent_displays\":%u,\"connections\":12,\"x_errors\":%u}\n",
         assertions, CONNECTIONS, x_errors);
  return 0;
}
