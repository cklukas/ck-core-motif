/*
 * Motif appearance-profile selection.
 */
#ifdef HAVE_CONFIG_H
#include "PaintI.h"
#include <config.h>
#endif

#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <math.h>
#include <limits.h>
#include <string.h>

#include <X11/IntrinsicP.h>
#include <X11/CompositeP.h>
#include <X11/ShellP.h>
#include <X11/StringDefs.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/Xlibint.h>

#include <Xm/Appearance.h>
#include <Xm/Xm.h>
#include "XmI.h"
#include "RowColumnP.h"
#include "RCLayoutI.h"
#include <Xm/RepType.h>
#include <Xm/PrimitiveP.h>
#include <Xm/GadgetP.h>
#include <Xm/NotebookP.h>
#include <Xm/SegmentedControl.h>
#include <Xm/ScrollBarP.h>
#include <Xm/Scale.h>
#include <Xm/ScrolledW.h>
#include <Xm/TextF.h>
#include <Xm/SpinB.h>
#include <Xm/ComboBoxP.h>
#include <Xm/ArrowB.h>
#include <Xm/ArrowBG.h>
#include <Xm/ToggleB.h>
#include <Xm/ToggleBG.h>
#include <Xm/FormSection.h>
#include <Xm/Avatar.h>
#include <Xm/Divider.h>
#include <Xm/RatingControl.h>
#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#endif

#define APPEARANCE_RESOURCE "ckCoreMotif.appearanceProfile"
#define APPEARANCE_CLASS "CkCoreMotif.AppearanceProfile"
#define DENSITY_RESOURCE "ckCoreMotif.appearanceDensity"
#define DENSITY_CLASS "CkCoreMotif.AppearanceDensity"
#define CLASSIC_PROFILE "classic"
#define MODERN_LIGHT_PROFILE "modern-light"
#define HIGH_CONTRAST_PROFILE "high-contrast"
#define MODERN_DARK_PROFILE "modern-dark"

static XmRepTypeId appearance_role_type = XmREP_TYPE_INVALID;

void
_XmInitializeAppearance(void)
{
    static String names[] = {"normal", "page_title", "section", "secondary",
                            "code", "primary", "destructive"};
    _XmProcessLock();
    if (appearance_role_type == XmREP_TYPE_INVALID)
        appearance_role_type = XmRepTypeRegister(XmRAppearanceRole, names, NULL,
                                                XtNumber(names));
    _XmProcessUnlock();
}

unsigned char
_XmValidateAppearanceRole(Widget widget, unsigned char role, unsigned char fallback)
{
    if (role <= XmAPPEARANCE_ROLE_DESTRUCTIVE) return role;
    XmeWarning(widget, "Invalid appearanceRole; retaining the previous role");
    return fallback;
}

static unsigned char
appearance_role(Widget widget)
{
    if (XmIsPrimitive(widget)) return ((XmPrimitiveWidget)widget)->primitive.appearance_role;
    if (XmIsManager(widget)) return ((XmManagerWidget)widget)->manager.appearance_role;
    if (XmIsGadget(widget)) return ((XmGadget)widget)->gadget.appearance_role;
    return XmAPPEARANCE_ROLE_NORMAL;
}

/*
 * Profile patterns are resolved independently of the application's Xrm
 * database. Construction establishes classic/application resources first;
 * appearance applies only resources not explicitly owned by the application.
 * In particular a class-specific profile pattern cannot outrank a broad
 * application wildcard.
 */
static const char modern_light_resources[] =
    "*background: #f1f2f4\n"
    "*foreground: #202124\n"
    "*insensitiveForeground: #747b84\n"
    "*topShadowColor: #9aa3ae\n"
    "*bottomShadowColor: #9aa3ae\n"
    "*highlightColor: #1a73e8\n"
    "*highlightThickness: 1\n"
    "*shadowThickness: 1\n"
    "*armColor: #dbeafe\n"
    "*selectColor: #dbeafe\n"
    "*XmLabel.traversalOn: False\n"
    "*XmLabel.highlightThickness: 0\n"
    "*XmLabelGadget.traversalOn: False\n"
    "*XmLabelGadget.highlightThickness: 0\n"
    "*XmPushButton.background: #e2e6eb\n"
    "*XmPushButtonGadget.background: #e2e6eb\n"
    "*XmSpinBox.background: #ffffff\n"
    "*XmSimpleSpinBox.background: #ffffff\n"
    "*XmComboBox.background: #ffffff\n"
    "*XmTextField.background: #ffffff\n"
    "*XmSearchBox.background: #ffffff\n"
    "*XmText.background: #ffffff\n"
    "*XmList.background: #ffffff\n"
    "*XmPushButton.traversalOn: True\n"
    "*XmToggleButton.traversalOn: True\n"
    "*XmTextField.traversalOn: True\n"
    "*XmSearchBox.traversalOn: True\n"
    "*XmScrollBar.highlightThickness: 1\n"
    "*XmScrollBar.shadowThickness: 0\n"
    "*XmScale.shadowThickness: 0\n"
    "*XmScale.sliderMark: none\n"
    "*XmScrollBar.showArrows: each_side\n"
    "*XmScrollBar.sliderVisual: foreground\n"
    "*XmScrollBar.troughColor: #f1f2f4\n"
    "*XmScrollBar.foreground: #747b84\n"
    "*XmScrollBar.topShadowColor: #c5cad1\n"
    "*XmPushButton.shadowThickness: 1\n"
    "*XmPushButton.highlightThickness: 2\n"
    "*XmPushButton.bottomShadowColor: #747b84\n"
    "*XmPushButtonGadget.shadowThickness: 1\n"
    "*XmPushButtonGadget.highlightThickness: 2\n"
    "*XmPushButtonGadget.bottomShadowColor: #747b84\n"
    "*XmIconButton.shadowThickness: 1\n"
    "*XmIconButton.highlightThickness: 2\n"
    "*XmDrawnButton.shadowThickness: 1\n"
    "*XmDrawnButton.highlightThickness: 2\n"
    "*XmDrawnButton.bottomShadowColor: #747b84\n"
    "*XmArrowButton.shadowThickness: 1\n"
    "*XmArrowButtonGadget.shadowThickness: 1\n"
    "*XmArrowButton.bottomShadowColor: #747b84\n"
    "*XmArrowButtonGadget.bottomShadowColor: #747b84\n"
    "*XmTextField.shadowThickness: 1\n"
    "*XmSearchBox.shadowThickness: 1\n"
    "*XmText.shadowThickness: 1\n"
    "*XmTextField.highlightThickness: 2\n"
    "*XmSearchBox.highlightThickness: 2\n"
    "*XmText.highlightThickness: 2\n"
    "*XmIconGadget.shadowThickness: 0\n"
    "*XmCascadeButton.shadowThickness: 0\n"
    "*XmCascadeButtonGadget.shadowThickness: 0\n"
    "*XmAvatar.background: #d3e3fd\n"
    "*XmAvatar.foreground: #174ea6\n"
    "*XmAvatar.highlightThickness: 0\n"
    "*XmAvatar.shadowThickness: 0\n"
    "*XmContainer.shadowThickness: 0\n"
    "*XmContainer.outlineLineStyle: no_line\n"
    "*XmScrolledWindow.shadowThickness: 1\n"
    "*XmScrolledWindow.spacing: 0\n"
    "*XmForm.shadowThickness: 0\n"
    "*XmBulletinBoard.shadowThickness: 0\n"
    "*XmMessageBox.shadowThickness: 0\n"
    "*XmSelectionBox.shadowThickness: 0\n"
    "*XmCommand.shadowThickness: 0\n"
    "*XmFileSelectionBox.shadowThickness: 0\n"
    "*XmNotebook.frameBackground: #f1f2f4\n"
    "*XmNotebook.backPageBackground: #f1f2f4\n"
    "*XmNotebook.backPageForeground: #9aa3ae\n"
    "*XmTabStack.tabMode: scrolled\n"
    "*XmTabBox.tabMode: scrolled\n"
    "*XmNotebook.bindingWidth: 0\n"
    "*XmNotebook.backPageSize: 4\n"
    "*XmNotebook.frameShadowThickness: 1\n"
    "*XmNotebook.borderWidth: 0\n"
    "*XmToggleSwitch.shadowThickness: 0\n"
    "*XmToggleSwitch.highlightThickness: 2\n"
    "*XmToggleButton.shadowThickness: 0\n"
    "*XmToggleButtonGadget.shadowThickness: 0\n"
    "*XmToggleButton.selectColor: #1967d2\n"
    "*XmToggleButtonGadget.selectColor: #1967d2\n"
    "*XmToggleButton.highlightThickness: 2\n"
    "*XmToggleButtonGadget.highlightThickness: 2\n"
    "*XmToggleButton.unselectColor: #ffffff\n"
    "*XmToggleButtonGadget.unselectColor: #ffffff\n";

static const char high_contrast_resources[] =
    "*background: #000000\n"
    "*XmNotebook.frameBackground: #000000\n"
    "*XmNotebook.backPageBackground: #000000\n"
    "*XmNotebook.backPageForeground: #ffffff\n"
    "*foreground: #ffffff\n"
    "*topShadowColor: #ffffff\n"
    "*bottomShadowColor: #ffffff\n"
    "*highlightColor: #ffff00\n"
    "*highlightThickness: 3\n"
    "*shadowThickness: 2\n"
    "*armColor: #0000ff\n"
    "*selectColor: #0000ff\n"
    "*insensitiveForeground: #b3b3b3\n"
    "*XmLabel.traversalOn: True\n"
    "*XmLabel.highlightThickness: 3\n"
    "*XmPushButton.traversalOn: True\n"
    "*XmToggleButton.traversalOn: True\n"
    "*XmTextField.traversalOn: True\n"
    "*XmSearchBox.traversalOn: True\n"
    "*XmScrollBar.highlightThickness: 3\n";

static const char modern_dark_resources[] =
    "*background: #202124\n"
    "*foreground: #f1f3f4\n"
    "*topShadowColor: #5f6368\n"
    "*bottomShadowColor: #5f6368\n"
    "*highlightColor: #8ab4f8\n"
    "*highlightThickness: 1\n"
    "*shadowThickness: 1\n"
    "*armColor: #3c4043\n"
    "*selectColor: #3c4043\n"
    "*insensitiveForeground: #9aa0a6\n"
    "*XmToggleButton.bottomShadowColor: #9aa0a6\n"
    "*XmToggleButtonGadget.bottomShadowColor: #9aa0a6\n"
    "*XmLabel.traversalOn: False\n"
    "*XmLabel.highlightThickness: 0\n"
    "*XmLabelGadget.traversalOn: False\n"
    "*XmLabelGadget.highlightThickness: 0\n"
    "*XmPushButton.traversalOn: True\n"
    "*XmToggleButton.traversalOn: True\n"
    "*XmTextField.traversalOn: True\n"
    "*XmSearchBox.traversalOn: True\n"
    "*XmScrollBar.highlightThickness: 1\n"
    "*XmScrollBar.shadowThickness: 0\n"
    "*XmScale.shadowThickness: 0\n"
    "*XmScale.sliderMark: none\n"
    "*XmScrollBar.showArrows: each_side\n"
    "*XmScrollBar.sliderVisual: foreground\n"
    "*XmScrollBar.troughColor: #202124\n"
    "*XmScrollBar.foreground: #9aa0a6\n"
    "*XmScrollBar.topShadowColor: #50545b\n"
    "*XmPushButton.shadowThickness: 1\n"
    "*XmPushButton.highlightThickness: 2\n"
    "*XmPushButton.bottomShadowColor: #7b8086\n"
    "*XmPushButtonGadget.shadowThickness: 1\n"
    "*XmPushButtonGadget.highlightThickness: 2\n"
    "*XmPushButtonGadget.bottomShadowColor: #7b8086\n"
    "*XmIconButton.shadowThickness: 1\n"
    "*XmIconButton.highlightThickness: 2\n"
    "*XmDrawnButton.shadowThickness: 1\n"
    "*XmDrawnButton.highlightThickness: 2\n"
    "*XmDrawnButton.bottomShadowColor: #7b8086\n"
    "*XmArrowButton.shadowThickness: 1\n"
    "*XmArrowButtonGadget.shadowThickness: 1\n"
    "*XmTextField.shadowThickness: 1\n"
    "*XmSearchBox.shadowThickness: 1\n"
    "*XmText.shadowThickness: 1\n"
    "*XmTextField.highlightThickness: 2\n"
    "*XmSearchBox.highlightThickness: 2\n"
    "*XmText.highlightThickness: 2\n"
    "*XmIconGadget.shadowThickness: 0\n"
    "*XmCascadeButton.shadowThickness: 0\n"
    "*XmCascadeButtonGadget.shadowThickness: 0\n"
    "*XmAvatar.background: #334b6d\n"
    "*XmAvatar.foreground: #d3e3fd\n"
    "*XmAvatar.highlightThickness: 0\n"
    "*XmAvatar.shadowThickness: 0\n"
    "*XmContainer.shadowThickness: 0\n"
    "*XmContainer.outlineLineStyle: no_line\n"
    "*XmScrolledWindow.shadowThickness: 1\n"
    "*XmScrolledWindow.spacing: 0\n"
    "*XmForm.shadowThickness: 0\n"
    "*XmBulletinBoard.shadowThickness: 0\n"
    "*XmMessageBox.shadowThickness: 0\n"
    "*XmSelectionBox.shadowThickness: 0\n"
    "*XmCommand.shadowThickness: 0\n"
    "*XmFileSelectionBox.shadowThickness: 0\n"
    "*XmTextField.bottomShadowColor: #5f6368\n"
    "*XmSearchBox.bottomShadowColor: #5f6368\n"
    "*XmArrowButton.bottomShadowColor: #7b8086\n"
    "*XmArrowButtonGadget.bottomShadowColor: #7b8086\n"
    "*XmText.bottomShadowColor: #5f6368\n"
    "*XmNotebook.frameBackground: #202124\n"
    "*XmNotebook.backPageBackground: #202124\n"
    "*XmNotebook.backPageForeground: #5f6368\n"
    "*XmTabStack.tabMode: scrolled\n"
    "*XmTabBox.tabMode: scrolled\n"
    "*XmNotebook.bindingWidth: 0\n"
    "*XmNotebook.backPageSize: 4\n"
    "*XmNotebook.frameShadowThickness: 1\n"
    "*XmNotebook.borderWidth: 0\n"
    "*XmToggleSwitch.shadowThickness: 0\n"
    "*XmToggleSwitch.highlightThickness: 2\n"
    "*XmToggleButton.shadowThickness: 0\n"
    "*XmToggleButtonGadget.shadowThickness: 0\n"
    "*XmToggleButton.selectColor: #8ab4f8\n"
    "*XmToggleButtonGadget.selectColor: #8ab4f8\n"
    "*XmToggleButton.highlightThickness: 2\n"
    "*XmToggleButtonGadget.highlightThickness: 2\n"
    "*XmToggleButton.unselectColor: #202124\n"
    "*XmToggleButtonGadget.unselectColor: #202124\n";

