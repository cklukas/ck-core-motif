/* Generated; do not edit. */
#ifndef CKMOTIF_GALLERY_MANIFEST_H
#define CKMOTIF_GALLERY_MANIFEST_H

#ifdef PRINTING_SUPPORTED
#define CKMOTIF_GALLERY_PRINT_CLASS xmPrintShellWidgetClass
#else
#define CKMOTIF_GALLERY_PRINT_CLASS NULL
#endif

typedef struct {
    const char *id;
    const char *label;
    const char *category;
    const char *class_symbol;
    const char *kind;
    WidgetClass class;
    int popup_shell;
    const char *source_fragment;
    const char *interaction;
    const char *docs_anchor;
    const char *screenshot;
    int constructible;
} GalleryManifestEntry;

static GalleryManifestEntry gallery_manifest[] = {
    {"xm.buttonboxwidgetclass", "buttonboxwidgetclass", "layout", "xmButtonBoxWidgetClass", "widget", NULL, 0, "gallery.source.buttonboxwidgetclass", "gallery.interaction.buttonboxwidgetclass", "gallery.component.buttonboxwidgetclass", "gallery.screenshot.buttonboxwidgetclass", 1},
    {"xm.colorselectorwidgetclass", "colorselectorwidgetclass", "feedback-dialogs", "xmColorSelectorWidgetClass", "widget", NULL, 0, "gallery.source.colorselectorwidgetclass", "gallery.interaction.colorselectorwidgetclass", "gallery.component.colorselectorwidgetclass", "gallery.screenshot.colorselectorwidgetclass", 1},
    {"xm.combinationbox2widgetclass", "combinationbox2widgetclass", "commands-navigation", "xmCombinationBox2WidgetClass", "widget", NULL, 0, "gallery.source.combinationbox2widgetclass", "gallery.interaction.combinationbox2widgetclass", "gallery.component.combinationbox2widgetclass", "gallery.screenshot.combinationbox2widgetclass", 1},
    {"xm.disclosurewidgetclass", "disclosurewidgetclass", "commands-navigation", "xmDisclosureWidgetClass", "widget", NULL, 0, "gallery.source.disclosurewidgetclass", "gallery.interaction.disclosurewidgetclass", "gallery.component.disclosurewidgetclass", "gallery.screenshot.disclosurewidgetclass", 1},
    {"xm.fontselectorwidgetclass", "fontselectorwidgetclass", "text-input", "xmFontSelectorWidgetClass", "widget", NULL, 0, "gallery.source.fontselectorwidgetclass", "gallery.interaction.fontselectorwidgetclass", "gallery.component.fontselectorwidgetclass", "gallery.screenshot.fontselectorwidgetclass", 1},
    {"xm.hierarchywidgetclass", "hierarchywidgetclass", "data-views", "xmHierarchyWidgetClass", "base", NULL, 0, "gallery.source.hierarchywidgetclass", "gallery.interaction.hierarchywidgetclass", "gallery.component.hierarchywidgetclass", "gallery.screenshot.hierarchywidgetclass", 0},
    {"xm.ext18listwidgetclass", "ext18listwidgetclass", "data-views", "xmExt18ListWidgetClass", "widget", NULL, 0, "gallery.source.ext18listwidgetclass", "gallery.interaction.ext18listwidgetclass", "gallery.component.ext18listwidgetclass", "gallery.screenshot.ext18listwidgetclass", 1},
    {"xm.multilistwidgetclass", "multilistwidgetclass", "data-views", "xmMultiListWidgetClass", "widget", NULL, 0, "gallery.source.multilistwidgetclass", "gallery.interaction.multilistwidgetclass", "gallery.component.multilistwidgetclass", "gallery.screenshot.multilistwidgetclass", 1},
    {"xm.iconboxwidgetclass", "iconboxwidgetclass", "data-views", "xmIconBoxWidgetClass", "widget", NULL, 0, "gallery.source.iconboxwidgetclass", "gallery.interaction.iconboxwidgetclass", "gallery.component.iconboxwidgetclass", "gallery.screenshot.iconboxwidgetclass", 1},
    {"xm.outlinewidgetclass", "outlinewidgetclass", "data-views", "xmOutlineWidgetClass", "widget", NULL, 0, "gallery.source.outlinewidgetclass", "gallery.interaction.outlinewidgetclass", "gallery.component.outlinewidgetclass", "gallery.screenshot.outlinewidgetclass", 1},
    {"xm.treewidgetclass", "treewidgetclass", "data-views", "xmTreeWidgetClass", "widget", NULL, 0, "gallery.source.treewidgetclass", "gallery.interaction.treewidgetclass", "gallery.component.treewidgetclass", "gallery.screenshot.treewidgetclass", 1},
    {"xm.arrowbuttonwidgetclass", "arrowbuttonwidgetclass", "buttons-selection", "xmArrowButtonWidgetClass", "widget", NULL, 0, "gallery.source.arrowbuttonwidgetclass", "gallery.interaction.arrowbuttonwidgetclass", "gallery.component.arrowbuttonwidgetclass", "gallery.screenshot.arrowbuttonwidgetclass", 1},
    {"xm.arrowbuttongadgetclass", "arrowbuttongadgetclass", "buttons-selection", "xmArrowButtonGadgetClass", "gadget", NULL, 0, "gallery.source.arrowbuttongadgetclass", "gallery.interaction.arrowbuttongadgetclass", "gallery.component.arrowbuttongadgetclass", "gallery.screenshot.arrowbuttongadgetclass", 1},
    {"xm.bulletinboardwidgetclass", "bulletinboardwidgetclass", "layout", "xmBulletinBoardWidgetClass", "widget", NULL, 0, "gallery.source.bulletinboardwidgetclass", "gallery.interaction.bulletinboardwidgetclass", "gallery.component.bulletinboardwidgetclass", "gallery.screenshot.bulletinboardwidgetclass", 1},
    {"xm.cascadebuttonwidgetclass", "cascadebuttonwidgetclass", "commands-navigation", "xmCascadeButtonWidgetClass", "widget", NULL, 0, "gallery.source.cascadebuttonwidgetclass", "gallery.interaction.cascadebuttonwidgetclass", "gallery.component.cascadebuttonwidgetclass", "gallery.screenshot.cascadebuttonwidgetclass", 1},
    {"xm.cascadebuttongadgetclass", "cascadebuttongadgetclass", "commands-navigation", "xmCascadeButtonGadgetClass", "gadget", NULL, 0, "gallery.source.cascadebuttongadgetclass", "gallery.interaction.cascadebuttongadgetclass", "gallery.component.cascadebuttongadgetclass", "gallery.screenshot.cascadebuttongadgetclass", 1},
    {"xm.columnwidgetclass", "columnwidgetclass", "layout", "xmColumnWidgetClass", "widget", NULL, 0, "gallery.source.columnwidgetclass", "gallery.interaction.columnwidgetclass", "gallery.component.columnwidgetclass", "gallery.screenshot.columnwidgetclass", 1},
    {"xm.comboboxwidgetclass", "comboboxwidgetclass", "commands-navigation", "xmComboBoxWidgetClass", "widget", NULL, 0, "gallery.source.comboboxwidgetclass", "gallery.interaction.comboboxwidgetclass", "gallery.component.comboboxwidgetclass", "gallery.screenshot.comboboxwidgetclass", 1},
    {"xm.commandwidgetclass", "commandwidgetclass", "commands-navigation", "xmCommandWidgetClass", "widget", NULL, 0, "gallery.source.commandwidgetclass", "gallery.interaction.commandwidgetclass", "gallery.component.commandwidgetclass", "gallery.screenshot.commandwidgetclass", 1},
    {"xm.containerwidgetclass", "containerwidgetclass", "layout", "xmContainerWidgetClass", "widget", NULL, 0, "gallery.source.containerwidgetclass", "gallery.interaction.containerwidgetclass", "gallery.component.containerwidgetclass", "gallery.screenshot.containerwidgetclass", 1},
    {"xm.datafieldwidgetclass", "datafieldwidgetclass", "text-input", "xmDataFieldWidgetClass", "widget", NULL, 0, "gallery.source.datafieldwidgetclass", "gallery.interaction.datafieldwidgetclass", "gallery.component.datafieldwidgetclass", "gallery.screenshot.datafieldwidgetclass", 1},
    {"xm.dialogshellwidgetclass", "dialogshellwidgetclass", "commands-navigation", "xmDialogShellWidgetClass", "widget", NULL, 1, "gallery.source.dialogshellwidgetclass", "gallery.interaction.dialogshellwidgetclass", "gallery.component.dialogshellwidgetclass", "gallery.screenshot.dialogshellwidgetclass", 1},
    {"xm.displayclass", "displayclass", "layout", "xmDisplayClass", "base", NULL, 0, "gallery.source.displayclass", "gallery.interaction.displayclass", "gallery.component.displayclass", "gallery.screenshot.displayclass", 0},
    {"xm.dragcontextclass", "dragcontextclass", "layout", "xmDragContextClass", "object", NULL, 0, "gallery.source.dragcontextclass", "gallery.interaction.dragcontextclass", "gallery.component.dragcontextclass", "gallery.screenshot.dragcontextclass", 0},
    {"xm.dragiconobjectclass", "dragiconobjectclass", "layout", "xmDragIconObjectClass", "object", NULL, 0, "gallery.source.dragiconobjectclass", "gallery.interaction.dragiconobjectclass", "gallery.component.dragiconobjectclass", "gallery.screenshot.dragiconobjectclass", 0},
    {"xm.dragovershellwidgetclass", "dragovershellwidgetclass", "layout", "xmDragOverShellWidgetClass", "widget", NULL, 1, "gallery.source.dragovershellwidgetclass", "gallery.interaction.dragovershellwidgetclass", "gallery.component.dragovershellwidgetclass", "gallery.screenshot.dragovershellwidgetclass", 1},
    {"xm.drawingareawidgetclass", "drawingareawidgetclass", "media", "xmDrawingAreaWidgetClass", "widget", NULL, 0, "gallery.source.drawingareawidgetclass", "gallery.interaction.drawingareawidgetclass", "gallery.component.drawingareawidgetclass", "gallery.screenshot.drawingareawidgetclass", 1},
    {"xm.drawnbuttonwidgetclass", "drawnbuttonwidgetclass", "layout", "xmDrawnButtonWidgetClass", "widget", NULL, 0, "gallery.source.drawnbuttonwidgetclass", "gallery.interaction.drawnbuttonwidgetclass", "gallery.component.drawnbuttonwidgetclass", "gallery.screenshot.drawnbuttonwidgetclass", 1},
    {"xm.dropsitemanagerobjectclass", "dropsitemanagerobjectclass", "media", "xmDropSiteManagerObjectClass", "object", NULL, 0, "gallery.source.dropsitemanagerobjectclass", "gallery.interaction.dropsitemanagerobjectclass", "gallery.component.dropsitemanagerobjectclass", "gallery.screenshot.dropsitemanagerobjectclass", 0},
    {"xm.droptransferobjectclass", "droptransferobjectclass", "media", "xmDropTransferObjectClass", "object", NULL, 0, "gallery.source.droptransferobjectclass", "gallery.interaction.droptransferobjectclass", "gallery.component.droptransferobjectclass", "gallery.screenshot.droptransferobjectclass", 0},
    {"xm.fileselectionboxwidgetclass", "fileselectionboxwidgetclass", "commands-navigation", "xmFileSelectionBoxWidgetClass", "widget", NULL, 0, "gallery.source.fileselectionboxwidgetclass", "gallery.interaction.fileselectionboxwidgetclass", "gallery.component.fileselectionboxwidgetclass", "gallery.screenshot.fileselectionboxwidgetclass", 1},
    {"xm.formwidgetclass", "formwidgetclass", "layout", "xmFormWidgetClass", "widget", NULL, 0, "gallery.source.formwidgetclass", "gallery.interaction.formwidgetclass", "gallery.component.formwidgetclass", "gallery.screenshot.formwidgetclass", 1},
    {"xm.framewidgetclass", "framewidgetclass", "layout", "xmFrameWidgetClass", "widget", NULL, 0, "gallery.source.framewidgetclass", "gallery.interaction.framewidgetclass", "gallery.component.framewidgetclass", "gallery.screenshot.framewidgetclass", 1},
    {"xm.gadgetclass", "gadgetclass", "layout", "xmGadgetClass", "base", NULL, 0, "gallery.source.gadgetclass", "gallery.interaction.gadgetclass", "gallery.component.gadgetclass", "gallery.screenshot.gadgetclass", 0},
    {"xm.grabshellwidgetclass", "grabshellwidgetclass", "commands-navigation", "xmGrabShellWidgetClass", "widget", NULL, 1, "gallery.source.grabshellwidgetclass", "gallery.interaction.grabshellwidgetclass", "gallery.component.grabshellwidgetclass", "gallery.screenshot.grabshellwidgetclass", 1},
    {"xm.icongadgetclass", "icongadgetclass", "media", "xmIconGadgetClass", "gadget", NULL, 0, "gallery.source.icongadgetclass", "gallery.interaction.icongadgetclass", "gallery.component.icongadgetclass", "gallery.screenshot.icongadgetclass", 1},
    {"xm.iconheaderclass", "iconheaderclass", "media", "xmIconHeaderClass", "widget", NULL, 0, "gallery.source.iconheaderclass", "gallery.interaction.iconheaderclass", "gallery.component.iconheaderclass", "gallery.screenshot.iconheaderclass", 1},
    {"xm.iconbuttonwidgetclass", "iconbuttonwidgetclass", "buttons-selection", "xmIconButtonWidgetClass", "widget", NULL, 0, "gallery.source.iconbuttonwidgetclass", "gallery.interaction.iconbuttonwidgetclass", "gallery.component.iconbuttonwidgetclass", "gallery.screenshot.iconbuttonwidgetclass", 1},
    {"xm.labelwidgetclass", "labelwidgetclass", "text-input", "xmLabelWidgetClass", "widget", NULL, 0, "gallery.source.labelwidgetclass", "gallery.interaction.labelwidgetclass", "gallery.component.labelwidgetclass", "gallery.screenshot.labelwidgetclass", 1},
    {"xm.labelgadgetclass", "labelgadgetclass", "text-input", "xmLabelGadgetClass", "gadget", NULL, 0, "gallery.source.labelgadgetclass", "gallery.interaction.labelgadgetclass", "gallery.component.labelgadgetclass", "gallery.screenshot.labelgadgetclass", 1},
    {"xm.listwidgetclass", "listwidgetclass", "buttons-selection", "xmListWidgetClass", "widget", NULL, 0, "gallery.source.listwidgetclass", "gallery.interaction.listwidgetclass", "gallery.component.listwidgetclass", "gallery.screenshot.listwidgetclass", 1},
    {"xm.mainwindowwidgetclass", "mainwindowwidgetclass", "layout", "xmMainWindowWidgetClass", "widget", NULL, 0, "gallery.source.mainwindowwidgetclass", "gallery.interaction.mainwindowwidgetclass", "gallery.component.mainwindowwidgetclass", "gallery.screenshot.mainwindowwidgetclass", 1},
    {"xm.managerwidgetclass", "managerwidgetclass", "layout", "xmManagerWidgetClass", "base", NULL, 0, "gallery.source.managerwidgetclass", "gallery.interaction.managerwidgetclass", "gallery.component.managerwidgetclass", "gallery.screenshot.managerwidgetclass", 0},
    {"xm.menushellwidgetclass", "menushellwidgetclass", "commands-navigation", "xmMenuShellWidgetClass", "widget", NULL, 1, "gallery.source.menushellwidgetclass", "gallery.interaction.menushellwidgetclass", "gallery.component.menushellwidgetclass", "gallery.screenshot.menushellwidgetclass", 1},
    {"xm.messageboxwidgetclass", "messageboxwidgetclass", "commands-navigation", "xmMessageBoxWidgetClass", "widget", NULL, 0, "gallery.source.messageboxwidgetclass", "gallery.interaction.messageboxwidgetclass", "gallery.component.messageboxwidgetclass", "gallery.screenshot.messageboxwidgetclass", 1},
    {"xm.colorpickerwidgetclass", "colorpickerwidgetclass", "buttons-selection", "xmColorPickerWidgetClass", "widget", NULL, 0, "gallery.source.colorpickerwidgetclass", "gallery.interaction.colorpickerwidgetclass", "gallery.component.colorpickerwidgetclass", "gallery.screenshot.colorpickerwidgetclass", 1},
    {"xm.flyoutwidgetclass", "flyoutwidgetclass", "commands-navigation", "xmFlyoutWidgetClass", "widget", NULL, 0, "gallery.source.flyoutwidgetclass", "gallery.interaction.flyoutwidgetclass", "gallery.component.flyoutwidgetclass", "gallery.screenshot.flyoutwidgetclass", 1},
    {"xm.teachingtipwidgetclass", "teachingtipwidgetclass", "commands-navigation", "xmTeachingTipWidgetClass", "widget", NULL, 0, "gallery.source.teachingtipwidgetclass", "gallery.interaction.teachingtipwidgetclass", "gallery.component.teachingtipwidgetclass", "gallery.screenshot.teachingtipwidgetclass", 1},
    {"xm.infobarwidgetclass", "infobarwidgetclass", "feedback-dialogs", "xmInfoBarWidgetClass", "widget", NULL, 0, "gallery.source.infobarwidgetclass", "gallery.interaction.infobarwidgetclass", "gallery.component.infobarwidgetclass", "gallery.screenshot.infobarwidgetclass", 1},
    {"xm.breadcrumbbarwidgetclass", "breadcrumbbarwidgetclass", "commands-navigation", "xmBreadcrumbBarWidgetClass", "widget", NULL, 0, "gallery.source.breadcrumbbarwidgetclass", "gallery.interaction.breadcrumbbarwidgetclass", "gallery.component.breadcrumbbarwidgetclass", "gallery.screenshot.breadcrumbbarwidgetclass", 1},
    {"xm.treeviewwidgetclass", "treeviewwidgetclass", "layout", "xmTreeViewWidgetClass", "widget", NULL, 0, "gallery.source.treeviewwidgetclass", "gallery.interaction.treeviewwidgetclass", "gallery.component.treeviewwidgetclass", "gallery.screenshot.treeviewwidgetclass", 1},
    {"xm.calendarwidgetclass", "calendarwidgetclass", "data-views", "xmCalendarWidgetClass", "widget", NULL, 0, "gallery.source.calendarwidgetclass", "gallery.interaction.calendarwidgetclass", "gallery.component.calendarwidgetclass", "gallery.screenshot.calendarwidgetclass", 1},
    {"xm.datepickerwidgetclass", "datepickerwidgetclass", "data-views", "xmDatePickerWidgetClass", "widget", NULL, 0, "gallery.source.datepickerwidgetclass", "gallery.interaction.datepickerwidgetclass", "gallery.component.datepickerwidgetclass", "gallery.screenshot.datepickerwidgetclass", 1},
    {"xm.timepickerwidgetclass", "timepickerwidgetclass", "data-views", "xmTimePickerWidgetClass", "widget", NULL, 0, "gallery.source.timepickerwidgetclass", "gallery.interaction.timepickerwidgetclass", "gallery.component.timepickerwidgetclass", "gallery.screenshot.timepickerwidgetclass", 1},
    {"xm.datagridwidgetclass", "datagridwidgetclass", "data-views", "xmDataGridWidgetClass", "widget", NULL, 0, "gallery.source.datagridwidgetclass", "gallery.interaction.datagridwidgetclass", "gallery.component.datagridwidgetclass", "gallery.screenshot.datagridwidgetclass", 1},
    {"xm.segmentedcontrolwidgetclass", "segmentedcontrolwidgetclass", "buttons-selection", "xmSegmentedControlWidgetClass", "widget", NULL, 0, "gallery.source.segmentedcontrolwidgetclass", "gallery.interaction.segmentedcontrolwidgetclass", "gallery.component.segmentedcontrolwidgetclass", "gallery.screenshot.segmentedcontrolwidgetclass", 1},
    {"xm.chipwidgetclass", "chipwidgetclass", "feedback-dialogs", "xmChipWidgetClass", "widget", NULL, 0, "gallery.source.chipwidgetclass", "gallery.interaction.chipwidgetclass", "gallery.component.chipwidgetclass", "gallery.screenshot.chipwidgetclass", 1},
    {"xm.badgewidgetclass", "badgewidgetclass", "feedback-dialogs", "xmBadgeWidgetClass", "widget", NULL, 0, "gallery.source.badgewidgetclass", "gallery.interaction.badgewidgetclass", "gallery.component.badgewidgetclass", "gallery.screenshot.badgewidgetclass", 1},
    {"xm.iconlabelwidgetclass", "iconlabelwidgetclass", "text-input", "xmIconLabelWidgetClass", "widget", NULL, 0, "gallery.source.iconlabelwidgetclass", "gallery.interaction.iconlabelwidgetclass", "gallery.component.iconlabelwidgetclass", "gallery.screenshot.iconlabelwidgetclass", 1},
    {"xm.dividerwidgetclass", "dividerwidgetclass", "layout", "xmDividerWidgetClass", "widget", NULL, 0, "gallery.source.dividerwidgetclass", "gallery.interaction.dividerwidgetclass", "gallery.component.dividerwidgetclass", "gallery.screenshot.dividerwidgetclass", 1},
    {"xm.avatarwidgetclass", "avatarwidgetclass", "feedback-dialogs", "xmAvatarWidgetClass", "widget", NULL, 0, "gallery.source.avatarwidgetclass", "gallery.interaction.avatarwidgetclass", "gallery.component.avatarwidgetclass", "gallery.screenshot.avatarwidgetclass", 1},
    {"xm.linkwidgetclass", "linkwidgetclass", "commands-navigation", "xmLinkWidgetClass", "widget", NULL, 0, "gallery.source.linkwidgetclass", "gallery.interaction.linkwidgetclass", "gallery.component.linkwidgetclass", "gallery.screenshot.linkwidgetclass", 1},
    {"xm.searchboxwidgetclass", "searchboxwidgetclass", "text-input", "xmSearchBoxWidgetClass", "widget", NULL, 0, "gallery.source.searchboxwidgetclass", "gallery.interaction.searchboxwidgetclass", "gallery.component.searchboxwidgetclass", "gallery.screenshot.searchboxwidgetclass", 1},
    {"xm.promptwidgetclass", "promptwidgetclass", "text-input", "xmPromptWidgetClass", "widget", NULL, 0, "gallery.source.promptwidgetclass", "gallery.interaction.promptwidgetclass", "gallery.component.promptwidgetclass", "gallery.screenshot.promptwidgetclass", 1},
    {"xm.formsectionwidgetclass", "formsectionwidgetclass", "layout", "xmFormSectionWidgetClass", "widget", NULL, 0, "gallery.source.formsectionwidgetclass", "gallery.interaction.formsectionwidgetclass", "gallery.component.formsectionwidgetclass", "gallery.screenshot.formsectionwidgetclass", 1},
    {"xm.emptystatewidgetclass", "emptystatewidgetclass", "feedback-dialogs", "xmEmptyStateWidgetClass", "widget", NULL, 0, "gallery.source.emptystatewidgetclass", "gallery.interaction.emptystatewidgetclass", "gallery.component.emptystatewidgetclass", "gallery.screenshot.emptystatewidgetclass", 1},
    {"xm.toastwidgetclass", "toastwidgetclass", "commands-navigation", "xmToastWidgetClass", "widget", NULL, 0, "gallery.source.toastwidgetclass", "gallery.interaction.toastwidgetclass", "gallery.component.toastwidgetclass", "gallery.screenshot.toastwidgetclass", 1},
    {"xm.commandpalettewidgetclass", "commandpalettewidgetclass", "commands-navigation", "xmCommandPaletteWidgetClass", "widget", NULL, 0, "gallery.source.commandpalettewidgetclass", "gallery.interaction.commandpalettewidgetclass", "gallery.component.commandpalettewidgetclass", "gallery.screenshot.commandpalettewidgetclass", 1},
    {"xm.skeletonwidgetclass", "skeletonwidgetclass", "feedback-dialogs", "xmSkeletonWidgetClass", "widget", NULL, 0, "gallery.source.skeletonwidgetclass", "gallery.interaction.skeletonwidgetclass", "gallery.component.skeletonwidgetclass", "gallery.screenshot.skeletonwidgetclass", 1},
    {"xm.rowactionswidgetclass", "rowactionswidgetclass", "layout", "xmRowActionsWidgetClass", "widget", NULL, 0, "gallery.source.rowactionswidgetclass", "gallery.interaction.rowactionswidgetclass", "gallery.component.rowactionswidgetclass", "gallery.screenshot.rowactionswidgetclass", 1},
    {"xm.drophighlightwidgetclass", "drophighlightwidgetclass", "media", "xmDropHighlightWidgetClass", "widget", NULL, 0, "gallery.source.drophighlightwidgetclass", "gallery.interaction.drophighlightwidgetclass", "gallery.component.drophighlightwidgetclass", "gallery.screenshot.drophighlightwidgetclass", 1},
    {"xm.statusbarwidgetclass", "statusbarwidgetclass", "layout", "xmStatusBarWidgetClass", "widget", NULL, 0, "gallery.source.statusbarwidgetclass", "gallery.interaction.statusbarwidgetclass", "gallery.component.statusbarwidgetclass", "gallery.screenshot.statusbarwidgetclass", 1},
    {"xm.toolbarwidgetclass", "toolbarwidgetclass", "layout", "xmToolbarWidgetClass", "widget", NULL, 0, "gallery.source.toolbarwidgetclass", "gallery.interaction.toolbarwidgetclass", "gallery.component.toolbarwidgetclass", "gallery.screenshot.toolbarwidgetclass", 1},
    {"xm.sidebarwidgetclass", "sidebarwidgetclass", "layout", "xmSidebarWidgetClass", "widget", NULL, 0, "gallery.source.sidebarwidgetclass", "gallery.interaction.sidebarwidgetclass", "gallery.component.sidebarwidgetclass", "gallery.screenshot.sidebarwidgetclass", 1},
    {"xm.autosuggestboxwidgetclass", "autosuggestboxwidgetclass", "text-input", "xmAutoSuggestBoxWidgetClass", "widget", NULL, 0, "gallery.source.autosuggestboxwidgetclass", "gallery.interaction.autosuggestboxwidgetclass", "gallery.component.autosuggestboxwidgetclass", "gallery.screenshot.autosuggestboxwidgetclass", 1},
    {"xm.numberboxwidgetclass", "numberboxwidgetclass", "feedback-dialogs", "xmNumberBoxWidgetClass", "widget", NULL, 0, "gallery.source.numberboxwidgetclass", "gallery.interaction.numberboxwidgetclass", "gallery.component.numberboxwidgetclass", "gallery.screenshot.numberboxwidgetclass", 1},
    {"xm.ratingcontrolwidgetclass", "ratingcontrolwidgetclass", "buttons-selection", "xmRatingControlWidgetClass", "widget", NULL, 0, "gallery.source.ratingcontrolwidgetclass", "gallery.interaction.ratingcontrolwidgetclass", "gallery.component.ratingcontrolwidgetclass", "gallery.screenshot.ratingcontrolwidgetclass", 1},
    {"xm.notebookwidgetclass", "notebookwidgetclass", "layout", "xmNotebookWidgetClass", "widget", NULL, 0, "gallery.source.notebookwidgetclass", "gallery.interaction.notebookwidgetclass", "gallery.component.notebookwidgetclass", "gallery.screenshot.notebookwidgetclass", 1},
    {"xm.panedwindowwidgetclass", "panedwindowwidgetclass", "layout", "xmPanedWindowWidgetClass", "widget", NULL, 0, "gallery.source.panedwindowwidgetclass", "gallery.interaction.panedwindowwidgetclass", "gallery.component.panedwindowwidgetclass", "gallery.screenshot.panedwindowwidgetclass", 1},
    {"xm.panedwidgetclass", "panedwidgetclass", "layout", "xmPanedWidgetClass", "widget", NULL, 0, "gallery.source.panedwidgetclass", "gallery.interaction.panedwidgetclass", "gallery.component.panedwidgetclass", "gallery.screenshot.panedwidgetclass", 1},
    {"xm.primitivewidgetclass", "primitivewidgetclass", "layout", "xmPrimitiveWidgetClass", "base", NULL, 0, "gallery.source.primitivewidgetclass", "gallery.interaction.primitivewidgetclass", "gallery.component.primitivewidgetclass", "gallery.screenshot.primitivewidgetclass", 0},
    {"xm.printshellwidgetclass", "printshellwidgetclass", "layout", "xmPrintShellWidgetClass", "widget", NULL, 1, "gallery.source.printshellwidgetclass", "gallery.interaction.printshellwidgetclass", "gallery.component.printshellwidgetclass", "gallery.screenshot.printshellwidgetclass", 1},
    {"xm.pushbuttonwidgetclass", "pushbuttonwidgetclass", "buttons-selection", "xmPushButtonWidgetClass", "widget", NULL, 0, "gallery.source.pushbuttonwidgetclass", "gallery.interaction.pushbuttonwidgetclass", "gallery.component.pushbuttonwidgetclass", "gallery.screenshot.pushbuttonwidgetclass", 1},
    {"xm.progressbarwidgetclass", "progressbarwidgetclass", "feedback-dialogs", "xmProgressBarWidgetClass", "widget", NULL, 0, "gallery.source.progressbarwidgetclass", "gallery.interaction.progressbarwidgetclass", "gallery.component.progressbarwidgetclass", "gallery.screenshot.progressbarwidgetclass", 1},
    {"xm.progressringwidgetclass", "progressringwidgetclass", "feedback-dialogs", "xmProgressRingWidgetClass", "widget", NULL, 0, "gallery.source.progressringwidgetclass", "gallery.interaction.progressringwidgetclass", "gallery.component.progressringwidgetclass", "gallery.screenshot.progressringwidgetclass", 1},
    {"xm.pushbuttongadgetclass", "pushbuttongadgetclass", "buttons-selection", "xmPushButtonGadgetClass", "gadget", NULL, 0, "gallery.source.pushbuttongadgetclass", "gallery.interaction.pushbuttongadgetclass", "gallery.component.pushbuttongadgetclass", "gallery.screenshot.pushbuttongadgetclass", 1},
    {"xm.rowcolumnwidgetclass", "rowcolumnwidgetclass", "layout", "xmRowColumnWidgetClass", "widget", NULL, 0, "gallery.source.rowcolumnwidgetclass", "gallery.interaction.rowcolumnwidgetclass", "gallery.component.rowcolumnwidgetclass", "gallery.screenshot.rowcolumnwidgetclass", 1},
    {"xm.simplespinboxwidgetclass", "simplespinboxwidgetclass", "buttons-selection", "xmSimpleSpinBoxWidgetClass", "widget", NULL, 0, "gallery.source.simplespinboxwidgetclass", "gallery.interaction.simplespinboxwidgetclass", "gallery.component.simplespinboxwidgetclass", "gallery.screenshot.simplespinboxwidgetclass", 1},
    {"xm.scalewidgetclass", "scalewidgetclass", "buttons-selection", "xmScaleWidgetClass", "widget", NULL, 0, "gallery.source.scalewidgetclass", "gallery.interaction.scalewidgetclass", "gallery.component.scalewidgetclass", "gallery.screenshot.scalewidgetclass", 1},
    {"xm.spinnerwidgetclass", "spinnerwidgetclass", "buttons-selection", "xmSpinnerWidgetClass", "widget", NULL, 0, "gallery.source.spinnerwidgetclass", "gallery.interaction.spinnerwidgetclass", "gallery.component.spinnerwidgetclass", "gallery.screenshot.spinnerwidgetclass", 1},
    {"xm.stepperwidgetclass", "stepperwidgetclass", "buttons-selection", "xmStepperWidgetClass", "widget", NULL, 0, "gallery.source.stepperwidgetclass", "gallery.interaction.stepperwidgetclass", "gallery.component.stepperwidgetclass", "gallery.screenshot.stepperwidgetclass", 1},
    {"xm.screenclass", "screenclass", "layout", "xmScreenClass", "base", NULL, 0, "gallery.source.screenclass", "gallery.interaction.screenclass", "gallery.component.screenclass", "gallery.screenshot.screenclass", 0},
    {"xm.scrollbarwidgetclass", "scrollbarwidgetclass", "buttons-selection", "xmScrollBarWidgetClass", "widget", NULL, 0, "gallery.source.scrollbarwidgetclass", "gallery.interaction.scrollbarwidgetclass", "gallery.component.scrollbarwidgetclass", "gallery.screenshot.scrollbarwidgetclass", 1},
    {"xm.scrolledwindowwidgetclass", "scrolledwindowwidgetclass", "layout", "xmScrolledWindowWidgetClass", "widget", NULL, 0, "gallery.source.scrolledwindowwidgetclass", "gallery.interaction.scrolledwindowwidgetclass", "gallery.component.scrolledwindowwidgetclass", "gallery.screenshot.scrolledwindowwidgetclass", 1},
    {"xm.selectionboxwidgetclass", "selectionboxwidgetclass", "layout", "xmSelectionBoxWidgetClass", "widget", NULL, 0, "gallery.source.selectionboxwidgetclass", "gallery.interaction.selectionboxwidgetclass", "gallery.component.selectionboxwidgetclass", "gallery.screenshot.selectionboxwidgetclass", 1},
    {"xm.separatorgadgetclass", "separatorgadgetclass", "layout", "xmSeparatorGadgetClass", "gadget", NULL, 0, "gallery.source.separatorgadgetclass", "gallery.interaction.separatorgadgetclass", "gallery.component.separatorgadgetclass", "gallery.screenshot.separatorgadgetclass", 1},
    {"xm.separatorwidgetclass", "separatorwidgetclass", "layout", "xmSeparatorWidgetClass", "widget", NULL, 0, "gallery.source.separatorwidgetclass", "gallery.interaction.separatorwidgetclass", "gallery.component.separatorwidgetclass", "gallery.screenshot.separatorwidgetclass", 1},
    {"xm.spinboxwidgetclass", "spinboxwidgetclass", "buttons-selection", "xmSpinBoxWidgetClass", "widget", NULL, 0, "gallery.source.spinboxwidgetclass", "gallery.interaction.spinboxwidgetclass", "gallery.component.spinboxwidgetclass", "gallery.screenshot.spinboxwidgetclass", 1},
    {"xm.tabboxwidgetclass", "tabboxwidgetclass", "layout", "xmTabBoxWidgetClass", "widget", NULL, 0, "gallery.source.tabboxwidgetclass", "gallery.interaction.tabboxwidgetclass", "gallery.component.tabboxwidgetclass", "gallery.screenshot.tabboxwidgetclass", 1},
    {"xm.tabstackwidgetclass", "tabstackwidgetclass", "layout", "xmTabStackWidgetClass", "widget", NULL, 0, "gallery.source.tabstackwidgetclass", "gallery.interaction.tabstackwidgetclass", "gallery.component.tabstackwidgetclass", "gallery.screenshot.tabstackwidgetclass", 1},
    {"xm.textwidgetclass", "textwidgetclass", "text-input", "xmTextWidgetClass", "widget", NULL, 0, "gallery.source.textwidgetclass", "gallery.interaction.textwidgetclass", "gallery.component.textwidgetclass", "gallery.screenshot.textwidgetclass", 1},
    {"xm.textfieldwidgetclass", "textfieldwidgetclass", "text-input", "xmTextFieldWidgetClass", "widget", NULL, 0, "gallery.source.textfieldwidgetclass", "gallery.interaction.textfieldwidgetclass", "gallery.component.textfieldwidgetclass", "gallery.screenshot.textfieldwidgetclass", 1},
    {"xm.togglebuttonwidgetclass", "togglebuttonwidgetclass", "buttons-selection", "xmToggleButtonWidgetClass", "widget", NULL, 0, "gallery.source.togglebuttonwidgetclass", "gallery.interaction.togglebuttonwidgetclass", "gallery.component.togglebuttonwidgetclass", "gallery.screenshot.togglebuttonwidgetclass", 1},
    {"xm.toggleswitchwidgetclass", "toggleswitchwidgetclass", "buttons-selection", "xmToggleSwitchWidgetClass", "widget", NULL, 0, "gallery.source.toggleswitchwidgetclass", "gallery.interaction.toggleswitchwidgetclass", "gallery.component.toggleswitchwidgetclass", "gallery.screenshot.toggleswitchwidgetclass", 1},
    {"xm.togglebuttongadgetclass", "togglebuttongadgetclass", "buttons-selection", "xmToggleButtonGadgetClass", "gadget", NULL, 0, "gallery.source.togglebuttongadgetclass", "gallery.interaction.togglebuttongadgetclass", "gallery.component.togglebuttongadgetclass", "gallery.screenshot.togglebuttongadgetclass", 1},
};

