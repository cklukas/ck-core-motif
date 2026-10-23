/* Native/provider text deltas through the actual optional transport. */
#include <Xm/Accessible.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
typedef struct {
  XtAppContext app;
  Widget shell, field, text;
  XmAccessibility *bridge;
  XmAccessibleId field_id, text_id, custom, password;
  char *value;
  int mode, attempts;
} Fixture;
static const char *scalar_at(const char *p, int offset) {
  while (*p && offset--) {
    ++p;
    while (((unsigned char)*p & 0xc0) == 0x80) ++p;
  }
  return p;
}
static int scalar_length(const char *p) {
  int length = 0;
  while (*p) { p = scalar_at(p, 1); ++length; }
  return length;
}
static Boolean query(XtPointer data, XmAccessibleInfo *i) {
  Fixture *f = data;
  i->role = XmACCESSIBLE_TEXT; i->has_text = True;
  i->name = XtNewString("Projected text");
  i->text_length = scalar_length(f->value); i->caret = i->text_length;
  return True;
}
static Boolean password_query(XtPointer data, XmAccessibleInfo *i) {
  query(data, i); i->role = XmACCESSIBLE_PASSWORD_TEXT; return True;
}
static char *range(XtPointer data, int start, int end) {
  Fixture *f = data;
  const char *first = scalar_at(f->value, start), *last = scalar_at(first, end - start);
  size_t bytes = (size_t)(last - first);
  char *result = XtMalloc(bytes + 1);
  memcpy(result, first, bytes); result[bytes] = 0;
  if (f->mode == 1) {
    f->mode = 0;
    XmAccessibilityRemove(f->bridge, f->custom);
  } else if (f->mode == 2) {
    f->mode = 0;
    XmAccessibilityChanged(f->bridge, f->custom);
  } else if (f->mode == 3) { result[0] = (char)0xff; }
  else if (f->mode == 4) {
    XmAccessibility *bridge = f->bridge;
    f->mode = 0; f->bridge = NULL;
    XmAccessibilityDestroy(bridge);
  }
  return result;
}
static void reject(Widget w, XtPointer d, XtPointer call) {
  (void)w; (void)d;
  ((XmTextVerifyCallbackStruct *)call)->doit = False;
}
static void command(XtPointer data, int *fd, XtInputId *input) {
  Fixture *f = data;
  char line[64];
  (void)fd; (void)input;
  if (!fgets(line, sizeof(line), stdin) || !strncmp(line, "quit", 4)) {
    if (f->bridge) XmAccessibilityDestroy(f->bridge);
    XtDestroyWidget(f->shell);
    XtFree(f->value); exit(0);
  }
  if (!strncmp(line, "insert", 6)) XmTextFieldReplace(f->field, 1, 1, "\316\262\360\237\230\200");
  else if (!strncmp(line, "replace", 7)) XmTextFieldReplace(f->field, 1, 3, "e\314\201");
  else if (!strncmp(line, "clear", 5)) XmTextFieldSetString(f->field, "");
  else if (!strncmp(line, "noop", 4)) {
    XmTextFieldSetString(f->field, ""); XmAccessibilityRefresh(f->bridge);
    XmAccessibilityChanged(f->bridge, f->field_id);
  } else if (!strncmp(line, "caret", 5)) {
    XmTextSetInsertionPosition(f->text, 1);
    XmAccessibilityChanged(f->bridge, f->text_id);
  } else if (!strncmp(line, "readonly", 8)) XmTextReplace(f->text, 1, 2, "z");
  else if (!strncmp(line, "rejected", 8)) {
    XtAddCallback(f->text, XmNmodifyVerifyCallback, reject, NULL);
    XmTextReplace(f->text, 0, 1, "NO");
    XtRemoveCallback(f->text, XmNmodifyVerifyCallback, reject, NULL);
  } else if (!strncmp(line, "long", 4)) {
    char *value = XtMalloc(5002); memset(value, 'a', 5000); value[5000] = 0;
    XmTextSetString(f->text, value); XtFree(value);
  } else if (!strncmp(line, "middle", 6)) XmTextReplace(f->text, 2047, 2048, "\360\237\230\200");
  else if (!strncmp(line, "custom", 6)) {
    XtFree(f->value); f->value = XtNewString("replacement");
    XmAccessibilityChanged(f->bridge, f->custom);
  } else if (!strncmp(line, "unicode", 7)) {
    XtFree(f->value); f->value = XtNewString("r\316\262\360\237\230\200t");
    XmAccessibilityChanged(f->bridge, f->custom);
  } else if (!strncmp(line, "destroy", 7)) {
    XmAccessibleProvider p = {0};
    p.query = query; p.text = range;
    f->custom = XmAccessibilityAdd(f->bridge, f->text_id, &p, f);
    f->mode = 4; XmAccessibilityChanged(f->bridge, f->custom);
  } else if (!strncmp(line, "remove", 6) || !strncmp(line, "nested", 6) || !strncmp(line, "invalid", 7)) {
    f->mode = line[0] == 'r' ? 1 : line[0] == 'n' ? 2 : 3;
    XmAccessibilityChanged(f->bridge, f->custom);
  } else if (!strncmp(line, "recover", 7)) {
    f->mode = 0; XmAccessibilityChanged(f->bridge, f->custom);
  } else if (!strncmp(line, "password", 8)) {
    XmAccessibilityChanged(f->bridge, f->password);
  } else exit(4);
  puts("{\"done\":true}"); fflush(stdout);
}
static void connected(XtPointer data, XtIntervalId *timer) {
  Fixture *f = data; (void)timer;
  if (XmAccessibilityGetStatus(f->bridge) == XmACCESSIBILITY_CONNECTED) {
    printf("{\"bus\":\"%s\",\"field\":%u,\"text\":%u,\"custom\":%u,\"password\":%u}\n",
      XmAccessibilityBusName(f->bridge), f->field_id, f->text_id, f->custom, f->password);
    fflush(stdout);
    XtAppAddInput(f->app, STDIN_FILENO, (XtPointer)XtInputReadMask, command, f);
  } else if (++f->attempts < 100) XtAppAddTimeOut(f->app, 100, connected, f);
  else exit(2);
}
int main(int argc, char **argv) {
  Fixture f = {0}; Widget box; XmAccessibleProvider p = {0};
  if (!setlocale(LC_ALL, "")) return 5;
  XtSetLanguageProc(NULL, NULL, NULL);
  f.shell = XtVaAppInitialize(&f.app, "TextEventFixture", NULL, 0, &argc, argv, NULL, NULL);
  box = XtVaCreateManagedWidget("box", xmRowColumnWidgetClass, f.shell, NULL);
  f.field = XtVaCreateManagedWidget("field", xmTextFieldWidgetClass, box, XmNvalue, "AB", NULL);
  f.text = XtVaCreateManagedWidget("text", xmTextWidgetClass, box, XmNvalue, "abc", XmNeditable, False, NULL);
  XtRealizeWidget(f.shell);
  f.bridge = XmAccessibilityCreate(f.shell, "Text event fixture");
  if (!f.bridge) return 1;
  f.field_id = XmAccessibilityWidget(f.bridge, f.field); f.text_id = XmAccessibilityWidget(f.bridge, f.text);
  f.value = XtNewString("initial"); p.query = query; p.text = range;
  f.custom = XmAccessibilityAdd(f.bridge, f.text_id, &p, &f);
  p.query = password_query;
  f.password = XmAccessibilityAdd(f.bridge, f.text_id, &p, &f);
  XmAccessibilityConnect(f.bridge); XtAppAddTimeOut(f.app, 100, connected, &f);
  XtAppMainLoop(f.app); return 1;
}
