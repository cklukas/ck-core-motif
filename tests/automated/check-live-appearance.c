/* Installed-consumer proof for the live appearance transaction. */
#include <Xm/Appearance.h>
#include <Xm/Disclosure.h>
#include <Xm/Label.h>
#include <Xm/List.h>
#include <Xm/MessageB.h>
#include <Xm/PushB.h>
#include <Xm/RowColumn.h>
#include <Xm/ScrollBar.h>
#include <Xm/Text.h>
#include <Xm/TextF.h>
#include <Xm/ToggleB.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/Xlib.h>
#include <stdio.h>
#include <string.h>

static XtSetValuesFunc original_push_button_set_values;
static Display *reentrant_display;
static Boolean reentrant_attempted;
static Boolean reentrant_result;
static Boolean reentrant_set_result;

static Boolean
reentrant_set_values(Widget old_widget, Widget request_widget,
                     Widget new_widget, ArgList args, Cardinal *num_args)
{
    Boolean result = original_push_button_set_values(old_widget,
                                                      request_widget,
                                                      new_widget, args,
                                                      num_args);

    if (!reentrant_attempted && reentrant_display != NULL) {
        reentrant_attempted = True;
        reentrant_result = XmApplyAppearanceProfile(
            reentrant_display, XmAPPEARANCE_HIGH_CONTRAST);
        reentrant_set_result = XmSetAppearanceProfile(
            reentrant_display, XmAPPEARANCE_MODERN_LIGHT);
    }
    return result;
}

static int
fail(const char *message)
{
    fprintf(stderr, "live appearance: %s\n", message);
    return 1;
}

static Boolean
text_values_intact(Widget text, Widget text_field)
{
    char *text_value = XmTextGetString(text);
    char *field_value = XmTextFieldGetString(text_field);
    Boolean intact = text_value != NULL && field_value != NULL &&
                     strcmp(text_value, "appearance text") == 0 &&
                     strcmp(field_value, "appearance field") == 0;
    XtFree(text_value);
    XtFree(field_value);
    return intact;
}

static void
value_changed(Widget widget, XtPointer closure, XtPointer call_data)
{
    int *count = (int *)closure;
    (void)widget;
    (void)call_data;
    ++*count;
}

static Pixel
named_pixel(Display *display, const char *name)
{
    XColor exact, screen;
    Colormap colormap = DefaultColormap(display, DefaultScreen(display));
    if (!XAllocNamedColor(display, colormap, name, &screen, &exact))
        return 0;
    return screen.pixel;
}

static Boolean
background_changed(Widget widget, Pixel original)
{
    Pixel current;
    XtVaGetValues(widget, XmNbackground, &current, NULL);
    return current != original;
}

