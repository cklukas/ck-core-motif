/* SPDX-License-Identifier: LGPL-2.0-or-later */
#include <Xm/Label.h>
#include <Xm/Notebook.h>
#include <limits.h>
#include <stdio.h>

#define LOW_PAGE (INT_MIN + 100)
#define HIGH_PAGE (INT_MAX - 100)

#define CHECK(condition) do { \
    if (!(condition)) { fprintf(stderr, "%s:%d: %s\n", __FILE__, __LINE__, \
                                #condition); return 1; } \
    ++checks; \
} while (0)

static unsigned long checks;

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, notebook, low, high;
    XmNotebookPageInfo info;

    shell = XtVaAppInitialize(&app, "NotebookOrderRegression", NULL, 0,
                              &argc, argv, NULL, NULL);
    notebook = XmVaCreateManagedNotebook(shell, "notebook",
                                         XmNfirstPageNumber, LOW_PAGE,
                                         XmNlastPageNumber, HIGH_PAGE,
                                         NULL);
    low = XtVaCreateManagedWidget("low", xmLabelWidgetClass, notebook,
                                 XmNnotebookChildType, XmPAGE,
                                 XmNpageNumber, LOW_PAGE, NULL);
    high = XtVaCreateManagedWidget("high", xmLabelWidgetClass, notebook,
                                  XmNnotebookChildType, XmPAGE,
                                  XmNpageNumber, HIGH_PAGE, NULL);
    (void)low;
    (void)high;
    XtRealizeWidget(shell);

    XmNotebookPageStatus low_status = XmNotebookGetPageInfo(notebook, LOW_PAGE, &info);
    CHECK(low_status == XmPAGE_FOUND);
    CHECK(info.page_widget == low);
    XmNotebookPageStatus high_status = XmNotebookGetPageInfo(notebook, HIGH_PAGE, &info);
    CHECK(high_status == XmPAGE_FOUND);
    CHECK(info.page_widget == high);

    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS Notebook extreme page ordering: %lu checks\n", checks);
    return 0;
}
