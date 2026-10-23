/* SPDX-License-Identifier: LGPL-2.0-or-later */
/* Native controls for an external screen reader; no synthetic AT-SPI events. */
#include <Xm/Xm.h>
#include <Xm/Accessible.h>
#include <Xm/RowColumn.h>
#include <Xm/PushB.h>
#include <Xm/ToggleB.h>
#include <Xm/CascadeB.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ScrollBar.h>
#include <Xm/Disclosure.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static Widget shell, button, toggle, title, editor, readonly_text, list, disclosure, scrollbar;
static XmAccessibility *accessibility;
static unsigned int activations;

static void activated(Widget widget, XtPointer closure, XtPointer data)
{
    (void)widget; (void)closure; (void)data;
    ++activations;
}
static void print_hex(const char *text)
{
    while (*text) printf("%02x", (unsigned char)*text++);
}
static void snapshot(void)
{
    int value, slider, increment, page;
    char *content = XmTextGetString(editor);
    XmScrollBarGetValues(scrollbar, &value, &slider, &increment, &page);
    printf("{\"connected\":%s,\"activations\":%u,\"checked\":%s,\"expanded\":%s,\"scroll\":%d,\"text\":\"",
        XmAccessibilityGetStatus(accessibility) == XmACCESSIBILITY_CONNECTED ? "true" : "false",
        activations, XmToggleButtonGetState(toggle) ? "true" : "false",
        XmDisclosureGetExpanded(disclosure) ? "true" : "false", value);
    print_hex(content); printf("\",\"caret\":%ld,\"focus\":\"%s\"}\n", (long)XmTextGetInsertionPosition(editor), XmGetFocusWidget(shell) ? XtName(XmGetFocusWidget(shell)) : "");
    XtFree(content); fflush(stdout);
}
static void input(XtPointer closure, int *fd, XtInputId *id)
{
    char command[128];
    (void)closure; (void)fd; (void)id;
    if (!fgets(command, sizeof command, stdin) || !strncmp(command, "quit", 4)) {
        XmAccessibilityDestroy(accessibility);
        XtDestroyWidget(shell);
        exit(0);
    }
    if (!strncmp(command, "set-boundary", 12)) {
        XtVaSetValues(editor, XmNwordWrap, False, NULL);
        XmTextSetString(editor, "A\303\251\360\237\230\200 e\314\201. \344\270\255\r\nNext.\n");
    }
    if (!strncmp(command, "set-wrap", 8)) {
        XtVaSetValues(editor, XmNwordWrap, True, XmNwidth, 180, NULL);
        XmTextSetString(editor, "One two three four five six seven eight nine ten.\n");
    }
    snapshot();
}
static void identity(const char *name, Widget widget)
{
    Position x, y; Dimension width, height;
    XtTranslateCoords(widget, 0, 0, &x, &y);
    XtVaGetValues(widget, XmNwidth, &width, XmNheight, &height, NULL);
    printf(",\"%s\":{\"accessible\":%u,\"window\":%lu,\"x\":%d,\"y\":%d,\"width\":%u,\"height\":%u}",
           name, XmAccessibilityWidget(accessibility, widget), XtWindow(widget), (int)x, (int)y, (unsigned int)width, (unsigned int)height);
}
int main(int argc, char **argv)
{
    XtAppContext app;
    Widget column, menus, file_menu, file, item, details;
    XmString items[3]; XmFontListEntry entry; XmFontList fonts;
    if (!setlocale(LC_ALL, "")) return 2;
    XtSetLanguageProc(NULL, NULL, NULL);
    shell = XtVaAppInitialize(&app, "NativeControls", NULL, 0, &argc, argv, NULL,
                             XmNtitle, "Native Motif Controls", NULL);
    entry = XmFontListEntryLoad(XtDisplay(shell), "DejaVu Sans", XmFONT_IS_XFT, XmFONTLIST_DEFAULT_TAG);
    if (!entry) return 3;
    fonts = XmFontListAppendEntry(NULL, entry); XmFontListEntryFree(&entry);
    column = XtVaCreateManagedWidget("controls", xmRowColumnWidgetClass, shell,
                                    XmNorientation, XmVERTICAL, XmNspacing, 8, NULL);
    menus = XmCreateMenuBar(column, "menu", NULL, 0); XtManageChild(menus);
    file_menu = XmCreatePulldownMenu(menus, "fileMenu", NULL, 0);
    item = XtVaCreateManagedWidget("Open document", xmPushButtonWidgetClass, file_menu, NULL);
    XtAddCallback(item, XmNactivateCallback, activated, NULL);
    file = XtVaCreateManagedWidget("File", xmCascadeButtonWidgetClass, menus,
                                  XmNsubMenuId, file_menu, XmNmnemonic, (KeySym)'F', NULL);
    button = XtVaCreateManagedWidget("Apply changes", xmPushButtonWidgetClass, column,
                                    XmNnavigationType, XmTAB_GROUP, NULL);
    XtAddCallback(button, XmNactivateCallback, activated, NULL);
    toggle = XtVaCreateManagedWidget("Show grid", xmToggleButtonWidgetClass, column,
                                    XmNnavigationType, XmTAB_GROUP, NULL);
    title = XtVaCreateManagedWidget("Document title", xmTextFieldWidgetClass, column,
                                   XmNfontList, fonts, XmNcolumns, 35,
                                   XmNvalue, "Quarterly notes", XmNnavigationType, XmTAB_GROUP, NULL);
    editor = XtVaCreateManagedWidget("Document editor", xmTextWidgetClass, column,
                                    XmNfontList, fonts, XmNcolumns, 35, XmNrows, 3,
                                    XmNvalue, "Hello e\314\201 \344\270\255 \360\237\231\202",
                                    XmNeditMode, XmMULTI_LINE_EDIT, XmNnavigationType, XmTAB_GROUP, NULL);
    readonly_text = XtVaCreateManagedWidget("Read only status", xmTextFieldWidgetClass, column,
                                           XmNfontList, fonts, XmNcolumns, 35, XmNeditable, False,
                                           XmNvalue, "Ready", XmNnavigationType, XmTAB_GROUP, NULL);
    items[0] = XmStringCreateLocalized("Alpha"); items[1] = XmStringCreateLocalized("Beta");
    items[2] = XmStringCreateLocalized("Gamma");
    list = XtVaCreateManagedWidget("Document choices", xmListWidgetClass, column,
                                  XmNitems, items, XmNitemCount, 3, XmNvisibleItemCount, 3,
                                  XmNselectionPolicy, XmBROWSE_SELECT, XmNnavigationType, XmTAB_GROUP, NULL);
    XmStringFree(items[0]); XmStringFree(items[1]); XmStringFree(items[2]);
    disclosure = XtVaCreateManagedWidget("Details", xmDisclosureWidgetClass, column, NULL);
    details = XtVaCreateManagedWidget("Inspect details", xmPushButtonWidgetClass,
                                      XmDisclosureGetContent(disclosure), XmNnavigationType, XmTAB_GROUP, NULL);
    scrollbar = XtVaCreateManagedWidget("Zoom level", xmScrollBarWidgetClass, column,
                                       XmNorientation, XmHORIZONTAL, XmNminimum, 0, XmNmaximum, 100,
                                       XmNsliderSize, 10, XmNvalue, 20, XmNheight, 28, XmNtraversalOn, True,
                                       XmNnavigationType, XmTAB_GROUP, NULL);
    XmFontListFree(fonts); XtRealizeWidget(shell);
    accessibility = XmAccessibilityCreate(shell, "Native Motif Controls");
    if (!accessibility || !XmAccessibilityConnect(accessibility)) return 4;
    printf("{\"pid\":%ld,\"shell\":%lu,\"bus\":\"%s\"", (long)getpid(), XtWindow(shell), XmAccessibilityBusName(accessibility));
    identity("button", button); identity("toggle", toggle); identity("title", title);
    identity("editor", editor); identity("readonly", readonly_text); identity("list", list);
    identity("disclosure", XmDisclosureGetHeader(disclosure)); identity("details", details);
    identity("scrollbar", scrollbar); identity("file", file); identity("menuitem", item);
    printf("}\n"); fflush(stdout);
    XtAppAddInput(app, STDIN_FILENO, (XtPointer)XtInputReadMask, input, NULL);
    XtAppMainLoop(app);
    return 0;
}
