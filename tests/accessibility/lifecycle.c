/* Native live-tree indexing, allocation and callback lifetime regression. */
#define _POSIX_C_SOURCE 200809L
#include "../../lib/Xm/AccessibleI.h"
#include <Xm/DrawingA.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/ArrowB.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

static void require(int value, const char *message) {
  if (!value) { fprintf(stderr, "%s\n", message); exit(1); }
}
static double now(void) {
  struct timespec value;
  require(!clock_gettime(CLOCK_MONOTONIC, &value), "monotonic clock");
  return value.tv_sec + value.tv_nsec / 1e9;
}
struct ButtonContract { int arrow, calls; };
static void button_callback(Widget widget, XtPointer data, XtPointer call) {
  struct ButtonContract *value = data;
  int reason, clicks;
  XEvent *event;
  require(call != NULL, "native button callback data missing");
  if (value->arrow) {
    XmArrowButtonCallbackStruct *native = call;
    reason = native->reason; event = native->event; clicks = native->click_count;
  } else {
    XmPushButtonCallbackStruct *native = call;
    reason = native->reason; event = native->event; clicks = native->click_count;
  }
  require(reason == XmCR_ACTIVATE && event == NULL && clicks == 1,
          "semantic activation violated complete native button callback contract");
  if (++value->calls == 2) XtDestroyWidget(widget);
}
static void button_contracts(XmAccessibility *bridge, Widget parent) {
  WidgetClass classes[] = {xmPushButtonWidgetClass, xmPushButtonGadgetClass,
                           xmArrowButtonWidgetClass};
  size_t index;
  for (index = 0; index < XtNumber(classes); ++index) {
    struct ButtonContract value = {index == 2, 0};
    Widget button = XtVaCreateManagedWidget("nativeContract", classes[index],
                                           parent, NULL);
    XmAccessibleId id;
    XmAccessibleInfo info;
    XtAddCallback(button, XmNactivateCallback, button_callback, &value);
    XmAccessibilityRefresh(bridge);
    id = XmAccessibilityWidget(bridge, button);
    require(id && XmAccessibilityAction(bridge, id, 0) && value.calls == 1,
            "first native button callback failed");
    require(XmAccessibilityAction(bridge, id, 0) && value.calls == 2,
            "destroying native button callback failed");
    require(!XmAccessibilityQuery(bridge, id, &info) &&
                !XmAccessibilityAction(bridge, id, 0),
            "destroyed native button remained actionable");
  }
  puts("Full native widget/gadget/arrow callback contracts and self-destruction passed");
}
static Boolean query(XtPointer data, XmAccessibleInfo *info) {
  require(*(int *)data == 1, "queried released provider payload");
  info->role = XmACCESSIBLE_LABEL;
  info->name = XtNewString("Semantic item");
  return True;
}
static size_t allocated(XmAccessibility *bridge) {
  XmAccessibleNode *node;
  size_t bytes = sizeof(*bridge) + bridge->node_count * sizeof(*node) +
      (bridge->id_capacity + bridge->widget_capacity) * sizeof(node);
  for (node = bridge->nodes; node; node = node->next)
    bytes += node->child_capacity * sizeof(node);
  for (node = bridge->retired_nodes; node; node = node->retired_next)
    bytes += sizeof(*node);
  return bytes;
}
static void flat(XmAccessibility *bridge, XmAccessibleId canvas,
                 const XmAccessibleProvider *provider, int *alive,
                 size_t count, XmAccessibleId *last) {
  XmAccessibleId parent = XmAccessibilityAdd(bridge, canvas, provider, alive);
  XmAccessibleId *ids = calloc(count, sizeof(*ids));
  XmAccessibleInfo info;
  size_t index, bytes;
  double start, created, queried, removed;
  require(parent > *last && ids, "parent admission/allocation");
  start = now();
  for (index = 0; index < count; ++index) {
    ids[index] = XmAccessibilityAdd(bridge, parent, provider, alive);
    require(ids[index] > *last, "semantic IDs must remain monotonic");
    *last = ids[index];
  }
  created = now();
  bytes = allocated(bridge);
  require(_XmAccessibilityChildCount(bridge, parent) == count, "child count");
  for (index = 0; index < count; ++index) {
    require(_XmAccessibilityChildAt(bridge, parent, index) == ids[index],
            "registration order changed");
    require(_XmAccessibilityIndexInParent(bridge, ids[index]) == (int)index,
            "child index changed");
    require(XmAccessibilityQuery(bridge, ids[index], &info), "live node missing");
    XmAccessibleInfoClear(&info);
  }
  queried = now();
  XmAccessibilityRemove(bridge, parent);
  removed = now();
  for (index = 0; index < count; ++index)
    require(!XmAccessibilityQuery(bridge, ids[index], &info), "removed ID remained live");
  require(bridge->id_capacity == 64 && !bridge->retired_nodes,
          "empty subtree retained historical ID table or retired storage");
  printf("nodes=%zu create=%.9f query=%.9f remove=%.9f live_bytes=%zu after_bytes=%zu node_bytes=%zu\n",
         count, created - start, queried - created, removed - queried,
         bytes, allocated(bridge), sizeof(XmAccessibleNode));
  fflush(stdout);
  free(ids);
}
struct Reentrant {
  XmAccessibility *bridge;
  XmAccessibleId id;
  XtAppContext app;
  int calls;
};
static Boolean remove_query(XtPointer data, XmAccessibleInfo *info) {
  struct Reentrant *value = data;
  ++value->calls;
  XmAccessibilityRemove(value->bridge, value->id);
  require(!_XmAccessibilityNode(value->bridge, value->id), "removed callback ID still indexed");
  require(value->bridge->retired_nodes != NULL, "callback node freed before dispatch returned");
  info->name = XtNewString("Removed during query");
  return True;
}
static Boolean action_query(XtPointer data, XmAccessibleInfo *info) {
  (void)data;
  info->name = XtNewString("Destroy bridge action");
  info->action_count = 1;
  return True;
}
static Boolean destroy_action(XtPointer data, unsigned int action) {
  struct Reentrant *value = data;
  (void)action;
  XmAccessibilityDestroy(value->bridge);
  require(value->bridge->destroy_requested && !value->bridge->deferred_destroy,
          "nested destruction scheduled a reentrant zero-timeout loop");
  while (XtAppPending(value->app)) XtAppProcessEvent(value->app, XtIMAll);
  ++value->calls;
  return True;
}
int main(int argc, char **argv) {
  XtAppContext app;
  Widget shell, canvas;
  XmAccessibility *bridge;
  XmAccessibleProvider provider = {0}, reentrant = {0};
  XmAccessibleId canvas_id, last = 0, parent, children[4], chain;
  XmAccessibleInfo info;
  size_t index, native_bytes;
  int alive = 1;
  struct Reentrant value;
  shell = XtVaAppInitialize(&app, "SemanticLifecycle", NULL, 0, &argc, argv, NULL, NULL);
  canvas = XtVaCreateManagedWidget("canvas", xmDrawingAreaWidgetClass, shell,
                                   XmNwidth, 120, XmNheight, 80, NULL);
  XtRealizeWidget(shell);
  bridge = XmAccessibilityCreate(shell, "Semantic lifecycle fixture");
  require(bridge != NULL, "native bridge");
  canvas_id = XmAccessibilityWidget(bridge, canvas);
  require(canvas_id, "native widget index");
  button_contracts(bridge, canvas);
  native_bytes = allocated(bridge);
  provider.query = query;
  flat(bridge, canvas_id, &provider, &alive, 1000, &last);
  flat(bridge, canvas_id, &provider, &alive, 10000, &last);
  parent = XmAccessibilityAdd(bridge, canvas_id, &provider, &alive);
  for (index = 0; index < 4; ++index)
    children[index] = XmAccessibilityAdd(bridge, parent, &provider, &alive);
  XmAccessibilityRemove(bridge, children[1]);
  require(_XmAccessibilityChildCount(bridge, parent) == 3 &&
              _XmAccessibilityChildAt(bridge, parent, 1) == children[2] &&
              _XmAccessibilityIndexInParent(bridge, children[3]) == 2,
          "middle-child removal lost surviving order/index");
  require(!XmAccessibilityQuery(bridge, children[1], &info), "middle-child stale ID");
  XmAccessibilityRemove(bridge, parent);
  parent = chain = XmAccessibilityAdd(bridge, canvas_id, &provider, &alive);
  for (index = 0; index < 20000; ++index) {
    chain = XmAccessibilityAdd(bridge, chain, &provider, &alive);
    require(chain > last, "deep-tree admission or monotonic ID");
    last = chain;
  }
  XmAccessibilityRemove(bridge, parent);
  require(!XmAccessibilityQuery(bridge, chain, &info) &&
              bridge->id_capacity == 64 && allocated(bridge) == native_bytes,
          "iterative deep-tree teardown retained nodes/index/child allocations");
  value.bridge = bridge; value.app = app; value.calls = 0;
  reentrant.query = remove_query;
  /* Set ID after registration while disconnected: no premature provider query. */
  value.id = XmAccessibilityAdd(bridge, canvas_id, &reentrant, &value);
  require(value.id && !XmAccessibilityQuery(bridge, value.id, &info),
          "self-removing query falsely succeeded");
  require(value.calls == 1 && !bridge->retired_nodes, "retired callback node leaked");
  reentrant.query = action_query; reentrant.action = destroy_action;
  value.id = XmAccessibilityAdd(bridge, canvas_id, &reentrant, &value);
  require(XmAccessibilityAction(bridge, value.id, 0), "destroy action failed");
  require(value.calls == 2 && bridge->deferred_destroy,
          "outer callback did not schedule deferred bridge destruction");
  XtAppProcessEvent(app, XtIMTimer); /* Bridge is no longer usable afterward. */
  alive = 0;
  XtDestroyWidget(shell);
  puts("Ordered live indexes, deep teardown, bounded allocations and reentrant provider lifetimes passed");
  return 0;
}