/* Layout defaults are shared by all modern palettes. Keep density independent
 * of palette selection, and resolve these same patterns for startup and live
 * changes. Natural control sizes remain the responsibility of each widget. */
typedef struct {
    const char *resource;
    Dimension comfortable;
    Dimension compact;
} AppearanceDensityResource;

static const AppearanceDensityResource density_resources[] = {
    {"*XmCascadeButton.marginWidth", 12, 8},
    {"*XmCascadeButton.marginHeight", 6, 3},
    {"*XmCascadeButtonGadget.marginWidth", 12, 8},
    {"*XmCascadeButtonGadget.marginHeight", 6, 3},
    {"*XmPushButton.marginWidth", 12, 8},
    {"*XmLabel.marginWidth", 2, 2},
    {"*XmLabel.marginHeight", 2, 2},
    {"*XmLabelGadget.marginWidth", 2, 2},
    {"*XmLabelGadget.marginHeight", 2, 2},
    {"*XmPushButton.marginHeight", 6, 3},
    {"*XmPushButtonGadget.marginWidth", 12, 8},
    {"*XmPushButtonGadget.marginHeight", 6, 3},
    {"*XmIconButton.horizontalMargin", 12, 8},
    {"*XmIconButton.verticalMargin", 6, 3},
    {"*XmIconButton.iconTextPadding", 8, 6},
    {"*XmDrawnButton.marginWidth", 12, 8},
    {"*XmDrawnButton.marginHeight", 6, 3},
    {"*XmToggleButton.marginWidth", 12, 8},
    {"*XmToggleButton.marginHeight", 6, 4},
    {"*XmToggleButton.indicatorSize", 18, 16},
    {"*XmToggleButton.spacing", 8, 6},
    {"*XmToggleSwitch.spacing", 8, 6},
    {"*XmToggleButtonGadget.marginWidth", 12, 8},
    {"*XmToggleButtonGadget.marginHeight", 6, 4},
    {"*XmToggleButtonGadget.indicatorSize", 18, 16},
    {"*XmToggleButtonGadget.spacing", 8, 6},
    {"*XmTextField.marginWidth", 8, 6},
    {"*XmSearchBox.marginWidth", 8, 6},
    {"*XmTextField.marginHeight", 4, 2},
    {"*XmSearchBox.marginHeight", 4, 2},
    {"*XmText.marginWidth", 8, 6},
    {"*XmText.marginHeight", 4, 2},
    {"*XmList.listMarginWidth", 8, 4},
    {"*XmList.listMarginHeight", 8, 4},
    {"*XmList.listSpacing", 4, 2},
    {"*XmIconLabel.iconLabelSpacing", 6, 4},
    {"*XmIconGadget.marginWidth", 4, 2},
    {"*XmIconGadget.marginHeight", 4, 2},
    {"*XmIconGadget.spacing", 6, 4},
    {"*XmContainer.marginWidth", 8, 4},
    {"*XmContainer.marginHeight", 8, 4},
    {"*XmOutline.indentSpace", 30, 24},
    {"*XmFormSection.formSectionMarginWidth", 12, 8},
    {"*XmFormSection.formSectionMarginHeight", 12, 8},
    {"*XmFormSection.formSectionSpacing", 8, 6},
    {"*XmAvatar.avatarSize", 48, 40},
    {"*XmDivider.dividerSpacing", 8, 6},
    {"*XmNumberBox.spacing", 4, 2},
    {"*XmStepper.spacing", 4, 2},
    {"*XmColorPicker.spacing", 8, 6},
    {"*XmFormSection.content.spacing", 8, 6},
    {"*XmNotebook.innerMarginWidth", 8, 4},
    {"*XmNotebook.innerMarginHeight", 8, 4},
    {"*XmTabStack.tabMarginWidth", 12, 8},
    {"*XmTabStack.tabMarginHeight", 6, 3},
    {"*XmTabBox.tabMarginWidth", 12, 8},
    {"*XmTabBox.tabMarginHeight", 6, 3},
    {"*XmMessageBox.marginWidth", 16, 8},
    {"*XmMessageBox.marginHeight", 16, 8},
    {"*XmSelectionBox.marginWidth", 16, 8},
    {"*XmSelectionBox.marginHeight", 16, 8},
    {"*XmCommand.marginWidth", 16, 8},
    {"*XmCommand.marginHeight", 16, 8},
    {"*XmFileSelectionBox.marginWidth", 16, 8},
    {"*XmFileSelectionBox.marginHeight", 16, 8}
};

typedef enum {
    AP_BACKGROUND,
    AP_FOREGROUND,
    AP_TOP_SHADOW,
    AP_BOTTOM_SHADOW,
    AP_HIGHLIGHT,
    AP_HIGHLIGHT_THICKNESS,
    AP_SHADOW_THICKNESS,
    AP_ARM,
    AP_SELECT,
    AP_INSENSITIVE_FOREGROUND,
    AP_TRAVERSAL,
    AP_MARGIN_WIDTH,
    AP_MARGIN_HEIGHT,
    AP_LIST_MARGIN_WIDTH,
    AP_LIST_MARGIN_HEIGHT,
    AP_LIST_SPACING,
    AP_FRAME_SHADOW_THICKNESS,
    AP_BORDER_WIDTH,
    AP_INDICATOR_SIZE,
    AP_RENDER_TABLE,
    AP_FRAME_BACKGROUND,
    AP_BACK_PAGE_BACKGROUND,
    AP_BACK_PAGE_FOREGROUND,
    AP_BINDING_WIDTH,
    AP_BACK_PAGE_SIZE,
    AP_INNER_MARGIN_WIDTH,
    AP_INNER_MARGIN_HEIGHT,
    AP_UNSELECT_COLOR,
    AP_SPACING,
    AP_TROUGH_COLOR,
    AP_SHOW_ARROWS,
    AP_SLIDER_VISUAL,
    AP_SLIDER_MARK,
    AP_TAB_MARGIN_WIDTH,
    AP_TAB_MARGIN_HEIGHT,
    AP_TAB_MODE,
    AP_EXPANDED_PIXMAP,
    AP_COLLAPSED_PIXMAP,
    AP_ICON_LABEL_SPACING,
    AP_HORIZONTAL_MARGIN,
    AP_VERTICAL_MARGIN,
    AP_ICON_TEXT_PADDING,
    AP_ALIGNMENT,
    AP_SECTION_MARGIN_WIDTH,
    AP_SECTION_MARGIN_HEIGHT,
    AP_SECTION_SPACING,
    AP_AVATAR_SIZE,
    AP_DIVIDER_SPACING,
    AP_OUTLINE_LINE_STYLE,
    AP_OUTLINE_INDENT_SPACE,
    AP_COUNT
} AppearanceProperty;

typedef struct _XmAppearanceWidgetState {
    Widget widget;
    Widget parent;
    Display *display;
    Boolean supported[AP_COUNT];
    Boolean explicit_arg[AP_COUNT];
    Boolean owned[AP_COUNT];
    XtArgVal original[AP_COUNT];
    unsigned int references;
    Boolean retired;
    Boolean initialized;
    unsigned char applied_role;
    XmRenderTable original_font;
    XmRenderTable current_font;
    double font_scale;
    unsigned char font_role;
    Widget font_provider;
    Widget foreground_provider;
    unsigned char font_kind;
    unsigned char font_provider_overrides;
    Boolean inherit_font_role;
    Boolean explicit_role;
    Boolean dimension_argument[2];
    Boolean constrained_dimension[2];
    Dimension original_scrollbar_thickness;
    Boolean gadget;
    struct _XmAppearanceWidgetState *next;
} XmAppearanceWidgetState;

typedef struct {
    XmAppearanceWidgetState *state;
    XtArgVal values[AP_COUNT];
    Boolean owned[AP_COUNT];
    Boolean apply[AP_COUNT];
    XmRenderTable prepared_font;
    XmRenderTable retained_font;
    XmRenderTable observed_font;
    double font_scale;
    unsigned char font_role;
    Boolean arrow_geometry;
    Dimension arrow_extent;
    Boolean scrollbar_geometry;
    unsigned scrollbar_axis;
    Dimension scrollbar_extent;
} XmAppearanceChangePlan;

typedef struct _XmAppearanceDisplayState {
    Display *display;
    XrmDatabase active_database;
    Boolean owns_active_database;
    Boolean transaction_active;
    Boolean defer_layout;
    unsigned int resource_commit_depth;
    XmAppearanceProfile committing_profile;
    XmAppearanceDensity committing_density;
    Boolean committing_geometry;
    Widget hooks;
    struct _XmAppearanceDisplayState *next;
} XmAppearanceDisplayState;

/* Some class initialize prehooks resolve a default font before the common
 * object-registration hook. Preserve that provenance until registration. */
typedef struct _XmAppearancePendingFont {
    Widget widget;
    Widget provider;
    Display *display;
    unsigned char kind;
    struct _XmAppearancePendingFont *next;
} XmAppearancePendingFont;
static XmAppearancePendingFont *appearance_pending_fonts;

static XmAppearanceWidgetState *appearance_widgets;
static XmAppearanceDisplayState *appearance_displays;

static void appearance_release_gadget_states(Display *display);
static void appearance_created(Widget, XtPointer, XtPointer);
static void appearance_changed(Widget, XtPointer, XtPointer);
static void appearance_hooks_destroyed(Widget, XtPointer, XtPointer);
static void appearance_widget_destroyed(Widget, XtPointer, XtPointer);

void
_XmDestroyAppearanceDisplay(Display *display)
{
    XmAppearanceDisplayState **link;
    XmAppearanceDisplayState *state;
    XrmDatabase active_database;

    if (display == NULL)
        return;

    appearance_release_gadget_states(display);

    _XmProcessLock();
    {
        XmAppearancePendingFont **pending = &appearance_pending_fonts;
        while (*pending) {
            XmAppearancePendingFont *item = *pending;
            if (item->display == display) {
                *pending = item->next;
                XtFree((char *)item);
            } else pending = &item->next;
        }
    }
    link = &appearance_displays;
    while (*link != NULL && (*link)->display != display)
        link = &(*link)->next;
    state = *link;
    if (state == NULL) {
        _XmProcessUnlock();
        return;
    }
    *link = state->next;
    active_database = state->active_database;
    state->active_database = NULL;
    if (state->hooks != NULL) {
        XtRemoveCallback(state->hooks, XtNcreateHook, appearance_created, state);
        XtRemoveCallback(state->hooks, XtNchangeHook, appearance_changed, state);
        XtRemoveCallback(state->hooks, XtNdestroyCallback,
                         appearance_hooks_destroyed, state);
    }
    XtFree((char *)state);
    _XmProcessUnlock();

    if (active_database != NULL && XrmGetDatabase(display) == active_database) {
        XrmSetDatabase(display, NULL);
        XrmDestroyDatabase(active_database);
    }
}

static const char *
appearance_property_name(AppearanceProperty property)
{
    static const char *const names[AP_COUNT] = {
        XmNbackground, XmNforeground, XmNtopShadowColor,
        XmNbottomShadowColor, XmNhighlightColor, XmNhighlightThickness,
        XmNshadowThickness, XmNarmColor, XmNselectColor,
        XmNinsensitiveForeground, XmNtraversalOn, XmNmarginWidth,
        XmNmarginHeight, XmNlistMarginWidth, XmNlistMarginHeight,
        XmNlistSpacing, XmNframeShadowThickness, XmNborderWidth,
        XmNindicatorSize, XmNrenderTable,
        XmNframeBackground, XmNbackPageBackground, XmNbackPageForeground,
        XmNbindingWidth, XmNbackPageSize, XmNinnerMarginWidth, XmNinnerMarginHeight,
        XmNunselectColor, XmNspacing, XmNtroughColor, XmNshowArrows, XmNsliderVisual, XmNsliderMark,
        XmNtabMarginWidth, XmNtabMarginHeight, XmNtabMode, XmNexpandedStatePixmap, XmNcollapsedStatePixmap, XmNiconLabelSpacing,
        XmNhorizontalMargin, XmNverticalMargin, XmNiconTextPadding, XmNalignment,
        XmNformSectionMarginWidth, XmNformSectionMarginHeight, XmNformSectionSpacing,
        XmNavatarSize, XmNdividerSpacing, XmNoutlineLineStyle, XmNindentSpace
    };
    return names[property];
}

