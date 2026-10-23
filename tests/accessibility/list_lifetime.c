/* Ordinary selection callbacks may destroy List/shell or reenter selection. */
#include <Xm/Accessible.h>
#include <Xm/List.h>
#include <X11/Shell.h>
#include <assert.h>
#include <stdio.h>

typedef struct { Widget shell; Boolean whole_shell, reenter; unsigned int calls; } Context;
static void destroy_selected(Widget widget, XtPointer data, XtPointer call) {
  Context *c = data; (void)call;
  ++c->calls;
  if (c->reenter && c->calls == 1) { XmListSelectPos(widget, 2, True); return; }
  XtDestroyWidget(c->whole_shell ? c->shell : widget);
}
int main(int argc, char **argv) {
  XtAppContext app;
  Display *display;
  unsigned int passed = 0;
  XtToolkitInitialize(); app = XtCreateApplicationContext();
  display = XtOpenDisplay(app, NULL, "ListLifetime", "ListLifetime", NULL, 0, &argc, argv);
  assert(display);
  for (int semantic = 0; semantic < 2; ++semantic)
    for (int whole_shell = 0; whole_shell < 2; ++whole_shell)
      for (int reenter = 0; reenter < 2; ++reenter) {
        Widget shell, list;
        XmString items[2]; XmAccessibility *bridge;
        XmAccessibleId owner, row; XmAccessibleInfo info;
        Context c;
        shell = XtAppCreateShell("ListLifetime", "ListLifetime", applicationShellWidgetClass, display, NULL, 0);
        items[0] = XmStringCreateLocalized("First"); items[1] = XmStringCreateLocalized("Second");
        list = XtVaCreateManagedWidget("list", xmListWidgetClass, shell,
              XmNitems, items, XmNitemCount, 2, XmNselectionPolicy, XmBROWSE_SELECT, NULL);
        XmStringFree(items[0]); XmStringFree(items[1]); XtRealizeWidget(shell);
        bridge = XmAccessibilityCreate(shell, "List lifetime"); assert(bridge);
        owner = XmAccessibilityWidget(bridge, list); assert(owner);
        XmListSelectPos(list, 1, False);
        row = XmAccessibilitySelectedChild(bridge, owner, 0); assert(row);
        c.shell = shell; c.whole_shell = whole_shell; c.reenter = reenter; c.calls = 0;
        XtAddCallback(list, XmNbrowseSelectionCallback, destroy_selected, &c);
        if (semantic)
          assert(XmAccessibilityChangeSelection(bridge, owner, XmACCESSIBLE_CLEAR_SELECTION, 0));
        else XmListSelectPos(list, 2, True);
        assert(c.calls == (reenter ? 2U : 1U));
        assert(!XmAccessibilityQuery(bridge, owner, &info));
        assert(!XmAccessibilityQuery(bridge, row, &info));
        assert(!XmAccessibilityAction(bridge, row, 0));
        XmAccessibilityDestroy(bridge);
        if (!whole_shell) XtDestroyWidget(shell);
        ++passed;
      }
  XtCloseDisplay(display); XtDestroyApplicationContext(app);
  printf("Native list callback destruction: %u cases passed\n", passed);
  return 0;
}
