/* Executable widget/provider/AT-SPI fixture. LGPL-2.0-or-later. */
#include <Xm/Accessible.h>
#include <Xm/Form.h>
#include <Xm/PushB.h>
#include <Xm/TextF.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct {
  XtAppContext app;
  Widget shell, button, entry;
  XmAccessibility *bridge;
  XmAccessibleId table, cell;
  int activated, checks, expect_unavailable;
} Fixture;
static Boolean table_query(XtPointer data, XmAccessibleInfo *info) {
  (void)data;
  info->role = XmACCESSIBLE_TABLE;
  info->rows = info->columns = 1;
  info->name = XtNewString("Semantic table");
  info->states = XmACCESSIBLE_STATE(8) | XmACCESSIBLE_STATE(30);
  return True;
}
static Boolean cell_query(XtPointer data, XmAccessibleInfo *info) {
  (void)data;
  info->role = XmACCESSIBLE_TABLE_CELL;
  info->rows = info->columns = -1;
  info->name = XtNewString("Row one, column one");
  info->has_text = True;
  info->text_length = 3;
  return True;
}
static char *cell_text(XtPointer data, int start, int end) {
  const char *value = "A\303\251\360\237\230\200";
  const int offsets[] = {0, 1, 3, 7};
  char *result;
  int count;
  (void)data;
  if (start < 0 || end < start || end > 3)
    return NULL;
  count = offsets[end] - offsets[start];
  result = XtMalloc((Cardinal)count + 1);
  memcpy(result, value + offsets[start], (size_t)count);
  result[count] = 0;
  return result;
}
static XmAccessibleId table_cell(XtPointer data, int row, int column) {
  Fixture *f = (Fixture *)data;
  return !row && !column ? f->cell : 0;
}
static void activate(Widget widget, XtPointer data, XtPointer call) {
  Fixture *f = (Fixture *)data;
  XmString label;
  (void)call;
  ++f->activated;
  label = XmStringCreateLocalized("Activated by accessibility");
  XtVaSetValues(widget, XmNlabelString, label, NULL);
  XmStringFree(label);
  XmAccessibilityChanged(f->bridge, XmAccessibilityWidget(f->bridge, widget));
}
static void check(XtPointer data, XtIntervalId *id) {
  Fixture *f = (Fixture *)data;
  XmAccessibilityStatus status = XmAccessibilityGetStatus(f->bridge);
  (void)id;
  if (f->expect_unavailable && (status == XmACCESSIBILITY_UNAVAILABLE ||
                                status == XmACCESSIBILITY_NOT_BUILT)) {
    char *slice = XmAccessibilityText(
        f->bridge, XmAccessibilityWidget(f->bridge, f->entry), 1, 3);
    if (!slice || strcmp(slice, "\303\251\360\237\230\200") ||
        !XmAccessibilityAction(
            f->bridge, XmAccessibilityWidget(f->bridge, f->button), 0) ||
        f->activated != 1)
      exit(1);
    XtFree(slice);
    XmAccessibilityDestroy(f->bridge);
    f->bridge = NULL;
    XmTextFieldSetString(f->entry, "Native editing after bridge teardown");
    XtDestroyWidget(f->shell);
    puts("{\"unavailable\":true,\"native_action\":true,\"after_destroy_edit\":"
         "true}");
    exit(0);
  }
  if (status == XmACCESSIBILITY_CONNECTED) {
    printf(
        "{\"bus\":\"%s\",\"button\":%u,\"text\":%u,\"table\":%u,\"cell\":%u}\n",
        XmAccessibilityBusName(f->bridge),
        XmAccessibilityWidget(f->bridge, f->button),
        XmAccessibilityWidget(f->bridge, f->entry), f->table, f->cell);
    fflush(stdout);
    return;
  }
  if (++f->checks >= 50 || status == XmACCESSIBILITY_NOT_BUILT ||
      status == XmACCESSIBILITY_UNAVAILABLE ||
      status == XmACCESSIBILITY_DISCONNECTED) {
    fprintf(stderr, "AT-SPI transport unavailable (status %d)\n", (int)status);
    XmAccessibilityDestroy(f->bridge);
    XtDestroyWidget(f->shell);
    exit(2);
  }
  XtAppAddTimeOut(f->app, 100, check, data);
}
static void finish(XtPointer data, XtIntervalId *id) {
  Fixture *f = (Fixture *)data;
  (void)id;
  XmAccessibilityDestroy(f->bridge);
  XtDestroyWidget(f->shell);
  exit(0);
}
int main(int argc, char **argv) {
  Fixture f;
  Widget form;
  XmAccessibleProvider table_provider, cell_provider;
  XmAccessibleInfo info;
  memset(&f, 0, sizeof(f));
  if (argc == 2 && !strcmp(argv[1], "--expect-unavailable")) {
    f.expect_unavailable = 1;
    argc = 1;
  }
  memset(&table_provider, 0, sizeof(table_provider));
  memset(&cell_provider, 0, sizeof(cell_provider));
  setlocale(LC_ALL, "");
  XtSetLanguageProc(NULL, NULL, NULL);
  f.shell = XtVaAppInitialize(&f.app, "AccessibilityFixture", NULL, 0, &argc,
                              argv, NULL, NULL);
  form = XtVaCreateManagedWidget("form", xmFormWidgetClass, f.shell, NULL);
  f.button =
      XtVaCreateManagedWidget("Activate", xmPushButtonWidgetClass, form, NULL);
  f.entry = XtVaCreateManagedWidget(
      "entry", xmTextFieldWidgetClass, form, XmNtopAttachment, XmATTACH_WIDGET,
      XmNtopWidget, f.button, XmNvalue, "A\303\251\360\237\230\200", NULL);
  XtRealizeWidget(f.shell);
  f.bridge = XmAccessibilityCreate(f.shell, "Motif Accessibility Fixture");
  table_provider.query = table_query;
  table_provider.cell = table_cell;
  cell_provider.query = cell_query;
  cell_provider.text = cell_text;
  f.table = XmAccessibilityAdd(f.bridge, XmAccessibilityWidget(f.bridge, form),
                               &table_provider, &f);
  f.cell = XmAccessibilityAdd(f.bridge, f.table, &cell_provider, &f);
  if (!f.cell || !XmAccessibilityQuery(f.bridge, f.cell, &info) ||
      info.text_length != 3)
    return 1;
  XmAccessibleInfoClear(&info);
  XtAddCallback(f.button, XmNactivateCallback, activate, &f);
  XmAccessibilityConnect(f.bridge);
  XtAppAddTimeOut(f.app, 100, check, &f);
  XtAppAddTimeOut(f.app, 20000, finish, &f);
  XtAppMainLoop(f.app);
  return 0;
}