static Boolean
appearance_property_is_dimension(AppearanceProperty property)
{
    return property == AP_HIGHLIGHT_THICKNESS ||
           property == AP_SHADOW_THICKNESS ||
           property == AP_MARGIN_WIDTH || property == AP_MARGIN_HEIGHT ||
           property == AP_LIST_MARGIN_WIDTH ||
           property == AP_LIST_MARGIN_HEIGHT || property == AP_LIST_SPACING ||
           property == AP_FRAME_SHADOW_THICKNESS || property == AP_BORDER_WIDTH ||
           property == AP_INDICATOR_SIZE || property == AP_BINDING_WIDTH ||
           property == AP_BACK_PAGE_SIZE || property == AP_INNER_MARGIN_WIDTH ||
           property == AP_INNER_MARGIN_HEIGHT || property == AP_SPACING ||
           property == AP_TAB_MARGIN_WIDTH || property == AP_TAB_MARGIN_HEIGHT ||
           property == AP_ICON_LABEL_SPACING || property == AP_HORIZONTAL_MARGIN ||
           property == AP_VERTICAL_MARGIN || property == AP_ICON_TEXT_PADDING ||
           property == AP_SECTION_MARGIN_WIDTH || property == AP_SECTION_MARGIN_HEIGHT ||
           property == AP_SECTION_SPACING || property == AP_AVATAR_SIZE ||
           property == AP_DIVIDER_SPACING || property == AP_OUTLINE_INDENT_SPACE;
}

static Boolean
appearance_property_is_boolean(AppearanceProperty property)
{
    return property == AP_TRAVERSAL;
}

static Boolean
appearance_property_is_color(AppearanceProperty property)
{
    return property != AP_RENDER_TABLE && !appearance_property_is_dimension(property) &&
           !appearance_property_is_boolean(property) &&
           property != AP_SHOW_ARROWS && property != AP_SLIDER_VISUAL &&
           property != AP_SLIDER_MARK && property != AP_TAB_MODE && property != AP_ALIGNMENT &&
           property != AP_EXPANDED_PIXMAP && property != AP_COLLAPSED_PIXMAP &&
           property != AP_OUTLINE_LINE_STYLE;
}

static const char *
widget_class_name(Widget widget)
{
    WidgetClass class = XtClass(widget);
    return class != NULL ? class->core_class.class_name : "";
}

static const char *
profile_name(XmAppearanceProfile profile)
{
    switch (profile) {
    case XmAPPEARANCE_CLASSIC:
        return CLASSIC_PROFILE;
    case XmAPPEARANCE_MODERN_LIGHT:
        return MODERN_LIGHT_PROFILE;
    case XmAPPEARANCE_HIGH_CONTRAST:
        return HIGH_CONTRAST_PROFILE;
    case XmAPPEARANCE_MODERN_DARK:
        return MODERN_DARK_PROFILE;
    default:
        return NULL;
    }
}

/* Canonical profile databases are immutable and shared for the process
 * lifetime, like the static resource strings from which they are built. Both
 * Xt construction and live transactions resolve the same Xrm patterns. */
static XrmDatabase
profile_defaults(XmAppearanceProfile profile, XmAppearanceDensity density)
{
    static XrmDatabase databases[4][2];
    const char *resources;
    XrmDatabase result;
    if (density != XmAPPEARANCE_DENSITY_COMFORTABLE &&
        density != XmAPPEARANCE_DENSITY_COMPACT) return NULL;
    if (profile == XmAPPEARANCE_CLASSIC) return NULL;
    switch (profile) {
    case XmAPPEARANCE_MODERN_LIGHT: resources = modern_light_resources; break;
    case XmAPPEARANCE_HIGH_CONTRAST: resources = high_contrast_resources; break;
    case XmAPPEARANCE_MODERN_DARK: resources = modern_dark_resources; break;
    default: return NULL;
    }
    _XmProcessLock();
    if (databases[profile][density] == NULL) {
        XrmDatabase database = XrmGetStringDatabase(resources);
        if (database != NULL) {
            for (Cardinal i = 0; i < XtNumber(density_resources); ++i) {
                char value[24];
                const AppearanceDensityResource *entry = &density_resources[i];
                snprintf(value, sizeof(value), "%u", (unsigned int)
                    (density == XmAPPEARANCE_DENSITY_COMPACT
                        ? entry->compact : entry->comfortable));
                XrmPutStringResource(&database, entry->resource, value);
            }
            databases[profile][density] = database;
        }
    }
    result = databases[profile][density];
    _XmProcessUnlock();
    return result;
}

static Boolean appearance_database_value(Widget widget, XrmDatabase database,
    AppearanceProperty property, XrmValue *value_return);

static const char *
profile_property_string(Widget widget, XmAppearanceProfile profile,
                        XmAppearanceDensity density,
                        AppearanceProperty property)
{
    XrmValue value;
    unsigned char role = appearance_role(widget);
    if (profile == XmAPPEARANCE_MODERN_LIGHT || profile == XmAPPEARANCE_MODERN_DARK) {
        Boolean dark = profile == XmAPPEARANCE_MODERN_DARK;
        if (property == AP_ALIGNMENT &&
            (XmIsToggleButton(widget) || XmIsToggleButtonGadget(widget))) {
            unsigned char indicator;
            XtVaGetValues(widget,XmNindicatorOn,&indicator,NULL);
            /* Button-like toggles retain their ordinary centered caption. */
            if (indicator != XmINDICATOR_NONE) return "alignment_beginning";
        }
        /* Embedded editors share their composite's outer frame. The plain
         * ComboBox still has separate editor and list surfaces. Ownership is
         * resolved by the normal property transaction, preserving app borders. */
        if (property == AP_SHADOW_THICKNESS && XmIsTextField(widget) && XtParent(widget)) {
            Widget parent = XtParent(widget);
            if (XtIsSubclass(parent, xmSpinBoxWidgetClass) ||
                (XmIsComboBox(parent) &&
                 ((XmComboBoxWidget)parent)->combo_box.type != XmCOMBO_BOX))
                return "0";
        }
        if (XtParent(widget) && XmIsSegmentedControl(XtParent(widget))) {
            if (property == AP_SELECT) return dark ? "#334b6d" : "#d3e3fd";
            if (property == AP_UNSELECT_COLOR) return dark ? "#202124" : "#ffffff";
            if (property == AP_SHADOW_THICKNESS) return "1";
        }
        if (XtParent(widget) && XmIsNotebook(XtParent(widget)) &&
            widget->core.constraints != NULL) {
            XmNotebookConstraintPtr constraint =
                (XmNotebookConstraintPtr)widget->core.constraints;
            if (constraint->notebook.child_type == XmMAJOR_TAB ||
                constraint->notebook.child_type == XmMINOR_TAB) {
                if (property == AP_BACKGROUND)
                    return dark ? "#202124" : "#f1f2f4";
                if (property == AP_SHADOW_THICKNESS) return "1";
                if (property == AP_TOP_SHADOW || property == AP_BOTTOM_SHADOW)
                    return dark ? "#5f6368" : "#9aa3ae";
            }
        }
        if (role == XmAPPEARANCE_ROLE_SECONDARY && property == AP_FOREGROUND)
            return dark ? "#bdc1c6" : "#5f6368";
        if (role == XmAPPEARANCE_ROLE_PRIMARY || role == XmAPPEARANCE_ROLE_DESTRUCTIVE) {
            if (property == AP_FOREGROUND) return dark ? "#202124" : "#ffffff";
            if (property == AP_BACKGROUND)
                return role == XmAPPEARANCE_ROLE_PRIMARY ? (dark ? "#8ab4f8" : "#1967d2") :
                                                         (dark ? "#f28b82" : "#b3261e");
            if (property == AP_ARM || property == AP_SELECT)
                return role == XmAPPEARANCE_ROLE_PRIMARY ? (dark ? "#aecbfa" : "#174ea6") :
                                                         (dark ? "#f6aea9" : "#8c1d18");
        }
    }
    if (property == AP_SLIDER_MARK && XmIsScale(widget)) {
        unsigned char mode;
        XtVaGetValues(widget, XmNslidingMode, &mode, NULL);
        if (mode == XmTHERMOMETER) return NULL;
    }
    /* Thermometers encode a filled value range rather than a movable thumb. */
    if ((property == AP_SLIDER_VISUAL || property == AP_SLIDER_MARK) && XmIsScrollBar(widget) &&
        ((XmScrollBarWidget)widget)->scrollBar.sliding_mode == XmTHERMOMETER)
        return NULL;
    /* RowColumn permits shadows only for menus. Preserve the class baseline
     * on work areas/options rather than asking SetValues to reject styling. */
    if (property == AP_SHADOW_THICKNESS && XmIsRowColumn(widget)) {
        unsigned char type = ((XmRowColumnWidget)widget)->row_column.type;
        if (type != XmMENU_BAR && type != XmMENU_POPUP && type != XmMENU_PULLDOWN)
            return NULL;
        if (type == XmMENU_BAR &&
            (profile == XmAPPEARANCE_MODERN_LIGHT || profile == XmAPPEARANCE_MODERN_DARK))
            return "0";
    }
    if (appearance_database_value(widget, profile_defaults(profile, density), property,
                                  &value))
        return value.addr;
    return NULL;
}

static Boolean
profile_property_value(Widget widget, XmAppearanceProfile profile,
                       XmAppearanceDensity density,
                       AppearanceProperty property, XtArgVal *value_return)
{
    const char *text = profile_property_string(widget, profile, density, property);
    char *end = NULL;
    unsigned long number;

    if (text == NULL)
        return False;
    if (property == AP_TAB_MODE) {
        XmTabMode value;
        XrmValue from = {strlen(text)+1, (XPointer)text};
        XrmValue to = {sizeof(value), (XPointer)&value};
        if (!XtConvertAndStore(widget, XmRString, &from, XmRTabMode, &to)) return False;
        *value_return = value;
        return True;
    }
    if (property == AP_SHOW_ARROWS || property == AP_SLIDER_VISUAL ||
        property == AP_SLIDER_MARK || property == AP_ALIGNMENT ||
        property == AP_OUTLINE_LINE_STYLE) {
        unsigned char value;
        XrmValue from = {strlen(text)+1, (XPointer)text};
        XrmValue to = {sizeof(value), (XPointer)&value};
        if (!XtConvertAndStore(widget, XmRString, &from,
                property == AP_OUTLINE_LINE_STYLE ? XmRLineStyle :
                property == AP_ALIGNMENT ? XmRAlignment :
                property == AP_SHOW_ARROWS ? XmRShowArrows :
                property == AP_SLIDER_MARK ? XmRSliderMark : XmRSliderVisual, &to))
            return False;
        *value_return = value;
        return True;
    }
    if (appearance_property_is_color(property)) {
        Pixel pixel;
        XrmValue from, to;
        Widget context = XtIsWidget(widget) ? widget : XtParent(widget);
        from.addr = (XPointer)text;
        from.size = (unsigned int)strlen(text) + 1;
        to.addr = (XPointer)&pixel;
        to.size = sizeof(pixel);
        /* Use Xt's cached pixel converter and the actual widget colormap,
         * just as ordinary application resource conversion does. */
        if (!XtConvertAndStore(context, XmRString, &from, XmRPixel, &to))
            return False;
        *value_return = (XtArgVal)pixel;
        return True;
    }
    number = strtoul(text, &end, 10);
    if (appearance_property_is_boolean(property)) {
        if (strcmp(text, "True") == 0 || strcmp(text, "true") == 0) {
            *value_return = (XtArgVal)True;
            return True;
        }
        if (strcmp(text, "False") == 0 || strcmp(text, "false") == 0) {
            *value_return = (XtArgVal)False;
            return True;
        }
        return False;
    }
    if (errno == ERANGE || *text == '\0' || end == text || *end != '\0' ||
        number > (unsigned long)~(Dimension)0)
        return False;
    /* Profile metrics are logical pixels. Stored classic/application values
     * already are device pixels and never pass through this conversion. */
    *value_return = (XtArgVal)_XmDpiDefaultDimension(widget, (Dimension)number);
    return True;
}

/* Cached gadget resources and text backend resources live in secondary
 * tables; XtGetResourceList alone does not describe the public widget API. */
