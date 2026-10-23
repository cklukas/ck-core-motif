/* Generic providers may reenter bridge APIs from every callback boundary. */
#include <Xm/Accessible.h>
#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

enum Fault { NONE, COUNT_GROW, COUNT_REMOVE, SELECTED_REPLACE, SELECTED_REMOVE,
  SELECTED_CHANGED, SELECTED_DUPLICATE, FINAL_COUNT_GROW, CHANGE_GROW, REFUSE };
typedef struct {
  XmAccessibility *bridge;
  XmAccessibleId owner, children[5];
  enum Fault fault;
  unsigned int count_calls, selected_calls, change_calls;
  int count, changed;
} Model;
static XmAccessibleProvider child_provider;
static Boolean child_query(XtPointer data, XmAccessibleInfo *info) {
  (void)data; info->role = XmACCESSIBLE_LIST_ITEM; info->name = XtNewString("Row"); return True;
}
static Boolean owner_query(XtPointer data, XmAccessibleInfo *info) {
  (void)data; info->role = XmACCESSIBLE_LIST; info->has_selection = True;
  info->states = XmACCESSIBLE_STATE(8) | XmACCESSIBLE_STATE(24);
  info->name = XtNewString("Provider"); return True;
}
static void append(Model *m) {
  m->children[1] = XmAccessibilityAdd(m->bridge, m->owner, &child_provider, m);
  assert(m->children[1]); m->count = 2;
}
static int count(XtPointer data) {
  Model *m = data; ++m->count_calls;
  if ((m->fault == COUNT_GROW && m->count_calls == 1) ||
      (m->fault == FINAL_COUNT_GROW && m->count_calls == 2)) append(m);
  if (m->fault == COUNT_REMOVE && m->count_calls == 1)
    XmAccessibilityRemove(m->bridge, m->owner);
  return m->count;
}
static XmAccessibleId selected(XtPointer data, int index) {
  Model *m = data; ++m->selected_calls;
  if (m->selected_calls == 1) {
    if (m->fault == SELECTED_REPLACE) {
      XmAccessibilityRemove(m->bridge, m->children[0]);
      m->children[0] = XmAccessibilityAdd(m->bridge, m->owner, &child_provider, m);
      assert(m->children[0]);
    } else if (m->fault == SELECTED_REMOVE) XmAccessibilityRemove(m->bridge, m->owner);
    else if (m->fault == SELECTED_CHANGED) XmAccessibilityChanged(m->bridge, m->owner);
  }
  return m->children[m->fault == SELECTED_DUPLICATE ? 0 : index];
}
static Boolean is_selected(XtPointer data, XmAccessibleId id) {
  Model *m = data; (void)id;
  if (m->fault == SELECTED_REPLACE) {
    XmAccessibilityRemove(m->bridge, m->children[0]);
    m->children[0] = XmAccessibilityAdd(m->bridge, m->owner, &child_provider, m);
  }
  return True;
}
static Boolean change(XtPointer data, XmAccessibleSelectionOperation op, XmAccessibleId id) {
  Model *m = data; (void)op; (void)id; ++m->change_calls;
  if (m->fault == REFUSE) return False;
  if (m->fault == CHANGE_GROW) append(m);
  else m->count = 0;
  m->changed = 1;
  XmAccessibilityChanged(m->bridge, m->owner);
  return True;
}
static void setup(Model *m, XmAccessibility *bridge, enum Fault fault, int n) {
  XmAccessibleProvider p = {0};
  memset(m, 0, sizeof(*m)); m->bridge = bridge; m->fault = fault; m->count = n;
  p.query = owner_query; p.selection_count = count; p.selected_child = selected;
  p.child_selected = is_selected; p.change_selection = change;
  m->owner = XmAccessibilityAdd(bridge, XmAccessibilityRoot(bridge), &p, m);
  assert(m->owner);
  for (int k = 0; k < n; ++k) {
    m->children[k] = XmAccessibilityAdd(bridge, m->owner, &child_provider, m);
    assert(m->children[k]);
  }
}
int main(int argc, char **argv) {
  XtAppContext app; Widget shell; XmAccessibility *bridge; Model m;
  unsigned int passed = 0;
  shell = XtVaAppInitialize(&app, "SelectionReentrancy", NULL, 0, &argc, argv, NULL, NULL);
  bridge = XmAccessibilityCreate(shell, "Generic selection reentrancy"); assert(bridge);
  child_provider.query = child_query;
  /* Safe negative proof on the old implementation: this getter must reject a
     callback that changes the child cohort, even if its count fits the new one. */
  setup(&m, bridge, COUNT_GROW, 1);
  assert(XmAccessibilitySelectionCount(bridge, m.owner) == -1);
  assert(!m.change_calls); XmAccessibilityRemove(bridge, m.owner); ++passed;
  setup(&m, bridge, COUNT_REMOVE, 1);
  assert(!XmAccessibilitySelectedChild(bridge, m.owner, 0));
  assert(!m.selected_calls); ++passed;
  for (enum Fault f = COUNT_GROW; f <= FINAL_COUNT_GROW; ++f) {
    if (f == COUNT_REMOVE) continue;
    setup(&m, bridge, f, f == SELECTED_DUPLICATE ? 2 : 1);
    assert(!XmAccessibilityChangeSelection(bridge, m.owner, XmACCESSIBLE_CLEAR_SELECTION, 0));
    assert(!m.change_calls && !m.changed);
    XmAccessibilityRemove(bridge, m.owner); ++passed;
  }
  setup(&m, bridge, SELECTED_REPLACE, 1);
  assert(!XmAccessibilityChildSelected(bridge, m.owner, m.children[0]));
  XmAccessibilityRemove(bridge, m.owner); ++passed;
  setup(&m, bridge, REFUSE, 1);
  assert(!XmAccessibilityChangeSelection(bridge, m.owner, XmACCESSIBLE_CLEAR_SELECTION, 0));
  assert(m.change_calls == 1 && !m.changed && m.count == 1);
  XmAccessibilityRemove(bridge, m.owner); ++passed;
  setup(&m, bridge, CHANGE_GROW, 1);
  assert(!XmAccessibilityChangeSelection(bridge, m.owner, XmACCESSIBLE_CLEAR_SELECTION, 0));
  assert(m.change_calls == 1); XmAccessibilityRemove(bridge, m.owner); ++passed;
  setup(&m, bridge, NONE, 1);
  assert(XmAccessibilityChangeSelection(bridge, m.owner, XmACCESSIBLE_CLEAR_SELECTION, 0));
  assert(m.change_calls == 1 && m.changed && XmAccessibilitySelectionCount(bridge, m.owner) == 0);
  XmAccessibilityRemove(bridge, m.owner); ++passed;
  XmAccessibilityDestroy(bridge); XtDestroyWidget(shell);
  printf("Generic selection reentrancy: %u cases passed\n", passed);
  return 0;
}
