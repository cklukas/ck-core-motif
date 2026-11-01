/* Semantic role resources must agree at construction and after live updates. */
#include <Xm/XmAll.h>
#include <Xm/XmP.h>
#include <Xm/Appearance.h>
#include <Xm/Display.h>
#include <Xm/TabBox.h>
#include <Mrm/MrmPublic.h>
#include <X11/Shell.h>
#include <stdio.h>
#include <string.h>

#define CHECK(test) do { if (!(test)) { \
    fprintf(stderr, "FAIL line %d: %s\n", __LINE__, #test); return 1; } } while (0)

static int height(Widget widget)
{
    XmRenderTable table = NULL;
    int result = 0;
    XtVaGetValues(widget, XmNrenderTable, &table, NULL);
    if (table) XmRenderTableGetDefaultFontExtents(table, &result, NULL, NULL);
    return result;
}
static Dimension dimension(Widget widget, const char *resource)
{
    Dimension value = 0;
    XtVaGetValues(widget, resource, &value, NULL);
    return value;
}
static unsigned char role(Widget widget)
{
    unsigned char value = 255;
    XtVaGetValues(widget, XmNappearanceRole, &value, NULL);
    return value;
}
int main(int argc, char **argv)
{
    XtAppContext app;
    Display *display;
    Widget shell, root, normal, title, live, gadget, from_resource, explicit_font;
    Widget font_board, inherited_label, inherited_button, inherited_text, tab_box;
    XmAppearanceProfile profile;
    XmRenderTable saved = NULL;
    XrmDatabase database;
    int classic_height, body_height, title_height;
    CHECK(argc == 3);
    String uid_files[] = {argv[2]};
    profile = !strcmp(argv[1], "light") ? XmAPPEARANCE_MODERN_LIGHT :
        !strcmp(argv[1], "dark") ? XmAPPEARANCE_MODERN_DARK : XmAPPEARANCE_HIGH_CONTRAST;
    argc = 1;
    MrmInitialize();
    XtToolkitInitialize();
    app = XtCreateApplicationContext();
    display = XtOpenDisplay(app, NULL, "roles", "Roles", NULL, 0, &argc, argv);
    CHECK(display);
    CHECK(XmSetDisplayScaleOverride(display, 1.0));
    CHECK(XmToolkitABI() == XmABI_VERSION);
    database = XrmGetDatabase(display);
    XrmPutStringResource(&database, "Roles*fromResource.appearanceRole", "page_title");
    XrmSetDatabase(display, database);
    shell = XtAppCreateShell("roles", "Roles", applicationShellWidgetClass, display, NULL, 0);
    root = XtVaCreateManagedWidget("root", xmRowColumnWidgetClass, shell, NULL);
    normal = XtVaCreateManagedWidget("normal", xmLabelWidgetClass, root, NULL);
    classic_height = height(normal);
    CHECK(classic_height > 0);
    XtVaGetValues(normal, XmNrenderTable, &saved, NULL);
    saved = XmRenderTableCopy(saved, NULL, 0);
    CHECK(saved);
    CHECK(XmApplyAppearanceProfile(display, profile));
    tab_box = XtVaCreateWidget("tabBox", xmTabBoxWidgetClass, root, NULL);
    body_height = height(normal);
    CHECK(body_height > 0);
    title = XtVaCreateManagedWidget("title", xmLabelWidgetClass, root,
        XmNappearanceRole, XmAPPEARANCE_ROLE_PAGE_TITLE, NULL);
    live = XtVaCreateManagedWidget("live", xmLabelWidgetClass, root, NULL);
    gadget = XtVaCreateManagedWidget("gadget", xmLabelGadgetClass, root, NULL);
    from_resource = XtVaCreateManagedWidget("fromResource", xmLabelWidgetClass, root, NULL);
    explicit_font = XtVaCreateManagedWidget("explicit", xmLabelWidgetClass, root,
        XmNfontList, saved, XmNappearanceRole, XmAPPEARANCE_ROLE_PAGE_TITLE, NULL);
    CHECK(role(title) == XmAPPEARANCE_ROLE_PAGE_TITLE);
    CHECK(role(from_resource) == XmAPPEARANCE_ROLE_PAGE_TITLE);
    title_height = height(title);
    CHECK(title_height > body_height);
    CHECK(height(from_resource) == title_height);
    CHECK(height(explicit_font) == classic_height);
    font_board = XtVaCreateManagedWidget("fontBoard", xmBulletinBoardWidgetClass, root,
        XmNlabelFontList, saved, XmNbuttonRenderTable, saved,
        XmNtextFontList, saved, NULL);
    inherited_label = XtVaCreateManagedWidget("inheritedLabel", xmLabelWidgetClass, font_board, NULL);
    inherited_button = XtVaCreateManagedWidget("inheritedButton", xmPushButtonWidgetClass, font_board, NULL);
    inherited_text = XtVaCreateManagedWidget("inheritedText", xmTextFieldWidgetClass, font_board, NULL);
    CHECK(height(inherited_label) == classic_height);
    CHECK(height(inherited_button) == classic_height);
    CHECK(height(inherited_text) == classic_height);
    XmRenderTableFree(saved);
    saved = NULL;
    XtVaSetValues(live, XmNappearanceRole, XmAPPEARANCE_ROLE_PAGE_TITLE, NULL);
    XtVaSetValues(gadget, XmNappearanceRole, XmAPPEARANCE_ROLE_PAGE_TITLE, NULL);
    CHECK(height(live) == title_height);
    CHECK(height(gadget) == title_height);
    XtVaSetValues(tab_box, XmNappearanceRole, XmAPPEARANCE_ROLE_PAGE_TITLE, NULL);
    CHECK(height(tab_box) == title_height);
    XtVaSetValues(live, XmNappearanceRole, 255, NULL);
    CHECK(role(live) == XmAPPEARANCE_ROLE_PAGE_TITLE);
    CHECK(height(live) == title_height);
    XtVaSetValues(live, XmNappearanceRole, XmAPPEARANCE_ROLE_NORMAL, NULL);
    CHECK(height(live) == body_height);
    /* An equal-valued explicit table is still owned by the application. */
    XtVaGetValues(live, XmNrenderTable, &saved, NULL);
    XtVaSetValues(live, XmNrenderTable, saved, NULL);
    XtVaSetValues(live, XmNappearanceRole, XmAPPEARANCE_ROLE_PAGE_TITLE, NULL);
    CHECK(height(live) == body_height);
    CHECK(XmApplyAppearanceProfile(display, XmAPPEARANCE_CLASSIC));
    CHECK(height(normal) == classic_height);
    CHECK(height(explicit_font) == classic_height);
    CHECK(XmApplyAppearanceProfile(display, profile));
    CHECK(height(normal) == body_height);
    CHECK(height(title) == title_height);
    CHECK(height(live) == body_height);
    CHECK(height(explicit_font) == classic_height);
    CHECK(height(inherited_label) == classic_height);
    CHECK(height(inherited_button) == classic_height);
    CHECK(height(inherited_text) == classic_height);
    {
        MrmHierarchy hierarchy;
        MrmType class_code;
        Widget form, uil_title, uil_section, uil_primary, uil_code;
        CHECK(MrmOpenHierarchy(1, uid_files, NULL, &hierarchy) == MrmSUCCESS);
        CHECK(MrmFetchWidget(hierarchy, "roleForm", root, &form, &class_code) == MrmSUCCESS);
        uil_title = XtNameToWidget(form, "roleTitle");
        uil_section = XtNameToWidget(form, "roleSection");
        uil_primary = XtNameToWidget(form, "rolePrimary");
        uil_code = XtNameToWidget(form, "roleCode");
        CHECK(uil_title && uil_section && uil_primary && uil_code);
        {
            XColor exact, screen; Colormap colormap; Pixel widget_color, gadget_color;
            XtVaGetValues(form, XmNcolormap, &colormap, NULL);
            CHECK(XAllocNamedColor(display, colormap, "#0033cc", &screen, &exact));
            XtVaGetValues(uil_title, XmNinsensitiveForeground, &widget_color, NULL);
            XtVaGetValues(uil_section, XmNinsensitiveForeground, &gadget_color, NULL);
            CHECK(widget_color == screen.pixel && gadget_color == screen.pixel);
        }
        CHECK(role(form) == XmAPPEARANCE_ROLE_NORMAL);
        CHECK(role(uil_title) == XmAPPEARANCE_ROLE_PAGE_TITLE);
        CHECK(role(uil_section) == XmAPPEARANCE_ROLE_SECTION);
        CHECK(role(uil_primary) == XmAPPEARANCE_ROLE_PRIMARY);
        CHECK(role(uil_code) == XmAPPEARANCE_ROLE_CODE);
        CHECK(height(uil_title) == title_height);
        CHECK(height(uil_section) > body_height);
        CHECK(MrmCloseHierarchy(hierarchy) == MrmSUCCESS);
    }
    {
        const double scales[] = {1.5, 2.0, 3.0, 1.0};
        Dimension baseline_margin = 0;
        Widget toggle = XtVaCreateManagedWidget("scaleToggle", xmToggleButtonWidgetClass,
                                                root, NULL);
        Widget toggle_gadget = XtVaCreateManagedWidget("scaleToggleGadget", xmToggleButtonGadgetClass,
                                                       root, NULL);
        Widget fixed_toggle = XtVaCreateWidget("fixedToggle", xmToggleButtonWidgetClass,
                        root, XmNindicatorSize, 19, XmNmarginWidth, 17, NULL);
        CHECK(dimension(toggle, XmNindicatorSize) == 18);
        XmToggleButtonSetState(toggle, True, False);
        XtVaGetValues(normal, XmNmarginHeight, &baseline_margin, NULL);
        for (unsigned int i = 0; i < XtNumber(scales); ++i) {
            Widget fresh;
            Dimension margin = 0;
            CHECK(XmSetDisplayScaleOverride(display, scales[i]));
            fresh = XtVaCreateManagedWidget("fresh", xmLabelWidgetClass, root, NULL);
            CHECK(height(normal) == height(fresh));
            CHECK(height(normal) >= body_height);
            CHECK(height(explicit_font) == classic_height);
            CHECK(height(inherited_button) == classic_height);
            CHECK(dimension(toggle, XmNindicatorSize) == (Dimension)(18 * scales[i] + 0.5));
            CHECK(dimension(toggle_gadget, XmNindicatorSize) == (Dimension)(18 * scales[i] + 0.5));
            CHECK(dimension(fixed_toggle, XmNindicatorSize) == 19);
            CHECK(dimension(fixed_toggle, XmNmarginWidth) == 17);
            CHECK(XmToggleButtonGetState(toggle));
            XtVaGetValues(normal, XmNmarginHeight, &margin, NULL);
            if (margin != (Dimension)(baseline_margin * scales[i] + 0.5))
                fprintf(stderr, "scale %g: baseline margin %u, live margin %u\n",
                        scales[i], baseline_margin, margin);
            CHECK(margin == (Dimension)(baseline_margin * scales[i] + 0.5));
            XtDestroyWidget(fresh);
        }
        CHECK(height(normal) == body_height);
        CHECK(height(title) == title_height);
        /* Font preparation fails before any scale callback mutates controls. */
        {
            double retained_scale = 0;
            CHECK(!XmSetDisplayScaleOverride(display, 1e100));
            CHECK(XmGetDisplayScaleOverride(display, &retained_scale));
            CHECK(retained_scale == 1.0);
            CHECK(height(normal) == body_height);
            CHECK(height(title) == title_height);
        }
    }
    XtDestroyWidget(shell);
    XtCloseDisplay(display);
    XtDestroyApplicationContext(app);
    puts("PASS semantic role construction, conversion, live updates, font ownership and classic restoration");
    return 0;
}
