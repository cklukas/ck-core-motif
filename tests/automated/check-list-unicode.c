/* SPDX-License-Identifier: LGPL-2.0-or-later
 * Interpose only the XIM lookup boundary to test valid and malformed commits
 * deterministically. This does not claim live input-method qualification.
 */
#include <Xm/List.h>
#include <Xm/XmIm.h>
#include <locale.h>
#include <stdio.h>
#include <string.h>

static const char *commit;

int XmImMbLookupString(Widget widget, XKeyPressedEvent *event, char *buffer,
                       int capacity, KeySym *keysym, int *status)
{
    size_t length = strlen(commit);
    (void)widget; (void)event; (void)keysym;
    if (length > (size_t)capacity) { *status = XBufferOverflow; return (int)length; }
    memcpy(buffer, commit, length);
    *status = XLookupChars;
    return (int)length;
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, list;
    XmString items[5];
    const char *labels[] = {"alpha", "\303\251clair", "\346\235\261\344\272\254",
                            "\303", "\303\251cole"};
    const char *inputs[] = {"\303\251", "\303\251", "\346\235\261", "\377", "\303", "a"};
    int expected[] = {2, 5, 3, 3, 3, 1};
    unsigned int i;
    int failed = 0;
    XEvent event;
    if (!setlocale(LC_ALL, "")) return 2;
    XtSetLanguageProc(NULL, NULL, NULL);
    shell = XtVaAppInitialize(&app, "ListUnicodeRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    for (i = 0; i < XtNumber(items); ++i)
        items[i] = XmStringCreateLocalized((char *)labels[i]);
    list = XtVaCreateManagedWidget("list", xmListWidgetClass, shell,
        XmNitems, items, XmNitemCount, XtNumber(items),
        XmNmatchBehavior, XmQUICK_NAVIGATE, XmNselectionPolicy, XmBROWSE_SELECT, NULL);
    for (i = 0; i < XtNumber(items); ++i) XmStringFree(items[i]);
    XtRealizeWidget(shell);
    memset(&event, 0, sizeof(event));
    event.xkey.type = KeyPress;
    event.xkey.display = XtDisplay(list);
    event.xkey.window = XtWindow(list);
    XmListSelectPos(list, 1, False);
    XmListSetKbdItemPos(list, 1);
    for (i = 0; i < XtNumber(inputs); ++i) {
        int *positions = NULL, count = 0;
        commit = inputs[i];
        XtCallActionProc(list, "ListQuickNavigate", &event, NULL, 0);
        if (!XmListGetSelectedPos(list, &positions, &count) ||
            count != 1 || positions[0] != expected[i]) {
            fprintf(stderr, "Unicode navigation case %u: selection=%d expected=%d\n",
                    i, count ? positions[0] : 0, expected[i]);
            failed = 1;
        }
        XtFree((char *)positions);
    }
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    if (!failed) puts("PASS multibyte navigation, cycling and malformed/incomplete commits");
    return failed;
}
