/* Observe native widgets; all composition is supplied by a real XIM service. */
#include <Xm/Xm.h>
#include <Xm/RowColumn.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/TextP.h>
#include <Xm/TextFP.h>
#include <Xm/XmIm.h>
#include <locale.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

static Widget shell, text, field;
static unsigned int text_changes, field_changes;

static void changed(Widget widget, XtPointer closure, XtPointer call_data)
{
    unsigned int *count = (unsigned int *)closure;
    (void)widget;
    (void)call_data;
    ++*count;
}

static void hex(const char *value)
{
    for (; *value; ++value) printf("%02x", (unsigned char)*value);
}

static void snapshot(void)
{
    char *t = XmTextGetString(text), *f = XmTextFieldGetString(field);
    XmTextWidget tw = (XmTextWidget)text;
    XmTextFieldWidget fw = (XmTextFieldWidget)field;
    printf("{\"text\":\""); hex(t);
    printf("\",\"field\":\""); hex(f);
    printf("\",\"tp\":%d,\"fp\":%d,\"tv\":%u,\"fv\":%u,"
           "\"tl\":%ld,\"fl\":%ld,\"tc\":%ld,\"fc\":%ld,"
           "\"te\":%ld,\"fe\":%ld,\"xim\":%s}\n",
           tw->text.onthespot->under_preedit, fw->text.onthespot->under_preedit,
           text_changes, field_changes,
           (long)XmTextGetLastPosition(text), (long)XmTextFieldGetLastPosition(field),
           (long)XmTextGetInsertionPosition(text), (long)XmTextFieldGetInsertionPosition(field),
           (long)tw->text.onthespot->end, (long)fw->text.onthespot->end,
           XmImGetXIM(field) ? "true" : "false");
    XtFree(t); XtFree(f); fflush(stdout);
}

static void input(XtPointer closure, int *fd, XtInputId *id)
{
    char line[80];
    (void)closure; (void)fd; (void)id;
    if (!fgets(line, sizeof line, stdin) || !strncmp(line, "quit", 4)) {
        XtDestroyWidget(shell);
        exit(0);
    }
    /* Native focus traversal, never direct preedit callback invocation. */
    if (!strncmp(line, "focus-field", 11)) XmProcessTraversal(field, XmTRAVERSE_CURRENT);
    if (!strncmp(line, "focus-text", 10)) XmProcessTraversal(text, XmTRAVERSE_CURRENT);
    if (!strncmp(line, "clear", 5)) {
        XmTextSetString(text, "");
        XmTextFieldSetString(field, "");
    }
    snapshot();
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget box;
    XmFontListEntry entry;
    XmFontList fonts;
    String fallback[] = {"*preeditType: OnTheSpot", "*inputMethod: ibus", NULL};
    if (!setlocale(LC_ALL, "")) return 2;
    XtSetLanguageProc(NULL, NULL, NULL);
    shell = XtVaAppInitialize(&app, "XimProbe", NULL, 0, &argc, argv, fallback, NULL);
    if (getenv("XM_XIM_PER_WIDGET")) XtVaSetValues(shell, XmNinputPolicy, XmPER_WIDGET, NULL);
    entry = XmFontListEntryLoad(XtDisplay(shell), "DejaVu Sans", XmFONT_IS_XFT, XmFONTLIST_DEFAULT_TAG);
    if (!entry) return 3;
    fonts = XmFontListAppendEntry(NULL, entry);
    XmFontListEntryFree(&entry);
    box = XtVaCreateManagedWidget("box", xmRowColumnWidgetClass, shell, NULL);
    field = XtVaCreateManagedWidget("field", xmTextFieldWidgetClass, box,
                                   XmNcolumns, 35, XmNfontList, fonts, NULL);
    text = XtVaCreateManagedWidget("text", xmTextWidgetClass, box, XmNcolumns, 35,
                                  XmNrows, 4, XmNeditMode, XmMULTI_LINE_EDIT,
                                  XmNfontList, fonts, NULL);
    XmFontListFree(fonts);
    XtAddCallback(text, XmNvalueChangedCallback, changed, &text_changes);
    XtAddCallback(field, XmNvalueChangedCallback, changed, &field_changes);
    XtRealizeWidget(shell);
    XmProcessTraversal(field, XmTRAVERSE_CURRENT);
    printf("{\"pid\":%ld,\"shell\":%lu,\"field_window\":%lu,\"text_window\":%lu}\n",
           (long)getpid(), XtWindow(shell), XtWindow(field), XtWindow(text));
    fflush(stdout);
    XtAppAddInput(app, STDIN_FILENO, (XtPointer)XtInputReadMask, input, NULL);
    XtAppMainLoop(app);
    return 0;
}