static const char *gallery_categories[] = {
    "layout",
    "feedback-dialogs",
    "commands-navigation",
    "text-input",
    "data-views",
    "buttons-selection",
    "media",
};
#define GALLERY_CATEGORY_COUNT 7
#define GALLERY_MANIFEST_COUNT \
    (sizeof(gallery_manifest) / sizeof(gallery_manifest[0]))

static void gallery_manifest_initialize(void)
{
#if defined(__GNUC__) || defined(__clang__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wdeprecated-declarations"
    gallery_manifest[0].class = xmButtonBoxWidgetClass;
    gallery_manifest[1].class = xmColorSelectorWidgetClass;
    gallery_manifest[2].class = xmCombinationBox2WidgetClass;
    gallery_manifest[3].class = xmDisclosureWidgetClass;
    gallery_manifest[4].class = xmFontSelectorWidgetClass;
    gallery_manifest[6].class = xmExt18ListWidgetClass;
    gallery_manifest[7].class = xmMultiListWidgetClass;
    gallery_manifest[8].class = xmIconBoxWidgetClass;
    gallery_manifest[9].class = xmOutlineWidgetClass;
    gallery_manifest[10].class = xmTreeWidgetClass;
    gallery_manifest[11].class = xmArrowButtonWidgetClass;
    gallery_manifest[12].class = xmArrowButtonGadgetClass;
    gallery_manifest[13].class = xmBulletinBoardWidgetClass;
    gallery_manifest[14].class = xmCascadeButtonWidgetClass;
    gallery_manifest[15].class = xmCascadeButtonGadgetClass;
    gallery_manifest[16].class = xmColumnWidgetClass;
    gallery_manifest[17].class = xmComboBoxWidgetClass;
    gallery_manifest[18].class = xmCommandWidgetClass;
    gallery_manifest[19].class = xmContainerWidgetClass;
    gallery_manifest[20].class = xmDataFieldWidgetClass;
    gallery_manifest[21].class = xmDialogShellWidgetClass;
    gallery_manifest[25].class = xmDragOverShellWidgetClass;
    gallery_manifest[26].class = xmDrawingAreaWidgetClass;
    gallery_manifest[27].class = xmDrawnButtonWidgetClass;
    gallery_manifest[30].class = xmFileSelectionBoxWidgetClass;
    gallery_manifest[31].class = xmFormWidgetClass;
    gallery_manifest[32].class = xmFrameWidgetClass;
    gallery_manifest[34].class = xmGrabShellWidgetClass;
    gallery_manifest[35].class = xmIconGadgetClass;
    gallery_manifest[36].class = xmIconHeaderClass;
    gallery_manifest[37].class = xmIconButtonWidgetClass;
    gallery_manifest[38].class = xmLabelWidgetClass;
    gallery_manifest[39].class = xmLabelGadgetClass;
    gallery_manifest[40].class = xmListWidgetClass;
    gallery_manifest[41].class = xmMainWindowWidgetClass;
    gallery_manifest[43].class = xmMenuShellWidgetClass;
    gallery_manifest[44].class = xmMessageBoxWidgetClass;
    gallery_manifest[45].class = xmColorPickerWidgetClass;
    gallery_manifest[46].class = xmFlyoutWidgetClass;
    gallery_manifest[47].class = xmTeachingTipWidgetClass;
    gallery_manifest[48].class = xmInfoBarWidgetClass;
    gallery_manifest[49].class = xmBreadcrumbBarWidgetClass;
    gallery_manifest[50].class = xmTreeViewWidgetClass;
    gallery_manifest[51].class = xmCalendarWidgetClass;
    gallery_manifest[52].class = xmDatePickerWidgetClass;
    gallery_manifest[53].class = xmTimePickerWidgetClass;
    gallery_manifest[54].class = xmDataGridWidgetClass;
    gallery_manifest[55].class = xmSegmentedControlWidgetClass;
    gallery_manifest[56].class = xmChipWidgetClass;
    gallery_manifest[57].class = xmBadgeWidgetClass;
    gallery_manifest[58].class = xmIconLabelWidgetClass;
    gallery_manifest[59].class = xmDividerWidgetClass;
    gallery_manifest[60].class = xmAvatarWidgetClass;
    gallery_manifest[61].class = xmLinkWidgetClass;
    gallery_manifest[62].class = xmSearchBoxWidgetClass;
    gallery_manifest[63].class = xmPromptWidgetClass;
    gallery_manifest[64].class = xmFormSectionWidgetClass;
    gallery_manifest[65].class = xmEmptyStateWidgetClass;
    gallery_manifest[66].class = xmToastWidgetClass;
    gallery_manifest[67].class = xmCommandPaletteWidgetClass;
    gallery_manifest[68].class = xmSkeletonWidgetClass;
    gallery_manifest[69].class = xmRowActionsWidgetClass;
    gallery_manifest[70].class = xmDropHighlightWidgetClass;
    gallery_manifest[71].class = xmStatusBarWidgetClass;
    gallery_manifest[72].class = xmToolbarWidgetClass;
    gallery_manifest[73].class = xmSidebarWidgetClass;
    gallery_manifest[74].class = xmAutoSuggestBoxWidgetClass;
    gallery_manifest[75].class = xmNumberBoxWidgetClass;
    gallery_manifest[76].class = xmRatingControlWidgetClass;
    gallery_manifest[77].class = xmNotebookWidgetClass;
    gallery_manifest[78].class = xmPanedWindowWidgetClass;
    gallery_manifest[79].class = xmPanedWidgetClass;
    gallery_manifest[81].class = CKMOTIF_GALLERY_PRINT_CLASS;
    gallery_manifest[82].class = xmPushButtonWidgetClass;
    gallery_manifest[83].class = xmProgressBarWidgetClass;
    gallery_manifest[84].class = xmProgressRingWidgetClass;
    gallery_manifest[85].class = xmPushButtonGadgetClass;
    gallery_manifest[86].class = xmRowColumnWidgetClass;
    gallery_manifest[87].class = xmSimpleSpinBoxWidgetClass;
    gallery_manifest[88].class = xmScaleWidgetClass;
    gallery_manifest[89].class = xmSpinnerWidgetClass;
    gallery_manifest[90].class = xmStepperWidgetClass;
    gallery_manifest[92].class = xmScrollBarWidgetClass;
    gallery_manifest[93].class = xmScrolledWindowWidgetClass;
    gallery_manifest[94].class = xmSelectionBoxWidgetClass;
    gallery_manifest[95].class = xmSeparatorGadgetClass;
    gallery_manifest[96].class = xmSeparatorWidgetClass;
    gallery_manifest[97].class = xmSpinBoxWidgetClass;
    gallery_manifest[98].class = xmTabBoxWidgetClass;
    gallery_manifest[99].class = xmTabStackWidgetClass;
    gallery_manifest[100].class = xmTextWidgetClass;
    gallery_manifest[101].class = xmTextFieldWidgetClass;
    gallery_manifest[102].class = xmToggleButtonWidgetClass;
    gallery_manifest[103].class = xmToggleSwitchWidgetClass;
    gallery_manifest[104].class = xmToggleButtonGadgetClass;
}
#pragma GCC diagnostic pop
#endif

#endif
