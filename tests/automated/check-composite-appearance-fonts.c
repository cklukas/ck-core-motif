/* Composite implementation children must share their owner's font policy. */
#include <Xm/XmAll.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <stdio.h>

#define CHECK(x) do { if (!(x)) { fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #x); return 1; } } while (0)

static XmRenderTable table(Widget w)
{
    XmRenderTable result = NULL;
    XtVaGetValues(w, XmNrenderTable, &result, NULL);
    return result;
}
static int font_height(Widget w)
{
    int result = 0;
    XmRenderTable t = table(w);
    if (t) XmRenderTableGetDefaultFontExtents(t, &result, NULL, NULL);
    return result;
}

int main(int argc, char **argv)
{
    XtAppContext app;
    Display *dpy;
    Widget shell, root, combo, text, list, scale, title, tabs, tab_box;
    XmString caption = XmStringCreateLocalized("Composite title");
    XmFontList classic;
    int classic_height;
    shell = XtVaAppInitialize(&app, "CompositeFonts", NULL, 0, &argc, argv, NULL, NULL);
    dpy = XtDisplay(shell);
    CHECK(XmSetDisplayScaleOverride(dpy, 1.0));
    root = XtVaCreateManagedWidget("root", xmRowColumnWidgetClass, shell, NULL);
    combo = XtVaCreateManagedWidget("combo", xmComboBoxWidgetClass, root,
        XmNcomboBoxType, XmDROP_DOWN_LIST, NULL);
    XtVaGetValues(combo, XmNtextField, &text, XmNlist, &list, NULL);
    CHECK(text && list);
    classic = XmRenderTableCopy(table(combo), NULL, 0);
    classic_height = font_height(combo);
    scale = XtVaCreateManagedWidget("scale", xmScaleWidgetClass, root,
        XmNtitleString, caption, NULL);
    title = XtNameToWidget(scale, "Title");
    tabs = XtVaCreateManagedWidget("tabs", xmTabStackWidgetClass, root, NULL);
    tab_box = XtNameToWidget(tabs, "tabBox");
    CHECK(title && tab_box);
    XtRealizeWidget(shell);
    CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_MODERN_LIGHT));
    CHECK(font_height(combo) == font_height(text));
    CHECK(font_height(combo) == font_height(list));
    CHECK(font_height(scale) == font_height(title));
    CHECK(font_height(tabs) == font_height(tab_box));
    XtVaSetValues(combo, XmNappearanceRole, XmAPPEARANCE_ROLE_PAGE_TITLE, NULL);
    XtVaSetValues(scale, XmNappearanceRole, XmAPPEARANCE_ROLE_PAGE_TITLE, NULL);
    XtVaSetValues(tabs, XmNappearanceRole, XmAPPEARANCE_ROLE_PAGE_TITLE, NULL);
    CHECK(font_height(combo) > classic_height);
    CHECK(font_height(combo) == font_height(text));
    CHECK(font_height(combo) == font_height(list));
    CHECK(font_height(scale) == font_height(title));
    CHECK(font_height(tabs) == font_height(tab_box));
    CHECK(XmSetDisplayScaleOverride(dpy, 1.5));
    CHECK(font_height(combo) == font_height(text));
    CHECK(font_height(combo) == font_height(list));
    CHECK(font_height(scale) == font_height(title));
    CHECK(font_height(tabs) == font_height(tab_box));
    {
        Widget fresh_shell = XtAppCreateShell("fresh", "CompositeFonts", applicationShellWidgetClass, dpy, NULL, 0);
        Widget form = XtVaCreateManagedWidget("form", xmFormWidgetClass, fresh_shell, NULL);
        Widget fresh = XtVaCreateManagedWidget("freshCombo", xmComboBoxWidgetClass, form,
            XmNcomboBoxType, XmDROP_DOWN_LIST,
            XmNappearanceRole, XmAPPEARANCE_ROLE_PAGE_TITLE, NULL);
        Widget fresh_text = NULL;
        Dimension actual_height = 0;
        XtVaGetValues(fresh, XmNtextField, &fresh_text, NULL);
        CHECK(font_height(fresh) == font_height(fresh_text));
        XtRealizeWidget(fresh_shell);
        XtVaGetValues(fresh_text, XmNheight, &actual_height, NULL);
        fprintf(stderr, "fresh combo text height=%u font height=%d\n", actual_height, font_height(fresh_text));
        CHECK(actual_height >= font_height(fresh_text));
        {
            Widget fixed = XtVaCreateWidget("fixedCombo", xmComboBoxWidgetClass, form,
                XmNcomboBoxType, XmDROP_DOWN_LIST, XmNwidth, 177, XmNheight, 96, NULL);
            Dimension width = 0, height = 0;
            XtVaSetValues(fixed, XmNappearanceRole, XmAPPEARANCE_ROLE_PAGE_TITLE, NULL);
            XtVaGetValues(fixed, XmNwidth, &width, XmNheight, &height, NULL);
            CHECK(width == 177 && height == 96);
            XtVaSetValues(fixed, XmNwidth, 0, XmNheight, 0, NULL);
            XtVaGetValues(fixed, XmNwidth, &width, XmNheight, &height, NULL);
            CHECK(width > 0 && height >= font_height(fixed));
        }
        XtDestroyWidget(fresh_shell);
    }
    /* Generated Column labels inherit modern typography, while both parent
     * defaults and per-entry application fonts remain authoritative. */
    {
        Widget column = XtVaCreateManagedWidget("column", xmColumnWidgetClass, root, NULL);
        Widget reference = XtVaCreateManagedWidget("reference", xmLabelWidgetClass, root, NULL);
        Widget entry = XtVaCreateManagedWidget("entry", xmTextFieldWidgetClass, column, NULL);
        Widget label = XtNameToWidget(column, "entry_label");
        Widget custom = XtVaCreateManagedWidget("customColumn", xmColumnWidgetClass, root,
            XmNdefaultEntryLabelRenderTable, classic, NULL);
        Widget custom_entry = XtVaCreateManagedWidget("entry", xmTextFieldWidgetClass, custom, NULL);
        Widget custom_label = XtNameToWidget(custom, "entry_label");
        Widget fixed_entry = XtVaCreateManagedWidget("fixed", xmTextFieldWidgetClass, column,
            XmNentryLabelRenderTable, classic, NULL);
        Widget fixed_label = XtNameToWidget(column, "fixed_label");
        CHECK(entry && custom_entry && fixed_entry && label && custom_label && fixed_label);
        CHECK(font_height(label) == font_height(reference));
        CHECK(font_height(custom_label) == classic_height);
        CHECK(font_height(fixed_label) == classic_height);
        CHECK(XmSetDisplayScaleOverride(dpy, 2.0));
        CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_MODERN_DARK));
        CHECK(font_height(label) == font_height(reference));
        CHECK(font_height(custom_label) == classic_height);
        CHECK(font_height(fixed_label) == classic_height);
        CHECK(XmSetDisplayScaleOverride(dpy, 1.5));
        XtVaSetValues(entry, XmNentryLabelFontList, classic, NULL);
        CHECK(font_height(label) == classic_height);
        CHECK(XmSetDisplayScaleOverride(dpy, 2.0));
        CHECK(font_height(label) == classic_height);
        XtVaSetValues(entry, XmNentryLabelFontList, NULL, NULL);
        CHECK(font_height(label) == font_height(reference));
        CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_MODERN_LIGHT));
        CHECK(font_height(label) == font_height(reference));
        XtVaSetValues(fixed_entry, XmNentryLabelRenderTable, NULL, NULL);
        CHECK(font_height(fixed_label) == font_height(reference));
        CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_CLASSIC));
        CHECK(font_height(fixed_label) == classic_height);
        CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_MODERN_LIGHT));
        CHECK(font_height(fixed_label) == font_height(reference));
        XtVaSetValues(column, XmNdefaultEntryLabelFontList, classic, NULL);
        CHECK(font_height(label) == classic_height);
        CHECK(XmSetDisplayScaleOverride(dpy, 1.5));
        CHECK(font_height(label) == classic_height);
    }
    /* Both resource-database aliases preserve parent font ownership. */
    {
        XrmDatabase database = XrmGetDatabase(dpy);
        const char *names[] = {"xrmColumnFont", "xrmColumnRender"};
        XrmPutStringResource(&database, "*xrmColumnFont.defaultEntryLabelFontList", "fixed");
        XrmPutStringResource(&database, "*xrmColumnRender.defaultEntryLabelRenderTable", "fixed");
        XrmSetDatabase(dpy, database);
        for (unsigned n = 0; n < XtNumber(names); ++n) {
            Widget column = XtVaCreateManagedWidget(names[n], xmColumnWidgetClass, root, NULL);
            Widget entry = XtVaCreateManagedWidget("entry", xmTextFieldWidgetClass, column, NULL);
            Widget label = XtNameToWidget(column, "entry_label");
            int height = font_height(label);
            CHECK(entry && label && height > 0);
            CHECK(XmSetDisplayScaleOverride(dpy, 2.0));
            CHECK(font_height(label) == height);
            CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_MODERN_DARK));
            CHECK(font_height(label) == height);
            CHECK(XmSetDisplayScaleOverride(dpy, 1.5));
        }
    }
    /* A deliberate child font survives subsequent profile transactions. */
    XtVaSetValues(text, XmNrenderTable, classic, NULL);
    CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_MODERN_DARK));
    CHECK(font_height(text) == classic_height);
    CHECK(font_height(list) == font_height(combo));
    /* Setting a composite font explicitly continues to propagate normally. */
    XtVaSetValues(combo, XmNrenderTable, classic, NULL);
    CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_MODERN_LIGHT));
    CHECK(font_height(combo) == classic_height);
    CHECK(font_height(text) == classic_height);
    CHECK(font_height(list) == classic_height);
    CHECK(XmApplyAppearanceProfile(dpy, XmAPPEARANCE_CLASSIC));
    CHECK(font_height(text) == classic_height);
    CHECK(font_height(scale) == font_height(title));
    CHECK(font_height(tabs) == font_height(tab_box));
    XmRenderTableFree(classic);
    XmStringFree(caption);
    XtDestroyWidget(shell);
    XtDestroyApplicationContext(app);
    puts("PASS composite appearance fonts");
    return 0;
}