static const char *
appearance_resource_class_named(Widget widget, const char *name)
{
    XtResourceList resources = NULL;
    XmSecondaryResourceData *secondary = NULL;
    Cardinal count = 0, blocks;
    const char *result = NULL;

    XtGetResourceList(XtClass(widget), &resources, &count);
    for (Cardinal i = 0; i < count; ++i)
        if (strcmp(resources[i].resource_name, name) == 0) {
            result = resources[i].resource_class;
            break;
        }
    XtFree((char *)resources);
    if (result != NULL) return result;
    blocks = XmGetSecondaryResourceData(XtClass(widget), &secondary);
    for (Cardinal i = 0; i < blocks; ++i) {
        for (Cardinal j = 0; j < secondary[i]->num_resources; ++j)
            if (strcmp(secondary[i]->resources[j].resource_name, name) == 0)
                result = secondary[i]->resources[j].resource_class;
        XtFree((char *)secondary[i]->resources);
        XtFree((char *)secondary[i]);
    }
    XtFree((char *)secondary);
    return result;
}

static const char *
appearance_resource_class(Widget widget, AppearanceProperty property)
{
    return appearance_resource_class_named(widget, appearance_property_name(property));
}

static Boolean
appearance_resource_supported(Widget widget, AppearanceProperty property)
{
    return appearance_resource_class(widget, property) != NULL;
}

static Boolean
appearance_explicit_arg(ArgList args, Cardinal num_args,
                        AppearanceProperty property)
{
    const char *name = appearance_property_name(property);
    for (Cardinal i = 0; i < num_args; ++i)
        if (strcmp(args[i].name, name) == 0 ||
            (property == AP_RENDER_TABLE && strcmp(args[i].name, XmNfontList) == 0))
            return True;
    return False;
}

static Boolean
appearance_database_typed_value(Widget widget, XrmDatabase database,
                                const char *name, const char *resource_class,
                                XrmRepresentation *type, XrmValue *value_return)
{
    Widget chain[64];
    XrmName names[66];
    XrmClass classes[66];
    Widget current = widget;
    int count = 0;
    if (database == NULL || resource_class == NULL)
        return False;
    while (current != NULL && count < 64) {
        chain[count++] = current;
        current = XtParent(current);
    }
    for (int i = 0; i < count; ++i) {
        Widget item = chain[count - i - 1];
        names[i] = XrmStringToQuark(XtName(item));
        classes[i] = XtIsApplicationShell(item)
            ? ((ApplicationShellWidget)item)->application.xrm_class
            : XrmStringToQuark(widget_class_name(item));
    }
    names[count] = XrmStringToQuark(name);
    classes[count] = XrmStringToQuark(resource_class);
    names[count + 1] = NULLQUARK;
    classes[count + 1] = NULLQUARK;
    return XrmQGetResource(database, names, classes, type, value_return);
}

static Boolean
appearance_database_named_value(Widget widget, XrmDatabase database,
                                const char *name, const char *resource_class,
                                XrmValue *value_return)
{
    XrmRepresentation type;
    return appearance_database_typed_value(widget, database, name,
                                           resource_class, &type, value_return);
}

static Boolean
appearance_database_value(Widget widget, XrmDatabase database,
                          AppearanceProperty property, XrmValue *value_return)
{
    if (appearance_database_named_value(widget, database,
            appearance_property_name(property), appearance_resource_class(widget, property), value_return))
        return True;
    return property == AP_RENDER_TABLE && appearance_database_named_value(widget, database,
        XmNfontList, appearance_resource_class_named(widget, XmNfontList), value_return);
}

static Bool
clone_resource(XrmDatabase *database, XrmBindingList bindings,
               XrmQuarkList quarks, XrmRepresentation *type,
               XrmValue *value, XPointer closure)
{
    XrmDatabase *target = (XrmDatabase *)closure;
    (void)database;
    XrmQPutResource(target, bindings, quarks, *type, value);
    return False;
}

static XrmDatabase
clone_database(XrmDatabase source)
{
    XrmDatabase copy = XrmGetStringDatabase("");
    XrmName names[1] = {NULLQUARK};
    XrmClass classes[1] = {NULLQUARK};
    if (copy == NULL)
        return NULL;
    if (source != NULL && XrmEnumerateDatabase(source, names, classes,
                                               XrmEnumAllLevels,
                                               clone_resource,
                                               (XPointer)&copy)) {
        XrmDestroyDatabase(copy);
        return NULL;
    }
    return copy;
}

static XmAppearanceDisplayState *
find_display_state(Display *display)
{
    XmAppearanceDisplayState *state;
    for (state = appearance_displays; state != NULL; state = state->next)
        if (state->display == display)
            return state;
    return NULL;
}

static void
appearance_free_widget_state(XmAppearanceWidgetState *state)
{
    if (state->original_font) XmRenderTableFree(state->original_font);
    if (state->current_font) XmRenderTableFree(state->current_font);
    XtFree((char *)state);
}

/*
 * Appearance changes call XtSetValues() on application objects.  A class
 * SetValues hook may therefore call back into this API while the outer
 * transaction is still applying.  Resource-database replacement is not
 * composable in that situation: accepting the nested operation would let
 * the two snapshots observe and publish different profiles.  Reject the
 * nested operation explicitly; the caller remains responsible for deciding
 * whether to retry it after the outer transaction completes.
 */
static Boolean
appearance_transaction_begin(XmAppearanceDisplayState *state)
{
    Boolean available;

    _XmProcessLock();
    available = !state->transaction_active;
    if (available)
        state->transaction_active = True;
    _XmProcessUnlock();
    return available;
}

static void
appearance_transaction_end(XmAppearanceDisplayState *state)
{
    _XmProcessLock();
    state->transaction_active = False;
    _XmProcessUnlock();
}

static Boolean
appearance_transaction_is_active(Display *display)
{
    XmAppearanceDisplayState *state = find_display_state(display);
    Boolean active = False;

    if (state != NULL) {
        _XmProcessLock();
        active = state->transaction_active;
        _XmProcessUnlock();
    }
    return active;
}

/* Layout managers must not normalize siblings against a mixture of old and
 * new appearance values. The transaction holds this phase only while committing
 * resources, then performs cross-child layout with all new values present. */
/* Geometry must see the target profile while resource changes are committed. */
XmAppearanceProfile
_XmAppearanceGeometryProfile(Display *display)
{
    XmAppearanceDisplayState *state = find_display_state(display);
    XmAppearanceProfile profile = XmAPPEARANCE_CLASSIC;
    if (state != NULL && state->committing_geometry) return state->committing_profile;
    (void)XmGetAppearanceProfile(display, &profile);
    return profile;
}

XmAppearanceDensity
_XmAppearanceGeometryDensity(Display *display)
{
    XmAppearanceDisplayState *state = find_display_state(display);
    XmAppearanceDensity density = XmAPPEARANCE_DENSITY_COMFORTABLE;
    if (state != NULL && state->committing_geometry) return state->committing_density;
    (void)XmGetAppearanceDensity(display, &density);
    return density;
}

/* Composite resources are already planned independently for every child. */
Boolean
_XmAppearanceDefersChildPropagation(Display *display)
{
    XmAppearanceDisplayState *state = find_display_state(display);
    return state != NULL && state->resource_commit_depth != 0;
}

Boolean
_XmAppearanceDefersLayout(Display *display)
{
    XmAppearanceDisplayState *state = find_display_state(display);
    return state != NULL && state->defer_layout;
}

static XmAppearanceWidgetState *
find_live_widget_state(Widget widget)
{
    XmAppearanceWidgetState *state;

    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->gadget && !state->retired && state->widget == widget)
            return state;
    return NULL;
}

/* Rendering must distinguish defaults from explicit equal-valued resources. */
Boolean
_XmAppearanceOwnsResource(Widget widget, const char *name)
{
    Boolean owned = False;
    _XmProcessLock();
    /* This query also serves gadgets. Their Object destroy callbacks retire
     * state before an address can be reused, just as for windowed controls. */
    XmAppearanceWidgetState *state = appearance_widgets;
    while (state && (state->retired || state->widget != widget)) state = state->next;
    if (state && state->initialized)
        for (AppearanceProperty property = 0; property < AP_COUNT; ++property)
            if (!strcmp(name, appearance_property_name(property))) {
                owned = state->supported[property] && state->owned[property];
                break;
            }
    _XmProcessUnlock();
    return owned;
}

static void
appearance_release_gadget_states(Display *display)
{
    XmAppearanceWidgetState **link;
    XmAppearanceWidgetState *state;

    _XmProcessLock();
    link = &appearance_widgets;
    while (*link != NULL) {
        state = *link;
        if (state->display == display && state->gadget) {
            if (state->widget != NULL)
                XtRemoveCallback(state->widget, XmNdestroyCallback,
                    appearance_widget_destroyed, state);
            state->retired = True;
            *link = state->next;
            if (state->references == 0)
                appearance_free_widget_state(state);
            continue;
        }
        link = &state->next;
    }
    _XmProcessUnlock();
}

static int
appearance_display_closed(Display *display, XExtCodes *codes)
{
    (void)codes;
    _XmDestroyAppearanceDisplay(display);
    return 0;
}

static XmAppearanceDisplayState *
ensure_display_state(Display *display)
{
    XmAppearanceDisplayState *state = find_display_state(display);
    if (state != NULL)
        return state;
    state = (XmAppearanceDisplayState *)XtCalloc(1, sizeof(*state));
    if (state == NULL)
        return NULL;
    state->display = display;
    /* Selection is also valid on an XOpenDisplay connection before Xt is
     * initialized. Use Xlib lifetime tracking here; install Xt hooks only when
     * an actual Motif object registers on the initialized Xt display. */
    {
        XExtCodes *extension = XAddExtension(display);
        if (extension == NULL) {
            XtFree((char *)state);
            return NULL;
        }
        XESetCloseDisplay(display, extension->extension, appearance_display_closed);
    }
    state->next = appearance_displays;
    appearance_displays = state;
    return state;
}

static void
appearance_widget_destroyed(Widget widget, XtPointer closure,
                            XtPointer call_data)
{
    XmAppearanceWidgetState *state = (XmAppearanceWidgetState *)closure;
    XmAppearanceWidgetState **link;
    (void)widget;
    (void)call_data;
    _XmProcessLock();
    state->retired = True;
    state->widget = NULL;
    if (state->references == 0) {
        link = &appearance_widgets;
        while (*link != NULL && *link != state)
            link = &(*link)->next;
        if (*link == state)
            *link = state->next;
        appearance_free_widget_state(state);
    }
    _XmProcessUnlock();
}

static void
read_appearance_values(XmAppearanceWidgetState *state, XtArgVal *values)
{
    Arg get_args[AP_COUNT];
    Cardinal count = 0;
    /* GetValues writes the resource's actual size, which can be smaller than
     * XtArgVal (Dimension/Boolean). Clear the remaining bytes before comparing. */
    memset(values, 0, AP_COUNT * sizeof(*values));
    for (AppearanceProperty property = 0; property < AP_COUNT; ++property)
        if (state->supported[property])
        {
            XtSetArg(get_args[count],
                     (String)appearance_property_name(property),
                     &values[property]);
            ++count;
        }
    if (count != 0)
        XtGetValues(state->widget, get_args, count);
}

static Boolean initialize_appearance_state(XmAppearanceWidgetState *state,
                                        XmAppearanceProfile current_profile);

static const char *const appearance_provider_font_names[3][4] = {
    {XmNlabelRenderTable, XmNlabelFontList,
     XmNdefaultEntryLabelRenderTable, XmNdefaultEntryLabelFontList},
    {XmNbuttonRenderTable, XmNbuttonFontList},
    {XmNtextRenderTable, XmNtextFontList}
};
static const char *const appearance_provider_font_classes[3][4] = {
    {XmCLabelRenderTable, XmCLabelFontList, XmCRenderTable, XmCFontList},
    {XmCButtonRenderTable, XmCButtonFontList},
    {XmCTextRenderTable, XmCTextFontList}
};

/* The default-font trait returns a borrowed table. Remember its provenance
 * while the child is being initialized, before the create hook assigns modern
 * defaults. A table inherited from an application choice remains authoritative. */
void
_XmAppearanceNoteFontDefault(Widget widget, Widget provider, unsigned char kind)
{
    XmAppearanceWidgetState *state;
    XmAppearancePendingFont *pending;
    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->retired && state->widget == widget) {
            if (!state->initialized) {
                state->font_provider = provider;
                state->font_kind = kind;
            }
            return;
        }
    if (!XmIsPrimitive(widget) && !XmIsManager(widget) && !XmIsGadget(widget))
        return;
    for (pending = appearance_pending_fonts; pending; pending = pending->next)
        if (pending->widget == widget) break;
    if (pending == NULL) {
        pending = (XmAppearancePendingFont *)XtMalloc(sizeof(*pending));
        if (pending == NULL) {
            XmeWarning(widget, "Cannot retain default font provenance");
            return;
        }
        pending->widget = widget;
        pending->display = XtDisplayOfObject(widget);
        pending->next = appearance_pending_fonts;
        appearance_pending_fonts = pending;
    }
    pending->provider = provider;
    pending->kind = kind;
}

static Boolean
appearance_provider_font_explicit(XmAppearanceWidgetState *state)
{
    unsigned int index;
    if (state->font_kind < XmLABEL_FONTLIST || state->font_kind > XmTEXT_FONTLIST)
        return False;
    index = state->font_kind - XmLABEL_FONTLIST;
    for (Widget provider = state->font_provider; provider; provider = XtParent(provider)) {
        XmAppearanceWidgetState *parent = find_live_widget_state(provider);
        XrmValue value;
        if (parent && (parent->font_provider_overrides & (1u << index)))
            return True;
        for (unsigned int alias = 0; alias < XtNumber(appearance_provider_font_names[0]); ++alias)
            if (appearance_provider_font_names[index][alias] &&
                appearance_database_named_value(provider, XrmGetDatabase(state->display),
                    appearance_provider_font_names[index][alias],
                    appearance_provider_font_classes[index][alias], &value))
                return True;
    }
    return False;
}

