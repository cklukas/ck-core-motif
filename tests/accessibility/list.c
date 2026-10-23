/* Native list identity/selection fixture. External clients use the real bus. */
#include <Xm/Accessible.h>
#include <Xm/List.h>
#include <Xm/ListP.h>
#include <Xm/RowColumn.h>
#include <locale.h>
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
static Widget shell, list, multiple;
static XmAccessibility *bridge;
static XtAppContext app;
static unsigned int attempts, callbacks, rejected;
static XmAccessibleId custom;
static int remove_on_count;
static int callback_start, callback_end, callback_cursor;
static void (*arm_fault)(void);
static void (*clear_fault)(void);
static unsigned int (*fault_hits)(void);
static Boolean custom_query(XtPointer data, XmAccessibleInfo *info) {
  (void)data; info->role = XmACCESSIBLE_LIST; info->name = XtNewString("Rejecting provider");
  info->has_selection = True; info->states = XmACCESSIBLE_STATE(8) | XmACCESSIBLE_STATE(24); return True;
}
static Boolean child_query(XtPointer data, XmAccessibleInfo *info) {
  (void)data; info->role = XmACCESSIBLE_LIST_ITEM; info->name = XtNewString("Custom row"); return True;
}
static int custom_count(XtPointer data) {
  (void)data;
  if (remove_on_count) { remove_on_count = 0; XmAccessibilityRemove(bridge, custom); }
  return 0;
}
static XmAccessibleId custom_selected(XtPointer data, int index) { (void)data; (void)index; return 0; }
static Boolean custom_is_selected(XtPointer data, XmAccessibleId id) { (void)data; (void)id; return False; }
static Boolean custom_change(XtPointer data, XmAccessibleSelectionOperation operation, XmAccessibleId id) {
  (void)data; (void)operation; (void)id; ++rejected; return False;
}
static void selected(Widget w, XtPointer data, XtPointer call) {
  XmListWidget owner = (XmListWidget)w;
  (void)data; (void)call; ++callbacks;
  callback_start = owner->list.StartItem + 1;
  callback_end = owner->list.EndItem + 1;
  callback_cursor = owner->list.CurrentKbdItem + 1;
}
static void state(void) {
  int *positions = NULL, count = 0, k;
  if (list) (void)XmListGetSelectedPos(list, &positions, &count);
  printf("{\"callbacks\":%u,\"callback_start\":%d,\"callback_end\":%d,\"callback_cursor\":%d,\"rejected\":%u,\"fault_hits\":%u,\"selected\":[", callbacks, callback_start, callback_end, callback_cursor, rejected, fault_hits ? fault_hits() : 0);
  for (k = 0; k < count; ++k) printf("%s%d", k ? "," : "", positions[k]);
  XtFree((char *)positions);
  positions = NULL; count = 0;
  if (multiple) (void)XmListGetSelectedPos(multiple, &positions, &count);
  printf("],\"multiple\":[");
  for (k = 0; k < count; ++k) printf("%s%d", k ? "," : "", positions[k]);
  XtFree((char *)positions); printf("]}\n"); fflush(stdout);
}
static void command(XtPointer data, int *fd, XtInputId *input) {
  char line[64]; XmString value;
  (void)data; (void)fd; (void)input;
  if (!fgets(line, sizeof line, stdin) || !strncmp(line, "quit", 4)) {
    XmAccessibilityDestroy(bridge); XtDestroyWidget(shell); exit(0);
  }
  if (!strncmp(line, "fault-clear", 11)) { if (!clear_fault) exit(8); clear_fault(); }
  else if (!strncmp(line, "fault-insert", 12)) {
    if (!arm_fault) exit(8);
    value = XmStringCreateLocalized("Fault"); arm_fault();
    XmListAddItemUnselected(list, value, 1); XmStringFree(value);
  } else if (!strncmp(line, "insert", 6)) {
    value = XmStringCreateLocalized("First"); XmListAddItemUnselected(list, value, 1); XmStringFree(value);
  } else if (!strncmp(line, "delete", 6)) XmListDeletePos(list, 4);
  else if (!strncmp(line, "replace", 7)) {
    value = XmStringCreateLocalized("Alpha"); XmListReplaceItemsPos(list, &value, 1, 2); XmStringFree(value);
  } else if (!strncmp(line, "reorder", 7)) {
    XmString values[4];
    values[0] = XmStringCreateLocalized("Same"); values[1] = XmStringCreateLocalized("Alpha");
    values[2] = XmStringCreateLocalized("Same"); values[3] = XmStringCreateLocalized("\344\270\255");
    XtVaSetValues(list, XmNitems, values, XmNitemCount, 4, NULL);
    for (int k = 0; k < 4; ++k) XmStringFree(values[k]);
  } else if (!strncmp(line, "remove-provider-on-count", 24)) remove_on_count = 1;
  else if (!strncmp(line, "native-select", 13)) XmListSelectPos(list, 2, False);
  else if (!strncmp(line, "disable", 7)) XtSetSensitive(list, False);
  else if (!strncmp(line, "enable", 6)) XtSetSensitive(list, True);
  else if (!strncmp(line, "destroy", 7)) { XtDestroyWidget(list); list = NULL; }
  else if (strncmp(line, "state", 5)) exit(4);
  state();
}
static void connected(XtPointer data, XtIntervalId *timer) {
  (void)data; (void)timer;
  if (XmAccessibilityGetStatus(bridge) == XmACCESSIBILITY_CONNECTED) {
    printf("{\"bus\":\"%s\",\"window\":%lu,\"list\":%u,\"multiple\":%u,\"custom\":%u}\n",
        XmAccessibilityBusName(bridge), XtWindow(shell), XmAccessibilityWidget(bridge, list),
        XmAccessibilityWidget(bridge, multiple), custom); fflush(stdout);
    XtAppAddInput(app, STDIN_FILENO, (XtPointer)XtInputReadMask, command, NULL);
  } else if (++attempts < 100) XtAppAddTimeOut(app, 100, connected, NULL);
  else exit(2);
}
int main(int argc, char **argv) {
  Widget column; XmString items[4];
  XmAccessibleProvider provider = {0}, child_provider = {0};
  void *symbol;
  _Static_assert(sizeof(arm_fault) == sizeof(symbol), "native function pointer transport");
  symbol = dlsym(RTLD_DEFAULT, "motif_list_fault_arm"); memcpy(&arm_fault, &symbol, sizeof arm_fault);
  symbol = dlsym(RTLD_DEFAULT, "motif_list_fault_clear"); memcpy(&clear_fault, &symbol, sizeof clear_fault);
  symbol = dlsym(RTLD_DEFAULT, "motif_list_fault_hits"); memcpy(&fault_hits, &symbol, sizeof fault_hits);
  if (!setlocale(LC_ALL, "")) return 5;
  XtSetLanguageProc(NULL, NULL, NULL);
  shell = XtVaAppInitialize(&app, "NativeList", NULL, 0, &argc, argv, NULL, NULL);
  column = XtVaCreateManagedWidget("controls", xmRowColumnWidgetClass, shell, NULL);
  items[0] = XmStringCreateLocalized("Alpha"); items[1] = XmStringCreateLocalized("Same");
  items[2] = XmStringCreateLocalized("Same"); items[3] = XmStringCreateLocalized("\344\270\255");
  list = XtVaCreateManagedWidget("Document choices", xmListWidgetClass, column,
      XmNitems, items, XmNitemCount, 4, XmNvisibleItemCount, 4, XmNselectionPolicy, XmBROWSE_SELECT, NULL);
  multiple = XtVaCreateManagedWidget("Multiple choices", xmListWidgetClass, column,
      XmNitems, items, XmNitemCount, 4, XmNvisibleItemCount, 4, XmNselectionPolicy, XmEXTENDED_SELECT, NULL);
  for (int k = 0; k < 4; ++k) XmStringFree(items[k]);
  XtAddCallback(list, XmNbrowseSelectionCallback, selected, NULL);
  XtAddCallback(multiple, XmNextendedSelectionCallback, selected, NULL);
  XtRealizeWidget(shell);
  bridge = XmAccessibilityCreate(shell, "Native list fixture");
  if (!bridge) return 1;
  provider.query = custom_query; provider.selection_count = custom_count;
  provider.selected_child = custom_selected; provider.child_selected = custom_is_selected;
  provider.change_selection = custom_change;
  custom = XmAccessibilityAdd(bridge, XmAccessibilityWidget(bridge, column), &provider, NULL);
  child_provider.query = child_query;
  if (!custom || !XmAccessibilityAdd(bridge, custom, &child_provider, NULL)) return 6;
  XmAccessibilityConnect(bridge); XtAppAddTimeOut(app, 100, connected, NULL);
  XtAppMainLoop(app); return 1;
}
