/* Native Unicode clipboard interoperability, with an independent Xlib peer.
 * LGPL-2.0-or-later. The driver provides bounded exact-byte payload files. */
#define _POSIX_C_SOURCE 200809L
#include <X11/Shell.h>
#include <X11/Xatom.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/select.h>
#include <time.h>
#include <unistd.h>

#define LIMIT (16u * 1024u * 1024u)
#define CHUNK 16384u
static void fail(const char *message) {
  fprintf(stderr, "%s\n", message);
  exit(1);
}
static unsigned char *load(const char *path, size_t *length) {
  FILE *f = fopen(path, "rb");
  long size;
  unsigned char *bytes;
  if (!f || fseek(f, 0, SEEK_END) || (size = ftell(f)) < 0 ||
      size > (long)LIMIT || fseek(f, 0, SEEK_SET))
    fail("cannot read bounded payload");
  bytes = malloc((size_t)size + 1);
  if (!bytes || fread(bytes, 1, (size_t)size, f) != (size_t)size || fclose(f))
    fail("payload read failed");
  bytes[size] = 0;
  if (memchr(bytes, 0, (size_t)size))
    fail("text payload has embedded NUL");
  *length = (size_t)size;
  return bytes;
}
static void save(const char *path, const unsigned char *bytes, size_t length) {
  FILE *f = fopen(path, "wb");
  if (!f || fwrite(bytes, 1, length, f) != length || fclose(f))
    fail("received payload write failed");
}
static void next_event(Display *display, XEvent *event) {
  if (!XPending(display)) {
    fd_set readable;
    struct timeval timeout = {20, 0};
    FD_ZERO(&readable);
    FD_SET(ConnectionNumber(display), &readable);
    XFlush(display);
    if (select(ConnectionNumber(display) + 1, &readable, NULL, NULL,
               &timeout) <= 0)
      fail("selection peer timed out");
  }
  XNextEvent(display, event);
}
static Bool timestamp_event(Display *display, XEvent *event, XPointer data) {
  XPropertyEvent *wanted = (XPropertyEvent *)data;
  (void)display;
  return event->type == PropertyNotify &&
         event->xproperty.window == wanted->window &&
         event->xproperty.atom == wanted->atom &&
         event->xproperty.state == PropertyNewValue;
}
static Time server_time(Display *display, Window window) {
  Atom property = XInternAtom(display, "_XM_CLIPBOARD_TEST_TIME", False);
  unsigned char byte = 0;
  XEvent event;
  XWindowAttributes attributes;
  XGetWindowAttributes(display, window, &attributes);
  XSelectInput(display, window,
               attributes.your_event_mask | PropertyChangeMask);
  XChangeProperty(display, window, property, XA_INTEGER, 8, PropModeReplace,
                  &byte, 1);
  {
    XPropertyEvent wanted;
    wanted.window = window;
    wanted.atom = property;
    /* Preserve unrelated pending Xt selection/INCR cleanup events. */
    XIfEvent(display, &event, timestamp_event, (XPointer)&wanted);
  }
  return event.xproperty.time;
}
static Window peer_window(Display *display) {
  Window window = XCreateSimpleWindow(display, DefaultRootWindow(display), 0, 0,
                                      1, 1, 0, 0, 0);
  XSelectInput(display, window, PropertyChangeMask);
  return window;
}
static int request(const char *output) {
  Display *display = XOpenDisplay(NULL);
  Window window;
  Atom clipboard, utf8, property, incr, type;
  XEvent event;
  unsigned char *bytes = malloc(LIMIT), *part = NULL;
  unsigned long count, after, total = 0, chunks = 0;
  int format, incremental = 0;
  if (!display || !bytes)
    fail("Xlib requestor initialization failed");
  window = peer_window(display);
  clipboard = XInternAtom(display, "CLIPBOARD", False);
  utf8 = XInternAtom(display, "UTF8_STRING", False);
  property = XInternAtom(display, "_XM_CLIPBOARD_TEST_DATA", False);
  incr = XInternAtom(display, "INCR", False);
  XConvertSelection(display, clipboard, utf8, property, window,
                    server_time(display, window));
  do {
    next_event(display, &event);
  } while (event.type != SelectionNotify);
  if (event.xselection.property == None)
    fail("native clipboard refused UTF8_STRING");
  if (XGetWindowProperty(display, window, property, 0, LIMIT / 4, False,
                         AnyPropertyType, &type, &format, &count, &after,
                         &part) != Success ||
      after)
    fail("initial clipboard property invalid or exceeds bound");
  if (type == incr) {
    if (format != 32 || count != 1 || *(unsigned long *)part > LIMIT)
      fail("INCR header is not one bounded format32 logical item");
    incremental = 1;
    XFree(part);
    part = NULL;
    XDeleteProperty(display, window, property);
    for (;;) {
      do {
        next_event(display, &event);
      } while (event.type != PropertyNotify ||
               event.xproperty.window != window ||
               event.xproperty.atom != property ||
               event.xproperty.state != PropertyNewValue);
      if (XGetWindowProperty(display, window, property, 0, LIMIT / 4, True,
                             AnyPropertyType, &type, &format, &count, &after,
                             &part) != Success ||
          after || type != utf8 || format != 8 || count > LIMIT - total)
        fail("INCR chunk has invalid type, format or length");
      if (!count) {
        XFree(part);
        break;
      }
      memcpy(bytes + total, part, count);
      total += count;
      ++chunks;
      XFree(part);
      part = NULL;
    }
  } else {
    if (type != utf8 || format != 8 || count > LIMIT)
      fail("UTF8_STRING property malformed");
    memcpy(bytes, part, count);
    total = count;
    chunks = 1;
    XFree(part);
  }
  save(output, bytes, total);
  printf("{\"incr\":%s,\"bytes\":%lu,\"chunks\":%lu}\n",
         incremental ? "true" : "false", total, chunks);
  XDestroyWindow(display, window);
  XCloseDisplay(display);
  free(bytes);
  return 0;
}
static int own(const char *input) {
  Display *display = XOpenDisplay(NULL);
  Window window, requestor = None;
  Atom clipboard, utf8, targets, timestamp, incr, destination = None;
  Time owned_at;
  unsigned char *bytes;
  size_t length, offset = 0, chunks = 0;
  int transferring = 0;
  XEvent event;
  if (!display)
    fail("Xlib owner initialization failed");
  bytes = load(input, &length);
  window = peer_window(display);
  clipboard = XInternAtom(display, "CLIPBOARD", False);
  utf8 = XInternAtom(display, "UTF8_STRING", False);
  targets = XInternAtom(display, "TARGETS", False);
  timestamp = XInternAtom(display, "TIMESTAMP", False);
  incr = XInternAtom(display, "INCR", False);
  owned_at = server_time(display, window);
  XSetSelectionOwner(display, clipboard, window, owned_at);
  if (XGetSelectionOwner(display, clipboard) != window)
    fail("raw peer failed clipboard ownership");
  puts("{\"ready\":true}");
  fflush(stdout);
  for (;;) {
    next_event(display, &event);
    if (event.type == SelectionClear)
      break;
    if (event.type == SelectionRequest) {
      XSelectionRequestEvent *r = &event.xselectionrequest;
      XSelectionEvent reply;
      Atom property = r->property == None ? r->target : r->property;
      memset(&reply, 0, sizeof reply);
      reply.type = SelectionNotify;
      reply.display = display;
      reply.requestor = r->requestor;
      reply.selection = r->selection;
      reply.target = r->target;
      reply.time = r->time;
      reply.property = None;
      if (r->target == targets) {
        Atom available[] = {targets, utf8, timestamp};
        XChangeProperty(display, r->requestor, property, XA_ATOM, 32,
                        PropModeReplace, (unsigned char *)available, 3);
        reply.property = property;
      } else if (r->target == timestamp) {
        unsigned long value = owned_at;
        XChangeProperty(display, r->requestor, property, XA_INTEGER, 32,
                        PropModeReplace, (unsigned char *)&value, 1);
        reply.property = property;
      } else if (r->target == utf8 && !transferring) {
        reply.property = property;
        if (length > CHUNK) {
          unsigned long size = (unsigned long)length;
          requestor = r->requestor;
          destination = property;
          offset = chunks = 0;
          transferring = 1;
          XSelectInput(display, requestor, PropertyChangeMask);
          XChangeProperty(display, requestor, destination, incr, 32,
                          PropModeReplace, (unsigned char *)&size, 1);
        } else {
          XChangeProperty(display, r->requestor, property, utf8, 8,
                          PropModeReplace, bytes, (int)length);
          printf(
              "{\"transfer\":true,\"incr\":false,\"bytes\":%lu,\"chunks\":1}\n",
              (unsigned long)length);
          fflush(stdout);
        }
      }
      XSendEvent(display, r->requestor, False, 0, (XEvent *)&reply);
      XFlush(display);
    } else if (transferring && event.type == PropertyNotify &&
               event.xproperty.window == requestor &&
               event.xproperty.atom == destination &&
               event.xproperty.state == PropertyDelete) {
      const size_t count = length - offset < CHUNK ? length - offset : CHUNK;
      XChangeProperty(display, requestor, destination, utf8, 8, PropModeReplace,
                      bytes + offset, (int)count);
      offset += count;
      if (count)
        ++chunks;
      else {
        transferring = 0;
        printf(
            "{\"transfer\":true,\"incr\":true,\"bytes\":%lu,\"chunks\":%lu}\n",
            (unsigned long)length, (unsigned long)chunks);
        fflush(stdout);
      }
      XFlush(display);
    }
  }
  XDestroyWindow(display, window);
  XCloseDisplay(display);
  free(bytes);
  return 0;
}
static void timed_out(XtPointer data, XtIntervalId *id) {
  (void)data;
  (void)id;
  fail("native clipboard transfer timed out");
}
static int native(int argc, char **argv) {
  XtAppContext app;
  Widget shell, text;
  const int field = !strcmp(argv[2], "field"),
            importing = !strcmp(argv[1], "native-import");
  unsigned char *bytes;
  size_t length;
  Time timestamp;
  char *value;
  int xt_argc = 1;
  char *xt_argv[] = {argv[0], NULL};
  (void)argc;
  bytes = load(argv[3], &length);
  shell = XtVaAppInitialize(&app, "ClipboardFixture", NULL, 0, &xt_argc,
                            xt_argv, NULL, NULL);
  text = XtVaCreateManagedWidget(
      "text", field ? xmTextFieldWidgetClass : xmTextWidgetClass, shell,
      XmNwidth, 640, XmNheight, 240, NULL);
  {
    XmFontListEntry entry = XmFontListEntryLoad(
        XtDisplay(shell), "DejaVu Sans", XmFONT_IS_XFT, XmFONTLIST_DEFAULT_TAG);
    XmFontList fonts;
    XmFontType type;
    if (!entry || !XmFontListEntryGetFont(entry, &type) ||
        type != XmFONT_IS_XFT)
      fail("Unicode clipboard fixture requires an actual Xft rendition");
    fonts = XmFontListAppendEntry(NULL, entry);
    XmFontListEntryFree(&entry);
    XtVaSetValues(text, XmNfontList, fonts, NULL);
    XmFontListFree(fonts);
  }
  XtRealizeWidget(shell);
  XtAppAddTimeOut(app, 30000, timed_out, NULL);
  if (!importing) {
    if (field)
      XmTextFieldSetString(text, (char *)bytes);
    else
      XmTextSetString(text, (char *)bytes);
  } else {
    timestamp = server_time(XtDisplay(shell), XtWindow(shell));
    if (!(field ? XmTextFieldPaste(text) : XmTextPaste(text)))
      fail("native paste admission refused");
    for (;;) {
      value = field ? XmTextFieldGetString(text) : XmTextGetString(text);
      if (value && strlen(value) == length && !memcmp(value, bytes, length)) {
        XtFree(value);
        break;
      }
      XtFree(value);
      XtAppProcessEvent(app, XtIMAll);
    }
  }
  timestamp = server_time(XtDisplay(shell), XtWindow(shell));
  if (field)
    XmTextFieldSetSelection(text, 0, XmTextFieldGetLastPosition(text),
                            timestamp);
  else
    XmTextSetSelection(text, 0, XmTextGetLastPosition(text), timestamp);
  if (!(field ? XmTextFieldCopy(text, timestamp) : XmTextCopy(text, timestamp)))
    fail("native copy admission refused");
  if (XGetSelectionOwner(XtDisplay(shell),
                         XInternAtom(XtDisplay(shell), "CLIPBOARD", False)) ==
      None)
    fail("native copy did not own CLIPBOARD");
  printf("{\"ready\":true,\"native_import\":%s,\"bytes\":%lu}\n",
         importing ? "true" : "false", (unsigned long)length);
  fflush(stdout);
  free(bytes);
  XtAppMainLoop(app);
  return 0;
}
int main(int argc, char **argv) {
  if (!setlocale(LC_ALL, "") || MB_CUR_MAX < 4)
    fail("requires a UTF-8 locale");
  if (argc == 3 && !strcmp(argv[1], "request"))
    return request(argv[2]);
  if (argc == 3 && !strcmp(argv[1], "own"))
    return own(argv[2]);
  if (argc == 4 &&
      (!strcmp(argv[1], "native-owner") || !strcmp(argv[1], "native-import"))) {
    XtSetLanguageProc(NULL, NULL, NULL);
    return native(argc, argv);
  }
  fail("usage: clipboard_unicode request OUTPUT | own INPUT | "
       "native-{owner,import} {text,field} INPUT");
  return 1;
}