static unsigned char
appearance_font_role(XmAppearanceWidgetState *state)
{
    Widget provider = state->widget;
    unsigned int depth = 0;
    while (state->inherit_font_role && !state->explicit_role && state->font_provider && depth++ < 64) {
        XmAppearanceWidgetState *parent = find_live_widget_state(state->font_provider);
        if (parent == NULL || parent->retired) break;
        state = parent;
        provider = parent->widget;
    }
    unsigned char role = appearance_role(provider);
    if (role == XmAPPEARANCE_ROLE_NORMAL && !state->explicit_role &&
        XtIsSubclass(provider, xmAvatarWidgetClass))
        return XmAPPEARANCE_ROLE_SECTION;
    return role;
}

/* Composite constructors and font setters pass a table as an implementation
 * detail. Do not mistake that argument for an independent application font.
 * During an appearance transaction, retain the preflight ownership decisions:
 * an explicitly styled child must still be restored after its parent commits. */
void
_XmAppearanceInheritFont(Widget child, Widget owner)
{
    XmAppearanceWidgetState *state;
    XmAppearanceDisplayState *display_state;
    XrmValue value;
    if (!child || !owner) return;
    display_state = find_display_state(XtDisplayOfObject(child));
    if (display_state && display_state->transaction_active) return;
    for (state = appearance_widgets; state; state = state->next) {
        if (state->retired || state->widget != child) continue;
        if (!state->supported[AP_RENDER_TABLE]) return;
        state->font_provider = owner;
        state->inherit_font_role = True;
        state->explicit_arg[AP_RENDER_TABLE] = False;
        state->owned[AP_RENDER_TABLE] = !appearance_database_value(child,
            XrmGetDatabase(state->display), AP_RENDER_TABLE, &value);
        return;
    }
}

/* Composite foreground inheritance follows explicit resource ownership.
 * Theme-owned child surfaces retain their own palette unless an application
 * explicitly styles their provider. No pixel-equality inference is involved. */
static Boolean
appearance_inherited_foreground(XmAppearanceWidgetState *state, XtArgVal *value)
{
    unsigned int depth = 0;
    while (state->foreground_provider && depth++ < 64) {
        XmAppearanceWidgetState *parent = find_live_widget_state(state->foreground_provider);
        Boolean explicit;
        XrmValue database_value;
        if (!parent || !parent->supported[AP_FOREGROUND]) break;
        explicit = parent->initialized ? !parent->owned[AP_FOREGROUND] :
            (parent->explicit_arg[AP_FOREGROUND] ||
             appearance_database_value(parent->widget, XrmGetDatabase(parent->display),
                                       AP_FOREGROUND, &database_value));
        if (explicit) {
            Pixel pixel;
            XtVaGetValues(parent->widget, XmNforeground, &pixel, NULL);
            *value = pixel;
            return True;
        }
        state = parent;
    }
    return False;
}

void
_XmAppearanceInheritForeground(Widget child, Widget owner)
{
    XmAppearanceWidgetState *state;
    XmAppearanceDisplayState *display_state;
    XtArgVal value;
    Boolean was_active;
    if (!child || !owner || XtParent(child) != owner) return;
    state = find_live_widget_state(child);
    if (!state || !state->supported[AP_FOREGROUND]) return;
    state->foreground_provider = owner;
    /* The child create hook has already established database/argument ownership.
     * Do not clear it when registering the composite relationship. */
    if (!state->initialized || !state->owned[AP_FOREGROUND] ||
        !appearance_inherited_foreground(state, &value)) return;
    display_state = find_display_state(state->display);
    if (!display_state) return;
    ++state->references;
    was_active = display_state->transaction_active;
    display_state->transaction_active = True;
    XtVaSetValues(child, XmNforeground, (Pixel)value, NULL);
    display_state->transaction_active = was_active;
    --state->references;
    if (state->retired && state->references == 0)
        appearance_free_widget_state(state);
}

static Boolean
appearance_has_foreground_dependents(Widget owner)
{
    XmAppearanceWidgetState *state;
    for (state = appearance_widgets; state; state = state->next)
        if (!state->retired && state->foreground_provider == owner)
            return True;
    return False;
}

static XmRenderTable
appearance_inherited_application_font(XmAppearanceWidgetState *state)
{
    unsigned int depth = 0;
    while (state->inherit_font_role && state->font_provider && depth++ < 64) {
        XmAppearanceWidgetState *parent = find_live_widget_state(state->font_provider);
        if (!parent || !parent->initialized) break;
        if (parent->supported[AP_RENDER_TABLE] && !parent->owned[AP_RENDER_TABLE]) {
            XmRenderTable table = NULL;
            XtVaGetValues(parent->widget, XmNrenderTable, &table, NULL);
            return table;
        }
        state = parent;
    }
    return NULL;
}

/* Each table owns its fonts through ordinary rendition references. Pixels are
 * derived once from logical points and the widget's effective scale; Xft must
 * not apply a second resource-DPI conversion to an already scaled size. */
static XmRenderTable
appearance_create_font(Widget widget, unsigned char role, double scale)
{
    int points = role == XmAPPEARANCE_ROLE_PAGE_TITLE ? 18 :
                 role == XmAPPEARANCE_ROLE_SECTION ? 12 : 11;
    Boolean bold = role == XmAPPEARANCE_ROLE_SECTION;
    Boolean mono = role == XmAPPEARANCE_ROLE_CODE;
    double pixels = points * (96.0 / 72.0) * scale;
    XmRendition rendition;
    XmRenderTable table;
    Arg args[6];
    Cardinal count = 0;
    int height = 0;
    if (!isfinite(pixels) || pixels < 1.0 || pixels > 65535.0) return NULL;
#ifdef USE_XFT
    {
        Display *display = XtDisplayOfObject(widget);
        XftFont *font = XftFontOpen(display, XScreenNumberOfScreen(XtScreenOfObject(widget)),
            XFT_FAMILY, XftTypeString, mono ? "monospace" : "sans-serif",
            XFT_PIXEL_SIZE, XftTypeDouble, pixels,
            XFT_WEIGHT, XftTypeInteger, bold ? XFT_WEIGHT_BOLD : XFT_WEIGHT_MEDIUM, NULL);
        if (font == NULL) return NULL;
        XtSetArg(args[count], XmNfontType, XmFONT_IS_XFT); ++count;
        XtSetArg(args[count], XmNfontName, mono ? "monospace" : "sans-serif"); ++count;
        XtSetArg(args[count], XmNfontSize, points); ++count;
        XtSetArg(args[count], XmNxftFont, font); ++count;
        XtSetArg(args[count], XmNloadModel, XmLOAD_IMMEDIATE); ++count;
        rendition = XmRenditionCreate(widget, XmFONTLIST_DEFAULT_TAG, args, count);
        XftFontClose(display, font); /* Rendition retains its own reference. */
    }
#else
    {
        char primary[160], fixed[160];
        const char *selected = NULL;
        XFontStruct *probe = NULL;
        Display *display = XtDisplayOfObject(widget);
        /* Courier supplies scalable code text. A fixed-family bitmap may
         * advertise synthesized sizes which the server cannot actually open;
         * probe candidates before asking Xt to construct the rendition. */
        snprintf(primary, sizeof(primary), "-*-%s-%s-r-normal--%d-*-*-*-*-*-*-*",
                 mono ? "courier" : "helvetica", bold ? "bold" : "medium", (int)(pixels + 0.5));
        snprintf(fixed, sizeof(fixed), "-*-fixed-medium-r-*--%d-*-*-*-*-*-*-*", (int)(pixels + 0.5));
        const char *candidates[] = {primary, fixed, "fixed"};
        for (unsigned int i = 0; i < XtNumber(candidates); ++i) {
            probe = XLoadQueryFont(display, candidates[i]);
            if (probe) { selected = candidates[i]; break; }
        }
        if (!probe) return NULL;
        XtSetArg(args[count], XmNfontType, XmFONT_IS_FONT); ++count;
        XtSetArg(args[count], XmNfontName, selected); ++count;
        XtSetArg(args[count], XmNloadModel, XmLOAD_IMMEDIATE); ++count;
        rendition = XmRenditionCreate(widget, XmFONTLIST_DEFAULT_TAG, args, count);
        XFreeFont(display, probe);
    }
#endif
    if (rendition == NULL) return NULL;
    table = XmRenderTableAddRenditions(NULL, &rendition, 1, XmMERGE_REPLACE);
    XmRenditionFree(rendition);
    if (table) XmRenderTableGetDefaultFontExtents(table, &height, NULL, NULL);
#ifndef USE_XFT
    if (height < 1) {
        XmFontListEntry entry;
        if (table) XmRenderTableFree(table);
        entry = XmFontListEntryLoad(XtDisplayOfObject(widget), "fixed", XmFONT_IS_FONT, XmFONTLIST_DEFAULT_TAG);
        table = entry ? XmFontListAppendEntry(NULL, entry) : NULL;
        if (entry) XmFontListEntryFree(&entry);
        if (table) XmRenderTableGetDefaultFontExtents(table, &height, NULL, NULL);
    }
#endif
    if (height < 1) {
        if (table) XmRenderTableFree(table);
        return NULL;
    }
    return table;
}

/* Scale owns its internal slider allocation and minimum knob diameter. */
static Boolean
appearance_sizes_scrollbar(Widget widget)
{
    return XmIsScrollBar(widget) &&
        !(XtParent(widget) && XmIsScale(XtParent(widget)));
}

static Dimension
appearance_scrollbar_thickness(XmAppearanceWidgetState *state,
                               XmAppearanceProfile profile,
                               XmAppearanceDensity density)
{
    if (profile == XmAPPEARANCE_MODERN_LIGHT || profile == XmAPPEARANCE_MODERN_DARK)
        return _XmDpiDefaultDimension(state->widget,
            density == XmAPPEARANCE_DENSITY_COMPACT ? 16 : 18);
    return state->original_scrollbar_thickness;
}

static void
appearance_plan_clear(XmAppearanceChangePlan *plan)
{
    if (plan->prepared_font) XmRenderTableFree(plan->prepared_font);
    if (plan->retained_font) XmRenderTableFree(plan->retained_font);
    plan->prepared_font = plan->retained_font = NULL;
}

static Boolean
appearance_widget_prepare(XmAppearanceWidgetState *state,
                           XmAppearanceProfile new_profile,
                           XmAppearanceDensity density,
                           XmAppearanceChangePlan *plan)
{
    plan->state = state;
    memcpy(plan->owned, state->owned, sizeof(plan->owned));
    for (AppearanceProperty property = 0; property < AP_COUNT; ++property) {
        const char *profile_text;

        if (!state->supported[property])
            continue;
        /* Track pixmap ownership without changing or retaining its resource. */
        if (property == AP_EXPANDED_PIXMAP || property == AP_COLLAPSED_PIXMAP)
            continue;
        /* RowColumn permits shadow-width changes on menu panes and bars. Other
         * variants own that geometry internally, regardless of the profile. */
        if (property == AP_SHADOW_THICKNESS && XmIsRowColumn(state->widget) &&
            RC_Type(state->widget) != XmMENU_POPUP &&
            RC_Type(state->widget) != XmMENU_PULLDOWN &&
            RC_Type(state->widget) != XmMENU_BAR)
            continue;
        /* ScrolledWindow owns navigator window borders and lays them out at
         * zero. Reapplying a pre-insertion baseline would reject an otherwise
         * valid thickness change as a forbidden border-width request. */
        if (property == AP_BORDER_WIDTH && XmIsScrollBar(state->widget) &&
            XtParent(state->widget) && XmIsScrolledWindow(XtParent(state->widget)))
            continue;
        if (property == AP_RENDER_TABLE) {
            XmRenderTable inherited = appearance_inherited_application_font(state);
            if (!plan->owned[property]) {
                /* A composite may propagate its toolkit font into children.
                 * Retain explicit child tables before any parent is committed. */
                XtVaGetValues(state->widget, XmNrenderTable, &plan->observed_font, NULL);
                if (plan->observed_font) {
                    plan->retained_font = XmRenderTableCopy(plan->observed_font, NULL, 0);
                    if (!plan->retained_font) return False;
                }
                plan->values[property] = (XtArgVal)plan->retained_font;
            } else if (inherited) {
                plan->prepared_font = XmRenderTableCopy(inherited, NULL, 0);
                if (!plan->prepared_font) return False;
                plan->values[property] = (XtArgVal)plan->prepared_font;
            } else if (new_profile == XmAPPEARANCE_CLASSIC) {
                plan->values[property] = state->original[property];
            } else {
                double scale = 1.0;
                unsigned char role = appearance_font_role(state);
                (void)_XmGetWidgetDpiScale(state->widget, &scale);
                plan->font_scale = scale;
                plan->font_role = role;
                if (!state->current_font || state->font_scale != scale || state->font_role != role) {
                    plan->prepared_font = appearance_create_font(state->widget, role, scale);
                    if (!plan->prepared_font) return False;
                }
                plan->values[property] = (XtArgVal)(plan->prepared_font ? plan->prepared_font : state->current_font);
            }
            plan->apply[property] = True;
            continue;
        }
        /* ComboBox propagates its shadow into its editor during SetValues.
         * Retain an explicit child border before the parent commits, just as
         * explicit child font tables are retained above. */
        if (property == AP_SHADOW_THICKNESS && !plan->owned[property] &&
            XmIsTextField(state->widget) && XtParent(state->widget) &&
            XmIsComboBox(XtParent(state->widget))) {
            Dimension shadow;
            XtVaGetValues(state->widget, XmNshadowThickness, &shadow, NULL);
            plan->values[property] = shadow;
            plan->apply[property] = True;
            continue;
        }
        /* Ownership follows explicit arguments and Xt changes, not equality:
         * a parent's internal margin normalization is not an app override. */
        if (!plan->owned[property])
            continue;
        if (property == AP_FOREGROUND &&
            appearance_inherited_foreground(state, &plan->values[property])) {
            plan->apply[property] = True;
            continue;
        }
        profile_text = profile_property_string(state->widget, new_profile,
                                               density, property);
        if (profile_text == NULL)
            plan->values[property] = state->original[property];
        else if (!profile_property_value(state->widget, new_profile, density, property,
                                         &plan->values[property]))
            return False;
        plan->apply[property] = True;
    }
    if (XmIsArrowButton(state->widget) || XmIsArrowButtonGadget(state->widget)) {
        Dimension highlight,shadow;
        XtVaGetValues(state->widget,XmNhighlightThickness,&highlight,
                      XmNshadowThickness,&shadow,NULL);
        if (plan->apply[AP_HIGHLIGHT_THICKNESS])
            highlight = (Dimension)plan->values[AP_HIGHLIGHT_THICKNESS];
        if (plan->apply[AP_SHADOW_THICKNESS])
            shadow = (Dimension)plan->values[AP_SHADOW_THICKNESS];
        plan->arrow_geometry = True;
        plan->arrow_extent = _XmAppearanceArrowExtent(state->widget,new_profile,
                                                       density,highlight,shadow);
    }
    if (appearance_sizes_scrollbar(state->widget)) {
        unsigned char orientation;
        XtVaGetValues(state->widget, XmNorientation, &orientation, NULL);
        plan->scrollbar_geometry = True;
        plan->scrollbar_axis = orientation == XmHORIZONTAL ? 1 : 0;
        plan->scrollbar_extent = appearance_scrollbar_thickness(state,new_profile,density);
    }
    return True;
}