int
main(int argc, char **argv)
{
    XtAppContext app;
    Widget shell, ordinary, explicit, toggle, text, text_field;
    Widget label, list, scroll_bar, menu, menu_item, message_box;
    Widget disclosure, disclosure_label;
    Widget matrix[6];
    Pixel matrix_baseline[6];
    Display *display;
    Pixel baseline_background, light_background, contrast_background;
    Pixel dark_background, classic_background;
    Pixel ordinary_foreground, explicit_foreground;
    Pixel ordinary_expected, red;
    int callback_count = 0;
    XmAppearanceProfile profile;
    CoreClassPart *push_button_core;

    shell = XtVaAppInitialize(&app, "LiveAppearance", NULL, 0,
                              &argc, argv, NULL, NULL);
    display = XtDisplay(shell);
    if (!XmSetAppearanceProfile(display, XmAPPEARANCE_CLASSIC))
        return fail("classic profile selection failed");

    ordinary_expected = named_pixel(display, "#00ff00");
    red = named_pixel(display, "#ff0000");
    ordinary = XmCreatePushButton(shell, "ordinary", NULL, 0);
    explicit = XmCreatePushButton(shell, "explicit", NULL, 0);
    toggle = XmCreateToggleButton(shell, "toggle", NULL, 0);
    text = XmCreateText(shell, "text", NULL, 0);
    text_field = XmCreateTextField(shell, "textField", NULL, 0);
    label = XmCreateLabel(shell, "label", NULL, 0);
    list = XmCreateList(shell, "list", NULL, 0);
    scroll_bar = XmCreateScrollBar(shell, "scrollBar", NULL, 0);
    menu = XmCreatePulldownMenu(shell, "menu", NULL, 0);
    menu_item = XmCreatePushButton(menu, "menuItem", NULL, 0);
    message_box = XmCreateMessageBox(shell, "messageBox", NULL, 0);
    disclosure = XmCreateDisclosure(shell, "disclosure", NULL, 0);
    disclosure_label = XmCreateLabel(XmDisclosureGetContent(disclosure),
                                     "disclosureLabel", NULL, 0);
    {
        XmString item = XmStringCreateLocalized((char *)"appearance item");
        XmListAddItem(list, item, 0);
        XmStringFree(item);
    }
    XtVaSetValues(explicit, XmNforeground, red, NULL);
    XtAddCallback(toggle, XmNvalueChangedCallback, value_changed,
                  (XtPointer)&callback_count);
    XtManageChild(ordinary);
    XtManageChild(explicit);
    XtManageChild(toggle);
    XtManageChild(text);
    XtManageChild(text_field);
    XtManageChild(label);
    XtManageChild(list);
    XtManageChild(scroll_bar);
    XtManageChild(menu_item);
    XtManageChild(message_box);
    XtManageChild(disclosure_label);
    XtManageChild(disclosure);
    XtRealizeWidget(shell);
    XSync(display, False);
    XmTextSetString(text, "appearance text");
    XmTextFieldSetString(text_field, "appearance field");
    if (!text_values_intact(text, text_field))
        return fail("text values could not be initialized");

    XtVaGetValues(ordinary, XmNbackground, &baseline_background, NULL);
    XtVaGetValues(ordinary, XmNforeground, &ordinary_foreground, NULL);
    XtVaGetValues(explicit, XmNforeground, &explicit_foreground, NULL);
    matrix[0] = label;
    matrix[1] = list;
    matrix[2] = scroll_bar;
    matrix[3] = menu_item;
    matrix[4] = message_box;
    matrix[5] = disclosure;
    for (unsigned int i = 0; i < XtNumber(matrix); ++i)
        XtVaGetValues(matrix[i], XmNbackground, &matrix_baseline[i], NULL);
    XmToggleButtonSetState(toggle, True, True);
    if (!XmToggleButtonGetState(toggle) || callback_count != 1)
        return fail("toggle state/callback setup failed");

    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_MODERN_LIGHT) ||
        !XmGetAppearanceProfile(display, &profile) ||
        profile != XmAPPEARANCE_MODERN_LIGHT)
        return fail("modern-light transition failed");
    XtVaGetValues(ordinary, XmNbackground, &light_background, NULL);
    if (light_background == baseline_background)
        return fail("modern-light did not update a realized widget");
    for (unsigned int i = 0; i < XtNumber(matrix); ++i)
        if (!background_changed(matrix[i], matrix_baseline[i]))
            return fail("modern-light did not update the representative matrix");
    XtVaGetValues(ordinary, XmNforeground, &ordinary_foreground, NULL);
    XtVaGetValues(explicit, XmNforeground, &explicit_foreground, NULL);
    if (ordinary_foreground != ordinary_expected ||
        explicit_foreground != red || !XmToggleButtonGetState(toggle) ||
        callback_count != 1 || !text_values_intact(text, text_field))
        return fail("light transition changed owned state or precedence");

    /* A SetValues hook is allowed to call toolkit APIs.  The live profile
     * transaction must reject that nested swap and leave its own snapshot
     * authoritative. */
    push_button_core = &((CoreClassRec *)xmPushButtonWidgetClass)->core_class;
    original_push_button_set_values = push_button_core->set_values;
    push_button_core->set_values = reentrant_set_values;
    reentrant_display = display;
    reentrant_attempted = False;
    reentrant_result = True;
    reentrant_set_result = True;
    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_MODERN_DARK) ||
        !reentrant_attempted || reentrant_result || reentrant_set_result ||
        !XmGetAppearanceProfile(display, &profile) ||
        profile != XmAPPEARANCE_MODERN_DARK) {
        push_button_core->set_values = original_push_button_set_values;
        reentrant_display = NULL;
        return fail("nested appearance transaction was not rejected");
    }
    push_button_core->set_values = original_push_button_set_values;
    reentrant_display = NULL;

    XtVaGetValues(ordinary, XmNbackground, &dark_background, NULL);
    XtVaGetValues(ordinary, XmNforeground, &ordinary_foreground, NULL);
    XtVaGetValues(explicit, XmNforeground, &explicit_foreground, NULL);
    if (dark_background == light_background || ordinary_foreground != ordinary_expected ||
        explicit_foreground != red ||
        !XmToggleButtonGetState(toggle) || callback_count != 1 ||
        !text_values_intact(text, text_field))
        return fail("dark transition changed owned state or precedence");
    for (unsigned int i = 0; i < XtNumber(matrix); ++i)
        if (!background_changed(matrix[i], matrix_baseline[i]))
            return fail("modern-dark did not retain the representative matrix");

    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_HIGH_CONTRAST) ||
        !XmGetAppearanceProfile(display, &profile) ||
        profile != XmAPPEARANCE_HIGH_CONTRAST)
        return fail("high-contrast transition failed");
    XtVaGetValues(ordinary, XmNbackground, &contrast_background, NULL);
    XtVaGetValues(ordinary, XmNforeground, &ordinary_foreground, NULL);
    if (contrast_background != named_pixel(display, "#000000") ||
        ordinary_foreground != ordinary_expected ||
        explicit_foreground != red || !XmToggleButtonGetState(toggle) ||
        callback_count != 1 || !text_values_intact(text, text_field))
        return fail("high-contrast transition changed owned state or precedence");
    for (unsigned int i = 0; i < XtNumber(matrix); ++i)
        if (!background_changed(matrix[i], matrix_baseline[i]))
            return fail("high-contrast did not retain the representative matrix");

    if (!XmApplyAppearanceProfile(display, XmAPPEARANCE_CLASSIC))
        return fail("classic transition failed");
    XtVaGetValues(ordinary, XmNbackground, &classic_background, NULL);
    XtVaGetValues(ordinary, XmNforeground, &ordinary_foreground, NULL);
    if (classic_background != baseline_background ||
        ordinary_foreground != ordinary_expected ||
        !XmToggleButtonGetState(toggle) || callback_count != 1 ||
        !text_values_intact(text, text_field))
        return fail("classic transition did not restore the original value");

    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    puts("PASS WP-05.1 live appearance transaction");
    return 0;
}
