/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Native menu roles, real posting/activation, shared panes and popup focus. */
#include <Xm/Accessible.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/Label.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

static unsigned int assertions, activated;
static void require(Boolean value, const char *message) {
  ++assertions;
  if (!value) { fprintf(stderr, "%s\n", message); exit(1); }
}
static void activate(Widget widget, XtPointer closure, XtPointer call) {
  (void)widget; (void)closure; (void)call; ++activated;
}
static void drain(XtAppContext app, Display *display) {
  XSync(display, False);
  while (XtAppPending(app)) XtAppProcessEvent(app, XtIMAll);
}
static XmAccessibleInfo query(XmAccessibility *bridge, Widget widget,
                              unsigned int role, const char *name) {
  XmAccessibleInfo info;
  require(XmAccessibilityQuery(bridge, XmAccessibilityWidget(bridge, widget), &info),
          "native menu query failed");
  require(info.role == role, "wrong native menu role");
  require(info.name && !strcmp(info.name, name), "menu label changed");
  return info;
}
static void role(XmAccessibility *bridge, Widget widget, unsigned int wanted,
                 const char *name) {
  XmAccessibleInfo info = query(bridge, widget, wanted, name);
  XmAccessibleInfoClear(&info);
}
int main(int argc, char **argv) {
  XtAppContext app;
  Widget shell, column, bar, pane, nested, popup, cascade, other_cascade;
  Widget gadget_cascade, item, gadget_item, check, radio, label, popup_item;
  XmAccessibility *bridge;
  XmAccessibleInfo info;
  XmString title;
  XButtonPressedEvent posting;
  require(setlocale(LC_ALL, "") != NULL, "locale unavailable");
  XtSetLanguageProc(NULL, NULL, NULL);
  shell = XtVaAppInitialize(&app, "MenuAccessibility", NULL, 0, &argc, argv,
                            NULL, XmNtitle, "Native menu accessibility", NULL);
  column = XtVaCreateManagedWidget("controls", xmRowColumnWidgetClass, shell, NULL);
  bar = XmCreateMenuBar(column, "menu", NULL, 0); XtManageChild(bar);
  pane = XmCreatePulldownMenu(bar, "fileMenu", NULL, 0);
  nested = XmCreatePulldownMenu(pane, "nestedMenu", NULL, 0);
  cascade = XtVaCreateManagedWidget("File", xmCascadeButtonWidgetClass, bar,
                                    XmNsubMenuId, pane, NULL);
  other_cascade = XtVaCreateManagedWidget("Other", xmCascadeButtonWidgetClass, bar,
                                          XmNsubMenuId, pane, NULL);
  item = XtVaCreateManagedWidget("Open", xmPushButtonWidgetClass, pane, NULL);
  XtAddCallback(item, XmNactivateCallback, activate, NULL);
  gadget_item = XtVaCreateManagedWidget("Save", xmPushButtonGadgetClass, pane, NULL);
  XtAddCallback(gadget_item, XmNactivateCallback, activate, NULL);
  check = XtVaCreateManagedWidget("Grid", xmToggleButtonWidgetClass, pane, NULL);
  radio = XtVaCreateManagedWidget("Mode", xmToggleButtonGadgetClass, pane,
                                  XmNindicatorType, XmONE_OF_MANY, NULL);
  label = XtVaCreateManagedWidget("Section", xmLabelWidgetClass, pane, NULL);
  gadget_cascade = XtVaCreateManagedWidget("More", xmCascadeButtonGadgetClass,
                                           pane, XmNsubMenuId, nested, NULL);
  XtVaCreateManagedWidget("Nested action", xmPushButtonWidgetClass, nested, NULL);
  popup = XmCreatePopupMenu(column, "popup", NULL, 0);
  popup_item = XtVaCreateManagedWidget("Popup action", xmPushButtonWidgetClass,
                                       popup, NULL);
  XtAddCallback(popup_item, XmNactivateCallback, activate, NULL);
  XtRealizeWidget(shell);
  bridge = XmAccessibilityCreate(shell, "Native menu accessibility");
  require(bridge != NULL, "bridge creation failed");
  drain(app, XtDisplay(shell));
  role(bridge, bar, XmACCESSIBLE_MENU_BAR, "menu");
  role(bridge, pane, XmACCESSIBLE_MENU, "fileMenu");
  role(bridge, nested, XmACCESSIBLE_MENU, "nestedMenu");
  role(bridge, popup, XmACCESSIBLE_MENU, "popup");
  role(bridge, popup_item, XmACCESSIBLE_MENU_ITEM, "Popup action");
  role(bridge, item, XmACCESSIBLE_MENU_ITEM, "Open");
  role(bridge, gadget_item, XmACCESSIBLE_MENU_ITEM, "Save");
  role(bridge, check, XmACCESSIBLE_CHECK_MENU_ITEM, "Grid");
  role(bridge, radio, XmACCESSIBLE_RADIO_MENU_ITEM, "Mode");
  role(bridge, label, XmACCESSIBLE_LABEL, "Section");
  info = query(bridge, cascade, XmACCESSIBLE_MENU_ITEM, "File");
  require(info.action_count == 1 && (info.states & XmACCESSIBLE_STATE(42)) &&
              (info.states & XmACCESSIBLE_STATE(9)) &&
              !(info.states & XmACCESSIBLE_STATE(10)), "unposted cascade state");
  XmAccessibleInfoClear(&info);
  require(XmAccessibilityAction(bridge, XmAccessibilityWidget(bridge, cascade), 0),
          "cascade native activation failed");
  drain(app, XtDisplay(shell));
  info = query(bridge, cascade, XmACCESSIBLE_MENU_ITEM, "File");
  require((info.states & XmACCESSIBLE_STATE(10)) != 0, "posted cascade not expanded");
  XmAccessibleInfoClear(&info);
  info = query(bridge, other_cascade, XmACCESSIBLE_MENU_ITEM, "Other");
  require(!(info.states & XmACCESSIBLE_STATE(10)), "shared pane expands wrong owner");
  XmAccessibleInfoClear(&info);
  require(XmAccessibilityFocus(bridge, XmAccessibilityWidget(bridge, gadget_item)),
          "menu gadget focus failed");
  drain(app, XtDisplay(shell));
  info = query(bridge, gadget_item, XmACCESSIBLE_MENU_ITEM, "Save");
  require((info.states & XmACCESSIBLE_STATE(12)) != 0,
          "posted MenuShell leaf focus missing");
  XmAccessibleInfoClear(&info);
  require(XmAccessibilityAction(bridge, XmAccessibilityWidget(bridge, gadget_item), 0),
          "menu gadget native activation failed");
  drain(app, XtDisplay(shell));
  require(activated == 1, "menu native activation count");
  info = query(bridge, cascade, XmACCESSIBLE_MENU_ITEM, "File");
  require(!(info.states & XmACCESSIBLE_STATE(10)), "activation left menu posted");
  XmAccessibleInfoClear(&info);
  require(XmAccessibilityAction(bridge, XmAccessibilityWidget(bridge, cascade), 0),
          "cascade repost failed");
  drain(app, XtDisplay(shell));
  require(XmAccessibilityAction(bridge, XmAccessibilityWidget(bridge, gadget_cascade), 0),
          "nested gadget cascade native activation failed");
  drain(app, XtDisplay(shell));
  info = query(bridge, gadget_cascade, XmACCESSIBLE_MENU_ITEM, "More");
  require((info.states & XmACCESSIBLE_STATE(10)) != 0, "nested cascade not expanded");
  XmAccessibleInfoClear(&info);
  /* Normal native toggle activation must both change state and dismiss menus. */
  require(XmAccessibilityAction(bridge, XmAccessibilityWidget(bridge, check), 0),
          "menu toggle native activation failed");
  drain(app, XtDisplay(shell));
  info = query(bridge, check, XmACCESSIBLE_CHECK_MENU_ITEM, "Grid");
  require((info.states & XmACCESSIBLE_STATE(4)) != 0, "menu toggle did not change");
  XmAccessibleInfoClear(&info);
  memset(&posting, 0, sizeof(posting));
  posting.type = ButtonPress; posting.display = XtDisplay(shell);
  posting.window = XtWindow(column); posting.root = DefaultRootWindow(XtDisplay(shell));
  posting.button = Button3; posting.time = CurrentTime;
  posting.x_root = 40; posting.y_root = 40;
  XmMenuPosition(popup, &posting); XtManageChild(popup);
  drain(app, XtDisplay(shell));
  require(XmAccessibilityAction(bridge, XmAccessibilityWidget(bridge, popup_item), 0),
          "popup menu native activation failed");
  drain(app, XtDisplay(shell));
  require(activated == 2, "popup native activation count");
  XtSetSensitive(item, False);
  require(!XmAccessibilityAction(bridge, XmAccessibilityWidget(bridge, item), 0),
          "insensitive menu item activated");
  require(!XmAccessibilityAction(bridge, XmAccessibilityWidget(bridge, label), 0),
          "passive menu label activated");
  require(!XmAccessibilityAction(bridge, XmAccessibilityWidget(bridge, cascade), 1),
          "invalid cascade action accepted");
  title = XmStringCreateLocalized("Updated title");
  XtVaSetValues(item, XmNlabelString, title, NULL); XmStringFree(title);
  role(bridge, item, XmACCESSIBLE_MENU_ITEM, "Updated title");
  role(bridge, gadget_cascade, XmACCESSIBLE_MENU_ITEM, "More");
  XmAccessibilityDestroy(bridge);
  XtDestroyWidget(shell);
  printf("{\"assertions\":%u,\"native_activations\":%u}\n", assertions, activated);
  return 0;
}