/* Appearance changes must not turn a font update into an implicit request to
 * replace application-constrained geometry. Preserve the current allocation,
 * not the original requested pixels: a parent may legitimately resize it. */
static void
appearance_dimension_arguments(XmAppearanceWidgetState *state,
                               ArgList args, Cardinal count)
{
    const char *names[] = {XmNwidth, XmNheight};
    for (Cardinal i = 0; i < count; ++i)
        for (unsigned axis = 0; axis < 2; ++axis)
            if (!strcmp(args[i].name, names[axis])) {
                state->dimension_argument[axis] = True;
                state->constrained_dimension[axis] = (Dimension)args[i].value != 0;
            }
}

static void
appearance_widget_commit(XmAppearanceChangePlan *plan)
{
    XmAppearanceWidgetState *state = plan->state;
    Arg set_args[AP_COUNT + 2];
    Cardinal count = 0;
    XmRenderTable old_font = NULL;
    if (plan->prepared_font) {
        old_font = state->current_font;
        state->current_font = plan->prepared_font;
        plan->prepared_font = NULL;
        state->font_scale = plan->font_scale;
        state->font_role = plan->font_role;
    }
    if (plan->apply[AP_RENDER_TABLE] && !plan->owned[AP_RENDER_TABLE]) {
        XmRenderTable current = NULL;
        XtVaGetValues(state->widget, XmNrenderTable, &current, NULL);
        if (current == plan->observed_font) plan->apply[AP_RENDER_TABLE] = False;
    }
    state->applied_role = appearance_role(state->widget);
    memcpy(state->owned, plan->owned, sizeof(state->owned));
    for (AppearanceProperty property = 0; property < AP_COUNT; ++property)
        if (plan->apply[property]) {
            XtSetArg(set_args[count],
                     (String)appearance_property_name(property),
                     plan->values[property]);
            ++count;
        }
    if (count != 0 || plan->arrow_geometry || plan->scrollbar_geometry || XmIsPushButton(state->widget) ||
        XmIsPushButtonGadget(state->widget) || XmIsRatingControl(state->widget)) {
        const char *names[] = {XmNwidth, XmNheight};
        for (unsigned axis = 0; axis < 2; ++axis)
            if (state->constrained_dimension[axis]) {
                Dimension extent;
                XtVaGetValues(state->widget, names[axis], &extent, NULL);
                XtSetArg(set_args[count], (String)names[axis], extent);
                ++count;
            } else if (plan->arrow_geometry) {
                XtSetArg(set_args[count], (String)names[axis], plan->arrow_extent);
                ++count;
            } else if (plan->scrollbar_geometry && axis == plan->scrollbar_axis) {
                XtSetArg(set_args[count], (String)names[axis], plan->scrollbar_extent);
                ++count;
            }
        XmAppearanceDisplayState *display_state = find_display_state(state->display);
        ++display_state->resource_commit_depth;
        XtSetValues(state->widget, set_args, count);
        --display_state->resource_commit_depth;
    }
    if (old_font) XmRenderTableFree(old_font);
    if (state->widget != NULL && XtIsWidget(state->widget)) {
        if (_XmPaintBufferKind(state->widget))
            _XmPaintInvalidate(state->widget);
        else if (XtIsRealized(state->widget))
            XClearArea(XtDisplay(state->widget), XtWindow(state->widget),
                       0, 0, 0, 0, True);
    } else if (state->parent != NULL && XtIsRealized(state->parent)) {
        XClearArea(XtDisplay(state->parent), XtWindow(state->parent),
                   0, 0, 0, 0, True);
    }
}

static Boolean
initialize_appearance_state(XmAppearanceWidgetState *state,
                            XmAppearanceProfile current_profile)
{
    (void)current_profile;
    state->applied_role = appearance_role(state->widget);
    read_appearance_values(state, state->original);
    if (XmIsScrollBar(state->widget)) {
        unsigned char orientation;
        XtVaGetValues(state->widget, XmNorientation, &orientation, NULL);
        XtVaGetValues(state->widget, orientation == XmHORIZONTAL ? XmNheight : XmNwidth,
                      &state->original_scrollbar_thickness, NULL);
    }
    if (state->supported[AP_RENDER_TABLE] && state->original[AP_RENDER_TABLE] != 0) {
        state->original_font = XmRenderTableCopy((XmRenderTable)state->original[AP_RENDER_TABLE], NULL, 0);
        if (state->original_font == NULL) return False;
        state->original[AP_RENDER_TABLE] = (XtArgVal)state->original_font;
    }
    for (AppearanceProperty property = 0; property < AP_COUNT; ++property) {
        XrmValue xrm_value;
        XrmDatabase application = XrmGetDatabase(state->display);
        if (!state->supported[property] || state->explicit_arg[property])
            continue;
        /* An application override remains authoritative even when its value
         * happens to equal the current theme default. */
        state->owned[property] = !appearance_database_value(state->widget,
            application, property, &xrm_value);
    }
    if (state->supported[AP_RENDER_TABLE] && appearance_provider_font_explicit(state))
        state->owned[AP_RENDER_TABLE] = False;
    {
        XrmValue value;
        if (appearance_database_named_value(state->widget, XrmGetDatabase(state->display),
                XmNappearanceRole, XmCAppearanceRole, &value))
            state->explicit_role = True;
    }
    {
        const char *names[] = {XmNwidth, XmNheight};
        const char *classes[] = {XmCWidth, XmCHeight};
        for (unsigned axis = 0; axis < 2; ++axis) {
            XrmValue value;
            XrmRepresentation type;
            if (!state->dimension_argument[axis] &&
                appearance_database_typed_value(state->widget,
                    XrmGetDatabase(state->display), names[axis], classes[axis], &type, &value)) {
                Dimension extent = 0;
                XrmValue converted = {sizeof(extent), (XPointer)&extent};
                if (XtConvertAndStore(state->widget, XrmQuarkToString(type), &value,
                                      XmRDimension, &converted))
                    state->constrained_dimension[axis] = extent != 0;
            }
        }
    }
    state->initialized = True;
    return True;
}

static void
appearance_hooks_destroyed(Widget hooks, XtPointer closure, XtPointer data)
{
    XmAppearanceDisplayState *display_state = closure;
    (void)hooks;
    (void)data;
    display_state->hooks = NULL;
}

/* Xt's create hook runs after class initializers, initialize hooks and gadget
 * cache setup. Snapshot here rather than reading a partially initialized Text
 * backend or treating constructor normalization as an application override. */
static void
appearance_created(Widget hooks, XtPointer closure, XtPointer data)
{
    XtCreateHookData created = data;
    XmAppearanceDisplayState *display_state = closure;
    XmAppearanceWidgetState *state;
    XmAppearanceProfile profile = XmAPPEARANCE_CLASSIC;
    (void)hooks;
    if (created == NULL) return;
    (void)XmGetAppearanceProfile(display_state->display, &profile);
    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->retired && state->widget == created->widget &&
            !state->initialized) {
            if (!initialize_appearance_state(state, profile)) break;
            if (profile != XmAPPEARANCE_CLASSIC && XtIsRectObj(state->widget)) {
                XmAppearanceDensity density;
                XmAppearanceChangePlan plan;
                Boolean was_active = display_state->transaction_active;
                memset(&plan, 0, sizeof(plan));
                if (!XmGetAppearanceDensity(display_state->display, &density) ||
                    !appearance_widget_prepare(state, profile, density, &plan)) {
                    appearance_plan_clear(&plan);
                    XtAppWarning(XtWidgetToApplicationContext(state->widget),
                                 "Cannot prepare initial Motif appearance");
                    break;
                }
                ++state->references;
                display_state->transaction_active = True;
                appearance_widget_commit(&plan);
                appearance_plan_clear(&plan);
                display_state->transaction_active = was_active;
                --state->references;
                if (state->retired && state->references == 0)
                    appearance_free_widget_state(state);
            }
            break;
        }
}

static Boolean
appearance_apply_subtree(Display *display, Widget root,
                         XmAppearanceProfile old_profile,
                         XmAppearanceProfile new_profile,
                         XmAppearanceDensity density);

/* A composite constraint reset restores default-font provenance, not an
 * explicit copy of the provider's fallback table. Reapply immediately so the
 * label does not need a later profile or scale change to become correct. */
Boolean
_XmAppearanceResetDefaultFont(Widget child, Widget provider, unsigned char kind)
{
    XmAppearanceWidgetState *state = find_live_widget_state(child);
    XmAppearanceDisplayState *display_state;
    XmAppearanceProfile profile;
    XmAppearanceDensity density;
    XmRenderTable font, old_font;
    Widget old_provider;
    unsigned char old_kind;
    Boolean old_owned, old_explicit, old_inherit, success;
    XrmValue value;
    if (!state || !state->initialized || !state->supported[AP_RENDER_TABLE])
        return True;
    display_state = find_display_state(state->display);
    if (!display_state || !XmGetAppearanceProfile(state->display, &profile) ||
        !XmGetAppearanceDensity(state->display, &density) ||
        !appearance_transaction_begin(display_state)) return False;
    ++state->references;
    font = XmRenderTableCopy(XmeGetDefaultRenderTable(child, kind), NULL, 0);
    if (!font) {
        appearance_transaction_end(display_state);
        --state->references;
        return False;
    }
    old_font = state->original_font;
    old_provider = state->font_provider;
    old_kind = state->font_kind;
    old_owned = state->owned[AP_RENDER_TABLE];
    old_explicit = state->explicit_arg[AP_RENDER_TABLE];
    old_inherit = state->inherit_font_role;
    state->font_provider = provider;
    state->font_kind = kind;
    state->inherit_font_role = False;
    state->explicit_arg[AP_RENDER_TABLE] = False;
    state->owned[AP_RENDER_TABLE] =
        !appearance_database_value(child, XrmGetDatabase(state->display),
                                   AP_RENDER_TABLE, &value) &&
        !appearance_provider_font_explicit(state);
    state->original_font = font;
    state->original[AP_RENDER_TABLE] = (XtArgVal)font;
    success = appearance_apply_subtree(state->display, child, profile, profile, density);
    if (!success) {
        state->original_font = old_font;
        state->original[AP_RENDER_TABLE] = (XtArgVal)old_font;
        state->font_provider = old_provider;
        state->font_kind = old_kind;
        state->owned[AP_RENDER_TABLE] = old_owned;
        state->explicit_arg[AP_RENDER_TABLE] = old_explicit;
        state->inherit_font_role = old_inherit;
        XmRenderTableFree(font);
    } else if (old_font) {
        XmRenderTableFree(old_font);
    }
    appearance_transaction_end(display_state);
    --state->references;
    if (state->retired && state->references == 0)
        appearance_free_widget_state(state);
    return success;
}

static Boolean
appearance_has_argument(ArgList args, Cardinal count, const char *name)
{
    for (Cardinal i = 0; i < count; ++i)
        if (strcmp(args[i].name, name) == 0) return True;
    return False;
}

/* Equal-valued XtSetValues calls are still explicit application choices.
 * Record arguments, not just a later comparison with the previous value. */
