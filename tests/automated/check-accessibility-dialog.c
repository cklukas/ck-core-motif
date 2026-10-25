/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/Accessible.h>
#include <Xm/MessageB.h>
#include <Xm/Xm.h>
#include <stdio.h>
#include <string.h>

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, \
                                #condition); return 1; } \
    ++checks; \
} while (0)

static unsigned long checks;

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, dialog, message, ok;
    XmAccessibility *accessibility;
    XmAccessibleInfo info = {0};
    XmAccessibleId dialog_id, message_id, ok_id;
    XmString value = XmStringCreateLocalized((char *)"A dialog needs your attention.");

    shell = XtVaAppInitialize(&app, "AccessibilityDialogRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    dialog = XmCreateErrorDialog(shell, "errorDialog", NULL, 0);
    XtVaSetValues(dialog, XmNmessageString, value, NULL);
    XmStringFree(value);
    XtManageChild(dialog);
    XtRealizeWidget(shell);

    accessibility = XmAccessibilityCreate(shell, "AccessibilityDialogRegression");
    CHECK(accessibility != NULL);
    CHECK(XmAccessibilityAttachShell(accessibility, shell));
    XmAccessibilityRefresh(accessibility);
    dialog_id = XmAccessibilityWidget(accessibility, dialog);
    message = XmMessageBoxGetChild(dialog, XmDIALOG_MESSAGE_LABEL);
    ok = XmMessageBoxGetChild(dialog, XmDIALOG_OK_BUTTON);
    message_id = XmAccessibilityWidget(accessibility, message);
    ok_id = XmAccessibilityWidget(accessibility, ok);
    CHECK(dialog_id != XmACCESSIBLE_NONE && message_id != XmACCESSIBLE_NONE &&
          ok_id != XmACCESSIBLE_NONE);
    CHECK(XmAccessibilityQuery(accessibility, dialog_id, &info));
    CHECK(info.role == XmACCESSIBLE_PANEL);
    XmAccessibleInfoClear(&info);
    CHECK(XmAccessibilityQuery(accessibility, message_id, &info));
    CHECK(info.role == XmACCESSIBLE_LABEL && info.name != NULL &&
          strcmp(info.name, "A dialog needs your attention.") == 0);
    XmAccessibleInfoClear(&info);
    CHECK(XmAccessibilityQuery(accessibility, ok_id, &info));
    CHECK(info.role == XmACCESSIBLE_BUTTON && info.action_count >= 1);
    XmAccessibleInfoClear(&info);

    XmAccessibilityDestroy(accessibility);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS accessibility dialog role/name/action journey: %lu checks\n", checks);
    return 0;
}
