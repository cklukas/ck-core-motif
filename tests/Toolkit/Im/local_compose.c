/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Observe actual local XIM commits; never invoke input actions or callbacks. */
#include <Xm/Xm.h>
#include <Xm/Display.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/XmIm.h>
#include <X11/Xatom.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static Widget shell, field, text, display_widget;
static XtAppContext app;
static unsigned int field_changes, text_changes, x_errors, display_destroyed;
static const char *policy;
static void require(int condition, const char *message) {
  if (!condition) { fprintf(stderr, "%s\n", message); exit(1); }
}
static int x_error(Display *display, XErrorEvent *event) {
  char message[256];
  XGetErrorText(display, event->error_code, message, sizeof(message));
  fprintf(stderr, "native X error: %s\n", message);
  ++x_errors;
  return 0;
}
static void changed(Widget widget, XtPointer data, XtPointer call) {
  (void)widget; (void)call;
  ++*(unsigned int *)data;
}
static void retired(Widget widget, XtPointer data, XtPointer call) {
  (void)widget; (void)data; (void)call;
  ++display_destroyed;
  display_widget = NULL;
}
static void hex(const char *value) {
  for (; *value; ++value) printf("%02x", (unsigned char)*value);
}
static void snapshot(void) {
  char *f = XmTextFieldGetString(field), *t = XmTextGetString(text);
  XIM im = XmImGetXIM(field);
  XIC fi = XmImSetXIC(field, NULL), ti = XmImSetXIC(text, NULL);
  Widget focus = XmGetFocusWidget(shell);
  XSync(XtDisplay(shell), False);
  require(im && fi && ti, "local XIM/XIC registration disappeared");
  require(!x_errors, "local Compose produced native X errors");
  printf("{\"field\":\""); hex(f);
  printf("\",\"text\":\""); hex(t);
  printf("\",\"field_changes\":%u,\"text_changes\":%u,"
         "\"field_caret\":%ld,\"text_caret\":%ld,"
         "\"field_length\":%ld,\"text_length\":%ld,"
         "\"focus\":\"%s\",\"xim\":true,\"field_xic\":true,\"text_xic\":true,"
         "\"shared_xic\":%s,\"x_errors\":%u}\n",
         field_changes, text_changes,
         (long)XmTextFieldGetInsertionPosition(field), (long)XmTextGetInsertionPosition(text),
         (long)XmTextFieldGetLastPosition(field), (long)XmTextGetLastPosition(text),
         focus == field ? "field" : focus == text ? "text" : "other",
         fi == ti ? "true" : "false", x_errors);
  XtFree(f); XtFree(t);
  fflush(stdout);
}
static void input(XtPointer data, int *fd, XtInputId *id) {
  char line[80];
  (void)data; (void)fd; (void)id;
  if (!fgets(line, sizeof(line), stdin) || !strcmp(line, "quit\n")) {
    XtAppSetExitFlag(app);
    return;
  }
  if (!strcmp(line, "focus-field\n"))
    require(XmProcessTraversal(field, XmTRAVERSE_CURRENT), "native field traversal refused");
  else if (!strcmp(line, "focus-text\n"))
    require(XmProcessTraversal(text, XmTRAVERSE_CURRENT), "native Text traversal refused");
  else require(!strcmp(line, "snapshot\n"), "unknown local Compose observer request");
  /* The observer first synchronizes its XTest connection. A round trip on
   * this connection then drains the already delivered native key events
   * before reporting committed bytes/callback counts, including prefixes
   * that correctly produce no committed value change. */
  XSync(XtDisplay(shell),False);
  while (XtAppPending(app) & XtIMXEvent) XtAppProcessEvent(app,XtIMXEvent);
  snapshot();
}
int main(int argc, char **argv) {
  Widget box;
  Display *display;
  XIM im;
  XmFontListEntry entry;
  XmFontList fonts;
  XtInputId observer;
  Atom servers, actual_type;
  int actual_format;
  unsigned long count, remaining;
  unsigned char *server_atoms = NULL;
  const char *modifiers = getenv("XMODIFIERS");
  String fallback[] = {"*inputMethod: none", NULL};
  policy = getenv("XM_LOCAL_COMPOSE_POLICY");
  require(policy && (!strcmp(policy,"shell") || !strcmp(policy,"widget")), "explicit input policy required");
  require(modifiers && !strcmp(modifiers,"@im=none"), "explicit local IM modifier required");
  require(getenv("XCOMPOSEFILE") != NULL, "explicit frozen Compose input required");
  require(setlocale(LC_ALL,"C.UTF-8") != NULL, "UTF-8 locale unavailable");
  XtSetLanguageProc(NULL,NULL,NULL);
  require(XSupportsLocale(), "Xlib does not support the UTF-8 locale");
  require(XSetLocaleModifiers("@im=none") != NULL, "local input modifier refused");
  setvbuf(stdin, NULL, _IONBF, 0);
  shell = XtVaAppInitialize(&app,"LocalCompose",NULL,0,&argc,argv,fallback,
      XmNtitle,"Local Compose native qualification",XmNinputMethod,"none",
      XmNinputPolicy,!strcmp(policy,"widget") ? XmPER_WIDGET : XmPER_SHELL,NULL);
  display = XtDisplay(shell);
  XSetErrorHandler(x_error);
  servers = XInternAtom(display,"_XIM_SERVERS",True);
  if (servers != None) {
    require(XGetWindowProperty(display,DefaultRootWindow(display),servers,0,4096,False,
        XA_ATOM,&actual_type,&actual_format,&count,&remaining,&server_atoms) == Success,
        "cannot inspect private XIM server advertisement");
    require(count == 0 && remaining == 0, "external XIM server advertised on local-only display");
    if (server_atoms) XFree(server_atoms);
  }
  entry = XmFontListEntryLoad(display,"DejaVu Sans",XmFONT_IS_XFT,XmFONTLIST_DEFAULT_TAG);
  require(entry != NULL, "native Xft font unavailable");
  fonts = XmFontListAppendEntry(NULL,entry);
  XmFontListEntryFree(&entry);
  box = XtVaCreateManagedWidget("box",xmRowColumnWidgetClass,shell,NULL);
  field = XtVaCreateManagedWidget("field",xmTextFieldWidgetClass,box,
      XmNcolumns,35,XmNfontList,fonts,NULL);
  text = XtVaCreateManagedWidget("text",xmTextWidgetClass,box,
      XmNcolumns,35,XmNrows,4,XmNeditMode,XmMULTI_LINE_EDIT,XmNfontList,fonts,NULL);
  XmFontListFree(fonts);
  XtAddCallback(field,XmNvalueChangedCallback,changed,&field_changes);
  XtAddCallback(text,XmNvalueChangedCallback,changed,&text_changes);
  display_widget = XmGetXmDisplay(display);
  require(display_widget != NULL, "native display owner missing");
  XtAddCallback(display_widget,XmNdestroyCallback,retired,NULL);
  XtRealizeWidget(shell);
  require(XmProcessTraversal(field,XmTRAVERSE_CURRENT), "initial field traversal refused");
  im = XmImGetXIM(field);
  require(im && XmImSetXIC(field,NULL) && XmImSetXIC(text,NULL), "local XIM/XIC absent; fallback is not composition proof");
  printf("{\"pid\":%ld,\"shell\":%lu,\"field_window\":%lu,\"text_window\":%lu,"
         "\"policy\":\"%s\",\"local_im\":true,\"external_xim_servers\":0,\"xim_locale\":\"%s\"}\n",
         (long)getpid(),XtWindow(shell),XtWindow(field),XtWindow(text),policy,XLocaleOfIM(im));
  fflush(stdout);
  observer = XtAppAddInput(app,STDIN_FILENO,(XtPointer)XtInputReadMask,input,NULL);
  XtAppMainLoop(app);
  XtRemoveInput(observer);
  XtDestroyWidget(shell);
  if (display_widget) XtDestroyWidget(display_widget);
  require(display_destroyed == 1, "native display must retire exactly once");
  XSync(display,False);
  require(!x_errors, "native teardown produced X errors");
  XtCloseDisplay(display);
  XtDestroyApplicationContext(app);
  puts("{\"finished\":true,\"x_errors\":0}");
  return 0;
}