static void
appearance_changed(Widget hooks, XtPointer closure, XtPointer data)
{
    XtChangeHookData changed = data;
    XmAppearanceDisplayState *display_state = closure;
    XmAppearanceWidgetState *state;
    XtChangeHookSetValuesData values;
    (void)hooks;
    if (display_state->transaction_active || changed == NULL ||
        changed->type == NULL || strcmp(changed->type, XtHsetValues) != 0 ||
        changed->event_data == NULL) return;
    values = changed->event_data;
    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->retired && state->initialized &&
            state->widget == changed->widget) {
            appearance_dimension_arguments(state, values->args, values->num_args);
            for (AppearanceProperty property = 0; property < AP_COUNT; ++property)
                if (appearance_explicit_arg(values->args, values->num_args, property)) {
                    Boolean repaint = state->owned[property] &&
                        (property == AP_EXPANDED_PIXMAP || property == AP_COLLAPSED_PIXMAP);
                    state->explicit_arg[property] = True;
                    state->owned[property] = False;
                    if (repaint && _XmPaintBufferKind(state->widget))
                        _XmPaintInvalidate(state->widget);
                    else if (repaint && XtIsRealized(state->widget))
                        XClearArea(state->display, XtWindow(state->widget), 0, 0, 0, 0, True);
                }
            for (unsigned int kind = 0; kind < 3; ++kind)
                for (unsigned int alias = 0; alias < XtNumber(appearance_provider_font_names[0]); ++alias)
                    if (appearance_provider_font_names[kind][alias] &&
                        appearance_has_argument(values->args, values->num_args,
                                                appearance_provider_font_names[kind][alias]))
                        state->font_provider_overrides |= 1u << kind;
            Boolean foreground_changed =
                appearance_has_argument(values->args, values->num_args, XmNforeground) &&
                appearance_has_foreground_dependents(state->widget);
            if (foreground_changed || appearance_has_argument(values->args, values->num_args, XmNappearanceRole) ||
                ((XmIsToggleButton(state->widget) || XmIsToggleButtonGadget(state->widget)) &&
                 appearance_has_argument(values->args, values->num_args, XmNindicatorOn))) {
                XmAppearanceProfile profile;
                XmAppearanceDensity density;
                Boolean role_argument = appearance_has_argument(values->args, values->num_args,
                                                                 XmNappearanceRole);
                if (role_argument) state->explicit_role = True;
                if ((foreground_changed || appearance_has_argument(values->args,values->num_args,XmNindicatorOn) ||
                     state->applied_role != appearance_role(state->widget)) &&
                    XmGetAppearanceProfile(state->display, &profile) &&
                    XmGetAppearanceDensity(state->display, &density) &&
                    appearance_transaction_begin(display_state)) {
                    Boolean success;
                    ++state->references;
                    success = appearance_apply_subtree(state->display, state->widget,
                                                        profile, profile, density);
                    if (!success && !state->retired) {
                        if (role_argument)
                            XtVaSetValues(state->widget, XmNappearanceRole,
                                          state->applied_role, NULL);
                        XmeWarning(state->widget, "Cannot prepare Motif appearance defaults");
                    }
                    appearance_transaction_end(display_state);
                    --state->references;
                    if (state->retired && state->references == 0)
                        appearance_free_widget_state(state);
                }
            } else if ((XmIsArrowButton(state->widget) || XmIsArrowButtonGadget(state->widget) ||
                        appearance_sizes_scrollbar(state->widget)) &&
                (appearance_has_argument(values->args,values->num_args,XmNwidth) ||
                 appearance_has_argument(values->args,values->num_args,XmNheight) ||
                 appearance_has_argument(values->args,values->num_args,XmNhighlightThickness) ||
                 appearance_has_argument(values->args,values->num_args,XmNshadowThickness) ||
                 appearance_has_argument(values->args,values->num_args,XmNorientation))) {
                Widget control = state->widget;
                XtWidgetGeometry preferred = {0};
                Dimension width,height,reply_width,reply_height;
                ++state->references;
                if (XmIsScrollBar(control)) {
                    unsigned char orientation;
                    XmAppearanceProfile profile = XmAPPEARANCE_CLASSIC;
                    XmAppearanceDensity density = XmAPPEARANCE_DENSITY_COMFORTABLE;
                    (void)XmGetAppearanceProfile(state->display,&profile);
                    (void)XmGetAppearanceDensity(state->display,&density);
                    XtVaGetValues(control,XmNorientation,&orientation,
                                  XmNwidth,&preferred.width,XmNheight,&preferred.height,NULL);
                    if (orientation == XmHORIZONTAL)
                        preferred.height = appearance_scrollbar_thickness(state,profile,density);
                    else
                        preferred.width = appearance_scrollbar_thickness(state,profile,density);
                } else
                    XtQueryGeometry(control,NULL,&preferred);
                if (!state->retired) {
                    XtGeometryResult result;
                    XtVaGetValues(control,XmNwidth,&width,XmNheight,&height,NULL);
                    if (!state->constrained_dimension[0]) width=preferred.width;
                    if (!state->constrained_dimension[1]) height=preferred.height;
                    /* Geometry requests do not turn automatic axes into explicit
                     * setters. The parent retains final allocation authority. */
                    result=XtMakeResizeRequest(control,width,height,&reply_width,&reply_height);
                    if (result==XtGeometryAlmost && !state->retired)
                        (void)XtMakeResizeRequest(control,reply_width,reply_height,NULL,NULL);
                }
                --state->references;
                if (state->retired && state->references==0)
                    appearance_free_widget_state(state);
            }
            break;
        }
}

Boolean
_XmRegisterAppearanceChange(Widget widget, ArgList args, Cardinal num_args)
{
    XmAppearanceWidgetState *state;
    XmAppearanceWidgetState *existing;
    XmAppearanceDisplayState *display_state;

    if (widget == NULL || !XtIsObject(widget))
        return False;
    display_state = ensure_display_state(XtDisplayOfObject(widget));
    if (display_state == NULL) return False;
    if (display_state->hooks == NULL) {
        display_state->hooks = XtHooksOfDisplay(display_state->display);
        XtAddCallback(display_state->hooks, XtNcreateHook,
                      appearance_created, display_state);
        XtAddCallback(display_state->hooks, XtNchangeHook,
                      appearance_changed, display_state);
        XtAddCallback(display_state->hooks, XtNdestroyCallback,
                      appearance_hooks_destroyed, display_state);
    }
    state = (XmAppearanceWidgetState *)XtCalloc(1, sizeof(*state));
    if (state == NULL)
        return False;
    state->widget = widget;
    state->gadget = !XtIsWidget(widget);
    state->parent = state->gadget ? XtParent(widget) : NULL;
    state->display = XtDisplayOfObject(widget);
    state->explicit_role = appearance_has_argument(args, num_args, XmNappearanceRole);
    appearance_dimension_arguments(state, args, num_args);
    for (unsigned int kind = 0; kind < 3; ++kind)
        for (unsigned int alias = 0; alias < XtNumber(appearance_provider_font_names[0]); ++alias)
            if (appearance_provider_font_names[kind][alias] &&
                appearance_has_argument(args, num_args,
                                        appearance_provider_font_names[kind][alias]))
                state->font_provider_overrides |= 1u << kind;
    for (AppearanceProperty property = 0; property < AP_COUNT; ++property)
        state->supported[property] = appearance_resource_supported(widget,
                                                                     property);
    for (AppearanceProperty property = 0; property < AP_COUNT; ++property)
        state->explicit_arg[property] = appearance_explicit_arg(args, num_args,
                                                                 property);
    _XmProcessLock();
    for (existing = appearance_widgets; existing != NULL;
         existing = existing->next)
        if (existing->widget == widget) {
            _XmProcessUnlock();
            XtFree((char *)state);
            return True;
        }
    {
        XmAppearancePendingFont **pending = &appearance_pending_fonts;
        while (*pending && (*pending)->widget != widget)
            pending = &(*pending)->next;
        if (*pending) {
            XmAppearancePendingFont *item = *pending;
            state->font_provider = item->provider;
            state->font_kind = item->kind;
            *pending = item->next;
            XtFree((char *)item);
        }
    }
    state->next = appearance_widgets;
    appearance_widgets = state;
    _XmProcessUnlock();
    /* RectObj gadgets inherit Object's destroyCallback too. Retire their
     * state immediately, before Xt can reuse the address for another gadget. */
    XtAddCallback(widget, XmNdestroyCallback, appearance_widget_destroyed,
                  (XtPointer)state);
    return True;
}

static Boolean
appearance_in_subtree(Widget widget, Widget root)
{
    if (root == NULL) return True;
    for (; widget != NULL; widget = XtParent(widget))
        if (widget == root) return True;
    return False;
}

static int
appearance_parent_first(const void *left, const void *right)
{
    const XmAppearanceWidgetState *a = *(XmAppearanceWidgetState *const *)left;
    const XmAppearanceWidgetState *b = *(XmAppearanceWidgetState *const *)right;
    size_t a_depth = 0, b_depth = 0;
    Widget widget;
    for (widget = a->widget; widget != NULL; widget = XtParent(widget)) ++a_depth;
    for (widget = b->widget; widget != NULL; widget = XtParent(widget)) ++b_depth;
    return (a_depth > b_depth) - (a_depth < b_depth);
}

static Boolean
appearance_apply_subtree(Display *display, Widget root,
                         XmAppearanceProfile old_profile,
                         XmAppearanceProfile new_profile,
                         XmAppearanceDensity density)
{
    XmAppearanceWidgetState **snapshot = NULL;
    XmAppearanceChangePlan *plans = NULL;
    XmAppearanceWidgetState *state;
    XmAppearanceDisplayState *display_state = find_display_state(display);
    size_t count = 0;
    Boolean success = True;

    _XmProcessLock();
    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->retired && state->display == display &&
            appearance_in_subtree(state->widget, root))
            ++count;
    if (count != 0 &&
        ((size_t)count > SIZE_MAX / sizeof(*snapshot) ||
         (size_t)count > UINT_MAX / sizeof(*snapshot) ||
         (size_t)count > SIZE_MAX / sizeof(*plans))) {
        _XmProcessUnlock();
        return False;
    }
    if (count != 0)
        snapshot = (XmAppearanceWidgetState **)XtMalloc(
	    (Cardinal)(count * sizeof(*snapshot)));
    if (count != 0 && snapshot == NULL) {
        _XmProcessUnlock();
        return False;
    }
    if (count != 0)
        plans = (XmAppearanceChangePlan *)calloc(count, sizeof(*plans));
    if (count != 0 && plans == NULL) {
        XtFree((char *)snapshot);
        _XmProcessUnlock();
        return False;
    }
    count = 0;
    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->retired && state->display == display &&
            appearance_in_subtree(state->widget, root)) {
            ++state->references;
            snapshot[count++] = state;
        }
    _XmProcessUnlock();
    /* Composite SetValues may propagate fonts to children. Apply parents
     * first, then restore authoritative child choices from the preflight. */
    if (count > 1)
        qsort(snapshot, count, sizeof(*snapshot), appearance_parent_first);
    for (size_t i = 0; i < count; ++i) {
        state = snapshot[i];
        if (!state->retired && state->widget != NULL) {
            if (!state->initialized && !initialize_appearance_state(state, old_profile))
                success = False;
            if (success && !appearance_widget_prepare(state, new_profile, density, &plans[i]))
                success = False;
        }
        if (!success)
            break;
    }
    if (!success) {
        for (size_t i = 0; i < count; ++i) {
            appearance_plan_clear(&plans[i]);
            state = snapshot[i];
            _XmProcessLock();
            if (state->references != 0)
                --state->references;
            if (state->retired && state->references == 0)
                appearance_free_widget_state(state);
            _XmProcessUnlock();
        }
        if (snapshot != NULL)
            XtFree((char *)snapshot);
        if (plans != NULL)
            free(plans);
        return False;
    }
    if (display_state != NULL) {
        display_state->committing_profile = new_profile;
        display_state->committing_density = density;
        display_state->committing_geometry = True;
        display_state->defer_layout = True;
    }
    for (size_t i = 0; i < count; ++i) {
        state = snapshot[i];
        if (!state->retired && state->widget != NULL)
            appearance_widget_commit(&plans[i]);
    }
    if (display_state != NULL)
        display_state->defer_layout = False;
    /* Retain every snapshot reference through the layout pass: a SetValues or
     * geometry callback may retire another object. Normalize only after all
     * siblings have their new margins. */
    for (size_t i = 0; i < count; ++i) {
        state = snapshot[i];
        if (!state->retired && state->widget != NULL &&
            XmIsRowColumn(state->widget)) {
            _XmRCDoMarginAdjustment((XmRowColumnWidget)state->widget);
            _XmRCAdaptToSize((XmRowColumnWidget)state->widget, NULL, NULL);
        }
    }
    if (display_state != NULL)
        display_state->committing_geometry = False;
    for (size_t i = 0; i < count; ++i) {
        appearance_plan_clear(&plans[i]);
        state = snapshot[i];
        _XmProcessLock();
        if (state->references != 0)
            --state->references;
        if (state->retired && state->references == 0)
            appearance_free_widget_state(state);
        _XmProcessUnlock();
    }
    if (snapshot != NULL)
        XtFree((char *)snapshot);
    if (plans != NULL)
        free(plans);
    return success;
}

