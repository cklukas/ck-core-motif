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
#include <ctype.h>
#include <math.h>
#include <limits.h>
#include <string.h>

#include <X11/IntrinsicP.h>
#include <X11/CompositeP.h>
#include <X11/ShellP.h>
#include <X11/StringDefs.h>
#include <X11/Xatom.h>
#include <X11/Xresource.h>
#include <X11/Xutil.h>
#include <X11/Xlibint.h>

#include <Xm/Appearance.h>
#include <Xm/Screen.h>
#include <Xm/Xm.h>
#include "XmI.h"
#include "ColorObjI.h"
#include "RowColumnP.h"
#include "RCLayoutI.h"
#include <Xm/RepType.h>
#include <Xm/PrimitiveP.h>
#include <Xm/GadgetP.h>
#include <Xm/NotebookP.h>
#include <Xm/SegmentedControl.h>
#include <Xm/FontS.h>
#include <Xm/InfoBar.h>
#include <Xm/DropDown.h>
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
#include <Xm/PanedW.h>
#include <Xm/Paned.h>
#include <Xm/SashP.h>
#include <Xm/Container.h>
#include <Xm/SeparatoG.h>
#include <Xm/Separator.h>
#include <Xm/Link.h>
#include <Xm/PushB.h>
#include <Xm/PushBG.h>
#include <Xm/DrawnB.h>
#include <Xm/Text.h>
#include <Xm/List.h>
#include <Xm/SearchBox.h>
#include <Xm/CascadeB.h>
#include <Xm/CascadeBG.h>
#include <Xm/DialogS.h>
#include <Xm/MenuShell.h>
#ifdef USE_XFT
#include <X11/Xft/Xft.h>
#endif

#define APPEARANCE_RESOURCE "ckCoreMotif." XmNappearanceProfile
#define APPEARANCE_CLASS "CkCoreMotif." XmCAppearanceProfile
#define DENSITY_RESOURCE "ckCoreMotif." XmNappearanceDensity
#define DENSITY_CLASS "CkCoreMotif." XmCAppearanceDensity
#define ACCENT_COLOR_RESOURCE "ckCoreMotif." XmNaccentColor
#define ACCENT_COLOR_CLASS "CkCoreMotif." XmCAccentColor
#define ACCENT_FOREGROUND_RESOURCE "ckCoreMotif." XmNaccentForeground
#define ACCENT_FOREGROUND_CLASS "CkCoreMotif." XmCAccentForeground
#define DANGER_COLOR_RESOURCE "ckCoreMotif." XmNdangerColor
#define DANGER_COLOR_CLASS "CkCoreMotif." XmCDangerColor
#define SURFACE_BACKGROUND_RESOURCE "ckCoreMotif." XmNsurfaceBackground
#define SURFACE_BACKGROUND_CLASS "CkCoreMotif." XmCSurfaceBackground
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
    "*XmLink.foreground: #1967d2\n"
    "*XmDataField.background: #ffffff\n"
    "*XmDatePicker.background: #ffffff\n"
    "*XmCalendar.background: #ffffff\n"
    "*XmTimePicker.background: #ffffff\n"
    "*XmProgressBar.bottomShadowColor: #dfe3e7\n"
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
    "*XmMainWindow.shadowThickness: 0\n"
    "*XmForm.shadowThickness: 0\n"
    "*XmBulletinBoard.shadowThickness: 0\n"
    "*XmMessageBox.shadowThickness: 0\n"
    "*XmSelectionBox.shadowThickness: 0\n"
    "*XmCommand.shadowThickness: 0\n"
    "*XmFileSelectionBox.shadowThickness: 0\n"
    "*XmPanedWindow.shadowThickness: 0\n"
    "*XmPaned.shadowThickness: 0\n"
    "*XmPanedWindow.sashShadowThickness: 1\n"
    "*XmPaned.sashShadowThickness: 1\n"
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
    "*XmScrollBar.highlightThickness: 3\n"
    "*XmPanedWindow.sashShadowThickness: 2\n"
    "*XmPaned.sashShadowThickness: 2\n"
    "*XmMainWindow.shadowThickness: 0\n"
    "*XmForm.shadowThickness: 0\n"
    "*XmBulletinBoard.shadowThickness: 0\n"
    "*XmMessageBox.shadowThickness: 0\n"
    "*XmSelectionBox.shadowThickness: 0\n"
    "*XmCommand.shadowThickness: 0\n"
    "*XmFileSelectionBox.shadowThickness: 0\n"
    "*XmPanedWindow.shadowThickness: 0\n"
    "*XmPaned.shadowThickness: 0\n";

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
    "*XmLink.foreground: #8ab4f8\n"
    "*XmDataField.background: #202124\n"
    "*XmDatePicker.background: #303134\n"
    "*XmCalendar.background: #303134\n"
    "*XmTimePicker.background: #303134\n"
    "*XmProgressBar.bottomShadowColor: #3c4043\n"
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
    "*XmMainWindow.shadowThickness: 0\n"
    "*XmForm.shadowThickness: 0\n"
    "*XmBulletinBoard.shadowThickness: 0\n"
    "*XmMessageBox.shadowThickness: 0\n"
    "*XmSelectionBox.shadowThickness: 0\n"
    "*XmCommand.shadowThickness: 0\n"
    "*XmFileSelectionBox.shadowThickness: 0\n"
    "*XmPanedWindow.shadowThickness: 0\n"
    "*XmPaned.shadowThickness: 0\n"
    "*XmPanedWindow.sashShadowThickness: 1\n"
    "*XmPaned.sashShadowThickness: 1\n"
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
    {"*flyoutLabel.marginWidth", 12, 8},
    {"*flyoutLabel.marginHeight", 8, 6},
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
    {"*XmProgressBar.spacing", 8, 6},
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
    {"*XmFileSelectionBox.marginHeight", 16, 8},
    {"*XmPanedWindow.marginWidth", 8, 4},
    {"*XmPanedWindow.marginHeight", 8, 4},
    {"*XmPanedWindow.spacing", 12, 8},
    {"*XmPanedWindow.sashWidth", 12, 10},
    {"*XmPanedWindow.sashHeight", 12, 10},
    {"*XmPaned.marginWidth", 8, 4},
    {"*XmPaned.marginHeight", 8, 4},
    {"*XmPaned.sashWidth", 12, 10},
    {"*XmPaned.sashHeight", 12, 10},
    {"*XmSpinBox.marginWidth", 4, 2},
    {"*XmSpinBox.marginHeight", 4, 2},
    {"*XmSimpleSpinBox.marginWidth", 4, 2},
    {"*XmSimpleSpinBox.marginHeight", 4, 2},
    {"*XmComboBox.marginWidth", 4, 2},
    {"*XmComboBox.marginHeight", 4, 2}
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
    AP_SASH_WIDTH,
    AP_SASH_HEIGHT,
    AP_SASH_SHADOW_THICKNESS,
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
    Boolean hovered;
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

typedef struct _XmSemanticPalette {
    Boolean valid;
    XmAppearanceProfile profile;
    Boolean has_surface;
    Boolean has_accent;
    Boolean has_accent_fg;
    Boolean has_danger;
    char raw_surface[64];
    char raw_accent[64];
    char raw_accent_fg[64];
    char raw_danger[64];
    char surface_bg[8];
    char field_bg[8];
    char button_bg[8];
    char text_fg[8];
    char accent_color[8];
    char accent_fg[8];
    char accent_arm[8];
    char focus_ring[8];
    char link_fg[8];
    char selection_fill[8];
    char danger_color[8];
    char danger_fg[8];
    char danger_arm[8];
} XmSemanticPalette;

