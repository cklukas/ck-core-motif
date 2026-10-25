/* Validate the existing public Motif context-menu entry contract. */
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/Xm.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define CHECK(expr) do { \
    if (!(expr)) { fprintf(stderr, "check failed: %s\n", #expr); return 1; } \
    checks++; \
} while (0)

static unsigned checks;
static unsigned activated;

static void activate(Widget widget, XtPointer closure, XtPointer call_data)
{
    (void)widget;
    (void)closure;
    (void)call_data;
    activated++;
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell;
    Widget menu;
    Widget item;
    XmString label;
    XmString accelerator_text;
    XmString label_value;
    Pixmap icon;
    String accelerator = NULL;
    char *label_text = NULL;
    char *accelerator_value = NULL;
    XEvent event;

    shell = XtVaAppInitialize(&app, "ContextMenuContract", NULL, 0,
                              &argc, argv, NULL,
                              XmNwidth, 240, XmNheight, 120, NULL);
    menu = XmCreatePopupMenu(shell, (char *)"context", NULL, 0);
    label = XmStringCreateLocalized((char *)"Open");
    accelerator_text = XmStringCreateLocalized((char *)"Ctrl+O");
    item = XtVaCreateManagedWidget(
        "open", xmPushButtonWidgetClass, menu,
        XmNlabelType, XmSTRING,
        XmNlabelString, label,
        XmNaccelerator, "Ctrl<Key>O",
        XmNacceleratorText, accelerator_text,
        NULL);
    XmStringFree(label);
    XmStringFree(accelerator_text);
    XtAddCallback(item, XmNactivateCallback, activate, NULL);
    XtRealizeWidget(shell);

    XtVaGetValues(item,
                  XmNlabelString, &label_value,
                  XmNaccelerator, &accelerator,
                  XmNacceleratorText, &accelerator_text,
                  NULL);
    CHECK(label_value != NULL);
    CHECK(XmStringGetLtoR(label_value, XmFONTLIST_DEFAULT_TAG,
                          &label_text));
    CHECK(strcmp(label_text, "Open") == 0);
    CHECK(accelerator != NULL && strcmp(accelerator, "Ctrl<Key>O") == 0);
    CHECK(accelerator_text != NULL);
    CHECK(XmStringGetLtoR(accelerator_text, XmFONTLIST_DEFAULT_TAG,
                          &accelerator_value));
    CHECK(strcmp(accelerator_value, "Ctrl+O") == 0);

    icon = XCreatePixmap(XtDisplay(item), RootWindowOfScreen(XtScreen(item)), 1, 1,
                         DefaultDepthOfScreen(XtScreen(item)));
    XtVaSetValues(item, XmNlabelType, XmPIXMAP, XmNlabelPixmap, icon, NULL);
    XtVaGetValues(item, XmNlabelPixmap, &icon, NULL);
    CHECK(icon != XmUNSPECIFIED_PIXMAP);

    memset(&event, 0, sizeof(event));
    XtCallCallbacks(item, XmNactivateCallback, &event);
    CHECK(activated == 1);

    XFreePixmap(XtDisplay(item), icon);
    XtFree(label_text);
    XtFree(accelerator_value);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    printf("PASS existing context-menu contract: %u checks\n", checks);
    return 0;
}