Boolean
_XmApplyAppearanceChange(Display *display, XmAppearanceProfile old_profile,
                         XmAppearanceProfile new_profile,
                         XmAppearanceDensity density)
{
    return appearance_apply_subtree(display, NULL, old_profile, new_profile, density);
}

/* Scale metadata is staged by Display.c before this preflight. Reject
 * reentrancy and leave callbacks untouched if font/metric preparation fails. */
Boolean
_XmApplyAppearanceScale(Display *display, Widget root)
{
    XmAppearanceDisplayState *state = find_display_state(display);
    XmAppearanceProfile profile;
    XmAppearanceDensity density;
    Boolean success;
    if (!XmGetAppearanceProfile(display, &profile) ||
        !XmGetAppearanceDensity(display, &density)) return False;
    if (profile == XmAPPEARANCE_CLASSIC || state == NULL) return True;
    if (!appearance_transaction_begin(state)) return False;
    success = appearance_apply_subtree(display, root, profile, profile, density);
    appearance_transaction_end(state);
    return success;
}

/* An arrow is an affordance inside a hit target, not the entire target.
 * Keep it centered and separated from the border even when a layout manager
 * allocates a large cell. The caller has already excluded border/focus space. */
void
_XmAppearanceInsetArrow(Widget widget, Position *x, Position *y,
                         Dimension *width, Dimension *height)
{
    XmAppearanceProfile profile;
    XmAppearanceDensity density;
    unsigned int padding, side, available;
    if (widget == NULL || x == NULL || y == NULL ||
        width == NULL || height == NULL)
        return;
    if (!XmGetAppearanceProfile(XtDisplayOfObject(widget), &profile) ||
        (profile != XmAPPEARANCE_MODERN_LIGHT && profile != XmAPPEARANCE_MODERN_DARK))
        return;
    if (!XmGetAppearanceDensity(XtDisplayOfObject(widget), &density)) return;
    padding = _XmDpiDefaultDimension(widget, 4);
    side = _XmDpiDefaultDimension(widget,
        density == XmAPPEARANCE_DENSITY_COMPACT ? 16 : 18);
    available = *width < *height ? *width : *height;
    available = available > 2 * padding ? available - 2 * padding : 0;
    if (side > available) side = available;
    {
        long long next_x = (long long)*x + (long long)(*width - side) / 2;
        long long next_y = (long long)*y + (long long)(*height - side) / 2;
        *x = (Position)(next_x < SHRT_MIN ? SHRT_MIN :
                        next_x > SHRT_MAX ? SHRT_MAX : next_x);
        *y = (Position)(next_y < SHRT_MIN ? SHRT_MIN :
                        next_y > SHRT_MAX ? SHRT_MAX : next_y);
    }
    *width = *height = (Dimension)side;
}

/* Gadget constructors inherit manager colors before appearance is applied.
 * Use the manager's unstyled defaults for that baseline, so later classic
 * restoration does not preserve a modern parent's colors. Explicit parent
 * choices remain current values; callers supply those as their fallback. */
void
_XmAppearanceDefaultColors(Widget parent, Pixel *background, Pixel *foreground,
                          Pixel *top_shadow, Pixel *bottom_shadow, Pixel *highlight)
{
    static const AppearanceProperty properties[] = {AP_BACKGROUND, AP_FOREGROUND,
        AP_TOP_SHADOW, AP_BOTTOM_SHADOW, AP_HIGHLIGHT};
    Pixel *outputs[] = {background, foreground, top_shadow, bottom_shadow, highlight};
    XmAppearanceProfile profile;
    XmAppearanceWidgetState *state;
    if (parent == NULL || !XmGetAppearanceProfile(XtDisplayOfObject(parent), &profile) ||
        profile == XmAPPEARANCE_CLASSIC) return;
    state = find_live_widget_state(parent);
    if (state == NULL || !state->initialized) return;
    for (Cardinal i = 0; i < XtNumber(properties); ++i)
        if (outputs[i] != NULL && state->supported[properties[i]] &&
            state->owned[properties[i]])
            *outputs[i] = (Pixel)state->original[properties[i]];
}

static Boolean
profile_from_name(const char *name, XmAppearanceProfile *profile_return)
{
    if (name != NULL && strcmp(name, CLASSIC_PROFILE) == 0) {
        *profile_return = XmAPPEARANCE_CLASSIC;
        return True;
    }
    if (name != NULL && strcmp(name, MODERN_LIGHT_PROFILE) == 0) {
        *profile_return = XmAPPEARANCE_MODERN_LIGHT;
        return True;
    }
    if (name != NULL && strcmp(name, HIGH_CONTRAST_PROFILE) == 0) {
        *profile_return = XmAPPEARANCE_HIGH_CONTRAST;
        return True;
    }
    if (name != NULL && strcmp(name, MODERN_DARK_PROFILE) == 0) {
        *profile_return = XmAPPEARANCE_MODERN_DARK;
        return True;
    }
    return False;
}

Boolean
XmSetAppearanceProfile(Display *display, XmAppearanceProfile profile)
{
    XrmDatabase database;
    XrmValue value;
    char *type = NULL;
    XmAppearanceProfile current;
    XmAppearanceDensity density;
    XmAppearanceDisplayState *state;
    Boolean new_database = False;
    const char *name;

    if (display == NULL)
        return False;

    if (appearance_transaction_is_active(display))
        return False;

    name = profile_name(profile);
    if (name == NULL || !XmGetAppearanceDensity(display, &density))
        return False;

    database = XrmGetDatabase(display);
    if (database != NULL &&
        XrmGetResource(database, APPEARANCE_RESOURCE, APPEARANCE_CLASS,
                       &type, &value)) {
        if (!XmGetAppearanceProfile(display, &current) || current != profile)
            return False;
        return True;
    }
    if (database == NULL) {
        XrmInitialize();
        database = XrmGetStringDatabase("");
        if (database == NULL)
            return False;
        new_database = True;
    }

    state = ensure_display_state(display);
    if (state == NULL ||
        (profile != XmAPPEARANCE_CLASSIC && profile_defaults(profile, density) == NULL)) {
        if (new_database) XrmDestroyDatabase(database);
        return False;
    }
    if (new_database) {
        state->active_database = database;
        state->owns_active_database = True;
    }

    XrmPutStringResource(&database, APPEARANCE_RESOURCE, name);
    XrmSetDatabase(display, database);
    return True;
}

Boolean
XmGetAppearanceProfile(Display *display, XmAppearanceProfile *profile_return)
{
    XrmDatabase database;
    XrmValue value;
    char *type = NULL;

    if (display == NULL || profile_return == NULL)
        return False;

    *profile_return = XmAPPEARANCE_CLASSIC;
    database = XrmGetDatabase(display);
    if (database == NULL ||
        !XrmGetResource(database, APPEARANCE_RESOURCE, APPEARANCE_CLASS,
                        &type, &value))
        return True;

    return profile_from_name(value.addr, profile_return);
}

static Boolean
apply_appearance_settings(Display *display, XmAppearanceProfile profile,
                          XmAppearanceDensity density, Boolean select_profile)
{
    XmAppearanceDisplayState *state;
    XmAppearanceProfile current;
    XmAppearanceDensity current_density;
    XrmDatabase database;
    XrmDatabase old_database;
    const char *name;

    if (display == NULL || profile_name(profile) == NULL ||
        (density != XmAPPEARANCE_DENSITY_COMFORTABLE &&
         density != XmAPPEARANCE_DENSITY_COMPACT) ||
        appearance_transaction_is_active(display))
        return False;
    if (!XmGetAppearanceProfile(display, &current) ||
        !XmGetAppearanceDensity(display, &current_density))
        return False;
    if (current == profile && current_density == density)
        return True;
    state = ensure_display_state(display);
    if (state == NULL)
        return False;
    database = clone_database(XrmGetDatabase(display));
    if (database == NULL ||
        (profile != XmAPPEARANCE_CLASSIC && profile_defaults(profile, density) == NULL)) {
        if (database != NULL)
            XrmDestroyDatabase(database);
        return False;
    }
    if (!appearance_transaction_begin(state)) {
        XrmDestroyDatabase(database);
        return False;
    }
    name = profile_name(profile);
    /* Choosing density alone must not freeze the startup profile at classic. */
    if (select_profile || current != XmAPPEARANCE_CLASSIC) {
        XrmPutStringResource(&database, APPEARANCE_RESOURCE, name);
    } else {
        XrmValue value;
        char *type = NULL;
        XrmDatabase previous = XrmGetDatabase(display);
        if (previous != NULL && XrmGetResource(previous, APPEARANCE_RESOURCE,
                                              APPEARANCE_CLASS, &type, &value))
            XrmPutStringResource(&database, APPEARANCE_RESOURCE, name);
    }
    XrmPutStringResource(&database, DENSITY_RESOURCE,
        density == XmAPPEARANCE_DENSITY_COMPACT ? "compact" : "comfortable");
    old_database = XrmGetDatabase(display);
    if ((current != XmAPPEARANCE_CLASSIC || profile != XmAPPEARANCE_CLASSIC) &&
        !_XmApplyAppearanceChange(display, current, profile, density)) {
        appearance_transaction_end(state);
        XrmDestroyDatabase(database);
        return False;
    }
    /* Xt caches its per-screen resource database. Replacing/freeing that
     * database leaves later widget construction reading stale resources.
     * Merge only after preparation and widget commits have succeeded, keeping
     * the existing database identity and its original ownership. */
    if (old_database == NULL) state->owns_active_database = True;
    XrmCombineDatabase(database, &old_database, True);
    XrmSetDatabase(display, old_database);
    state->active_database = state->owns_active_database ? old_database : NULL;
    appearance_transaction_end(state);
    return True;
}

Boolean
XmApplyAppearanceProfile(Display *display, XmAppearanceProfile profile)
{
    XmAppearanceDensity density;
    return XmGetAppearanceDensity(display, &density) &&
           apply_appearance_settings(display, profile, density, True);
}

Boolean
XmGetAppearanceDensity(Display *display, XmAppearanceDensity *density_return)
{
    XrmValue value;
    char *type = NULL;
    XrmDatabase database;
    XmAppearanceDensity density = XmAPPEARANCE_DENSITY_COMFORTABLE;
    if (display == NULL || density_return == NULL) return False;
    database = XrmGetDatabase(display);
    if (database != NULL && XrmGetResource(database, DENSITY_RESOURCE,
                                          DENSITY_CLASS, &type, &value)) {
        if (value.addr == NULL) return False;
        if (strcmp(value.addr, "compact") == 0)
            density = XmAPPEARANCE_DENSITY_COMPACT;
        else if (strcmp(value.addr, "comfortable") != 0)
            return False;
    }
    *density_return = density;
    return True;
}

Boolean
XmApplyAppearanceDensity(Display *display, XmAppearanceDensity density)
{
    XmAppearanceProfile profile;
    return XmGetAppearanceProfile(display, &profile) &&
           apply_appearance_settings(display, profile, density, False);
}

Boolean
XmSetAppearanceDensity(Display *display, XmAppearanceDensity density)
{
    XmAppearanceDensity current;
    XmAppearanceWidgetState *state;
    Boolean controls_exist = False;
    if (display == NULL ||
        (density != XmAPPEARANCE_DENSITY_COMFORTABLE &&
         density != XmAPPEARANCE_DENSITY_COMPACT) ||
        appearance_transaction_is_active(display) ||
        !XmGetAppearanceDensity(display, &current)) return False;
    if (current == density) return True;
    _XmProcessLock();
    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->retired && state->display == display &&
            state->widget != NULL && !XtIsShell(state->widget)) {
            controls_exist = True;
            break;
        }
    _XmProcessUnlock();
    if (controls_exist) return False;
    return XmApplyAppearanceDensity(display, density);
}

Boolean
XmGetAppearanceSpacing(Display *display, XmAppearanceSpacing spacing,
                        Dimension *pixels_return)
{
    static const Dimension logical[] = {4, 8, 12, 16, 24};
    double scale = 1.0;
    if (display == NULL || pixels_return == NULL ||
        (unsigned int)spacing >= XtNumber(logical)) return False;
    (void)_XmGetDpiScale(display, &scale);
    *pixels_return = _XmDpiScaleDimension(scale, logical[spacing]);
    return True;
}

/* Cascades have no armColor resource. Resolve the shared selection palette
 * only for theme-owned backgrounds; custom surfaces retain their derived GC. */
Pixel
_XmAppearanceCascadeSelection(Widget widget, Pixel fallback)
{
    XmAppearanceProfile profile;
    XmAppearanceDensity density;
    Pixel pixel;
    XrmValue from, to = {sizeof(pixel), (XPointer)&pixel};
    Widget context = XtIsWidget(widget) ? widget : XtParent(widget);
    if (_XmAppearanceOwnsResource(widget, XmNbackground) &&
        XmGetAppearanceProfile(XtDisplayOfObject(widget), &profile) &&
        (profile == XmAPPEARANCE_MODERN_LIGHT || profile == XmAPPEARANCE_MODERN_DARK) &&
        XmGetAppearanceDensity(XtDisplayOfObject(widget), &density) &&
        appearance_database_named_value(widget, profile_defaults(profile,density),
            XmNarmColor, XmCArmColor, &from) &&
        XtConvertAndStore(context, XmRString, &from, XmRPixel, &to))
        return pixel;
    return fallback;
}
