/* Replacing after capacity growth must preserve the gap invariant. */
#include <Xm/XmAll.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main(int argc, char **argv) {
    XtAppContext app;
    setlocale(LC_ALL, "");
    Widget shell = XtVaAppInitialize(&app, "TextReplaceGap", NULL, 0,
                                    &argc, argv, NULL, NULL);
    Widget text = XtVaCreateManagedWidget("text", xmTextWidgetClass, shell,
                                         XmNeditable, False, NULL);
    const size_t sizes[] = {607, 2632, 607, 12000, 1, 32768, 0, 4000, 607};
    for (unsigned pass = 0; pass < 3; ++pass) {
        for (unsigned i = 0; i < XtNumber(sizes); ++i) {
            char *value = malloc(sizes[i] + 1);
            if (!value) return 2;
            memset(value, 'a' + i, sizes[i]); value[sizes[i]] = 0;
            XmTextSetString(text, value);
            char *actual = XmTextGetString(text);
            if (!actual || strcmp(value, actual)) {
                fprintf(stderr, "replacement %u/%u expected %zu bytes, got %zu\n",
                        pass, i, sizes[i], actual ? strlen(actual) : 0);
                return 1;
            }
            XtFree(actual); free(value);
        }
    }
    XtDestroyWidget(shell);
    puts("PASS text replacement across gap growth and compaction");
    return 0;
}