typedef struct _XmAppearanceDisplayState {
    Display *display;
    Widget display_widget;
    XrmName app_name_quark;
    XrmClass app_class_quark;
    Boolean profile_api_selected;
    XmAppearanceProfile api_profile;
    Boolean density_api_selected;
    XmAppearanceDensity api_density;
    Boolean profile_invalid_warned;
    Boolean density_invalid_warned;
    XmSemanticPalette palette;
    XrmDatabase active_database;
    Boolean owns_active_database;
    Boolean transaction_active;
    Boolean defer_layout;
    unsigned int resource_commit_depth;
    XmAppearanceProfile committing_profile;
    XmAppearanceDensity committing_density;
    Boolean committing_geometry;
    Boolean live_watch_installed;
    Boolean live_watch_disabled;
    XtAppContext live_app_context;
    XtEventDispatchProc prev_prop_dispatcher;
    XtIntervalId live_sync_timer_id;
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

static void appearance_release_widget_states(Display *display);
static void appearance_created(Widget, XtPointer, XtPointer);
static void appearance_changed(Widget, XtPointer, XtPointer);
static void appearance_hooks_destroyed(Widget, XtPointer, XtPointer);
static void appearance_destroying(Widget, XtPointer, XtPointer);

void
_XmDestroyAppearanceDisplay(Display *display)
{
    XmAppearanceDisplayState **link;
    XmAppearanceDisplayState *state;
    XrmDatabase active_database;

    if (display == NULL)
        return;

    appearance_release_widget_states(display);

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
    if (state->live_sync_timer_id != 0) {
        XtRemoveTimeOut(state->live_sync_timer_id);
        state->live_sync_timer_id = 0;
    }
    if (state->live_watch_installed) {
        (void)XtSetEventDispatcher(display, PropertyNotify,
                                   state->prev_prop_dispatcher);
        state->live_watch_installed = False;
    }
    if (state->hooks != NULL) {
        XtRemoveCallback(state->hooks, XtNcreateHook, appearance_created, state);
        XtRemoveCallback(state->hooks, XtNchangeHook, appearance_changed, state);
        XtRemoveCallback(state->hooks, XtNdestroyHook, appearance_destroying, state);
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
        XmNavatarSize, XmNdividerSpacing, XmNoutlineLineStyle, XmNindentSpace,
        XmNsashWidth, XmNsashHeight, XmNsashShadowThickness
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
           property == AP_DIVIDER_SPACING || property == AP_OUTLINE_INDENT_SPACE ||
           property == AP_SASH_WIDTH || property == AP_SASH_HEIGHT ||
           property == AP_SASH_SHADOW_THICKNESS;
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
static Boolean lookup_appearance_resource(Display *display,
    XrmDatabase database, const char *qualified_name,
    const char *qualified_class, const char *leaf_name,
    const char *leaf_class, XrmValue *value_return);
static XmAppearanceDisplayState *ensure_display_state(Display *display);
static Boolean appearance_is_dtterm_widget(Widget w);
static Boolean appearance_is_dthelp_display_area(Widget w);
static Boolean appearance_widget_in_menu_tree(Widget w);
static Boolean appearance_widget_in_secondary_tree(Widget w);
static Boolean appearance_colorobj_modern_color(
    XmAppearanceWidgetState *state, XmAppearanceProfile new_profile,
    AppearanceProperty property, XtArgVal *value_return);

static double
srgb_channel_to_linear_u8(unsigned int v)
{
    double c = (double)v / 255.0;
    return (c <= 0.04045) ? (c / 12.92) : pow((c + 0.055) / 1.055, 2.4);
}

static double
rgb_relative_luminance(const unsigned int rgb[3])
{
    return 0.2126 * srgb_channel_to_linear_u8(rgb[0]) +
           0.7152 * srgb_channel_to_linear_u8(rgb[1]) +
           0.0722 * srgb_channel_to_linear_u8(rgb[2]);
}

static double
rgb_contrast_ratio(const unsigned int a[3], const unsigned int b[3])
{
    double la = rgb_relative_luminance(a);
    double lb = rgb_relative_luminance(b);
    double lighter = (la > lb) ? la : lb;
    double darker  = (la > lb) ? lb : la;
    return (lighter + 0.05) / (darker + 0.05);
}

static void
format_rgb_hex(const unsigned int rgb[3], char out[8])
{
    snprintf(out, 8, "#%02x%02x%02x",
             rgb[0] > 255u ? 255u : rgb[0],
             rgb[1] > 255u ? 255u : rgb[1],
             rgb[2] > 255u ? 255u : rgb[2]);
}

static void
blend_rgb(const unsigned int a[3], const unsigned int b[3], double t,
          unsigned int out[3])
{
    for (unsigned int i = 0; i < 3; ++i) {
        double v = (1.0 - t) * (double)a[i] + t * (double)b[i];
        if (v < 0.0) v = 0.0;
        if (v > 255.0) v = 255.0;
        out[i] = (unsigned int)(v + 0.5);
    }
}

static Boolean
parse_rgb_color(Display *display, const char *spec, unsigned int out[3])
{
    XColor xc;
    Colormap cmap;
    if (display == NULL || spec == NULL || *spec == '\0')
        return False;
    cmap = DefaultColormap(display, DefaultScreen(display));
    if (!XParseColor(display, cmap, spec, &xc))
        return False;
    out[0] = (unsigned int)(xc.red >> 8);
    out[1] = (unsigned int)(xc.green >> 8);
    out[2] = (unsigned int)(xc.blue >> 8);
    return True;
}

static Boolean
clamp_rgb_for_contrast(unsigned int fg[3], const unsigned int bg[3],
                       double min_ratio)
{
    static const unsigned int black[3] = {0, 0, 0};
    static const unsigned int white[3] = {255, 255, 255};
    const unsigned int *target;
    double lo = 0.0, hi = 1.0;
    unsigned int best[3];

    if (rgb_contrast_ratio(fg, bg) >= min_ratio)
        return False;

    target = (rgb_contrast_ratio(white, bg) >= rgb_contrast_ratio(black, bg))
             ? white : black;
    if (rgb_contrast_ratio(target, bg) < min_ratio) {
        fg[0] = target[0];
        fg[1] = target[1];
        fg[2] = target[2];
        return True;
    }
    best[0] = target[0];
    best[1] = target[1];
    best[2] = target[2];
    for (int iter = 0; iter < 24; ++iter) {
        double mid = 0.5 * (lo + hi);
        unsigned int cand[3];
        blend_rgb(fg, target, mid, cand);
        if (rgb_contrast_ratio(cand, bg) >= min_ratio) {
            best[0] = cand[0];
            best[1] = cand[1];
            best[2] = cand[2];
            hi = mid;
        } else {
            lo = mid;
        }
    }
    fg[0] = best[0];
    fg[1] = best[1];
    fg[2] = best[2];
    return True;
}

static const XmSemanticPalette *
resolve_semantic_palette(Display *display, XmAppearanceProfile profile)
{
    XmAppearanceDisplayState *state;
    XrmDatabase db;
    XrmValue v_surf = {0, NULL}, v_acc = {0, NULL};
    XrmValue v_afg = {0, NULL}, v_dng = {0, NULL};
    const char *s_surf = "", *s_acc = "", *s_afg = "", *s_dng = "";
    Boolean dark;
    XmSemanticPalette *pal;
    unsigned int surf_rgb[3], text_rgb[3], acc_rgb[3], afg_rgb[3], dng_rgb[3], dfg_rgb[3];
    unsigned int tmp_rgb[3];
    static const unsigned int black[3] = {0, 0, 0};
    static const unsigned int white[3] = {255, 255, 255};

    if (display == NULL ||
        (profile != XmAPPEARANCE_MODERN_LIGHT &&
         profile != XmAPPEARANCE_MODERN_DARK))
        return NULL;
    state = ensure_display_state(display);
    if (state == NULL)
        return NULL;
    pal = &state->palette;
    db = XrmGetDatabase(display);
    if (db != NULL) {
        if (lookup_appearance_resource(display, db, SURFACE_BACKGROUND_RESOURCE,
                                       SURFACE_BACKGROUND_CLASS,
                                       XmNsurfaceBackground,
                                       XmCSurfaceBackground, &v_surf) &&
            v_surf.addr != NULL)
            s_surf = v_surf.addr;
        if (lookup_appearance_resource(display, db, ACCENT_COLOR_RESOURCE,
                                       ACCENT_COLOR_CLASS,
                                       XmNaccentColor, XmCAccentColor, &v_acc) &&
            v_acc.addr != NULL)
            s_acc = v_acc.addr;
        if (lookup_appearance_resource(display, db, ACCENT_FOREGROUND_RESOURCE,
                                       ACCENT_FOREGROUND_CLASS,
                                       XmNaccentForeground,
                                       XmCAccentForeground, &v_afg) &&
            v_afg.addr != NULL)
            s_afg = v_afg.addr;
        if (lookup_appearance_resource(display, db, DANGER_COLOR_RESOURCE,
                                       DANGER_COLOR_CLASS,
                                       XmNdangerColor, XmCDangerColor, &v_dng) &&
            v_dng.addr != NULL)
            s_dng = v_dng.addr;
    }

    if (pal->valid && pal->profile == profile &&
        strncmp(pal->raw_surface, s_surf, sizeof(pal->raw_surface) - 1) == 0 &&
        strncmp(pal->raw_accent, s_acc, sizeof(pal->raw_accent) - 1) == 0 &&
        strncmp(pal->raw_accent_fg, s_afg, sizeof(pal->raw_accent_fg) - 1) == 0 &&
        strncmp(pal->raw_danger, s_dng, sizeof(pal->raw_danger) - 1) == 0)
        return pal;

    memset(pal, 0, sizeof(*pal));
    pal->valid = True;
    pal->profile = profile;
    snprintf(pal->raw_surface, sizeof(pal->raw_surface), "%s", s_surf);
    snprintf(pal->raw_accent, sizeof(pal->raw_accent), "%s", s_acc);
    snprintf(pal->raw_accent_fg, sizeof(pal->raw_accent_fg), "%s", s_afg);
    snprintf(pal->raw_danger, sizeof(pal->raw_danger), "%s", s_dng);

    dark = (profile == XmAPPEARANCE_MODERN_DARK);
    if (dark) {
        surf_rgb[0] = 0x20; surf_rgb[1] = 0x21; surf_rgb[2] = 0x24;
        text_rgb[0] = 0xf1; text_rgb[1] = 0xf3; text_rgb[2] = 0xf4;
        acc_rgb[0]  = 0x8a; acc_rgb[1]  = 0xb4; acc_rgb[2]  = 0xf8;
        afg_rgb[0]  = 0x20; afg_rgb[1]  = 0x21; afg_rgb[2]  = 0x24;
        dng_rgb[0]  = 0xf2; dng_rgb[1]  = 0x8b; dng_rgb[2]  = 0x82;
        dfg_rgb[0]  = 0x20; dfg_rgb[1]  = 0x21; dfg_rgb[2]  = 0x24;
        snprintf(pal->surface_bg, sizeof(pal->surface_bg), "#202124");
        snprintf(pal->field_bg, sizeof(pal->field_bg), "#202124");
        snprintf(pal->button_bg, sizeof(pal->button_bg), "#202124");
        snprintf(pal->text_fg, sizeof(pal->text_fg), "#f1f3f4");
        snprintf(pal->accent_color, sizeof(pal->accent_color), "#8ab4f8");
        snprintf(pal->accent_fg, sizeof(pal->accent_fg), "#202124");
        snprintf(pal->accent_arm, sizeof(pal->accent_arm), "#aecbfa");
        snprintf(pal->focus_ring, sizeof(pal->focus_ring), "#8ab4f8");
        snprintf(pal->link_fg, sizeof(pal->link_fg), "#8ab4f8");
        snprintf(pal->selection_fill, sizeof(pal->selection_fill), "#3c4043");
        snprintf(pal->danger_color, sizeof(pal->danger_color), "#f28b82");
        snprintf(pal->danger_fg, sizeof(pal->danger_fg), "#202124");
        snprintf(pal->danger_arm, sizeof(pal->danger_arm), "#f6aea9");
    } else {
        surf_rgb[0] = 0xf1; surf_rgb[1] = 0xf2; surf_rgb[2] = 0xf4;
        text_rgb[0] = 0x20; text_rgb[1] = 0x21; text_rgb[2] = 0x24;
        acc_rgb[0]  = 0x19; acc_rgb[1]  = 0x67; acc_rgb[2]  = 0xd2;
        afg_rgb[0]  = 0xff; afg_rgb[1]  = 0xff; afg_rgb[2]  = 0xff;
        dng_rgb[0]  = 0xb3; dng_rgb[1]  = 0x26; dng_rgb[2]  = 0x1e;
        dfg_rgb[0]  = 0xff; dfg_rgb[1]  = 0xff; dfg_rgb[2]  = 0xff;
        snprintf(pal->surface_bg, sizeof(pal->surface_bg), "#f1f2f4");
        snprintf(pal->field_bg, sizeof(pal->field_bg), "#ffffff");
        snprintf(pal->button_bg, sizeof(pal->button_bg), "#e2e6eb");
        snprintf(pal->text_fg, sizeof(pal->text_fg), "#202124");
        snprintf(pal->accent_color, sizeof(pal->accent_color), "#1967d2");
        snprintf(pal->accent_fg, sizeof(pal->accent_fg), "#ffffff");
        snprintf(pal->accent_arm, sizeof(pal->accent_arm), "#174ea6");
        snprintf(pal->focus_ring, sizeof(pal->focus_ring), "#1a73e8");
        snprintf(pal->link_fg, sizeof(pal->link_fg), "#1967d2");
        snprintf(pal->selection_fill, sizeof(pal->selection_fill), "#dbeafe");
        snprintf(pal->danger_color, sizeof(pal->danger_color), "#b3261e");
        snprintf(pal->danger_fg, sizeof(pal->danger_fg), "#ffffff");
        snprintf(pal->danger_arm, sizeof(pal->danger_arm), "#8c1d18");
    }

    if (*s_surf != '\0') {
        if (!parse_rgb_color(display, s_surf, surf_rgb)) {
            XmeWarning(state->display_widget,
                       "Cannot parse surfaceBackground resource; falling back to profile default");
        } else {
            pal->has_surface = True;
            if (clamp_rgb_for_contrast(surf_rgb, text_rgb, 4.5)) {
                XmeWarning(state->display_widget,
                           "surfaceBackground resource violates 4.5:1 text contrast guardrail; clamping surface color");
            }
            format_rgb_hex(surf_rgb, pal->surface_bg);
            if (dark) {
                blend_rgb(surf_rgb, black, 0.30, tmp_rgb);
                format_rgb_hex(tmp_rgb, pal->field_bg);
                blend_rgb(surf_rgb, text_rgb, 0.10, tmp_rgb);
                format_rgb_hex(tmp_rgb, pal->button_bg);
            } else {
                blend_rgb(surf_rgb, white, 0.6, tmp_rgb);
                format_rgb_hex(tmp_rgb, pal->field_bg);
                blend_rgb(surf_rgb, text_rgb, 0.08, tmp_rgb);
                format_rgb_hex(tmp_rgb, pal->button_bg);
            }
        }
    }

    if (*s_acc != '\0') {
        if (!parse_rgb_color(display, s_acc, acc_rgb)) {
            XmeWarning(state->display_widget,
                       "Cannot parse accentColor resource; falling back to profile default");
        } else {
            pal->has_accent = True;
        }
    }
    if (pal->has_accent || pal->has_surface) {
        if (clamp_rgb_for_contrast(acc_rgb, surf_rgb, 3.0)) {
            XmeWarning(state->display_widget,
                       "accentColor resource violates 3.0:1 interactive contrast guardrail; clamping accent color");
        }
        format_rgb_hex(acc_rgb, pal->accent_color);
        format_rgb_hex(acc_rgb, pal->focus_ring);
        tmp_rgb[0] = acc_rgb[0];
        tmp_rgb[1] = acc_rgb[1];
        tmp_rgb[2] = acc_rgb[2];
        (void)clamp_rgb_for_contrast(tmp_rgb, surf_rgb, 4.5);
        format_rgb_hex(tmp_rgb, pal->link_fg);
        blend_rgb(acc_rgb, dark ? white : black, 0.18, tmp_rgb);
        format_rgb_hex(tmp_rgb, pal->accent_arm);
        blend_rgb(surf_rgb, acc_rgb, dark ? 0.28 : 0.18, tmp_rgb);
        format_rgb_hex(tmp_rgb, pal->selection_fill);
    }

    if (*s_afg != '\0') {
        if (!parse_rgb_color(display, s_afg, afg_rgb)) {
            XmeWarning(state->display_widget,
                       "Cannot parse accentForeground resource; falling back to profile default");
        } else {
            pal->has_accent_fg = True;
        }
    } else if (pal->has_accent) {
        const unsigned int *auto_fg =
            (rgb_contrast_ratio(white, acc_rgb) >= rgb_contrast_ratio(black, acc_rgb))
            ? white : text_rgb;
        afg_rgb[0] = auto_fg[0];
        afg_rgb[1] = auto_fg[1];
        afg_rgb[2] = auto_fg[2];
    }
    if (pal->has_accent_fg || pal->has_accent) {
        if (clamp_rgb_for_contrast(afg_rgb, acc_rgb, 4.5)) {
            if (pal->has_accent_fg) {
                XmeWarning(state->display_widget,
                           "accentForeground resource violates 4.5:1 text contrast guardrail; clamping accent foreground");
            }
        }
        if (rgb_contrast_ratio(afg_rgb, acc_rgb) < 4.5) {
            (void)clamp_rgb_for_contrast(acc_rgb, afg_rgb, 4.5);
            format_rgb_hex(acc_rgb, pal->accent_color);
        }
        format_rgb_hex(afg_rgb, pal->accent_fg);
    }

    if (*s_dng != '\0') {
        if (!parse_rgb_color(display, s_dng, dng_rgb)) {
            XmeWarning(state->display_widget,
                       "Cannot parse dangerColor resource; falling back to profile default");
        } else {
            pal->has_danger = True;
        }
    }
    if (pal->has_danger || pal->has_surface) {
        if (clamp_rgb_for_contrast(dng_rgb, surf_rgb, 3.0)) {
            XmeWarning(state->display_widget,
                       "dangerColor resource violates 3.0:1 interactive contrast guardrail; clamping danger color");
        }
        if (pal->has_danger) {
            const unsigned int *auto_dfg =
                (rgb_contrast_ratio(white, dng_rgb) >= rgb_contrast_ratio(black, dng_rgb))
                ? white : text_rgb;
            dfg_rgb[0] = auto_dfg[0];
            dfg_rgb[1] = auto_dfg[1];
            dfg_rgb[2] = auto_dfg[2];
            if (rgb_contrast_ratio(dfg_rgb, dng_rgb) < 4.5)
                (void)clamp_rgb_for_contrast(dng_rgb, dfg_rgb, 4.5);
        }
        format_rgb_hex(dng_rgb, pal->danger_color);
        format_rgb_hex(dfg_rgb, pal->danger_fg);
        blend_rgb(dng_rgb, dark ? white : black, 0.18, tmp_rgb);
        format_rgb_hex(tmp_rgb, pal->danger_arm);
    }

    return pal;
}

static const char *
profile_property_string(Widget widget, XmAppearanceProfile profile,
                        XmAppearanceDensity density,
                        AppearanceProperty property)
{
    XrmValue value;
    unsigned char role = appearance_role(widget);
    if (profile == XmAPPEARANCE_MODERN_LIGHT || profile == XmAPPEARANCE_MODERN_DARK) {
        Boolean dark = profile == XmAPPEARANCE_MODERN_DARK;
        const XmSemanticPalette *pal =
            resolve_semantic_palette(XtDisplayOfObject(widget), profile);
        Boolean has_tokens = pal != NULL &&
            (pal->has_surface || pal->has_accent ||
             pal->has_accent_fg || pal->has_danger);
        if (role == XmAPPEARANCE_ROLE_NORMAL && XmIsInfoBar(widget) && (property == AP_FOREGROUND || property == AP_BACKGROUND)) {
            unsigned char severity;XtVaGetValues(widget,XmNinfoSeverity,&severity,NULL);
            if(property == AP_BACKGROUND)
                return severity == XmINFO_SEVERITY_ERROR ? (dark?"#5c292d":"#fce8e6") :
                    severity == XmINFO_SEVERITY_WARNING ? (dark?"#4b3b17":"#fef7e0") : (dark?"#253b57":"#e8f0fe");
            return severity == XmINFO_SEVERITY_ERROR ? (dark?"#f6c7c3":"#a50e0e") :
                severity == XmINFO_SEVERITY_WARNING ? (dark?"#fdd663":"#754c00") : (dark?"#d3e3fd":"#174ea6");
        }
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
            if (property == AP_SELECT)
                return (has_tokens && (pal->has_accent || pal->has_surface))
                       ? pal->selection_fill : (dark ? "#334b6d" : "#d3e3fd");
            if (property == AP_UNSELECT_COLOR)
                return (has_tokens && pal->has_surface)
                       ? pal->field_bg : (dark ? "#202124" : "#ffffff");
            if (property == AP_SHADOW_THICKNESS) return "1";
        }
        if (XtParent(widget) && XmIsNotebook(XtParent(widget)) &&
            widget->core.constraints != NULL) {
            XmNotebookConstraintPtr constraint =
                (XmNotebookConstraintPtr)widget->core.constraints;
            if (constraint->notebook.child_type == XmMAJOR_TAB ||
                constraint->notebook.child_type == XmMINOR_TAB) {
                if (property == AP_BACKGROUND)
                    return (has_tokens && pal->has_surface)
                           ? pal->surface_bg : (dark ? "#202124" : "#f1f2f4");
                if (property == AP_SHADOW_THICKNESS) return "1";
                if (property == AP_TOP_SHADOW || property == AP_BOTTOM_SHADOW)
                    return dark ? "#5f6368" : "#9aa3ae";
            }
        }
        if (role == XmAPPEARANCE_ROLE_SECONDARY && property == AP_FOREGROUND)
            return dark ? "#bdc1c6" : "#5f6368";
        if (role == XmAPPEARANCE_ROLE_PRIMARY || role == XmAPPEARANCE_ROLE_DESTRUCTIVE) {
            if (has_tokens) {
                if (property == AP_FOREGROUND)
                    return role == XmAPPEARANCE_ROLE_PRIMARY ? pal->accent_fg : pal->danger_fg;
                if (property == AP_BACKGROUND)
                    return role == XmAPPEARANCE_ROLE_PRIMARY ? pal->accent_color : pal->danger_color;
                if (property == AP_ARM || property == AP_SELECT)
                    return role == XmAPPEARANCE_ROLE_PRIMARY ? pal->accent_arm : pal->danger_arm;
            }
            if (property == AP_FOREGROUND) return dark ? "#202124" : "#ffffff";
            if (property == AP_BACKGROUND)
                return role == XmAPPEARANCE_ROLE_PRIMARY ? (dark ? "#8ab4f8" : "#1967d2") :
                                                         (dark ? "#f28b82" : "#b3261e");
            if (property == AP_ARM || property == AP_SELECT)
                return role == XmAPPEARANCE_ROLE_PRIMARY ? (dark ? "#aecbfa" : "#174ea6") :
                                                         (dark ? "#f6aea9" : "#8c1d18");
        }
        if (has_tokens) {
            if (property == AP_HIGHLIGHT && (pal->has_accent || pal->has_surface))
                return pal->focus_ring;
            if (property == AP_FOREGROUND && XmIsLink(widget) &&
                (pal->has_accent || pal->has_surface))
                return pal->link_fg;
            if (property == AP_ARM && (pal->has_accent || pal->has_surface))
                return pal->selection_fill;
            if (property == AP_SELECT && (pal->has_accent || pal->has_surface)) {
                if (XmIsToggleButton(widget) || XmIsToggleButtonGadget(widget))
                    return pal->accent_color;
                return pal->selection_fill;
            }
            if (pal->has_surface) {
                if (property == AP_BACKGROUND) {
                    if (XmIsTextField(widget) || XmIsText(widget) ||
                        XmIsList(widget) || XmIsSearchBox(widget))
                        return pal->field_bg;
                    if (XmIsPushButton(widget) || XmIsPushButtonGadget(widget) ||
                        XmIsDrawnButton(widget) || XmIsArrowButton(widget) ||
                        XmIsArrowButtonGadget(widget))
                        return pal->button_bg;
                    return pal->surface_bg;
                }
                if (property == AP_FRAME_BACKGROUND ||
                    property == AP_BACK_PAGE_BACKGROUND ||
                    property == AP_TROUGH_COLOR)
                    return pal->surface_bg;
                if (property == AP_UNSELECT_COLOR)
                    return pal->field_bg;
            }
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
    /* MainWindow is a primary window layout manager rather than a scrolled
     * viewport frame; preserve its zero-thickness baseline across profiles. */
    if (property == AP_SHADOW_THICKNESS && XmIsMainWindow(widget))
        return "0";
    if (appearance_database_value(widget, profile_defaults(profile, density), property,
                                  &value))
        return value.addr;
    return NULL;
}

static Boolean
appearance_string_value(const char *text, XrmValue *value)
{
    size_t length;
    if (text == NULL || value == NULL)
        return False;
    length = strlen(text);
    if (length == SIZE_MAX || length + 1 > UINT_MAX)
        return False;
    value->size = (unsigned int)(length + 1);
    value->addr = (XPointer)text;
    return True;
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
        XmTabMode value = (XmTabMode)0;
        XrmValue from;
        XrmValue to = {sizeof(value), (XPointer)&value};
        if (!appearance_string_value(text, &from)) return False;
        if (!XtConvertAndStore(widget, XmRString, &from, XmRTabMode, &to)) return False;
        *value_return = value;
        return True;
    }
    if (property == AP_SHOW_ARROWS || property == AP_SLIDER_VISUAL ||
        property == AP_SLIDER_MARK || property == AP_ALIGNMENT ||
        property == AP_OUTLINE_LINE_STYLE) {
        unsigned char value = 0;
        XrmValue from;
        XrmValue to = {sizeof(value), (XPointer)&value};
        if (!appearance_string_value(text, &from)) return False;
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
        Pixel pixel = 0;
        XrmValue from, to;
        Widget context = XtIsWidget(widget) ? widget : XtParent(widget);
        if (!appearance_string_value(text, &from)) return False;
        to.addr = (XPointer)&pixel;
        to.size = sizeof(pixel);
        /* Use Xt's cached pixel converter and the actual widget colormap,
         * just as ordinary application resource conversion does. */
        if (!XtConvertAndStore(context, XmRString, &from, XmRPixel, &to)) {
            Display *dpy = XtDisplayOfObject(widget);
            Screen *scr = XtScreenOfObject(widget);
            Colormap cmap = context != NULL ? context->core.colormap :
                            DefaultColormapOfScreen(scr);
            XColor target_color;
            if (!XParseColor(dpy, cmap, text, &target_color))
                return False;
            if (XAllocColor(dpy, cmap, &target_color)) {
                pixel = target_color.pixel;
            } else {
                int ncells = CellsOfScreen(scr);
                if (ncells > 256) ncells = 256;
                if (ncells > 0) {
                    XColor cells[256];
                    for (int i = 0; i < ncells; ++i)
                        cells[i].pixel = (unsigned long)i;
                    XQueryColors(dpy, cmap, cells, ncells);
                    unsigned long best_dist = ULONG_MAX;
                    pixel = BlackPixelOfScreen(scr);
                    for (int i = 0; i < ncells; ++i) {
                        long dr = (long)(cells[i].red >> 8) - (long)(target_color.red >> 8);
                        long dg = (long)(cells[i].green >> 8) - (long)(target_color.green >> 8);
                        long db = (long)(cells[i].blue >> 8) - (long)(target_color.blue >> 8);
                        unsigned long dist = (unsigned long)(dr * dr + dg * dg + db * db);
                        if (dist < best_dist) {
                            best_dist = dist;
                            pixel = cells[i].pixel;
                        }
                    }
                } else {
                    return False;
                }
            }
        }
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
    return XrmQGetResource(database, names, classes, type, value_return)
        ? True : False;
}

static Boolean
appearance_database_named_value(Widget widget, XrmDatabase database,
                                const char *name, const char *resource_class,
                                XrmValue *value_return)
{
    XrmRepresentation type;
    if (!appearance_database_typed_value(widget, database, name,
                                         resource_class, &type, value_return))
        return False;
    if (widget != NULL) {
        Display *display = XtDisplayOfObject(widget);
        if (database == XrmGetDatabase(display)) {
            XmColorObj co;
            if (type == XrmStringToQuark(XmRString) &&
                value_return != NULL && value_return->addr != NULL &&
                strstr((const char *)value_return->addr, "-dt-interface ") != NULL)
                return False;
            co = _XmColorObjForDisplay(display);
            if (co != NULL && co->color_obj.colorIsRunning &&
                co->color_obj.useColorObj && co->color_obj.colors != NULL) {
                /* XmColorObj injects session palette pixels as XmRPixel entries;
                 * treat those as toolkit session defaults rather than explicit
                 * application overrides so modern profiles harmonize surfaces
                 * and elevate buttons while preserving raw pixels in original[]. */
                if (type == XrmStringToQuark(XmRPixel))
                    return False;
                if (type == XrmStringToQuark(XmRString) &&
                    value_return != NULL && value_return->addr != NULL &&
                    name != NULL && resource_class != NULL &&
                    (strcmp(name, XmNbackground) == 0 ||
                     strcmp(name, XmNforeground) == 0)) {
                    char wname[96], wclass[96];
                    char *wtype = NULL;
                    XrmValue wval = {0, NULL};
                    snprintf(wname, sizeof(wname), "__xm_wildcard__.%s", name);
                    snprintf(wclass, sizeof(wclass), "__XmWildcard__.%s",
                             resource_class);
                    if (XrmGetResource(database, wname, wclass, &wtype, &wval) &&
                        wval.addr == value_return->addr)
                        return False;
                    if (appearance_is_dthelp_display_area(widget)) {
                        const char *sv = (const char *)value_return->addr;
                        if (strcmp(sv, "White") == 0 ||
                            strcmp(sv, "white") == 0 ||
                            strcmp(sv, "Black") == 0 ||
                            strcmp(sv, "black") == 0)
                            return False;
                    }
                }
            }
        }
    }
    return True;
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

Boolean
_XmAppearanceInGeometryTransaction(Display *display)
{
    XmAppearanceDisplayState *state = find_display_state(display);
    return state != NULL && state->committing_geometry;
}

Boolean
_XmAppearanceDimensionConstrained(Widget widget, unsigned int axis)
{
    XmAppearanceWidgetState *state;
    Boolean constrained = False;
    if (widget == NULL || axis >= 2)
        return False;
    _XmProcessLock();
    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->retired && state->widget == widget) {
            constrained = state->dimension_argument[axis] ||
                          state->constrained_dimension[axis];
            break;
        }
    _XmProcessUnlock();
    return constrained;
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
    /* The display destroy hook retires both gadgets and windowed controls
     * before an address can be reused. */
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

Boolean
_XmAppearanceSetHovered(Widget widget, Boolean hovered)
{
    XmAppearanceWidgetState *state;
    XmAppearanceProfile profile = XmAPPEARANCE_CLASSIC;
    Boolean changed = False;

    if (widget == NULL ||
        !(XmIsPushButton(widget) || XmIsPushButtonGadget(widget) ||
          XmIsToggleButton(widget) || XmIsToggleButtonGadget(widget) ||
          XmIsArrowButton(widget) || XmIsArrowButtonGadget(widget) ||
          XmIsComboBox(widget)))
        return False;
    if (!XtIsSensitive(widget) || !XtIsManaged(widget))
        hovered = False;
    _XmProcessLock();
    for (state = appearance_widgets; state != NULL; state = state->next) {
        if (!state->retired && state->widget == widget) {
            if (state->hovered != hovered) {
                state->hovered = hovered;
                changed = True;
            }
            break;
        }
    }
    _XmProcessUnlock();
    if (!changed)
        return False;
    (void)XmGetAppearanceProfile(XtDisplayOfObject(widget), &profile);
    return (profile == XmAPPEARANCE_MODERN_LIGHT ||
            profile == XmAPPEARANCE_MODERN_DARK);
}

Boolean
_XmAppearanceIsHovered(Widget widget)
{
    XmAppearanceWidgetState *state;
    XmAppearanceProfile profile = XmAPPEARANCE_CLASSIC;
    Boolean hovered = False;

    if (widget == NULL || !XtIsSensitive(widget))
        return False;
    if (!XmGetAppearanceProfile(XtDisplayOfObject(widget), &profile) ||
        (profile != XmAPPEARANCE_MODERN_LIGHT &&
         profile != XmAPPEARANCE_MODERN_DARK))
        return False;
    _XmProcessLock();
    for (state = appearance_widgets; state != NULL; state = state->next) {
        if (!state->retired && state->widget == widget) {
            hovered = state->hovered;
            break;
        }
    }
    _XmProcessUnlock();
    return hovered;
}

static void
appearance_release_widget_states(Display *display)
{
    XmAppearanceWidgetState **link;
    XmAppearanceWidgetState *state;

    _XmProcessLock();
    link = &appearance_widgets;
    while (*link != NULL) {
        state = *link;
        if (state->display == display) {
            /* No callback resource on a possibly reclaimed object is touched. */
            state->widget = NULL;
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

/* Xt's phase-one destroy hook is independent of the application's mutable
 * destroyCallback resource, and runs before any hierarchy storage is freed. */
static void
appearance_destroying(Widget hooks, XtPointer closure, XtPointer data)
{
    XtDestroyHookData destroyed = data;
    XmAppearanceWidgetState **link, *state;
    (void)hooks;(void)closure;
    if (!destroyed) return;
    _XmProcessLock();
    link = &appearance_widgets;
    while (*link && (*link)->widget != destroyed->widget) link = &(*link)->next;
    if ((state = *link) != NULL) {
        *link = state->next;
        state->retired = True;
        state->widget = NULL;
        if (state->references == 0) appearance_free_widget_state(state);
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
    if (role == XmAPPEARANCE_ROLE_NORMAL && !state->explicit_role) {
        if (XtIsSubclass(provider, xmAvatarWidgetClass))
            return XmAPPEARANCE_ROLE_SECTION;
        if (XtParent(provider) != NULL &&
            strcmp(widget_class_name(XtParent(provider)), "DtTitleBox") == 0 &&
            XtName(provider) != NULL &&
            strcmp(XtName(provider), "title") == 0)
            return XmAPPEARANCE_ROLE_SECTION;
    }
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
            *value = (XtArgVal)pixel;
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
    Arg args[7];
    Cardinal count = 0;
    int height = 0;
    if (!isfinite(pixels) || pixels < 1.0 || pixels > 65535.0) return NULL;
#ifdef USE_XFT
    {
        Display *display = XtDisplayOfObject(widget);
        XftFont *font = XftFontOpen(display, XScreenNumberOfScreen(XtScreenOfObject(widget)),
            XFT_FAMILY, XftTypeString, mono ? "monospace" : "sans-serif",
            XFT_PIXEL_SIZE, XftTypeDouble, pixels,
            XFT_WEIGHT, XftTypeInteger, bold ? XFT_WEIGHT_BOLD : XFT_WEIGHT_MEDIUM,
            XFT_RGBA, XftTypeInteger, XFT_RGBA_NONE, NULL);
        if (font == NULL) return NULL;
        XtSetArg(args[count], XmNfontType, XmFONT_IS_XFT); ++count;
        XtSetArg(args[count], XmNfont, (XtArgVal)NULL); ++count;
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

XmRenderTable
_XmAppearanceCreateRenderTable(Widget widget, unsigned char role, double scale)
{
    if (widget == NULL)
        return NULL;
    if (!(scale > 0.0) || !isfinite(scale))
        scale = 1.0;
    return appearance_create_font(widget, role, scale);
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
        /* PanedWindow and Paned manage child sash shadowThickness through
         * XmNsashShadowThickness on the parent composite. */
        if (property == AP_SHADOW_THICKNESS && XmIsSash(state->widget) &&
            XtParent(state->widget) &&
            (XmIsPanedWindow(XtParent(state->widget)) ||
             XtIsSubclass(XtParent(state->widget), xmPanedWidgetClass)))
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
        {
            Boolean bg_owned = state->supported[AP_BACKGROUND]
                ? plan->owned[AP_BACKGROUND]
                : (state->parent == NULL ||
                   _XmAppearanceOwnsResource(state->parent, XmNbackground));
            if (!bg_owned &&
                (property == AP_TOP_SHADOW || property == AP_BOTTOM_SHADOW ||
                 property == AP_SELECT || property == AP_ARM ||
                 property == AP_TROUGH_COLOR || property == AP_UNSELECT_COLOR ||
                 property == AP_FRAME_BACKGROUND ||
                 property == AP_BACK_PAGE_BACKGROUND)) {
                plan->values[property] = state->original[property];
                plan->apply[property] = True;
                continue;
            }
        }
        if (property == AP_FOREGROUND &&
            appearance_inherited_foreground(state, &plan->values[property])) {
            plan->apply[property] = True;
            continue;
        }
        if (appearance_colorobj_modern_color(state, new_profile, property,
                                             &plan->values[property])) {
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
        if (display_state)
            ++display_state->resource_commit_depth;
        XtSetValues(state->widget, set_args, count);
        if (display_state)
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

static Boolean appearance_is_color_swatch_widget(Widget widget);
static Boolean appearance_pixel_in_colorobj(Widget widget, Pixel px,
                                            Boolean include_fg);

static Boolean
initialize_appearance_state(XmAppearanceWidgetState *state,
                            XmAppearanceProfile current_profile)
{
    state->applied_role = appearance_role(state->widget);
    read_appearance_values(state, state->original);
    if (!appearance_is_color_swatch_widget(state->widget)) {
        for (AppearanceProperty property = 0; property < AP_COUNT; ++property) {
            if (!state->supported[property] || !state->explicit_arg[property] ||
                !appearance_property_is_color(property))
                continue;
            if (appearance_pixel_in_colorobj(
                    state->widget, (Pixel)state->original[property], True) ||
                (property == AP_BACKGROUND && XtParent(state->widget) != NULL &&
                 XtIsWidget(XtParent(state->widget)) &&
                 (Pixel)state->original[AP_BACKGROUND] ==
                     XtParent(state->widget)->core.background_pixel)) {
                state->explicit_arg[property] = False;
            }
        }
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
    if (current_profile != XmAPPEARANCE_CLASSIC &&
        state->supported[AP_BACKGROUND] && state->owned[AP_BACKGROUND]) {
        XmColorObj co = _XmColorObjForDisplay(state->display);
        if (co != NULL && co->color_obj.colorIsRunning &&
            co->color_obj.useColorObj && co->color_obj.colors != NULL) {
            int scr_num = XScreenNumberOfScreen(XtScreenOfObject(state->widget));
            if (scr_num >= 0 && scr_num < co->color_obj.numScreens &&
                co->color_obj.colors[scr_num] != NULL) {
                const XmPixelSet *pset = co->color_obj.colors[scr_num];
                Pixel cur_bg = (Pixel)state->original[AP_BACKGROUND];
                Boolean matches_raw = False;
                for (int k = 0; k < XmCO_NUM_COLORS; ++k) {
                    if (pset[k].bg == cur_bg || pset[k].sc == cur_bg) {
                        matches_raw = True;
                        break;
                    }
                }
                if (!matches_raw) {
                    int fallback_slot = -1;
                    for (Widget p = XtParent(state->widget); p != NULL;
                         p = XtParent(p)) {
                        XmAppearanceWidgetState *ps = find_live_widget_state(p);
                        if (ps != NULL && ps->initialized &&
                            ps->supported[AP_BACKGROUND]) {
                            Pixel pbg = (Pixel)ps->original[AP_BACKGROUND];
                            for (int k = 0; k < XmCO_NUM_COLORS; ++k) {
                                if (pset[k].bg == pbg) {
                                    fallback_slot = k;
                                    break;
                                }
                            }
                            if (fallback_slot >= 0)
                                break;
                        }
                    }
                    if (fallback_slot >= 0) {
                        state->original[AP_BACKGROUND] =
                            (XtArgVal)pset[fallback_slot].bg;
                        if (state->supported[AP_FOREGROUND] &&
                            state->owned[AP_FOREGROUND])
                            state->original[AP_FOREGROUND] =
                                (XtArgVal)pset[fallback_slot].fg;
                        if (state->supported[AP_TOP_SHADOW] &&
                            state->owned[AP_TOP_SHADOW])
                            state->original[AP_TOP_SHADOW] =
                                (XtArgVal)pset[fallback_slot].ts;
                        if (state->supported[AP_BOTTOM_SHADOW] &&
                            state->owned[AP_BOTTOM_SHADOW])
                            state->original[AP_BOTTOM_SHADOW] =
                                (XtArgVal)pset[fallback_slot].bs;
                        if (state->supported[AP_SELECT] &&
                            state->owned[AP_SELECT])
                            state->original[AP_SELECT] =
                                (XtArgVal)pset[fallback_slot].sc;
                        if (state->supported[AP_ARM] &&
                            state->owned[AP_ARM])
                            state->original[AP_ARM] =
                                (XtArgVal)pset[fallback_slot].sc;
                        if (state->supported[AP_TROUGH_COLOR] &&
                            state->owned[AP_TROUGH_COLOR])
                            state->original[AP_TROUGH_COLOR] =
                                (XtArgVal)pset[fallback_slot].sc;
                    }
                }
            }
        }
    }
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
    if (state->supported[AP_RENDER_TABLE] &&
        (appearance_provider_font_explicit(state) ||
         appearance_is_dtterm_widget(state->widget)))
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

static Boolean
appearance_is_color_swatch_widget(Widget widget)
{
    const char *name;
    if (widget == NULL)
        return False;
    name = XtName(widget);
    if (name == NULL)
        return False;
    return strncmp(name, "colorButton", 11) == 0 ||
           strcmp(name, "oldButton") == 0 ||
           strcmp(name, "newButton") == 0;
}

static Boolean
appearance_arg_value(ArgList args, Cardinal count, AppearanceProperty property,
                     XtArgVal *val_out)
{
    const char *name = appearance_property_name(property);
    Boolean found = False;
    for (Cardinal i = 0; i < count; ++i) {
        if (strcmp(args[i].name, name) == 0 ||
            (property == AP_RENDER_TABLE &&
             strcmp(args[i].name, XmNfontList) == 0)) {
            if (val_out != NULL)
                *val_out = args[i].value;
            found = True;
        }
    }
    return found;
}

static Boolean
appearance_pixel_in_colorobj(Widget widget, Pixel px, Boolean include_fg)
{
    Display *dpy;
    XmColorObj co;
    int scr_num;
    const XmPixelSet *pset;

    if (widget == NULL)
        return False;
    dpy = XtDisplayOfObject(widget);
    co = _XmColorObjForDisplay(dpy);
    if (co == NULL || !co->color_obj.colorIsRunning ||
        !co->color_obj.useColorObj || co->color_obj.colors == NULL)
        return False;
    scr_num = XScreenNumberOfScreen(XtScreenOfObject(widget));
    if (scr_num < 0 || scr_num >= co->color_obj.numScreens ||
        co->color_obj.colors[scr_num] == NULL)
        return False;
    pset = co->color_obj.colors[scr_num];
    for (int k = 0; k < XmCO_NUM_COLORS; ++k) {
        if (pset[k].bg == px || pset[k].ts == px ||
            pset[k].bs == px || pset[k].sc == px ||
            (include_fg && pset[k].fg == px))
            return True;
    }
    return False;
}

static Boolean
appearance_pixel_matches_parent_managed(Widget widget,
                                        AppearanceProperty property,
                                        Pixel px)
{
    Widget parent;
    XmAppearanceWidgetState *ps;

    if (widget == NULL || property != AP_BACKGROUND)
        return False;
    parent = XtParent(widget);
    if (parent == NULL || !XtIsWidget(parent))
        return False;
    _XmProcessLock();
    ps = find_live_widget_state(parent);
    _XmProcessUnlock();
    if (ps != NULL && ps->initialized && ps->supported[AP_BACKGROUND] &&
        !ps->owned[AP_BACKGROUND])
        return False;
    return parent->core.background_pixel == px;
}

static Boolean
appearance_arg_is_session_color(Widget widget, AppearanceProperty property,
                                ArgList args, Cardinal count,
                                XtArgVal *raw_out)
{
    XtArgVal val = 0;
    Pixel px;

    if (!appearance_property_is_color(property) ||
        appearance_is_color_swatch_widget(widget) ||
        !appearance_arg_value(args, count, property, &val))
        return False;
    px = (Pixel)val;
    if (raw_out != NULL)
        *raw_out = val;
    if (appearance_pixel_matches_parent_managed(widget, property, px))
        return True;
    if (XtIsWidget(widget))
        return appearance_pixel_in_colorobj(
            widget, px,
            property != AP_FOREGROUND || XmIsScrollBar(widget) ||
                appearance_has_argument(args, count, XmNbackground));
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
            XmAppearanceProfile cur_prof = XmAPPEARANCE_CLASSIC;
            (void)XmGetAppearanceProfile(state->display, &cur_prof);
            appearance_dimension_arguments(state, values->args, values->num_args);
            for (AppearanceProperty property = 0; property < AP_COUNT; ++property)
                if (appearance_explicit_arg(values->args, values->num_args, property)) {
                    XtArgVal session_val = 0;
                    if (appearance_arg_is_session_color(
                            state->widget, property, values->args,
                            values->num_args, &session_val)) {
                        if (appearance_pixel_in_colorobj(
                                state->widget, (Pixel)session_val, True)) {
                            state->original[property] = session_val;
                        }
                        continue;
                    }
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
            Boolean severity_changed = XmIsInfoBar(state->widget) && appearance_has_argument(values->args,values->num_args,XmNinfoSeverity);
            if (severity_changed || foreground_changed || appearance_has_argument(values->args, values->num_args, XmNappearanceRole) ||
                ((XmIsToggleButton(state->widget) || XmIsToggleButtonGadget(state->widget)) &&
                 appearance_has_argument(values->args, values->num_args, XmNindicatorOn))) {
                XmAppearanceProfile profile;
                XmAppearanceDensity density;
                Boolean role_argument = appearance_has_argument(values->args, values->num_args,
                                                                 XmNappearanceRole);
                if (role_argument) state->explicit_role = True;
                if ((severity_changed || foreground_changed || appearance_has_argument(values->args,values->num_args,XmNindicatorOn) ||
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

    /* Extension/cache Objects may be freed without an Xt destroy lifecycle.
     * Only Widgets and actual RectObj gadgets participate in visual state. */
    if (widget == NULL || !XtIsRectObj(widget))
        return False;
    display_state = ensure_display_state(XtDisplayOfObject(widget));
    if (display_state == NULL) return False;
    if (display_state->hooks == NULL) {
        String app_name = NULL, app_class = NULL;
        display_state->hooks = XtHooksOfDisplay(display_state->display);
        XtGetApplicationNameAndClass(display_state->display,
                                     &app_name, &app_class);
        if (app_name != NULL && *app_name != '\0')
            display_state->app_name_quark = XrmStringToQuark(app_name);
        if (app_class != NULL && *app_class != '\0')
            display_state->app_class_quark = XrmStringToQuark(app_class);
        XtAddCallback(display_state->hooks, XtNcreateHook,
                      appearance_created, display_state);
        XtAddCallback(display_state->hooks, XtNchangeHook,
                      appearance_changed, display_state);
        XtAddCallback(display_state->hooks, XtNdestroyHook,
                      appearance_destroying, display_state);
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
    {
        Widget parent = XtParent(widget);
        Boolean paned_internal =
            parent != NULL &&
            (XmIsPanedWindow(parent) ||
             XtIsSubclass(parent, xmPanedWidgetClass)) &&
            (XmIsSash(widget) ||
             ((XmIsSeparator(widget) || XmIsSeparatorGadget(widget)) &&
              XtName(widget) != NULL &&
              (strcmp(XtName(widget), "Separator") == 0 ||
               strcmp(XtName(widget), "separator") == 0)));
        if (!paned_internal)
            appearance_dimension_arguments(state, args, num_args);
    }
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
    if (XtParent(widget) != NULL &&
        strcmp(widget_class_name(XtParent(widget)), "DtTitleBox") == 0 &&
        XtName(widget) != NULL && strcmp(XtName(widget), "title") == 0) {
        state->explicit_arg[AP_RENDER_TABLE] = False;
    }
    if ((XmIsCascadeButtonGadget(widget) || XmIsCascadeButton(widget)) &&
        XtName(widget) != NULL &&
        strcmp(XtName(widget), "OptionButton") == 0 &&
        XtParent(widget) != NULL && XmIsRowColumn(XtParent(widget))) {
        state->explicit_arg[AP_BACKGROUND] = False;
        state->explicit_arg[AP_FOREGROUND] = False;
        state->explicit_arg[AP_TOP_SHADOW] = False;
        state->explicit_arg[AP_BOTTOM_SHADOW] = False;
        state->explicit_arg[AP_HIGHLIGHT] = False;
        state->explicit_arg[AP_RENDER_TABLE] = False;
    }
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
    /* These composites depend on final child font metrics, not on the
     * intermediate mixture seen while individual resource plans commit. */
    for (size_t i = 0; i < count; ++i) {
        state = snapshot[i];
        if (state->retired || !state->widget) continue;
        if (XmIsSegmentedControl(state->widget))
            _XmSegmentedControlRefreshGeometry(state->widget);
        else if (XtIsSubclass(state->widget,xmDropDownWidgetClass))
            _XmDropDownRefreshGeometry(state->widget);
        else if (XtIsSubclass(state->widget,xmFontSelectorWidgetClass))
            _XmFontSelectorRefreshPreview(state->widget);
        else if (XmIsContainer(state->widget))
            _XmContainerRefreshGeometry(state->widget);
    }
    for (size_t i = count; i-- > 0; ) {
        state = snapshot[i];
        if (state->retired || !state->widget) continue;
        if (XmIsPanedWindow(state->widget))
            _XmPanedWindowRefreshGeometry(state->widget);
        else if (XtIsSubclass(state->widget, xmPanedWidgetClass))
            _XmPanedRefreshGeometry(state->widget);
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
appearance_token_equals(const char *input, const char *expected)
{
    const unsigned char *start;
    const unsigned char *end;
    size_t len;

    if (input == NULL || expected == NULL)
        return False;
    start = (const unsigned char *)input;
    while (*start != '\0' && isspace(*start))
        ++start;
    end = start + strlen((const char *)start);
    while (end > start && isspace(end[-1]))
        --end;
    len = (size_t)(end - start);
    if (len == 0 || strlen(expected) != len)
        return False;
    for (size_t i = 0; i < len; ++i) {
        unsigned char c = start[i];
        unsigned char e = (unsigned char)expected[i];
        if (c >= 'A' && c <= 'Z')
            c = (unsigned char)(c - 'A' + 'a');
        if (e >= 'A' && e <= 'Z')
            e = (unsigned char)(e - 'A' + 'a');
        if (c == '_')
            c = '-';
        if (e == '_')
            e = '-';
        if (c != e)
            return False;
    }
    return True;
}

static Boolean
profile_from_name(const char *name, XmAppearanceProfile *profile_return)
{
    if (appearance_token_equals(name, CLASSIC_PROFILE)) {
        *profile_return = XmAPPEARANCE_CLASSIC;
        return True;
    }
    if (appearance_token_equals(name, MODERN_LIGHT_PROFILE)) {
        *profile_return = XmAPPEARANCE_MODERN_LIGHT;
        return True;
    }
    if (appearance_token_equals(name, HIGH_CONTRAST_PROFILE)) {
        *profile_return = XmAPPEARANCE_HIGH_CONTRAST;
        return True;
    }
    if (appearance_token_equals(name, MODERN_DARK_PROFILE)) {
        *profile_return = XmAPPEARANCE_MODERN_DARK;
        return True;
    }
    return False;
}

static Boolean
density_from_name(const char *name, XmAppearanceDensity *density_return)
{
    if (appearance_token_equals(name, "comfortable")) {
        *density_return = XmAPPEARANCE_DENSITY_COMFORTABLE;
        return True;
    }
    if (appearance_token_equals(name, "compact")) {
        *density_return = XmAPPEARANCE_DENSITY_COMPACT;
        return True;
    }
    return False;
}

typedef struct {
    XrmQuark leaf_name;
    XrmQuark leaf_class;
    XrmValue value;
    Boolean found;
} AppearanceEnumerateClosure;

static Bool
enumerate_appearance_resource(XrmDatabase *database, XrmBindingList bindings,
                              XrmQuarkList quarks, XrmRepresentation *type,
                              XrmValue *value, XPointer closure)
{
    AppearanceEnumerateClosure *search = (AppearanceEnumerateClosure *)closure;
    unsigned int len = 0;
    (void)database;
    (void)bindings;
    (void)type;
    if (quarks == NULL || value == NULL || value->addr == NULL)
        return False;
    while (quarks[len] != NULLQUARK)
        ++len;
    if ((len == 1 || len == 2) &&
        (quarks[len - 1] == search->leaf_name ||
         quarks[len - 1] == search->leaf_class)) {
        search->value = *value;
        search->found = True;
        return True;
    }
    return False;
}

static Boolean
lookup_appearance_resource(Display *display, XrmDatabase database,
                           const char *qualified_name,
                           const char *qualified_class,
                           const char *leaf_name,
                           const char *leaf_class,
                           XrmValue *value_return)
{
    XmAppearanceDisplayState *state;
    XrmValue wildcard_val = {0, NULL};
    XrmValue toolkit_val = {0, NULL};
    XrmValue app_val = {0, NULL};
    Boolean has_wildcard = False;
    Boolean has_toolkit = False;
    Boolean has_app = False;
    char *type = NULL;
    char wildcard_name[96], wildcard_class[96];

    if (database == NULL || value_return == NULL)
        return False;

    snprintf(wildcard_name, sizeof(wildcard_name),
             "__xm_wildcard__.%s", leaf_name);
    snprintf(wildcard_class, sizeof(wildcard_class),
             "__XmWildcard__.%s", leaf_class);
    if (XrmGetResource(database, wildcard_name, wildcard_class,
                       &type, &wildcard_val) && wildcard_val.addr != NULL)
        has_wildcard = True;

    if (XrmGetResource(database, qualified_name, qualified_class,
                       &type, &toolkit_val) && toolkit_val.addr != NULL)
        has_toolkit = True;

    state = display != NULL ? find_display_state(display) : NULL;
    if (state != NULL && state->app_name_quark != NULLQUARK &&
        state->app_class_quark != NULLQUARK) {
        XrmName names[3];
        XrmClass classes[3];
        XrmRepresentation rep_type;
        names[0] = state->app_name_quark;
        names[1] = XrmStringToQuark(leaf_name);
        names[2] = NULLQUARK;
        classes[0] = state->app_class_quark;
        classes[1] = XrmStringToQuark(leaf_class);
        classes[2] = NULLQUARK;
        if (XrmQGetResource(database, names, classes, &rep_type, &app_val) &&
            app_val.addr != NULL)
            has_app = True;
    }

    if (has_app && (!has_wildcard || app_val.addr != wildcard_val.addr)) {
        *value_return = app_val;
        return True;
    }
    if (has_toolkit && (!has_wildcard || toolkit_val.addr != wildcard_val.addr)) {
        *value_return = toolkit_val;
        return True;
    }
    if (has_toolkit) {
        *value_return = toolkit_val;
        return True;
    }
    if (has_app) {
        *value_return = app_val;
        return True;
    }
    if (XrmGetResource(database, leaf_name, leaf_class, &type, value_return) &&
        value_return->addr != NULL)
        return True;
    if (state == NULL || state->app_name_quark == NULLQUARK) {
        AppearanceEnumerateClosure search;
        XrmName empty_names[1] = {NULLQUARK};
        XrmClass empty_classes[1] = {NULLQUARK};
        memset(&search, 0, sizeof(search));
        search.leaf_name = XrmStringToQuark(leaf_name);
        search.leaf_class = XrmStringToQuark(leaf_class);
        if (XrmEnumerateDatabase(database, empty_names, empty_classes,
                                 XrmEnumAllLevels,
                                 enumerate_appearance_resource,
                                 (XPointer)&search) && search.found) {
            *value_return = search.value;
            return True;
        }
    }
    return False;
}

static Boolean
appearance_controls_exist(Display *display)
{
    XmAppearanceWidgetState *state;
    Boolean controls_exist = False;

    _XmProcessLock();
    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->retired && state->display == display &&
            state->widget != NULL && !XtIsShell(state->widget) &&
            !XmIsScreen(state->widget)) {
            controls_exist = True;
            break;
        }
    _XmProcessUnlock();
    return controls_exist;
}

static Boolean
appearance_any_widgets_exist(Display *display)
{
    XmAppearanceWidgetState *state;
    Boolean exist = False;

    _XmProcessLock();
    for (state = appearance_widgets; state != NULL; state = state->next)
        if (!state->retired && state->display == display &&
            state->widget != NULL) {
            exist = True;
            break;
        }
    _XmProcessUnlock();
    return exist;
}

void
_XmAppearanceInitializeDisplay(Widget xm_display)
{
    Display *display;
    XmAppearanceDisplayState *state;
    XmAppearanceProfile profile;
    XmAppearanceDensity density;
    String app_name = NULL, app_class = NULL;

    if (xm_display == NULL)
        return;
    display = XtDisplayOfObject(xm_display);
    if (display == NULL)
        return;
    state = ensure_display_state(display);
    if (state != NULL) {
        state->display_widget = xm_display;
        XtGetApplicationNameAndClass(display, &app_name, &app_class);
        if (app_name != NULL && *app_name != '\0')
            state->app_name_quark = XrmStringToQuark(app_name);
        if (app_class != NULL && *app_class != '\0')
            state->app_class_quark = XrmStringToQuark(app_class);
        if (state->app_name_quark == NULLQUARK &&
            xm_display->core.xrm_name != NULLQUARK)
            state->app_name_quark = xm_display->core.xrm_name;
        if (state->app_class_quark == NULLQUARK &&
            XtIsApplicationShell(xm_display) &&
            ((ApplicationShellWidget)xm_display)->application.xrm_class != NULLQUARK)
            state->app_class_quark =
                ((ApplicationShellWidget)xm_display)->application.xrm_class;
    }
    (void)XmGetAppearanceProfile(display, &profile);
    (void)XmGetAppearanceDensity(display, &density);
}

static Boolean
apply_appearance_settings(Display *display, XmAppearanceProfile profile,
                          XmAppearanceDensity density, Boolean select_profile);

Boolean
XmSetAppearanceProfile(Display *display, XmAppearanceProfile profile)
{
    XrmDatabase database;
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
    if (name == NULL || !XmGetAppearanceDensity(display, &density) ||
        !XmGetAppearanceProfile(display, &current))
        return False;

    state = ensure_display_state(display);
    if (state == NULL)
        return False;

    if (state->profile_api_selected || appearance_controls_exist(display)) {
        if (current != profile)
            return False;
        state->profile_api_selected = True;
        state->api_profile = profile;
        return True;
    }

    if (current != profile && appearance_any_widgets_exist(display))
        return apply_appearance_settings(display, profile, density, True);

    if (profile != XmAPPEARANCE_CLASSIC &&
        profile_defaults(profile, density) == NULL)
        return False;

    database = XrmGetDatabase(display);
    if (database == NULL) {
        XrmInitialize();
        database = XrmGetStringDatabase("");
        if (database == NULL)
            return False;
        new_database = True;
    }
    if (new_database) {
        state->active_database = database;
        state->owns_active_database = True;
    }

    XrmPutStringResource(&database, APPEARANCE_RESOURCE, name);
    XrmSetDatabase(display, database);
    state->profile_api_selected = True;
    state->api_profile = profile;
    return True;
}

Boolean
XmGetAppearanceProfile(Display *display, XmAppearanceProfile *profile_return)
{
    XmAppearanceDisplayState *state;
    XrmDatabase database;
    XrmValue value;

    if (display == NULL || profile_return == NULL)
        return False;

    *profile_return = XmAPPEARANCE_CLASSIC;
    state = find_display_state(display);
    if (state != NULL && state->committing_geometry) {
        *profile_return = state->committing_profile;
        return True;
    }
    if (state != NULL && state->profile_api_selected) {
        *profile_return = state->api_profile;
        return True;
    }
    database = XrmGetDatabase(display);
    if (database == NULL ||
        !lookup_appearance_resource(display, database, APPEARANCE_RESOURCE,
                                    APPEARANCE_CLASS, XmNappearanceProfile,
                                    XmCAppearanceProfile, &value))
        return True;

    if (profile_from_name(value.addr, profile_return))
        return True;

    state = ensure_display_state(display);
    if (state == NULL || !state->profile_invalid_warned) {
        if (state != NULL)
            state->profile_invalid_warned = True;
        XmeWarning(state != NULL ? state->display_widget : NULL,
                   "Cannot convert appearanceProfile resource to XmRAppearanceProfile; falling back to classic");
    }
    *profile_return = XmAPPEARANCE_CLASSIC;
    return True;
}

static Boolean
semantic_palette_dirty(Display *display, XmAppearanceDisplayState *state,
                       XmAppearanceProfile profile)
{
    XrmDatabase db;
    XrmValue v_surf = {0, NULL}, v_acc = {0, NULL};
    XrmValue v_afg = {0, NULL}, v_dng = {0, NULL};
    const char *s_surf = "", *s_acc = "", *s_afg = "", *s_dng = "";
    const XmSemanticPalette *pal;

    if (display == NULL || state == NULL ||
        (profile != XmAPPEARANCE_MODERN_LIGHT &&
         profile != XmAPPEARANCE_MODERN_DARK))
        return False;
    pal = &state->palette;
    if (!pal->valid || pal->profile != profile)
        return True;
    db = XrmGetDatabase(display);
    if (db != NULL) {
        if (lookup_appearance_resource(display, db, SURFACE_BACKGROUND_RESOURCE,
                                       SURFACE_BACKGROUND_CLASS,
                                       XmNsurfaceBackground,
                                       XmCSurfaceBackground, &v_surf) &&
            v_surf.addr != NULL)
            s_surf = v_surf.addr;
        if (lookup_appearance_resource(display, db, ACCENT_COLOR_RESOURCE,
                                       ACCENT_COLOR_CLASS,
                                       XmNaccentColor, XmCAccentColor, &v_acc) &&
            v_acc.addr != NULL)
            s_acc = v_acc.addr;
        if (lookup_appearance_resource(display, db, ACCENT_FOREGROUND_RESOURCE,
                                       ACCENT_FOREGROUND_CLASS,
                                       XmNaccentForeground,
                                       XmCAccentForeground, &v_afg) &&
            v_afg.addr != NULL)
            s_afg = v_afg.addr;
        if (lookup_appearance_resource(display, db, DANGER_COLOR_RESOURCE,
                                       DANGER_COLOR_CLASS,
                                       XmNdangerColor, XmCDangerColor, &v_dng) &&
            v_dng.addr != NULL)
            s_dng = v_dng.addr;
    }
    return strncmp(pal->raw_surface, s_surf, sizeof(pal->raw_surface) - 1) != 0 ||
           strncmp(pal->raw_accent, s_acc, sizeof(pal->raw_accent) - 1) != 0 ||
           strncmp(pal->raw_accent_fg, s_afg, sizeof(pal->raw_accent_fg) - 1) != 0 ||
           strncmp(pal->raw_danger, s_dng, sizeof(pal->raw_danger) - 1) != 0;
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
    state = ensure_display_state(display);
    if (state == NULL)
        return False;
    if (current == profile && current_density == density &&
        !semantic_palette_dirty(display, state, profile)) {
        if (select_profile) {
            state->profile_api_selected = True;
            state->api_profile = profile;
        } else {
            state->density_api_selected = True;
            state->api_density = density;
        }
        return True;
    }
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
    if (select_profile) {
        state->profile_api_selected = True;
        state->api_profile = profile;
    } else {
        state->density_api_selected = True;
        state->api_density = density;
    }
    appearance_transaction_end(state);
    {
        XmAppearanceWidgetState *ws;
        for (ws = appearance_widgets; ws != NULL; ws = ws->next) {
            if (ws->retired || ws->display != display || ws->widget == NULL)
                continue;
            if (XtIsWidget(ws->widget)) {
                if (_XmPaintBufferKind(ws->widget))
                    _XmPaintInvalidate(ws->widget);
                else if (XtIsRealized(ws->widget))
                    XClearArea(display, XtWindow(ws->widget), 0, 0, 0, 0, True);
            } else if (ws->parent != NULL && XtIsRealized(ws->parent)) {
                XClearArea(display, XtWindow(ws->parent), 0, 0, 0, 0, True);
            }
        }
    }
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
    XmAppearanceDisplayState *state;
    XrmValue value;
    XrmDatabase database;

    if (display == NULL || density_return == NULL)
        return False;

    *density_return = XmAPPEARANCE_DENSITY_COMFORTABLE;
    state = find_display_state(display);
    if (state != NULL && state->committing_geometry) {
        *density_return = state->committing_density;
        return True;
    }
    if (state != NULL && state->density_api_selected) {
        *density_return = state->api_density;
        return True;
    }
    database = XrmGetDatabase(display);
    if (database == NULL ||
        !lookup_appearance_resource(display, database, DENSITY_RESOURCE,
                                    DENSITY_CLASS, XmNappearanceDensity,
                                    XmCAppearanceDensity, &value))
        return True;

    if (density_from_name(value.addr, density_return))
        return True;

    state = ensure_display_state(display);
    if (state == NULL || !state->density_invalid_warned) {
        if (state != NULL)
            state->density_invalid_warned = True;
        XmeWarning(state != NULL ? state->display_widget : NULL,
                   "Cannot convert appearanceDensity resource to XmRAppearanceDensity; falling back to comfortable");
    }
    *density_return = XmAPPEARANCE_DENSITY_COMFORTABLE;
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
    XmAppearanceDisplayState *display_state;

    if (display == NULL ||
        (density != XmAPPEARANCE_DENSITY_COMFORTABLE &&
         density != XmAPPEARANCE_DENSITY_COMPACT) ||
        appearance_transaction_is_active(display) ||
        !XmGetAppearanceDensity(display, &current))
        return False;
    display_state = ensure_display_state(display);
    if (display_state == NULL)
        return False;
    if (current == density) {
        display_state->density_api_selected = True;
        display_state->api_density = density;
        return True;
    }
    if (appearance_controls_exist(display))
        return False;
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
    Pixel pixel = fallback;
    XrmValue from, to = {sizeof(pixel), (XPointer)&pixel};
    Widget context = XtIsWidget(widget) ? widget : XtParent(widget);
    if (_XmAppearanceOwnsResource(widget, XmNbackground) &&
        XmGetAppearanceProfile(XtDisplayOfObject(widget), &profile) &&
        (profile == XmAPPEARANCE_MODERN_LIGHT || profile == XmAPPEARANCE_MODERN_DARK) &&
        XmGetAppearanceDensity(XtDisplayOfObject(widget), &density)) {
        const XmSemanticPalette *pal =
            resolve_semantic_palette(XtDisplayOfObject(widget), profile);
        if (pal != NULL && (pal->has_accent || pal->has_surface)) {
            if (appearance_string_value(pal->selection_fill, &from) &&
                XtConvertAndStore(context, XmRString, &from, XmRPixel, &to))
                return pixel;
        }
        if (appearance_database_named_value(widget, profile_defaults(profile,density),
                XmNarmColor, XmCArmColor, &from) &&
            XtConvertAndStore(context, XmRString, &from, XmRPixel, &to))
            return pixel;
    }
    return fallback;
}

static int
appearance_ignore_x_error(Display *dpy, XErrorEvent *ev)
{
    (void)dpy;
    (void)ev;
    return 0;
}

static void
appearance_safe_select_property_mask(Display *display, Window win)
{
    XWindowAttributes wa;
    int (*old_handler)(Display *, XErrorEvent *);

    if (display == NULL || win == None)
        return;
    XSync(display, False);
    old_handler = XSetErrorHandler(appearance_ignore_x_error);
    memset(&wa, 0, sizeof(wa));
    if (XGetWindowAttributes(display, win, &wa))
        XSelectInput(display, win, wa.your_event_mask | PropertyChangeMask);
    XSync(display, False);
    (void)XSetErrorHandler(old_handler);
}

static Boolean
appearance_is_dtterm_widget(Widget w)
{
    WidgetClass wc;
    if (w == NULL || !XtIsWidget(w))
        return False;
    for (wc = XtClass(w); wc != NULL; wc = wc->core_class.superclass) {
        if (wc->core_class.class_name != NULL &&
            (strcmp(wc->core_class.class_name, "DtTermPrimitive") == 0 ||
             strcmp(wc->core_class.class_name, "DtTerm") == 0))
            return True;
    }
    return False;
}

static Boolean
appearance_is_dthelp_display_area(Widget w)
{
    if (w == NULL || !XtIsWidget(w) || !XmIsDrawnButton(w))
        return False;
    if (XtName(w) != NULL && strcmp(XtName(w), "DisplayArea") == 0)
        return True;
    if (XtParent(w) != NULL && XtName(XtParent(w)) != NULL &&
        strcmp(XtName(XtParent(w)), "DisplayAreaForm") == 0)
        return True;
    return False;
}

static Boolean
appearance_widget_in_menu_tree(Widget w)
{
    Widget cur;
    for (cur = w; cur != NULL; cur = XtParent(cur)) {
        if (XmIsMenuShell(cur))
            return True;
        if (XmIsRowColumn(cur) &&
            (RC_Type(cur) == XmMENU_BAR ||
             RC_Type(cur) == XmMENU_PULLDOWN ||
             RC_Type(cur) == XmMENU_POPUP))
            return True;
    }
    return False;
}

static Boolean
appearance_widget_in_secondary_tree(Widget w)
{
    Widget cur;
    if (appearance_widget_in_menu_tree(w))
        return True;
    for (cur = w; cur != NULL; cur = XtParent(cur)) {
        if (XmIsDialogShell(cur))
            return True;
    }
    return False;
}

static Boolean
appearance_rgb_to_pixel(Widget widget, const unsigned int rgb[3],
                        XtArgVal *value_return)
{
    char hex[8];
    Pixel pixel = 0;
    XrmValue from, to;
    Widget context;

    if (widget == NULL || value_return == NULL)
        return False;
    format_rgb_hex(rgb, hex);
    if (!appearance_string_value(hex, &from))
        return False;
    context = XtIsWidget(widget) ? widget : XtParent(widget);
    if (context == NULL)
        return False;
    to.addr = (XPointer)&pixel;
    to.size = sizeof(pixel);
    if (!XtConvertAndStore(context, XmRString, &from, XmRPixel, &to)) {
        Display *dpy = XtDisplayOfObject(widget);
        Screen *scr = XtScreenOfObject(widget);
        Colormap cmap = context->core.colormap;
        XColor xc;
        if (!XParseColor(dpy, cmap, hex, &xc))
            return False;
        if (!XAllocColor(dpy, cmap, &xc))
            pixel = (rgb_relative_luminance(rgb) > 0.4)
                        ? WhitePixelOfScreen(scr)
                        : BlackPixelOfScreen(scr);
        else
            pixel = xc.pixel;
    }
    *value_return = (XtArgVal)pixel;
    return True;
}

static Boolean
appearance_colorobj_modern_color(XmAppearanceWidgetState *state,
                                 XmAppearanceProfile new_profile,
                                 AppearanceProperty property,
                                 XtArgVal *value_return)
{
    static const unsigned int white[3] = {255, 255, 255};
    static const unsigned int black[3] = {0, 0, 0};
    Widget w;
    Display *dpy;
    Screen *scr;
    Colormap cmap;
    XmColorObj co;
    const XmSemanticPalette *pal;
    const XmPixelSet *pset;
    int scr_num, slot = -1, hint;
    unsigned char role;
    Pixel orig_bg = 0;
    XColor qcolors[2];
    unsigned int raw_rgb[3], acc_rgb[3], surf_rgb[3], fg_rgb[3];
    unsigned int btn_rgb[3], border_rgb[3], well_rgb[3], arm_rgb[3];
    unsigned int hl_rgb[3], muted_rgb[3], out_rgb[3];
    Boolean dark;

    if (state == NULL || state->widget == NULL || value_return == NULL)
        return False;
    if (new_profile != XmAPPEARANCE_MODERN_LIGHT &&
        new_profile != XmAPPEARANCE_MODERN_DARK)
        return False;
    if (!appearance_property_is_color(property))
        return False;

    w = state->widget;
    dpy = state->display;
    co = _XmColorObjForDisplay(dpy);
    if (co == NULL || !co->color_obj.colorIsRunning ||
        !co->color_obj.useColorObj || co->color_obj.colors == NULL)
        return False;

    pal = resolve_semantic_palette(dpy, new_profile);
    if (pal != NULL && pal->has_surface)
        return False;

    role = appearance_role(w);
    if (role == XmAPPEARANCE_ROLE_PRIMARY ||
        role == XmAPPEARANCE_ROLE_DESTRUCTIVE ||
        role == XmAPPEARANCE_ROLE_SECONDARY)
        return False;
    if (XmIsInfoBar(w) || XtIsSubclass(w, xmAvatarWidgetClass) || XmIsLink(w) ||
        (XtParent(w) != NULL && XmIsSegmentedControl(XtParent(w))))
        return False;

    scr = XtScreenOfObject(w);
    scr_num = XScreenNumberOfScreen(scr);
    if (scr_num < 0 || scr_num >= co->color_obj.numScreens ||
        co->color_obj.colors[scr_num] == NULL)
        return False;
    pset = co->color_obj.colors[scr_num];

    if (state->supported[AP_BACKGROUND]) {
        orig_bg = (Pixel)state->original[AP_BACKGROUND];
    } else if (state->parent != NULL) {
        XmAppearanceWidgetState *ps = find_live_widget_state(state->parent);
        if (ps != NULL && ps->initialized && ps->supported[AP_BACKGROUND])
            orig_bg = (Pixel)ps->original[AP_BACKGROUND];
        else
            XtVaGetValues(state->parent, XmNbackground, &orig_bg, NULL);
    }

    if (XmIsText(w) || XmIsTextField(w) || XmIsList(w) || XmIsSearchBox(w) ||
        appearance_is_dtterm_widget(w) || appearance_is_dthelp_display_area(w))
        hint = co->color_obj.text;
    else if (appearance_widget_in_menu_tree(w))
        hint = co->color_obj.secondary;
    else
        hint = co->color_obj.primary;

    if (appearance_is_dthelp_display_area(w) &&
        co->color_obj.text >= 0 && co->color_obj.text < XmCO_NUM_COLORS) {
        slot = co->color_obj.text;
    } else if (hint >= 0 && hint < XmCO_NUM_COLORS && pset[hint].bg == orig_bg) {
        slot = hint;
    } else if (!appearance_widget_in_menu_tree(w) &&
               appearance_widget_in_secondary_tree(w) &&
               co->color_obj.secondary >= 0 &&
               co->color_obj.secondary < XmCO_NUM_COLORS &&
               pset[co->color_obj.secondary].bg == orig_bg &&
               co->color_obj.primary >= 0 &&
               co->color_obj.primary < XmCO_NUM_COLORS) {
        /* In modern profiles, DialogShell windows share the clean primary
         * window surface & elevated button contrast shown in Style Manager's
         * Sample Dialog preview, while menus retain the secondary bar tint and
         * state->original[] preserves raw secondary pixels for Classic CDE. */
        slot = co->color_obj.primary;
    } else {
        for (int k = 0; k < XmCO_NUM_COLORS; ++k) {
            if (pset[k].bg == orig_bg) {
                slot = k;
                break;
            }
        }
        if (slot < 0) {
            for (int k = 0; k < XmCO_NUM_COLORS; ++k) {
                if (pset[k].sc == orig_bg) {
                    slot = (co->color_obj.text >= 0 &&
                            co->color_obj.text < XmCO_NUM_COLORS)
                               ? co->color_obj.text
                               : k;
                    break;
                }
            }
        }
    }
    if (slot < 0 || slot >= XmCO_NUM_COLORS)
        slot = (hint >= 0 && hint < XmCO_NUM_COLORS) ? hint : 0;

    cmap = DefaultColormapOfScreen(scr);
    {
        Widget owner = XtIsWidget(w) ? w : XtParent(w);
        if (owner != NULL)
            XtVaGetValues(owner, XmNcolormap, &cmap, NULL);
    }
    qcolors[0].pixel = pset[slot].bg;
    qcolors[1].pixel = pset[(co->color_obj.active >= 0 &&
                             co->color_obj.active < XmCO_NUM_COLORS)
                                ? co->color_obj.active
                                : 0].bg;
    XQueryColors(dpy, cmap, qcolors, 2);
    raw_rgb[0] = (unsigned int)(qcolors[0].red >> 8);
    raw_rgb[1] = (unsigned int)(qcolors[0].green >> 8);
    raw_rgb[2] = (unsigned int)(qcolors[0].blue >> 8);
    acc_rgb[0] = (unsigned int)(qcolors[1].red >> 8);
    acc_rgb[1] = (unsigned int)(qcolors[1].green >> 8);
    acc_rgb[2] = (unsigned int)(qcolors[1].blue >> 8);

    dark = (new_profile == XmAPPEARANCE_MODERN_DARK);
    if (!dark) {
        static const unsigned int base_ink[3] = {0x1d, 0x21, 0x29};
        static const unsigned int base_well[3] = {0xfb, 0xfc, 0xfe};
        static const unsigned int base_prim[3] = {0xf1, 0xf3, 0xf7};
        static const unsigned int base_sec[3] = {0xe6, 0xeb, 0xf2};
        static const unsigned int base_ws[3] = {0xe8, 0xec, 0xf2};
        unsigned int dark_tint[3];
        double lum;

        blend_rgb(raw_rgb, black, 0.72, dark_tint);
        blend_rgb(base_ink, dark_tint, 0.14, fg_rgb);
        if (slot == co->color_obj.text)
            blend_rgb(base_well, raw_rgb, 0.10, surf_rgb);
        else if (slot == co->color_obj.primary)
            blend_rgb(base_prim, raw_rgb, 0.25, surf_rgb);
        else if (slot == co->color_obj.secondary)
            blend_rgb(base_sec, raw_rgb, 0.28, surf_rgb);
        else
            blend_rgb(base_ws, raw_rgb, 0.45, surf_rgb);

        lum = (0.30 * surf_rgb[0] + 0.59 * surf_rgb[1] + 0.11 * surf_rgb[2]) / 255.0;
        if (lum < 0.62)
            blend_rgb(surf_rgb, white, 0.35, surf_rgb);
        (void)clamp_rgb_for_contrast(surf_rgb, fg_rgb, 5.5);
        blend_rgb(surf_rgb, white, 0.65, well_rgb);
        lum = (0.30 * surf_rgb[0] + 0.59 * surf_rgb[1] + 0.11 * surf_rgb[2]) / 255.0;
        if (lum > 0.88)
            blend_rgb(surf_rgb, fg_rgb, 0.07, btn_rgb);
        else
            blend_rgb(surf_rgb, white, 0.48, btn_rgb);
        blend_rgb(surf_rgb, fg_rgb, 0.25, border_rgb);
        blend_rgb(surf_rgb, acc_rgb, 0.22, arm_rgb);
    } else {
        static const unsigned int base_fg[3] = {0xee, 0xf2, 0xf7};
        static const unsigned int base_well[3] = {0x16, 0x19, 0x20};
        static const unsigned int base_prim[3] = {0x21, 0x25, 0x2f};
        static const unsigned int base_sec[3] = {0x2b, 0x31, 0x3d};
        static const unsigned int base_ws[3] = {0x26, 0x2b, 0x36};
        unsigned int tint_rgb[3];

        fg_rgb[0] = base_fg[0];
        fg_rgb[1] = base_fg[1];
        fg_rgb[2] = base_fg[2];
        if (rgb_relative_luminance(raw_rgb) > 0.20)
            blend_rgb(base_prim, raw_rgb, 0.28, tint_rgb);
        else {
            tint_rgb[0] = raw_rgb[0];
            tint_rgb[1] = raw_rgb[1];
            tint_rgb[2] = raw_rgb[2];
        }

        if (slot == co->color_obj.text)
            blend_rgb(base_well, tint_rgb, 0.45, surf_rgb);
        else if (slot == co->color_obj.primary)
            blend_rgb(base_prim, tint_rgb, 0.55, surf_rgb);
        else if (slot == co->color_obj.secondary)
            blend_rgb(base_sec, tint_rgb, 0.55, surf_rgb);
        else
            blend_rgb(base_ws, tint_rgb, 0.72, surf_rgb);

        (void)clamp_rgb_for_contrast(surf_rgb, fg_rgb, 5.5);
        blend_rgb(surf_rgb, black, 0.34, well_rgb);
        blend_rgb(surf_rgb, fg_rgb, 0.13, btn_rgb);
        blend_rgb(surf_rgb, fg_rgb, 0.25, border_rgb);
        blend_rgb(surf_rgb, acc_rgb, 0.28, arm_rgb);
    }

    hl_rgb[0] = acc_rgb[0];
    hl_rgb[1] = acc_rgb[1];
    hl_rgb[2] = acc_rgb[2];
    (void)clamp_rgb_for_contrast(hl_rgb, surf_rgb, 3.0);
    blend_rgb(surf_rgb, fg_rgb, 0.52, muted_rgb);

    if (XmIsScrollBar(w)) {
        if (property == AP_FOREGROUND) {
            blend_rgb(surf_rgb, fg_rgb, 0.48, out_rgb);
            return appearance_rgb_to_pixel(w, out_rgb, value_return);
        }
        if (property == AP_TROUGH_COLOR || property == AP_BACKGROUND)
            return appearance_rgb_to_pixel(w, well_rgb, value_return);
        if (property == AP_TOP_SHADOW || property == AP_BOTTOM_SHADOW)
            return appearance_rgb_to_pixel(w, border_rgb, value_return);
    }

    switch (property) {
    case AP_BACKGROUND: {
        Boolean in_menu =
            XtParent(w) != NULL && XmIsRowColumn(XtParent(w)) &&
            (RC_Type(XtParent(w)) == XmMENU_PULLDOWN ||
             RC_Type(XtParent(w)) == XmMENU_POPUP ||
             RC_Type(XtParent(w)) == XmMENU_BAR);
        Boolean is_notebook_tab = False;
        if (XtParent(w) != NULL && XmIsNotebook(XtParent(w)) &&
            w->core.constraints != NULL) {
            XmNotebookConstraintPtr nc =
                (XmNotebookConstraintPtr)w->core.constraints;
            if (nc->notebook.child_type == XmMAJOR_TAB ||
                nc->notebook.child_type == XmMINOR_TAB)
                is_notebook_tab = True;
        }
        if (XmIsTextField(w) || XmIsText(w) || XmIsList(w) ||
            XmIsSearchBox(w) || appearance_is_dtterm_widget(w) ||
            appearance_is_dthelp_display_area(w)) {
            return appearance_rgb_to_pixel(
                w, slot == co->color_obj.text ? surf_rgb : well_rgb,
                value_return);
        }
        if (!in_menu && !is_notebook_tab &&
            (XmIsPushButton(w) || XmIsPushButtonGadget(w) ||
             XmIsDrawnButton(w) || XmIsArrowButton(w) ||
             XmIsArrowButtonGadget(w) ||
             ((XmIsCascadeButton(w) || XmIsCascadeButtonGadget(w)) &&
              XtParent(w) != NULL && XmIsRowColumn(XtParent(w)) &&
              RC_Type(XtParent(w)) == XmMENU_OPTION))) {
            return appearance_rgb_to_pixel(w, btn_rgb, value_return);
        }
        return appearance_rgb_to_pixel(w, surf_rgb, value_return);
    }
    case AP_FOREGROUND:
        return appearance_rgb_to_pixel(w, fg_rgb, value_return);
    case AP_TOP_SHADOW:
    case AP_BOTTOM_SHADOW:
    case AP_BACK_PAGE_FOREGROUND:
        return appearance_rgb_to_pixel(w, border_rgb, value_return);
    case AP_HIGHLIGHT:
        return appearance_rgb_to_pixel(w, hl_rgb, value_return);
    case AP_SELECT:
        if (XmIsToggleButton(w) || XmIsToggleButtonGadget(w))
            return appearance_rgb_to_pixel(w, hl_rgb, value_return);
        return appearance_rgb_to_pixel(w, arm_rgb, value_return);
    case AP_ARM:
        return appearance_rgb_to_pixel(w, arm_rgb, value_return);
    case AP_TROUGH_COLOR:
    case AP_UNSELECT_COLOR:
        return appearance_rgb_to_pixel(w, well_rgb, value_return);
    case AP_FRAME_BACKGROUND:
    case AP_BACK_PAGE_BACKGROUND:
        return appearance_rgb_to_pixel(w, surf_rgb, value_return);
    case AP_INSENSITIVE_FOREGROUND:
        return appearance_rgb_to_pixel(w, muted_rgb, value_return);
    default:
        return False;
    }
}

static void
appearance_recolor_colorobj_widgets(Display *display,
                                    XmAppearanceDisplayState *display_state,
                                    XmColorObj co,
                                    const XmPixelSet *old_sets,
                                    const XmPixelSet *new_sets,
                                    XmAppearanceProfile target_profile,
                                    Boolean repaint_widgets)
{
    XmAppearanceWidgetState **snapshot = NULL;
    XmAppearanceWidgetState *ws;
    size_t count = 0;
    Boolean was_active = False;

    if (display == NULL || co == NULL || old_sets == NULL || new_sets == NULL)
        return;

    _XmProcessLock();
    for (ws = appearance_widgets; ws != NULL; ws = ws->next)
        if (!ws->retired && ws->display == display && ws->widget != NULL)
            ++count;
    if (count > 0 && count <= SIZE_MAX / sizeof(*snapshot))
        snapshot = (XmAppearanceWidgetState **)XtMalloc(
            (Cardinal)(count * sizeof(*snapshot)));
    if (snapshot == NULL) {
        _XmProcessUnlock();
        return;
    }
    count = 0;
    for (ws = appearance_widgets; ws != NULL; ws = ws->next)
        if (!ws->retired && ws->display == display && ws->widget != NULL) {
            ++ws->references;
            snapshot[count++] = ws;
        }
    _XmProcessUnlock();

    if (count > 1)
        qsort(snapshot, count, sizeof(*snapshot), appearance_parent_first);

    /* Guard toolkit-driven ColorObj updates so XtNchangeHook does not treat
     * palette refreshes as explicit application overrides or re-enter subtree
     * layout mid-loop, while keeping resource_commit_depth at 0 so XmManager
     * visual-change propagation updates child gadgets (e.g. DtIconGadget). */
    if (display_state != NULL) {
        was_active = display_state->transaction_active;
        display_state->transaction_active = True;
    }

    for (size_t i = 0; i < count; ++i) {
        Widget w;
        Pixel cur_bg = 0;
        Pixel cur_fg = 0;
        Boolean explicit_bg = False;
        Boolean bg_uses_select = False;
        Boolean fg_matches_old_palette = False;
        int slot = -1;
        int hint;
        Arg set_args[10];
        Cardinal n = 0;

        ws = snapshot[i];
        w = ws->widget;
        if (ws->retired || w == NULL)
            continue;
        if (XtName(w) != NULL && strncmp(XtName(w), "colorButton", 11) == 0)
            continue;
        if (!ws->initialized &&
            !initialize_appearance_state(ws, target_profile))
            continue;

        if (ws->supported[AP_BACKGROUND]) {
            XrmValue bg_val = {0, NULL};
            if (ws->explicit_arg[AP_BACKGROUND]) {
                explicit_bg = True;
            } else if (appearance_database_value(
                           w, XrmGetDatabase(display), AP_BACKGROUND, &bg_val)) {
                explicit_bg = True;
            }
            cur_bg = (Pixel)ws->original[AP_BACKGROUND];
        } else if (ws->parent != NULL) {
            XtVaGetValues(ws->parent, XmNbackground, &cur_bg, NULL);
        } else {
            XtVaGetValues(w, XmNbackground, &cur_bg, NULL);
        }
        if (ws->supported[AP_FOREGROUND])
            XtVaGetValues(w, XmNforeground, &cur_fg, NULL);

        if (!explicit_bg) {
            if (XmIsText(w) || XmIsTextField(w) || XmIsList(w) ||
                appearance_is_dtterm_widget(w) ||
                appearance_is_dthelp_display_area(w))
                hint = co->color_obj.text;
            else if (appearance_widget_in_secondary_tree(w))
                hint = co->color_obj.secondary;
            else
                hint = co->color_obj.primary;

            if (appearance_is_dthelp_display_area(w) &&
                co->color_obj.text >= 0 &&
                co->color_obj.text < XmCO_NUM_COLORS) {
                slot = co->color_obj.text;
            } else if (hint >= 0 && hint < XmCO_NUM_COLORS &&
                (old_sets[hint].bg == cur_bg ||
                 (!ws->supported[AP_BACKGROUND] &&
                  new_sets[hint].bg == cur_bg))) {
                slot = hint;
            } else {
                for (int k = 0; k < XmCO_NUM_COLORS; ++k) {
                    if (old_sets[k].bg == cur_bg ||
                        (!ws->supported[AP_BACKGROUND] &&
                         new_sets[k].bg == cur_bg)) {
                        slot = k;
                        break;
                    }
                }
                if (slot < 0) {
                    for (int k = 0; k < XmCO_NUM_COLORS; ++k) {
                        if (old_sets[k].sc == cur_bg ||
                            (!ws->supported[AP_BACKGROUND] &&
                             new_sets[k].sc == cur_bg)) {
                            slot = k;
                            bg_uses_select = True;
                            break;
                        }
                    }
                }
            }
        }
        if (slot < 0 || slot >= XmCO_NUM_COLORS) {
            if (ws->supported[AP_HIGHLIGHT] && !ws->explicit_arg[AP_HIGHLIGHT] &&
                co->color_obj.active >= 0 &&
                co->color_obj.active < XmCO_NUM_COLORS) {
                Pixel cur_hl = 0;
                XtVaGetValues(w, XmNhighlightColor, &cur_hl, NULL);
                if (cur_hl == old_sets[co->color_obj.active].bg ||
                    (Pixel)ws->original[AP_HIGHLIGHT] ==
                        old_sets[co->color_obj.active].bg) {
                    ws->original[AP_HIGHLIGHT] =
                        (XtArgVal)new_sets[co->color_obj.active].bg;
                    if (target_profile != XmAPPEARANCE_HIGH_CONTRAST) {
                        XtSetArg(set_args[0], XmNhighlightColor,
                                 new_sets[co->color_obj.active].bg);
                        XtSetValues(w, set_args, 1);
                        if (repaint_widgets) {
                            if (XtIsWidget(w)) {
                                if (_XmPaintBufferKind(w))
                                    _XmPaintInvalidate(w);
                                else if (XtIsRealized(w))
                                    XClearArea(display, XtWindow(w), 0, 0, 0,
                                               0, True);
                            } else if (ws->parent != NULL &&
                                       XtIsRealized(ws->parent)) {
                                XClearArea(display, XtWindow(ws->parent), 0, 0,
                                           0, 0, True);
                            }
                        }
                    }
                }
            }
            continue;
        }

        fg_matches_old_palette =
            (cur_fg == old_sets[slot].fg ||
             (Pixel)ws->original[AP_FOREGROUND] == old_sets[slot].fg);
        ws->original[AP_BACKGROUND] =
            (XtArgVal)(bg_uses_select ? new_sets[slot].sc : new_sets[slot].bg);
        if (!ws->explicit_arg[AP_FOREGROUND] || fg_matches_old_palette)
            ws->original[AP_FOREGROUND] = (XtArgVal)new_sets[slot].fg;
        ws->original[AP_TOP_SHADOW] = (XtArgVal)new_sets[slot].ts;
        ws->original[AP_BOTTOM_SHADOW] = (XtArgVal)new_sets[slot].bs;
        ws->original[AP_SELECT] = (XtArgVal)new_sets[slot].sc;
        ws->original[AP_ARM] = (XtArgVal)new_sets[slot].sc;
        ws->original[AP_TROUGH_COLOR] = (XtArgVal)new_sets[slot].sc;
        if (co->color_obj.active >= 0 &&
            co->color_obj.active < XmCO_NUM_COLORS)
            ws->original[AP_HIGHLIGHT] =
                (XtArgVal)new_sets[co->color_obj.active].bg;

        if (target_profile != XmAPPEARANCE_HIGH_CONTRAST) {
            if (ws->supported[AP_BACKGROUND]) {
                XtSetArg(set_args[n], XmNbackground,
                         bg_uses_select ? new_sets[slot].sc : new_sets[slot].bg);
                ++n;
            }
            if (ws->supported[AP_FOREGROUND] &&
                (!ws->explicit_arg[AP_FOREGROUND] || fg_matches_old_palette)) {
                XtSetArg(set_args[n], XmNforeground, new_sets[slot].fg);
                ++n;
            }
            if (ws->supported[AP_TOP_SHADOW]) {
                XtSetArg(set_args[n], XmNtopShadowColor, new_sets[slot].ts);
                ++n;
            }
            if (ws->supported[AP_BOTTOM_SHADOW]) {
                XtSetArg(set_args[n], XmNbottomShadowColor, new_sets[slot].bs);
                ++n;
            }
            if (ws->supported[AP_SELECT]) {
                XtSetArg(set_args[n], XmNselectColor, new_sets[slot].sc);
                ++n;
            }
            if (ws->supported[AP_ARM]) {
                XtSetArg(set_args[n], XmNarmColor, new_sets[slot].sc);
                ++n;
            }
            if (ws->supported[AP_TROUGH_COLOR]) {
                XtSetArg(set_args[n], XmNtroughColor, new_sets[slot].sc);
                ++n;
            }
            if (ws->supported[AP_HIGHLIGHT] &&
                !ws->explicit_arg[AP_HIGHLIGHT] &&
                co->color_obj.active >= 0 &&
                co->color_obj.active < XmCO_NUM_COLORS) {
                XtSetArg(set_args[n], XmNhighlightColor,
                         new_sets[co->color_obj.active].bg);
                ++n;
            }
            if (n > 0)
                XtSetValues(w, set_args, n);
        }

        if (repaint_widgets) {
            if (XtIsWidget(w)) {
                if (_XmPaintBufferKind(w))
                    _XmPaintInvalidate(w);
                else if (XtIsRealized(w))
                    XClearArea(display, XtWindow(w), 0, 0, 0, 0, True);
            } else if (ws->parent != NULL && XtIsRealized(ws->parent)) {
                XClearArea(display, XtWindow(ws->parent), 0, 0, 0, 0, True);
            }
        }
    }

    if (display_state != NULL)
        display_state->transaction_active = was_active;

    for (size_t i = 0; i < count; ++i) {
        ws = snapshot[i];
        _XmProcessLock();
        if (ws->references != 0)
            --ws->references;
        if (ws->retired && ws->references == 0)
            appearance_free_widget_state(ws);
        _XmProcessUnlock();
    }
    XtFree((char *)snapshot);
}

static char *
appearance_read_root_resource_manager(Display *display)
{
    Atom actual_type = None;
    int actual_format = 0;
    unsigned long nitems = 0, bytes_after = 0;
    unsigned char *prop = NULL;
    char *copy = NULL;

    if (display == NULL)
        return NULL;
    if (XGetWindowProperty(display, DefaultRootWindow(display),
                           XA_RESOURCE_MANAGER, 0L, 262144L, False,
                           XA_STRING, &actual_type, &actual_format,
                           &nitems, &bytes_after, &prop) != Success ||
        prop == NULL) {
        if (prop != NULL)
            XFree(prop);
        return NULL;
    }
    if (actual_type == XA_STRING && actual_format == 8 && nitems > 0) {
        copy = XtMalloc((Cardinal)(nitems + 1));
        if (copy != NULL) {
            memcpy(copy, prop, nitems);
            copy[nitems] = '\0';
        }
    }
    XFree(prop);
    return copy;
}

static void
appearance_sync_token_resource(Display *display, XrmDatabase rm_db,
                               XrmDatabase app_db,
                               const char *qual_name, const char *qual_class,
                               const char *leaf_name, const char *leaf_class)
{
    XrmValue val = {0, NULL};
    const char *str = "";

    if (app_db == NULL)
        return;
    if (rm_db != NULL &&
        lookup_appearance_resource(display, rm_db, qual_name, qual_class,
                                   leaf_name, leaf_class, &val) &&
        val.addr != NULL)
        str = val.addr;
    XrmPutStringResource(&app_db, qual_name, str);
}

static void
appearance_live_sync_timer(XtPointer closure, XtIntervalId *id)
{
    Display *display = (Display *)closure;
    XmAppearanceDisplayState *state = find_display_state(display);
    XmPixelSet old_sets[XmCO_NUM_COLORS];
    XmPixelSet new_sets[XmCO_NUM_COLORS];
    Boolean palette_changed = False;
    Boolean will_apply_appearance = False;
    XmAppearanceProfile cur_prof = XmAPPEARANCE_CLASSIC;
    XmAppearanceDensity cur_dens = XmAPPEARANCE_DENSITY_COMFORTABLE;
    XmAppearanceProfile target_prof = XmAPPEARANCE_CLASSIC;
    XmAppearanceDensity target_dens = XmAPPEARANCE_DENSITY_COMFORTABLE;
    char *rm_str = NULL;
    XrmDatabase rm_db = NULL;
    XrmDatabase app_db;

    (void)id;
    if (state == NULL)
        return;
    state->live_sync_timer_id = 0;
    if (appearance_transaction_is_active(display)) {
        if (state->live_app_context != NULL) {
            state->live_sync_timer_id = XtAppAddTimeOut(
                state->live_app_context, 10UL, appearance_live_sync_timer,
                (XtPointer)display);
        }
        return;
    }
    (void)XmGetAppearanceProfile(display, &cur_prof);
    (void)XmGetAppearanceDensity(display, &cur_dens);

    rm_str = appearance_read_root_resource_manager(display);
    if (rm_str != NULL) {
        XrmValue val = {0, NULL};
        rm_db = XrmGetStringDatabase(rm_str);
        XtFree(rm_str);
        if (rm_db != NULL) {
            if (lookup_appearance_resource(display, rm_db, APPEARANCE_RESOURCE,
                                           APPEARANCE_CLASS,
                                           XmNappearanceProfile,
                                           XmCAppearanceProfile, &val) &&
                val.addr != NULL)
                (void)profile_from_name(val.addr, &target_prof);
            val.addr = NULL;
            if (lookup_appearance_resource(display, rm_db, DENSITY_RESOURCE,
                                           DENSITY_CLASS,
                                           XmNappearanceDensity,
                                           XmCAppearanceDensity, &val) &&
                val.addr != NULL)
                (void)density_from_name(val.addr, &target_dens);
        }
    } else {
        target_prof = cur_prof;
        target_dens = cur_dens;
    }

    app_db = XrmGetDatabase(display);
    if (app_db != NULL && rm_db != NULL) {
        appearance_sync_token_resource(display, rm_db, app_db,
                                       SURFACE_BACKGROUND_RESOURCE,
                                       SURFACE_BACKGROUND_CLASS,
                                       XmNsurfaceBackground,
                                       XmCSurfaceBackground);
        appearance_sync_token_resource(display, rm_db, app_db,
                                       ACCENT_COLOR_RESOURCE,
                                       ACCENT_COLOR_CLASS,
                                       XmNaccentColor, XmCAccentColor);
        appearance_sync_token_resource(display, rm_db, app_db,
                                       ACCENT_FOREGROUND_RESOURCE,
                                       ACCENT_FOREGROUND_CLASS,
                                       XmNaccentForeground,
                                       XmCAccentForeground);
        appearance_sync_token_resource(display, rm_db, app_db,
                                       DANGER_COLOR_RESOURCE,
                                       DANGER_COLOR_CLASS,
                                       XmNdangerColor, XmCDangerColor);
        XrmPutStringResource(&app_db, APPEARANCE_RESOURCE,
                             profile_name(target_prof));
        XrmPutStringResource(&app_db, "*appearanceProfile",
                             profile_name(target_prof));
        XrmPutStringResource(&app_db, DENSITY_RESOURCE,
                             target_dens == XmAPPEARANCE_DENSITY_COMPACT
                                 ? "compact"
                                 : "comfortable");
        XrmPutStringResource(&app_db, "*appearanceDensity",
                             target_dens == XmAPPEARANCE_DENSITY_COMPACT
                                 ? "compact"
                                 : "comfortable");
    }
    if (rm_db != NULL)
        XrmDestroyDatabase(rm_db);

    /* Preserve cur_prof / cur_dens as the currently applied baseline while
     * apply_appearance_settings evaluates transitions against app_db. */
    state->profile_api_selected = True;
    state->api_profile = cur_prof;
    state->density_api_selected = True;
    state->api_density = cur_dens;

    memset(old_sets, 0, sizeof(old_sets));
    memset(new_sets, 0, sizeof(new_sets));
    palette_changed = _XmColorObjRefreshDisplay(display, old_sets, new_sets);
    if (palette_changed && target_prof != XmAPPEARANCE_CLASSIC) {
        state->palette.valid = False;
    }
    will_apply_appearance =
        (cur_prof != target_prof || cur_dens != target_dens ||
         (palette_changed && target_prof != XmAPPEARANCE_CLASSIC) ||
         semantic_palette_dirty(display, state, target_prof));

    if (palette_changed) {
        XmColorObj co = _XmColorObjForDisplay(display);
        appearance_recolor_colorobj_widgets(display, state, co,
                                            old_sets, new_sets, target_prof,
                                            !will_apply_appearance);
    }

    if (cur_dens != target_dens && cur_prof == XmAPPEARANCE_CLASSIC &&
        target_prof == XmAPPEARANCE_CLASSIC) {
        (void)apply_appearance_settings(display, cur_prof, target_dens, False);
    }
    if (will_apply_appearance) {
        (void)apply_appearance_settings(display, target_prof, target_dens, True);
    }

    state->profile_api_selected = False;
    state->density_api_selected = False;
}

static void
appearance_observe_property_notify(XmAppearanceDisplayState *state,
                                   const XPropertyEvent *ev)
{
    Display *dpy;
    Window cust_owner;
    Atom pset_atom;
    Boolean match = False;

    if (state == NULL || ev == NULL || state->live_watch_disabled ||
        state->live_app_context == NULL || ev->state != PropertyNewValue)
        return;
    dpy = state->display;
    if (ev->window == DefaultRootWindow(dpy) &&
        ev->atom == XA_RESOURCE_MANAGER) {
        match = True;
    } else {
        pset_atom = XInternAtom(dpy, XmSPIXEL_SET_PROP, True);
        if (pset_atom != None && ev->atom == pset_atom) {
            cust_owner = _XmColorObjSelectionOwner(dpy);
            if (cust_owner != None && ev->window == cust_owner)
                match = True;
        }
    }
    if (match && state->live_sync_timer_id == 0) {
        state->live_sync_timer_id = XtAppAddTimeOut(
            state->live_app_context, 0UL, appearance_live_sync_timer,
            (XtPointer)dpy);
    }
}

static Boolean
appearance_dispatch_property_notify(XEvent *event)
{
    Display *dpy;
    XmAppearanceDisplayState *state;
    XtEventDispatchProc prev;
    Boolean dispatched;

    if (event == NULL)
        return False;
    dpy = event->xany.display;
    state = find_display_state(dpy);
    if (state == NULL)
        return False;
    if (event->type == PropertyNotify)
        appearance_observe_property_notify(state, &event->xproperty);

    prev = state->prev_prop_dispatcher;
    (void)XtSetEventDispatcher(dpy, PropertyNotify, prev);
    dispatched = XtDispatchEvent(event);
    if (find_display_state(dpy) == state && state->live_watch_installed)
        (void)XtSetEventDispatcher(dpy, PropertyNotify,
                                   appearance_dispatch_property_notify);
    return dispatched;
}

void
_XmAppearanceInstallLiveWatch(Widget shell)
{
    Display *display;
    XmAppearanceDisplayState *state;
    XrmDatabase db;
    XrmValue val = {0, NULL};
    Window cust_owner;

    if (shell == NULL || !XtIsRealized(shell))
        return;
    display = XtDisplay(shell);
    if (display == NULL)
        return;
    state = ensure_display_state(display);
    if (state == NULL || state->live_watch_installed ||
        state->live_watch_disabled)
        return;

    db = XrmGetDatabase(display);
    if (db != NULL &&
        lookup_appearance_resource(display, db,
                                   "ckCoreMotif.liveAppearanceSync",
                                   "CkCoreMotif.LiveAppearanceSync",
                                   "liveAppearanceSync",
                                   "LiveAppearanceSync", &val) &&
        val.addr != NULL &&
        (strcmp(val.addr, "False") == 0 || strcmp(val.addr, "false") == 0 ||
         strcmp(val.addr, "0") == 0)) {
        state->live_watch_disabled = True;
        return;
    }

    state->live_app_context = XtWidgetToApplicationContext(shell);
    appearance_safe_select_property_mask(display, DefaultRootWindow(display));
    cust_owner = _XmColorObjSelectionOwner(display);
    if (cust_owner != None)
        appearance_safe_select_property_mask(display, cust_owner);

    state->prev_prop_dispatcher = XtSetEventDispatcher(
        display, PropertyNotify, appearance_dispatch_property_notify);
    state->live_watch_installed = True;
}
