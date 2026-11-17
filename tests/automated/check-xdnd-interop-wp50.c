/*
 * WP-50 regression test: bounded text/uri-list and image/png helpers,
 * cross-client UTF8_STRING clipboard/selection round-trips, and XDND v5
 * protocol interoperability with Motif XmDropSite consumers.
 */
#include "native-window.h"
#include <Xm/XmAll.h>
#include <Xm/CutPaste.h>
#include <Xm/DragDrop.h>
#include <Xm/Transfer.h>
#include <X11/Xatom.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define CHECK(cond) \
  do { \
    if (!(cond)) { \
      fprintf(stderr, "FAIL WP-50 XDND/UTF-8 interop line %d: %s\n", __LINE__, #cond); \
      exit(1); \
    } \
  } while (0)

static void
test_uri_and_png_helpers(void)
{
  const char raw_uris[] =
      "# leading comment\r\n"
      "file:///tmp/my%20folder/report%231.txt\r\n"
      "file://localhost/var/data/item.bin\n"
      "https://example.org/resource?x=1\r\n"
      "extra-ignored-tail";
  /* Slice excluding "extra-ignored-tail" to test bounded non-NUL-terminated input */
  size_t slice_len = strlen(raw_uris) - strlen("extra-ignored-tail");
  String *uris = NULL;
  Cardinal count = 0;

  CHECK(XmeParseUriList(raw_uris, slice_len, &uris, &count));
  CHECK(count == 3 && uris != NULL);
  CHECK(strcmp(uris[0], "file:///tmp/my%20folder/report%231.txt") == 0);
  CHECK(strcmp(uris[1], "file://localhost/var/data/item.bin") == 0);
  CHECK(strcmp(uris[2], "https://example.org/resource?x=1") == 0);

  String path0 = XmeUriToLocalPath(uris[0]);
  String path1 = XmeUriToLocalPath(uris[1]);
  String path2 = XmeUriToLocalPath(uris[2]);
  CHECK(path0 != NULL && strcmp(path0, "/tmp/my folder/report#1.txt") == 0);
  CHECK(path1 != NULL && strcmp(path1, "/var/data/item.bin") == 0);
  CHECK(path2 == NULL);
  XtFree(path0);
  XtFree(path1);

  /* Malformed / hostile URI inputs */
  CHECK(XmeUriToLocalPath("file://evil.example.com/etc/passwd") == NULL);
  CHECK(XmeUriToLocalPath("file:///tmp/bad%00null.txt") == NULL);
  CHECK(XmeUriToLocalPath("file:///tmp/truncated%2") == NULL);
  CHECK(XmeUriToLocalPath("file:///tmp/nonhex%GG") == NULL);

  /* Round-trip formatting */
  char *formatted = NULL;
  size_t formatted_len = 0;
  CHECK(XmeFormatUriList((const String *)uris, count, &formatted, &formatted_len));
  CHECK(formatted != NULL && formatted_len > 0);
  String *reparsed = NULL;
  Cardinal reparsed_count = 0;
  CHECK(XmeParseUriList(formatted, formatted_len, &reparsed, &reparsed_count));
  CHECK(reparsed_count == count);
  for (Cardinal i = 0; i < count; ++i)
    CHECK(strcmp(uris[i], reparsed[i]) == 0);
  XmeFreeUriList(reparsed, reparsed_count);
  XtFree(formatted);
  XmeFreeUriList(uris, count);

  /* Reject embedded NUL inside bounded length */
  const char embedded_nul[] = "file:///a\0file:///b\r\n";
  CHECK(!XmeParseUriList(embedded_nul, sizeof(embedded_nul) - 1, &uris, &count));

  /* Valid minimal PNG buffer (16x24): signature + IHDR + IDAT + IEND */
  static const unsigned char valid_png[] = {
    0x89, 'P', 'N', 'G', 0x0d, 0x0a, 0x1a, 0x0a,
    /* IHDR: length 13 */
    0x00, 0x00, 0x00, 0x0d, 'I', 'H', 'D', 'R',
    0x00, 0x00, 0x00, 0x10, /* width = 16 */
    0x00, 0x00, 0x00, 0x18, /* height = 24 */
    0x08, 0x06, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, /* CRC placeholder */
    /* IDAT: length 4 */
    0x00, 0x00, 0x00, 0x04, 'I', 'D', 'A', 'T',
    0x01, 0x02, 0x03, 0x04,
    0x00, 0x00, 0x00, 0x00,
    /* IEND: length 0 */
    0x00, 0x00, 0x00, 0x00, 'I', 'E', 'N', 'D',
    0xae, 0x42, 0x60, 0x82
  };
  Dimension png_w = 0, png_h = 0;
  CHECK(XmeValidatePngData(valid_png, sizeof(valid_png), &png_w, &png_h));
  CHECK(png_w == 16 && png_h == 24);

  /* Corrupt PNG cases: truncated, zero width, oversized chunk length */
  CHECK(!XmeValidatePngData(valid_png, 20, &png_w, &png_h));
  unsigned char bad_png[sizeof(valid_png)];
  memcpy(bad_png, valid_png, sizeof(valid_png));
  bad_png[19] = 0; /* width = 0 */
  CHECK(!XmeValidatePngData(bad_png, sizeof(bad_png), &png_w, &png_h));
  memcpy(bad_png, valid_png, sizeof(valid_png));
  bad_png[33] = 0x7f; /* oversized IDAT chunk length */
  CHECK(!XmeValidatePngData(bad_png, sizeof(bad_png), &png_w, &png_h));
}

static char dropped_uri_path[256];

static Boolean
initiator_convert_proc(Widget w, Atom *selection, Atom *target,
                       Atom *type_return, XtPointer *value_return,
                       unsigned long *length_return, int *format_return)
{
  (void)selection;
  static const char payload[] = "file:///var/tmp/from-motif-initiator.txt\r\n";
  Display *dpy = XtDisplayOfObject(w);
  Atom uri_a = XInternAtom(dpy, "text/uri-list", False);
  if (*target == uri_a || *target == XA_STRING) {
    char *copy = XtMalloc(sizeof(payload));
    memcpy(copy, payload, sizeof(payload) - 1);
    *type_return = *target;
    *value_return = (XtPointer)copy;
    *length_return = strlen(payload);
    *format_return = 8;
    return True;
  }
  return False;
}

static void
uri_transfer_proc(Widget w, XtPointer closure, Atom *seltype, Atom *type,
                  XtPointer value, unsigned long *length, int *format)
{
  (void)w;
  (void)closure;
  (void)seltype;
  (void)type;
  (void)format;
  if (value != NULL && length != NULL && *length > 0) {
    String *uris = NULL;
    Cardinal count = 0;
    if (XmeParseUriList((const char *)value, (size_t)*length, &uris, &count) &&
        count > 0) {
      String local = XmeUriToLocalPath(uris[0]);
      if (local != NULL) {
        snprintf(dropped_uri_path, sizeof(dropped_uri_path), "%s", local);
        XtFree(local);
      }
      XmeFreeUriList(uris, count);
    }
  }
}

static void
uri_drop_proc(Widget w, XtPointer client_data, XtPointer call_data)
{
  (void)client_data;
  XmDropProcCallbackStruct *cb = (XmDropProcCallbackStruct *)call_data;
  Arg args[4];
  Cardinal n = 0;
  XmDropTransferEntryRec entries[1];
  Atom uri_atom = XInternAtom(XtDisplay(w), "text/uri-list", False);

  if (cb->dropAction != XmDROP || cb->operation != XmDROP_COPY) {
    cb->dropSiteStatus = XmINVALID_DROP_SITE;
    XtSetArg(args[n], XmNtransferStatus, XmTRANSFER_FAILURE); n++;
    XmDropTransferStart(cb->dragContext, args, n);
    return;
  }
  entries[0].target = uri_atom;
  entries[0].client_data = NULL;
  XtSetArg(args[n], XmNdropTransfers, entries); n++;
  XtSetArg(args[n], XmNnumDropTransfers, 1); n++;
  XtSetArg(args[n], XmNtransferProc, uri_transfer_proc); n++;
  cb->operation = XmDROP_COPY;
  cb->dropSiteStatus = XmVALID_DROP_SITE;
  XmDropTransferStart(cb->dragContext, args, n);
}

static void
service_external_xdnd_source(Display *ext_dpy, Window ext_win,
                             Atom xdnd_selection, Atom utf8_atom,
                             Atom uri_atom, Atom targets_atom,
                             const char *utf8_payload,
                             const char *uri_payload,
                             Window *status_target_out,
                             int *status_accepted_out,
                             Atom *status_action_out,
                             int *finished_ok_out)
{
  Atom xdnd_status = XInternAtom(ext_dpy, "XdndStatus", False);
  Atom xdnd_finished = XInternAtom(ext_dpy, "XdndFinished", False);

  while (XPending(ext_dpy) > 0) {
    XEvent ev;
    XNextEvent(ext_dpy, &ev);
    if (ev.type == ClientMessage) {
      if (ev.xclient.message_type == xdnd_status) {
        if (status_target_out)
          *status_target_out = (Window)ev.xclient.data.l[0];
        if (status_accepted_out)
          *status_accepted_out = (int)(ev.xclient.data.l[1] & 1L);
        if (status_action_out)
          *status_action_out = (Atom)ev.xclient.data.l[4];
      } else if (ev.xclient.message_type == xdnd_finished) {
        if (finished_ok_out)
          *finished_ok_out = 1;
      }
    } else if (ev.type == SelectionRequest &&
               ev.xselectionrequest.owner == ext_win &&
               ev.xselectionrequest.selection == xdnd_selection) {
      XSelectionRequestEvent *req = &ev.xselectionrequest;
      XSelectionEvent reply;
      memset(&reply, 0, sizeof(reply));
      reply.type = SelectionNotify;
      reply.display = req->display;
      reply.requestor = req->requestor;
      reply.selection = req->selection;
      reply.target = req->target;
      reply.time = req->time;
      reply.property = None;

      Atom prop = (req->property != None) ? req->property : req->target;
      if (req->target == targets_atom) {
        Atom supported[3] = {utf8_atom, uri_atom, XA_STRING};
        XChangeProperty(ext_dpy, req->requestor, prop, XA_ATOM, 32,
                        PropModeReplace, (unsigned char *)supported, 3);
        reply.property = prop;
      } else if (req->target == uri_atom && uri_payload != NULL) {
        XChangeProperty(ext_dpy, req->requestor, prop, uri_atom, 8,
                        PropModeReplace, (const unsigned char *)uri_payload,
                        (int)strlen(uri_payload));
        reply.property = prop;
      } else if (req->target == utf8_atom || req->target == XA_STRING) {
        XChangeProperty(ext_dpy, req->requestor, prop, req->target, 8,
                        PropModeReplace, (const unsigned char *)utf8_payload,
                        (int)strlen(utf8_payload));
        reply.property = prop;
      }
      XSendEvent(ext_dpy, req->requestor, False, 0, (XEvent *)&reply);
      XFlush(ext_dpy);
    }
  }
}

static void
pump_both(XtAppContext app, Display *motif_dpy, Display *ext_dpy,
          Window ext_win, Atom xdnd_selection, Atom utf8_atom, Atom uri_atom,
          Atom targets_atom, const char *utf8_payload, const char *uri_payload,
          Window *status_target_out, int *status_accepted_out,
          Atom *status_action_out, int *finished_ok_out)
{
  for (int iter = 0; iter < 40; ++iter) {
    XSync(motif_dpy, False);
    XSync(ext_dpy, False);
    while (XtAppPending(app))
      XtAppProcessEvent(app, XtIMAll);
    service_external_xdnd_source(ext_dpy, ext_win, xdnd_selection, utf8_atom,
                                 uri_atom, targets_atom, utf8_payload,
                                 uri_payload, status_target_out,
                                 status_accepted_out, status_action_out,
                                 finished_ok_out);
  }
}

int
main(int argc, char **argv)
{
  test_uri_and_png_helpers();

  XtAppContext app;
  Widget shell = XtVaAppInitialize(&app, "Wp50XdndInterop", NULL, 0,
                                   &argc, argv, NULL,
                                   XmNx, 60, XmNy, 60,
                                   XmNwidth, 460, XmNheight, 260, NULL);
  Display *dpy = XtDisplay(shell);

  Widget board = XtVaCreateManagedWidget(
      "board", xmBulletinBoardWidgetClass, shell,
      XmNwidth, 460, XmNheight, 260, NULL);

  Widget tf = XtVaCreateManagedWidget(
      "tf", xmTextFieldWidgetClass, board,
      XmNx, 20, XmNy, 20, XmNwidth, 380, XmNheight, 40, NULL);

  XmString drop_lbl = XmStringCreateLocalized((char *)"URI Drop Target");
  Widget uri_box = XtVaCreateManagedWidget(
      "uriBox", xmLabelWidgetClass, board,
      XmNx, 20, XmNy, 90, XmNwidth, 380, XmNheight, 60,
      XmNlabelString, drop_lbl, NULL);
  XmStringFree(drop_lbl);

  Atom uri_atom = XInternAtom(dpy, "text/uri-list", False);
  Arg ds_args[4];
  Cardinal n = 0;
  XtSetArg(ds_args[n], XmNimportTargets, &uri_atom); n++;
  XtSetArg(ds_args[n], XmNnumImportTargets, 1); n++;
  XtSetArg(ds_args[n], XmNdropSiteOperations, XmDROP_COPY); n++;
  XtSetArg(ds_args[n], XmNdropProc, uri_drop_proc); n++;
  XmDropSiteRegister(uri_box, ds_args, n);

  XtRealizeWidget(shell);
  NativeWaitForViewable(board);

  /* Part 2: Cross-process UTF8_STRING clipboard round-trip */
  const char utf8_clip[] = "Gr\xc3\xbc\xc3\x9f Gott \xe2\x80\x94 \xce\xa9 \xe2\x89\x88 \xe2\x88\x9a" "2 \xe2\x80\x94 \xe6\x97\xa5\xe6\x9c\xac\xe8\xaa\x9e";
  long item_id = 0;
  XmString clip_label = XmStringCreateLocalized((char *)"WP50Clip");
  CHECK(XmClipboardRegisterFormat(dpy, (char *)"UTF8_STRING", 8) == XmClipboardSuccess);
  CHECK(XmClipboardStartCopy(dpy, XtWindow(shell), clip_label, CurrentTime,
                             NULL, NULL, &item_id) == XmClipboardSuccess);
  XmStringFree(clip_label);
  CHECK(XmClipboardCopy(dpy, XtWindow(shell), item_id, (char *)"UTF8_STRING",
                        (XtPointer)utf8_clip, (unsigned long)strlen(utf8_clip),
                        42, NULL) == XmClipboardSuccess);
  CHECK(XmClipboardEndCopy(dpy, XtWindow(shell), item_id) == XmClipboardSuccess);
  XSync(dpy, False);

  pid_t child = fork();
  CHECK(child >= 0);
  if (child == 0) {
    XtAppContext child_app;
    int c_argc = 1;
    char *c_argv[] = {(char *)"wp50-clip-reader", NULL};
    Widget c_shell = XtVaAppInitialize(&child_app, "Wp50ClipReader", NULL, 0,
                                       &c_argc, c_argv, NULL,
                                       XmNwidth, 64, XmNheight, 64, NULL);
    XtRealizeWidget(c_shell);
    Display *c_dpy = XtDisplay(c_shell);
    char retrieved[256];
    unsigned long out_len = 0;
    long priv_id = 0;
    memset(retrieved, 0, sizeof(retrieved));
    int rc = XmClipboardRetrieve(c_dpy, XtWindow(c_shell), (char *)"UTF8_STRING",
                                 retrieved, sizeof(retrieved) - 1,
                                 &out_len, &priv_id);
    if (rc != XmClipboardSuccess || out_len != strlen(utf8_clip) ||
        memcmp(retrieved, utf8_clip, out_len) != 0) {
      _exit(2);
    }
    XtDestroyWidget(c_shell);
    XtCloseDisplay(c_dpy);
    _exit(0);
  }
  int status = 0;
  CHECK(waitpid(child, &status, 0) == child);
  CHECK(WIFEXITED(status) && WEXITSTATUS(status) == 0);

  /* Part 3: Verify XdndAware v5 on Motif shell and test external XDND v5 drops */
  Atom xdnd_aware = XInternAtom(dpy, "XdndAware", False);
  Atom actual_type = None;
  int actual_fmt = 0;
  unsigned long nitems = 0, bytes_after = 0;
  unsigned char *prop_data = NULL;
  CHECK(XGetWindowProperty(dpy, XtWindow(shell), xdnd_aware, 0, 1, False,
                           AnyPropertyType, &actual_type, &actual_fmt,
                           &nitems, &bytes_after, &prop_data) == Success);
  CHECK(prop_data != NULL && nitems >= 1 && *( Atom *)prop_data >= 5);
  XFree(prop_data);

  Display *ext_dpy = XOpenDisplay(NULL);
  CHECK(ext_dpy != NULL);
  Window ext_win = XCreateSimpleWindow(
      ext_dpy, DefaultRootWindow(ext_dpy), 10, 10, 32, 32, 0, 0, 0);
  Atom xdnd_selection = XInternAtom(ext_dpy, "XdndSelection", False);
  Atom xdnd_enter = XInternAtom(ext_dpy, "XdndEnter", False);
  Atom xdnd_position = XInternAtom(ext_dpy, "XdndPosition", False);
  Atom xdnd_drop = XInternAtom(ext_dpy, "XdndDrop", False);
  Atom xdnd_leave = XInternAtom(ext_dpy, "XdndLeave", False);
  Atom xdnd_action_copy = XInternAtom(ext_dpy, "XdndActionCopy", False);
  Atom utf8_atom = XInternAtom(ext_dpy, "UTF8_STRING", False);
  Atom ext_uri_atom = XInternAtom(ext_dpy, "text/uri-list", False);
  Atom targets_atom = XInternAtom(ext_dpy, "TARGETS", False);

  XSetSelectionOwner(ext_dpy, xdnd_selection, ext_win, CurrentTime);
  XSync(ext_dpy, False);
  CHECK(XGetSelectionOwner(ext_dpy, xdnd_selection) == ext_win);

  /* 3a. External XDND v5 UTF-8 text drop onto Motif XmTextField */
  Position tf_rx = 0, tf_ry = 0;
  XtTranslateCoords(tf, (Position)(XtWidth(tf) / 2),
                    (Position)(XtHeight(tf) / 2), &tf_rx, &tf_ry);

  XEvent msg;
  memset(&msg, 0, sizeof(msg));
  msg.xclient.type = ClientMessage;
  msg.xclient.display = ext_dpy;
  msg.xclient.window = XtWindow(shell);
  msg.xclient.message_type = xdnd_enter;
  msg.xclient.format = 32;
  msg.xclient.data.l[0] = (long)ext_win;
  msg.xclient.data.l[1] = (5L << 24);
  msg.xclient.data.l[2] = (long)utf8_atom;
  msg.xclient.data.l[3] = (long)XA_STRING;
  msg.xclient.data.l[4] = None;
  XSendEvent(ext_dpy, XtWindow(shell), False, NoEventMask, &msg);

  memset(&msg, 0, sizeof(msg));
  msg.xclient.type = ClientMessage;
  msg.xclient.display = ext_dpy;
  msg.xclient.window = XtWindow(shell);
  msg.xclient.message_type = xdnd_position;
  msg.xclient.format = 32;
  msg.xclient.data.l[0] = (long)ext_win;
  msg.xclient.data.l[1] = 0;
  msg.xclient.data.l[2] = (((long)tf_rx & 0xffffL) << 16) | ((long)tf_ry & 0xffffL);
  msg.xclient.data.l[3] = CurrentTime;
  msg.xclient.data.l[4] = (long)xdnd_action_copy;
  XSendEvent(ext_dpy, XtWindow(shell), False, NoEventMask, &msg);

  Window status_target = None;
  int status_accepted = 0;
  Atom status_action = None;
  int finished_ok = 0;
  const char xdnd_utf8_payload[] = "XDNDv5Payload";

  pump_both(app, dpy, ext_dpy, ext_win, xdnd_selection, utf8_atom, ext_uri_atom,
            targets_atom, xdnd_utf8_payload, NULL, &status_target,
            &status_accepted, &status_action, &finished_ok);
  CHECK(status_target == XtWindow(shell));
  CHECK(status_accepted == 1);
  CHECK(status_action == xdnd_action_copy);

  memset(&msg, 0, sizeof(msg));
  msg.xclient.type = ClientMessage;
  msg.xclient.display = ext_dpy;
  msg.xclient.window = XtWindow(shell);
  msg.xclient.message_type = xdnd_drop;
  msg.xclient.format = 32;
  msg.xclient.data.l[0] = (long)ext_win;
  msg.xclient.data.l[2] = CurrentTime;
  XSendEvent(ext_dpy, XtWindow(shell), False, NoEventMask, &msg);

  pump_both(app, dpy, ext_dpy, ext_win, xdnd_selection, utf8_atom, ext_uri_atom,
            targets_atom, xdnd_utf8_payload, NULL, &status_target,
            &status_accepted, &status_action, &finished_ok);
  CHECK(finished_ok == 1);
  char *tf_val = XmTextFieldGetString(tf);
  CHECK(tf_val != NULL && strstr(tf_val, xdnd_utf8_payload) != NULL);
  XtFree(tf_val);

  /* 3b. External XDND v5 text/uri-list drop onto custom XmDropSite (uri_box) */
  Position ub_rx = 0, ub_ry = 0;
  XtTranslateCoords(uri_box, (Position)(XtWidth(uri_box) / 2),
                    (Position)(XtHeight(uri_box) / 2), &ub_rx, &ub_ry);
  status_accepted = 0;
  finished_ok = 0;
  memset(dropped_uri_path, 0, sizeof(dropped_uri_path));
  const char uri_drop_payload[] = "file:///var/tmp/dropped%20item.txt\r\n";

  memset(&msg, 0, sizeof(msg));
  msg.xclient.type = ClientMessage;
  msg.xclient.display = ext_dpy;
  msg.xclient.window = XtWindow(shell);
  msg.xclient.message_type = xdnd_enter;
  msg.xclient.format = 32;
  msg.xclient.data.l[0] = (long)ext_win;
  msg.xclient.data.l[1] = (5L << 24);
  msg.xclient.data.l[2] = (long)ext_uri_atom;
  XSendEvent(ext_dpy, XtWindow(shell), False, NoEventMask, &msg);

  memset(&msg, 0, sizeof(msg));
  msg.xclient.type = ClientMessage;
  msg.xclient.display = ext_dpy;
  msg.xclient.window = XtWindow(shell);
  msg.xclient.message_type = xdnd_position;
  msg.xclient.format = 32;
  msg.xclient.data.l[0] = (long)ext_win;
  msg.xclient.data.l[2] = (((long)ub_rx & 0xffffL) << 16) | ((long)ub_ry & 0xffffL);
  msg.xclient.data.l[3] = CurrentTime;
  msg.xclient.data.l[4] = (long)xdnd_action_copy;
  XSendEvent(ext_dpy, XtWindow(shell), False, NoEventMask, &msg);

  memset(&msg, 0, sizeof(msg));
  msg.xclient.type = ClientMessage;
  msg.xclient.display = ext_dpy;
  msg.xclient.window = XtWindow(shell);
  msg.xclient.message_type = xdnd_drop;
  msg.xclient.format = 32;
  msg.xclient.data.l[0] = (long)ext_win;
  msg.xclient.data.l[2] = CurrentTime;
  XSendEvent(ext_dpy, XtWindow(shell), False, NoEventMask, &msg);

  pump_both(app, dpy, ext_dpy, ext_win, xdnd_selection, utf8_atom, ext_uri_atom,
            targets_atom, xdnd_utf8_payload, uri_drop_payload, &status_target,
            &status_accepted, &status_action, &finished_ok);
  CHECK(status_accepted == 1);
  CHECK(finished_ok == 1);
  CHECK(strcmp(dropped_uri_path, "/var/tmp/dropped item.txt") == 0);

  /* 3c. Cancelled XDND session via XdndLeave */
  memset(&msg, 0, sizeof(msg));
  msg.xclient.type = ClientMessage;
  msg.xclient.display = ext_dpy;
  msg.xclient.window = XtWindow(shell);
  msg.xclient.message_type = xdnd_enter;
  msg.xclient.format = 32;
  msg.xclient.data.l[0] = (long)ext_win;
  msg.xclient.data.l[1] = (5L << 24);
  msg.xclient.data.l[2] = (long)utf8_atom;
  XSendEvent(ext_dpy, XtWindow(shell), False, NoEventMask, &msg);

  memset(&msg, 0, sizeof(msg));
  msg.xclient.type = ClientMessage;
  msg.xclient.display = ext_dpy;
  msg.xclient.window = XtWindow(shell);
  msg.xclient.message_type = xdnd_leave;
  msg.xclient.format = 32;
  msg.xclient.data.l[0] = (long)ext_win;
  XSendEvent(ext_dpy, XtWindow(shell), False, NoEventMask, &msg);
  pump_both(app, dpy, ext_dpy, ext_win, xdnd_selection, utf8_atom, ext_uri_atom,
            targets_atom, xdnd_utf8_payload, uri_drop_payload, NULL, NULL, NULL,
            NULL);

  /* Part 4: Motif XmDragStart initiator serving XdndSelection to external consumer */
  {
    Atom uri_local = XInternAtom(dpy, "text/uri-list", False);
    Atom exports[2] = {uri_local, XA_STRING};
    XEvent btn_ev;
    memset(&btn_ev, 0, sizeof(btn_ev));
    btn_ev.xbutton.type = ButtonPress;
    btn_ev.xbutton.display = dpy;
    btn_ev.xbutton.window = XtWindow(shell);
    btn_ev.xbutton.root = DefaultRootWindow(dpy);
    btn_ev.xbutton.time = CurrentTime;
    btn_ev.xbutton.x = 20;
    btn_ev.xbutton.y = 20;
    btn_ev.xbutton.x_root = 20;
    btn_ev.xbutton.y_root = 20;
    btn_ev.xbutton.button = Button2;
    btn_ev.xbutton.same_screen = True;

    Arg dargs[6];
    int dn = 0;
    XtSetArg(dargs[dn], XmNexportTargets, exports); dn++;
    XtSetArg(dargs[dn], XmNnumExportTargets, 2); dn++;
    XtSetArg(dargs[dn], XmNconvertProc, initiator_convert_proc); dn++;
    XtSetArg(dargs[dn], XmNdragOperations, XmDROP_COPY); dn++;
    Widget dc = XmDragStart(shell, &btn_ev, dargs, dn);
    CHECK(dc != NULL);
    XSync(dpy, False);

    Atom out_prop = XInternAtom(ext_dpy, "WP50_XDND_OUT", False);
    XDeleteProperty(ext_dpy, ext_win, out_prop);
    XConvertSelection(ext_dpy, xdnd_selection, ext_uri_atom, out_prop, ext_win,
                      CurrentTime);
    XFlush(ext_dpy);

    int got_notify = 0;
    for (int iter = 0; iter < 60 && !got_notify; ++iter) {
      while (XtAppPending(app) & XtIMXEvent)
        XtAppProcessEvent(app, XtIMXEvent);
      XSync(dpy, False);
      while (XPending(ext_dpy) > 0) {
        XEvent e;
        XNextEvent(ext_dpy, &e);
        if (e.type == SelectionNotify &&
            e.xselection.selection == xdnd_selection) {
          CHECK(e.xselection.property == out_prop);
          got_notify = 1;
        }
      }
      if (!got_notify)
        XSync(ext_dpy, False);
    }
    CHECK(got_notify == 1);

    Atom actual_type = None;
    int actual_fmt = 0;
    unsigned long nitems = 0, rem = 0;
    unsigned char *prop_bytes = NULL;
    CHECK(XGetWindowProperty(ext_dpy, ext_win, out_prop, 0, 256, True,
                             AnyPropertyType, &actual_type, &actual_fmt,
                             &nitems, &rem, &prop_bytes) == Success);
    CHECK(prop_bytes != NULL);
    CHECK(strstr((const char *)prop_bytes,
                 "file:///var/tmp/from-motif-initiator.txt") != NULL);
    XFree(prop_bytes);

    XmDragCancel(dc);
    while (XtAppPending(app) & XtIMXEvent)
      XtAppProcessEvent(app, XtIMXEvent);
  }

  XDestroyWindow(ext_dpy, ext_win);
  XCloseDisplay(ext_dpy);
  XtDestroyWidget(shell);
  XtCloseDisplay(dpy);
  XtDestroyApplicationContext(app);
  printf("PASS WP-50 XDND v5, UTF8_STRING clipboard, text/uri-list, and image/png interop\n");
  return 0;
}
