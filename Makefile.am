datadir = ${prefix}/share/Xm/doc

#data_DATA = BUGREPORT COPYRIGHT.MOTIF COPYING \
#            README RELEASE RELNOTES

EXTRA_DIST = tests/accessibility/scrollbar.c tests/accessibility/observe_scrollbar.py \
              tests/accessibility/modality.c tests/accessibility/selection_reentrancy.c \
              tests/accessibility/provider.c tests/accessibility/observe.py tests/accessibility/README.md \
              tests/accessibility/native_controls.c tests/accessibility/observe_orca.py \
              tests/accessibility/lifecycle.c tests/accessibility/native_geometry.c \
              tests/accessibility/tree_bus.c tests/accessibility/observe_tree.py \
             BUGREPORT \
             COPYING \
             README RELEASE RELNOTES \
             INSTALL.configure Makefile.imake-pure

MAINTAINERCLEANFILES = Makefile.in \
             configure libtool \
             config.guess \
             config.sub \
             config.log \
             ltconfig ltmain.sh \
             aclocal.m4 \
             config.h.in \
             install-sh \
             missing mkinstalldirs \
             compile

SUBDIRS	= bindings bitmaps \
             config \
             localized \
             lib \
             include \
             tools \
             clients \
             doc \
             demos
AUTOMAKE_OPTIONS = 1.4
ACLOCAL_AMFLAGS = -I .

# Focused fork manual; historical reference sources remain in doc/.
EXTRA_DIST += README.md ABI.md ckdocs.yml scripts/build-docs.py \
              scripts/build-xt-dependency.py patches/libXt-incr-lifetime.patch \
              tests/automated/check-docs-workflow-scope.py \
              tests/automated/check-docs-nav-complete.py \
              tests/automated/check-docs-provenance.py tests/automated/check-makestrs-comments.py \
              tests/automated/check-native-features.py \
              tests/Toolkit/ABI/cohort.c tests/Toolkit/Im/preedit_ranges.c \
              tests/Toolkit/Im/service.c tests/Toolkit/Im/service.py \
              tests/Toolkit/CutPaste/clipboard_unicode.c tests/Toolkit/CutPaste/clipboard_unicode.py \
              tests/Toolkit/CutPaste/clipboard_lifetime.py \
              tests/Toolkit/Unicode/layout.c tests/Toolkit/Unicode/line_context.c \
              tests/Toolkit/Unicode/incremental_map.c tests/Toolkit/Unicode/incremental_widgets.c \
              tests/Toolkit/Im/preedit_state.c \
              tests/Toolkit/Unicode/incremental_layout.c \
              tests/Toolkit/Unicode/text_unicode.c tests/Toolkit/Unicode/large_text.c \
              tests/disclosure/widget.c tests/disclosure/observe.py \
              WIDGET_REVIEW.md tests/headers/cxx20.cpp \
              tests/automated/check-dpi.c tests/automated/check-list-unicode.c \
              tests/automated/check-spinbox.c tests/automated/run-widget-regressions.py \
              tests/automated/check-wmd.py \
              docs/index.md docs/history.md docs/status.md docs/build.md \
              docs/abi.md docs/unicode.md docs/accessibility.md \
              docs/components.md docs/disclosure.md docs/documentation.md \
              .gitattributes .ckgit/build-commit .ckgit/ci.yml .github/workflows/pages.yml

# Keyboard activation lifetime regression.
EXTRA_DIST += tests/automated/check-button-timers.c docs/button-timers.md

# WP-16 additive binary switch widget and installed consumer contract.
EXTRA_DIST += tests/automated/check-toggle-switch.c tests/automated/run-toggle-switch.py \
              docs/toggle-switch.md
EXTRA_DIST += tests/automated/check-progress-bar.c tests/automated/run-progress-bar.py \
              docs/progress-bar.md
EXTRA_DIST += tests/automated/check-spinner.c tests/automated/run-spinner.py \
              docs/spinner.md
EXTRA_DIST += tests/automated/check-number-box.c tests/automated/run-number-box.py \
              docs/number-box.md
EXTRA_DIST += tests/automated/check-stepper.c tests/automated/run-stepper.py \
              docs/stepper.md
EXTRA_DIST += tests/automated/check-flyout.c tests/automated/run-flyout.py \
              docs/flyout.md
EXTRA_DIST += tests/automated/check-color-picker.c tests/automated/run-color-picker.py \
              docs/color-picker.md
EXTRA_DIST += tests/automated/check-rating-control.c tests/automated/run-rating-control.py \
              docs/rating-control.md

EXTRA_DIST += tests/automated/check-info-bar.c tests/automated/run-info-bar.py \
              docs/info-bar.md
EXTRA_DIST += tests/automated/check-breadcrumb-bar.c tests/automated/run-breadcrumb-bar.py \
              docs/breadcrumb-bar.md
EXTRA_DIST += tests/automated/check-tree-view.c tests/automated/run-tree-view.py \
              docs/tree-view.md
EXTRA_DIST += tests/automated/check-calendar.c tests/automated/run-calendar.py \
              docs/calendar.md
EXTRA_DIST += docs/date-picker.md tests/automated/check-date-picker.c \
              tests/automated/run-date-picker.py
EXTRA_DIST += docs/time-picker.md tests/automated/check-time-picker.c \
              tests/automated/run-time-picker.py
EXTRA_DIST += docs/segmented-control.md tests/automated/check-segmented-control.c \
              tests/automated/run-segmented-control.py
EXTRA_DIST += docs/data-grid.md tests/automated/check-data-grid.c \
              tests/automated/run-data-grid.py

EXTRA_DIST += tests/automated/check-paned-keyboard.c docs/paned-keyboard.md

EXTRA_DIST += tests/automated/check-combo-delete.c docs/combo-deletion.md

EXTRA_DIST += tests/automated/check-datafield-wcs.c docs/datafield-wide-text.md

# UIL debug listing regression.
EXTRA_DIST += tests/automated/check-uil-listing.py docs/uil-listing.md
EXTRA_DIST += tests/automated/check-uil-diagnostic-location-bounds.py docs/uil-diagnostic-location-bounds.md
EXTRA_DIST += tests/automated/check-uil-output-bounds.py docs/uil-output-bounds.md
EXTRA_DIST += tests/automated/check-mwm-image-path-bounds.py docs/mwm-image-path-bounds.md
EXTRA_DIST += tests/automated/check-mwm-image-path-allocation.py docs/mwm-image-path-allocation.md
EXTRA_DIST += tests/automated/check-mwm-exclusion-allocation.py docs/mwm-exclusion-allocation.md
EXTRA_DIST += tests/automated/check-mwm-menu-name-allocation.py docs/mwm-menu-name-allocation.md
EXTRA_DIST += tests/automated/check-mwm-menu-spec-allocation.py docs/mwm-menu-spec-allocation.md
EXTRA_DIST += tests/automated/check-mwm-exclusion-copy-allocation.py docs/mwm-exclusion-copy-allocation.md
EXTRA_DIST += tests/automated/check-mwm-menu-spec-copy-allocation.py docs/mwm-menu-spec-copy-allocation.md
EXTRA_DIST += tests/automated/check-mwm-menu-item-copy-allocation.py docs/mwm-menu-item-copy-allocation.md
EXTRA_DIST += tests/automated/check-mwm-menu-spec-copy-failure.py docs/mwm-menu-spec-copy-failure.md
EXTRA_DIST += tests/automated/check-mwm-menu-item-allocation.py docs/mwm-menu-item-allocation.md
EXTRA_DIST += tests/automated/check-mwm-match-insertion-allocation.py docs/mwm-match-insertion-allocation.md
EXTRA_DIST += tests/automated/check-mwm-menu-button-array-bounds.py docs/mwm-menu-button-array-bounds.md
EXTRA_DIST += tests/automated/check-mwm-insert-tree-allocation.py docs/mwm-insert-tree-allocation.md
EXTRA_DIST += tests/automated/check-mwm-save-pbinfo-growth.py docs/mwm-save-pbinfo-growth.md
EXTRA_DIST += tests/automated/check-mwm-rename-allocation.py docs/mwm-rename-allocation.md
EXTRA_DIST += tests/automated/check-mwm-client-command-label-allocation.py docs/mwm-client-command-label-allocation.md
EXTRA_DIST += tests/automated/check-mwm-cci-argument-allocation.py docs/mwm-cci-argument-allocation.md
EXTRA_DIST += tests/automated/check-mwm-cci-label-prefix.py docs/mwm-cci-label-prefix.md
EXTRA_DIST += tests/automated/check-mwm-default-menu-allocation.py docs/mwm-default-menu-allocation.md
EXTRA_DIST += tests/automated/check-mwm-workspace-restore-allocation.py docs/mwm-workspace-restore-allocation.md
EXTRA_DIST += tests/automated/check-mwm-restart-command-allocation.py docs/mwm-restart-command-allocation.md
EXTRA_DIST += tests/automated/check-mwm-client-list-growth.py docs/mwm-client-list-growth.md
EXTRA_DIST += tests/automated/check-mwm-db-home-allocation.py docs/mwm-db-home-allocation.md
EXTRA_DIST += tests/automated/check-mwm-workspace-list-serialization.py docs/mwm-workspace-list-serialization.md
EXTRA_DIST += tests/automated/check-mwm-label-parse-allocation.py docs/mwm-label-parse-allocation.md
EXTRA_DIST += tests/automated/check-mwm-separator-template-allocation.py docs/mwm-separator-template-allocation.md
EXTRA_DIST += tests/automated/check-container-cell-info-allocation.py docs/container-cell-info-allocation.md
EXTRA_DIST += tests/automated/check-container-transfer-allocation.py docs/container-transfer-allocation.md
EXTRA_DIST += tests/automated/check-xmstring-cache-allocation.py docs/xmstring-cache-allocation.md
EXTRA_DIST += tests/automated/check-unique-event-allocation.py docs/unique-event-allocation.md
EXTRA_DIST += tests/automated/check-xmstring-charset-allocation.py docs/xmstring-charset-allocation.md
EXTRA_DIST += tests/automated/check-scrolled-window-drag-allocation.py docs/scrolled-window-drag-allocation.md
EXTRA_DIST += tests/automated/check-real-xlations-allocation.py docs/real-xlations-allocation.md
EXTRA_DIST += tests/automated/check-print-allocation.py docs/print-allocation.md
EXTRA_DIST += tests/automated/check-filesb-filter-allocation.py docs/filesb-filter-allocation.md
EXTRA_DIST += tests/automated/check-picture-subgraph-allocation.py docs/picture-subgraph-allocation.md
EXTRA_DIST += tests/automated/check-render-token-allocation.py docs/render-token-allocation.md
EXTRA_DIST += tests/automated/check-dialog-name-bounds.py docs/dialog-name-bounds.md
EXTRA_DIST += tests/automated/check-editres-error-bounds.py docs/editres-error-bounds.md
EXTRA_DIST += tests/automated/check-spinbox-name-bounds.py docs/spinbox-name-bounds.md
EXTRA_DIST += tests/automated/check-column-label-name-bounds.py docs/column-label-name-bounds.md
EXTRA_DIST += tests/automated/check-xmos-init-path-bounds.py docs/xmos-init-path-bounds.md
EXTRA_DIST += tests/automated/check-xmos-cache-bounds.py docs/xmos-cache-bounds.md
EXTRA_DIST += tests/automated/check-label-wcs-bounds.py docs/label-wcs-bounds.md
EXTRA_DIST += tests/automated/check-font-pattern-bounds.py docs/font-pattern-bounds.md
EXTRA_DIST += tests/automated/check-menu-stream-bounds.py docs/menu-stream-bounds.md
EXTRA_DIST += tests/automated/check-mwm-protocol-oom.py docs/mwm-protocol-oom.md
EXTRA_DIST += tests/automated/check-mwm-virtual-key-atom-allocation.py docs/mwm-virtual-key-atom-allocation.md
EXTRA_DIST += tests/automated/check-mwm-invoke-message-allocation.py docs/mwm-invoke-message-allocation.md
EXTRA_DIST += tests/automated/check-mwm-multiscreen-selection.py docs/mwm-multiscreen-selection.md
EXTRA_DIST += tests/automated/check-mwm-fscreen-default.py docs/mwm-fscreen-default.md
EXTRA_DIST += tests/automated/check-mwm-icon-resource-bounds.py docs/mwm-icon-resource-bounds.md
EXTRA_DIST += tests/automated/check-mwm-menutree-bounds.py docs/mwm-menutree-bounds.md
EXTRA_DIST += tests/automated/check-screen-lookup-bounds.py docs/screen-lookup-bounds.md
EXTRA_DIST += tests/automated/check-wmcmd-label-bounds.py docs/wmcmd-label-bounds.md
EXTRA_DIST += tests/automated/check-tablist-debug-bounds.py docs/tablist-debug-bounds.md
EXTRA_DIST += tests/automated/check-charset-canonicalize-bounds.py docs/charset-canonicalization-bounds.md
EXTRA_DIST += tests/automated/check-tabstack-atom-name.py docs/tabstack-atom-name.md
EXTRA_DIST += tests/automated/check-geoutils-debug-bounds.py docs/geoutils-debug-bounds.md
EXTRA_DIST += tests/automated/check-colors-selector-format-bounds.py docs/color-selector-format-bounds.md
EXTRA_DIST += tests/automated/check-mwm-system-menu-bounds.py docs/mwm-system-menu-bounds.md

# Image scaling and malformed-image regression.
EXTRA_DIST += tests/automated/check-image-scaling.c tests/automated/run-image-regressions.py docs/image-scaling.md

# Text source range-boundary regression.
EXTRA_DIST += tests/automated/check-text-source-bounds.c tests/automated/run-text-source-regressions.py docs/text-source-bounds.md
EXTRA_DIST += tests/automated/check-container-tabs.c tests/automated/run-container-regressions.py docs/container-tabs.md
EXTRA_DIST += tests/automated/check-nontext-unicode.c tests/automated/run-nontext-unicode-regressions.py docs/nontext-unicode.md
EXTRA_DIST += tests/automated/check-notebook-order.c tests/automated/run-notebook-regressions.py docs/notebook-order.md
EXTRA_DIST += tests/automated/check-theme-option.sh docs/theme-configuration.md

# WP-00 modernization baseline: installed native hierarchy/geometry/focus/image fixture.
EXTRA_DIST += tests/modernization/baseline.c tests/automated/run-modernization-baseline.py \
              docs/modernization-baseline.md modernization_plan.md
EXTRA_DIST += tests/automated/check-appearance-profile.c tests/automated/run-appearance-profile.py docs/appearance-profiles.md
EXTRA_DIST += tests/automated/check-button-type-conversion.c tests/automated/run-button-type-regressions.py docs/button-type-conversion.md
EXTRA_DIST += tests/automated/check-charset-table-conversion.c tests/automated/run-charset-table-regressions.py docs/charset-table-conversion.md
EXTRA_DIST += tests/automated/check-keysym-table-conversion.c tests/automated/run-keysym-table-regressions.py docs/keysym-table-conversion.md
EXTRA_DIST += tests/automated/check-default-font-cache-bounds.py docs/default-font-cache-bounds.md
EXTRA_DIST += tests/automated/check-atom-list-bounds.py docs/atom-list-conversion.md
EXTRA_DIST += tests/automated/check-keysym-table-bounds.py
EXTRA_DIST += tests/automated/check-string-table-conversion.c tests/automated/run-string-table-regressions.py docs/string-table-conversion.md
EXTRA_DIST += tests/automated/check-cardinal-list-conversion.c tests/automated/run-cardinal-list-regressions.py docs/cardinal-list-conversion.md
EXTRA_DIST += tests/automated/check-xmstring-table-conversion.c tests/automated/run-xmstring-table-regressions.py docs/xmstring-table-conversion.md
EXTRA_DIST += tests/automated/check-text-property-concat-bounds.py docs/text-property-concat-bounds.md
EXTRA_DIST += tests/automated/check-cascade-resource-bounds.py docs/cascade-resource-bounds.md
EXTRA_DIST += tests/automated/check-obsolete-rectangle-bounds.py docs/obsolete-rectangle-bounds.md
EXTRA_DIST += tests/automated/check-reptype-bounds.py docs/reptype-bounds.md
EXTRA_DIST += tests/automated/check-picture-copy-bounds.py docs/picture-copy-bounds.md
EXTRA_DIST += tests/automated/check-accessible-utf8-bounds.py docs/accessibility-utf8-bounds.md
EXTRA_DIST += tests/automated/check-xmstring-rendition-bounds.py docs/xmstring-rendition-bounds.md
EXTRA_DIST += tests/automated/check-xmstring-table-growth.py
EXTRA_DIST += tests/automated/check-picture-buffer.c tests/automated/run-picture-regressions.py docs/picture-buffer.md
EXTRA_DIST += tests/automated/check-standard-targets-bounds.c tests/automated/run-standard-targets-regressions.py docs/standard-targets-bounds.md
EXTRA_DIST += tests/automated/check-external-selection.c tests/automated/run-external-selection-regressions.py docs/external-selection-lifecycle.md
EXTRA_DIST += tests/automated/check-unicode-clipboard.c tests/automated/run-unicode-clipboard-regressions.py docs/unicode-clipboard.md
EXTRA_DIST += tests/automated/check-modern-light-profile.c tests/automated/run-modern-light-profile.py
EXTRA_DIST += tests/automated/check-high-contrast-profile.c tests/automated/run-high-contrast-profile.py
EXTRA_DIST += docs/dpi-defaults.md
EXTRA_DIST += tests/automated/check-xlibre-dpi-contract.c tests/automated/run-xlibre-dpi-contract.py docs/xlibre-dpi-contract.md
EXTRA_DIST += tests/automated/check-mwm-session-db-path.py docs/mwm-session-db-path.md
EXTRA_DIST += tests/automated/check-mwm-display-env-bounds.py docs/mwm-display-env-bounds.md
EXTRA_DIST += tests/automated/check-mwm-screen-display-bounds.py docs/mwm-screen-display-bounds.md
EXTRA_DIST += tests/automated/check-mwm-property-warning-bounds.py docs/mwm-property-warning-bounds.md
EXTRA_DIST += tests/automated/check-mwm-feedback-fallback.py docs/mwm-feedback-fallback.md
EXTRA_DIST += tests/automated/check-mwm-error-bounds.py docs/mwm-error-bounds.md
EXTRA_DIST += tests/automated/check-mwm-screen-conflict-warning.py docs/mwm-screen-conflict-warning.md
EXTRA_DIST += tests/automated/check-mwm-screen-warp-warning.py docs/mwm-screen-warp-warning.md
EXTRA_DIST += tests/automated/check-mwm-font-warning-bounds.py docs/mwm-font-warning-bounds.md
EXTRA_DIST += tests/automated/check-mwm-binding-buffer-lifetime.py docs/mwm-binding-buffer-lifetime.md
EXTRA_DIST += tests/automated/check-mwm-session-resource-bounds.py docs/mwm-session-resource-bounds.md
EXTRA_DIST += tests/automated/check-mwm-command-tree-bounds.py docs/mwm-command-tree-bounds.md
EXTRA_DIST += tests/automated/check-mwm-config-reopen-warning.py docs/mwm-config-reopen-warning.md
EXTRA_DIST += tests/automated/check-mwm-temp-cleanup.py docs/mwm-temp-cleanup.md
EXTRA_DIST += tests/automated/check-mwm-parser-warning-bounds.py docs/mwm-parser-warning-bounds.md
EXTRA_DIST += tests/automated/check-mwm-config-allocation-warning.py docs/mwm-config-allocation-warning.md
EXTRA_DIST += tests/automated/check-svg-image.c tests/automated/run-svg-image.py docs/svg-images.md
EXTRA_DIST += docs/native-text-ime-matrix.md
EXTRA_DIST += tests/automated/check-mwm-bindings-path.py docs/mwm-bindings-path.md
EXTRA_DIST += docs/accessibility-journey-matrix.md
EXTRA_DIST += docs/cpp-support-library-proposal.md
EXTRA_DIST += tests/automated/check-mwm-menu-label-termination.py docs/mwm-menu-label-termination.md
EXTRA_DIST += tests/automated/check-mwm-session-command-growth.py docs/mwm-session-command-growth.md
EXTRA_DIST += tests/automated/check-form-layout-corpus.c tests/automated/run-form-layout-corpus.py docs/form-layout-corpus.md
EXTRA_DIST += tests/automated/check-mwm-function-arg-bounds.py docs/mwm-function-arg-bounds.md
EXTRA_DIST += tests/automated/check-form-layout-corpus.c tests/automated/run-form-layout-corpus.py docs/form-layout-corpus.md docs/form-layout-decision.md
EXTRA_DIST += tests/automated/check-png-pixel-layout.py docs/png-pixel-layout.md
EXTRA_DIST += docs/layout-manager-decision.md
EXTRA_DIST += tests/automated/check-png-signature-read.py docs/png-signature-validation.md
EXTRA_DIST += tests/automated/check-png-allocation-cleanup.py docs/png-allocation-cleanup.md
EXTRA_DIST += tests/automated/check-jpeg-pixel-layout.py docs/jpeg-pixel-layout.md
EXTRA_DIST += tests/automated/check-jpeg-error-cleanup.py docs/jpeg-error-cleanup.md
EXTRA_DIST += tests/automated/check-png-error-cleanup.py docs/png-error-cleanup.md
EXTRA_DIST += tests/automated/check-ui-completion-prototype.c tests/automated/run-ui-completion-prototype.py docs/ui-thread-completion-prototype.md
EXTRA_DIST += tests/automated/check-image-scaling-allocations.py docs/image-scaling-allocations.md
EXTRA_DIST += docs/ui-thread-completion-api-decision.md
EXTRA_DIST += tests/automated/check-image-scaling-depth1.py docs/image-scaling-depth1.md
EXTRA_DIST += docs/cpp-support-library-release-decision.md
EXTRA_DIST += tests/automated/check-image-rejection-cleanup.py docs/image-rejection-cleanup.md
EXTRA_DIST += tests/automated/check-image-screen-fallback.py docs/image-screen-fallback.md
EXTRA_DIST += tests/automated/check-image-conversion-cleanup.py docs/image-conversion-cleanup.md
EXTRA_DIST += tests/automated/check-dpi-control-metrics.c tests/automated/run-dpi-control-metrics.py docs/dpi-control-metrics.md
EXTRA_DIST += tests/automated/check-cache-pixmap-input.py docs/cache-pixmap-input.md
EXTRA_DIST += tests/automated/check-pixmap-create-failure.py docs/pixmap-create-failure.md
EXTRA_DIST += tests/automated/check-cache-geometry-status.py docs/cache-geometry-status.md
EXTRA_DIST += tests/automated/check-mask-name-allocation.py docs/mask-name-allocation.md
EXTRA_DIST += tests/automated/check-embedded-mask-ownership.py docs/embedded-mask-ownership.md
EXTRA_DIST += tests/automated/check-icon-mask-name-allocation.py docs/icon-mask-name-allocation.md
EXTRA_DIST += tests/automated/check-scale-value-bounds.py docs/scale-value-bounds.md

EXTRA_DIST += tests/automated/check-drop-site-warning-bounds.py docs/drop-site-warning-bounds.md

EXTRA_DIST += tests/automated/check-simple-widget-name-bounds.py docs/simple-widget-name-bounds.md

EXTRA_DIST += tests/automated/check-compound-segment-bounds.py docs/compound-segment-bounds.md

EXTRA_DIST += tests/automated/check-font-size-name-bounds.py docs/font-size-label-bounds.md

EXTRA_DIST += tests/automated/check-font-encoding-callback-types.py docs/font-encoding-callback-types.md

EXTRA_DIST += tests/automated/check-screen-index-scan.py docs/screen-index-scan.md

EXTRA_DIST += tests/automated/check-datafield-multibyte-scratch.py docs/datafield-multibyte-scratch.md

EXTRA_DIST += tests/automated/check-datafield-mblen-progress.py docs/datafield-mblen-progress.md

EXTRA_DIST += tests/automated/check-xmstring-wide-conversion-bounds.py docs/xmstring-wide-conversion-bounds.md

EXTRA_DIST += tests/automated/check-modern-dark-profile.c tests/automated/run-modern-dark-profile.py docs/modern-dark-profile.md

EXTRA_DIST += tests/automated/check-xmstring-copy-width.py docs/xmstring-copy-width.md

EXTRA_DIST += tests/automated/check-utf8-conversion-bounds.py docs/utf8-conversion-bounds.md

EXTRA_DIST += tests/automated/check-region-size-bounds.py docs/region-size-bounds.md

EXTRA_DIST += tests/automated/check-region-shadow-counts.py docs/region-shadow-counts.md

EXTRA_DIST += tests/automated/check-region-export-bounds.py docs/region-export-bounds.md

EXTRA_DIST += tests/automated/check-region-operation-capacity.py docs/region-operation-capacity.md

EXTRA_DIST += tests/automated/check-string-table-capacity.py docs/string-table-capacity.md
EXTRA_DIST += tests/automated/check-list-selection-internals.py
EXTRA_DIST += tests/automated/check-rcmenu-bounds.py docs/rcmenu-bounds.md
EXTRA_DIST += tests/automated/check-mrm-icon-bounds.py docs/mrm-icon-bounds.md
EXTRA_DIST += tests/automated/check-mrm-arg-bounds.py docs/mrm-arg-bounds.md
EXTRA_DIST += tests/automated/check-mrm-zero-counts.py docs/mrm-zero-counts.md
EXTRA_DIST += tests/automated/check-mrm-fontlist-bounds.py docs/mrm-fontlist-bounds.md
EXTRA_DIST += tests/automated/check-mrm-string-bounds.py docs/mrm-string-bounds.md
EXTRA_DIST += tests/automated/check-mrm-resource-bounds.py docs/mrm-resource-bounds.md
EXTRA_DIST += tests/automated/check-mrm-vector-bounds.py docs/mrm-vector-bounds.md
EXTRA_DIST += tests/automated/check-mrm-pointerlist-bounds.py docs/mrm-pointerlist-bounds.md
EXTRA_DIST += tests/automated/check-mrm-register-counts.py docs/mrm-register-counts.md
EXTRA_DIST += tests/automated/check-mrm-widechar-bounds.py docs/mrm-widechar-bounds.md
EXTRA_DIST += tests/automated/check-mrm-callback-bounds.py docs/mrm-callback-bounds.md
EXTRA_DIST += tests/automated/check-mrm-callback-copy-bounds.py docs/mrm-callback-copy-bounds.md
EXTRA_DIST += tests/automated/check-mrm-text-vector-bounds.py docs/mrm-text-vector-bounds.md
EXTRA_DIST += tests/automated/check-mrm-hierarchy-counts.py docs/mrm-hierarchy-bounds.md
EXTRA_DIST += tests/automated/check-mrm-children-bounds.py docs/mrm-children-bounds.md
EXTRA_DIST += tests/automated/check-mrm-callback-descriptor-bounds.py docs/mrm-callback-descriptor-bounds.md
EXTRA_DIST += tests/automated/check-mrm-arg-descriptor-bounds.py docs/mrm-arg-descriptor-bounds.md
EXTRA_DIST += tests/automated/check-mrm-extra-args.py docs/mrm-extra-args.md
EXTRA_DIST += tests/automated/check-mrm-icon-record-bounds.py docs/mrm-icon-record-bounds.md
EXTRA_DIST += tests/automated/check-mrm-guarantee-space.py docs/mrm-guarantee-space.md
EXTRA_DIST += tests/automated/check-mrm-wchar-append-bounds.py docs/mrm-wchar-append-bounds.md
EXTRA_DIST += tests/automated/check-mrm-string-append-bounds.py docs/mrm-string-append-bounds.md
EXTRA_DIST += tests/automated/check-mrm-cstring-append-bounds.py docs/mrm-cstring-append-bounds.md
EXTRA_DIST += tests/automated/check-mrm-resource-index.py docs/mrm-resource-index.md
EXTRA_DIST += tests/automated/check-mrm-fetch-fontlist-bounds.py docs/mrm-fetch-fontlist-bounds.md
EXTRA_DIST += tests/automated/check-mrm-setvalues-input.py docs/mrm-setvalues-input.md
EXTRA_DIST += tests/automated/check-mrm-filename-suffix.py docs/mrm-filename-suffix.md
EXTRA_DIST += tests/automated/check-mrm-error-format-bounds.py docs/mrm-error-format-bounds.md
EXTRA_DIST += tests/automated/check-mrm-icon-error-bounds.py docs/mrm-icon-error-bounds.md

EXTRA_DIST += \
    docs/maintenance-review.md \
    docs/storage-and-images.md \
    tests/automated/check-transfer-storage.c \
    tests/automated/check-xpm-input.c \
    tests/automated/check-xt-callback-lifetime.c \
    tests/automated/run-storage-regressions.py
EXTRA_DIST += tests/automated/check-mrm-conversion-error-bounds.py docs/mrm-conversion-error-bounds.md
EXTRA_DIST += tests/automated/check-mrm-file-open-input.py docs/mrm-file-open-input.md
EXTRA_DIST += tests/automated/check-mrm-header-string-bounds.py docs/mrm-header-string-bounds.md

EXTRA_DIST += \
    docs/additive-widget-contract.md \
    tests/automated/check-additive-widget-contract.c \
    tests/automated/run-additive-widget-contract.py
EXTRA_DIST += tests/automated/check-mrm-header-write-input.py docs/mrm-header-write-input.md
EXTRA_DIST += tests/automated/check-mrm-header-error-bounds.py docs/mrm-header-error-bounds.md
EXTRA_DIST += tests/automated/check-mrm-index-writer-input.py docs/mrm-index-writer-input.md
EXTRA_DIST += tests/automated/check-mrm-wref-input.py docs/mrm-wref-input.md
EXTRA_DIST += tests/automated/check-mrm-pointer-list-input.py docs/mrm-pointer-list-input.md
EXTRA_DIST += tests/automated/check-mrm-wci-lookup-input.py docs/mrm-wci-lookup-input.md
EXTRA_DIST += tests/automated/check-mrm-index-entry-input.py docs/mrm-index-entry-input.md
EXTRA_DIST += tests/automated/check-mrm-buffer-input.py docs/mrm-buffer-input.md
EXTRA_DIST += tests/automated/check-mrm-file-io-input.py docs/mrm-file-io-input.md
EXTRA_DIST += tests/automated/check-mrm-file-io-input.py docs/mrm-file-seek-bounds.md
EXTRA_DIST += tests/automated/check-mrm-literal-error-bounds.py docs/mrm-literal-error-bounds.md
EXTRA_DIST += tests/automated/check-mrm-swap-error-bounds.py docs/mrm-swap-error-bounds.md
EXTRA_DIST += tests/automated/check-mrm-wref-error-bounds.py docs/mrm-wref-error-bounds.md
EXTRA_DIST += tests/automated/check-mrm-widget-name-bounds.py docs/mrm-widget-name-bounds.md
EXTRA_DIST += tests/automated/check-mrm-display-string-bounds.py docs/mrm-display-string-bounds.md
EXTRA_DIST += tests/automated/check-mrm-wci-registration-bounds.py docs/mrm-wci-registration-bounds.md
EXTRA_DIST += tests/automated/check-mrm-alloc-string-bounds.py docs/mrm-alloc-string-bounds.md
EXTRA_DIST += tests/automated/check-mrm-pointer-list-string-bounds.py docs/mrm-pointer-list-string-bounds.md
EXTRA_DIST += tests/automated/check-mrm-wref-storage-bounds.py docs/mrm-wref-storage-bounds.md
EXTRA_DIST += tests/automated/check-mrm-fetch-index-bounds.py docs/mrm-fetch-index-bounds.md
EXTRA_DIST += tests/automated/check-mrm-time-output.py docs/mrm-time-output.md
EXTRA_DIST += tests/automated/check-mrm-wref-tagname-input.py docs/mrm-wref-tagname-input.md
EXTRA_DIST += tests/automated/check-mrm-wref-tagname-input.py docs/mrm-wref-tagname-input.md
EXTRA_DIST += tests/automated/check-mrm-wref-list-input.py docs/mrm-wref-list-input.md
EXTRA_DIST += tests/automated/check-mrm-conversion-allocation.py docs/mrm-conversion-allocation.md
EXTRA_DIST += tests/automated/check-mrm-wci-hash-allocation.py docs/mrm-wci-hash-allocation.md
EXTRA_DIST += tests/automated/check-mrm-wci-hash-callers.py docs/mrm-wci-hash-callers.md
EXTRA_DIST += tests/automated/check-mrm-wci-hash-helpers.py docs/mrm-wci-hash-helpers.md
EXTRA_DIST += tests/automated/check-mrm-wci-hash-init.py docs/mrm-wci-hash-init.md
EXTRA_DIST += tests/automated/check-mrm-module-size-bounds.py docs/mrm-module-size-bounds.md
EXTRA_DIST += tests/automated/check-mrm-fontlist-allocation.py docs/mrm-fontlist-allocation.md
EXTRA_DIST += tests/automated/check-mrm-hierarchy-name-registration.py docs/mrm-hierarchy-name-registration.md
EXTRA_DIST += tests/automated/check-mrm-hierarchy-open-input.py docs/mrm-hierarchy-open-input.md
EXTRA_DIST += tests/automated/check-mrm-wref-growth.py docs/mrm-wref-growth.md
EXTRA_DIST += tests/automated/check-mrm-wcr-output-pointers.py docs/mrm-wcr-output-pointers.md
EXTRA_DIST += tests/automated/check-mrm-context-output-pointer.py docs/mrm-context-output-pointer.md
EXTRA_DIST += tests/automated/check-mrm-idb-open-output-pointers.py docs/mrm-idb-open-output-pointers.md
EXTRA_DIST += tests/automated/check-mrm-idb-buffer-cleanup.py docs/mrm-idb-buffer-cleanup.md
EXTRA_DIST += tests/automated/check-mrm-idb-open-cleanup.py docs/mrm-idb-open-cleanup.md
EXTRA_DIST += tests/automated/check-mrm-idb-close-status.py docs/mrm-idb-close-status.md
EXTRA_DIST += tests/automated/check-mrm-idb-write-conflict-cleanup.py docs/mrm-idb-write-conflict-cleanup.md
EXTRA_DIST += tests/automated/check-mrm-idb-block-io.py docs/mrm-idb-block-io.md
EXTRA_DIST += tests/automated/check-mrm-idb-record-bounds.py docs/mrm-idb-record-bounds.md
EXTRA_DIST += tests/automated/check-mrm-idb-empty-decommit.py docs/mrm-idb-empty-decommit.md
EXTRA_DIST += tests/automated/check-mrm-idb-close-delete-mode.py docs/mrm-idb-close-delete-mode.md
EXTRA_DIST += tests/automated/check-mrm-idb-close-keep.py docs/mrm-idb-close-keep.md
EXTRA_DIST += tests/automated/check-mrm-idb-decommit-cleanup.py docs/mrm-idb-decommit-cleanup.md
EXTRA_DIST += tests/automated/check-mrm-index-inputs.py docs/mrm-index-inputs.md
EXTRA_DIST += tests/automated/check-mrm-index-return-inputs.py docs/mrm-index-return-inputs.md
EXTRA_DIST += tests/automated/check-mrm-index-mutation-inputs.py docs/mrm-index-mutation-inputs.md
EXTRA_DIST += tests/automated/check-mrm-index-record-copies.py docs/mrm-index-record-copies.md
EXTRA_DIST += tests/automated/check-mrm-index-collapse-bounds.py docs/mrm-index-collapse-bounds.md
EXTRA_DIST += tests/automated/check-mrm-index-buffer-validation.py docs/mrm-index-buffer-validation.md
EXTRA_DIST += tests/automated/check-mrm-index-entry-bounds.py docs/mrm-index-entry-bounds.md
EXTRA_DIST += tests/automated/check-mrm-index-split-bounds.py docs/mrm-index-split-bounds.md
EXTRA_DIST += tests/automated/check-mrm-root-allocation.py docs/mrm-root-allocation.md
EXTRA_DIST += tests/automated/check-mrm-child-repair-bounds.py docs/mrm-child-repair-bounds.md
EXTRA_DIST += tests/automated/check-mrm-index-read-bounds.py docs/mrm-index-read-bounds.md
EXTRA_DIST += tests/automated/check-mrm-btree-record-bounds.py docs/mrm-btree-record-bounds.md
EXTRA_DIST += tests/automated/check-mrm-rid-inputs.py docs/mrm-rid-inputs.md
EXTRA_DIST += tests/automated/check-mrm-header-inputs.py docs/mrm-header-inputs.md
EXTRA_DIST += tests/automated/check-mrm-header-data-bounds.py docs/mrm-header-data-bounds.md
EXTRA_DIST += tests/automated/check-mrm-header-filter-lifetime.py docs/mrm-header-filter-lifetime.md
EXTRA_DIST += tests/automated/check-mrm-buffer-postconditions.py docs/mrm-buffer-postconditions.md
EXTRA_DIST += tests/automated/check-mrm-buffer-valid-macro.py docs/mrm-buffer-valid-macro.md
EXTRA_DIST += tests/automated/check-mrm-decommit-state.py docs/mrm-decommit-state.md
EXTRA_DIST += tests/automated/check-mrm-decommit-all-input.py docs/mrm-decommit-all-input.md

EXTRA_DIST += tests/automated/check-mrm-cache-hit-record.py docs/mrm-cache-hit-record.md

EXTRA_DIST += tests/automated/check-mrm-record-type.py docs/mrm-record-type.md

EXTRA_DIST += tests/automated/check-mrm-buffer-pool-size.py docs/mrm-buffer-pool-size.md

EXTRA_DIST += tests/automated/check-mrm-match-filter-lifetime.py docs/mrm-match-filter-lifetime.md

EXTRA_DIST += tests/automated/check-mrm-data-offset.py docs/mrm-data-offset.md

EXTRA_DIST += tests/automated/check-mrm-data-payload.py docs/mrm-data-payload.md

EXTRA_DIST += tests/automated/check-mrm-put-entry-inputs.py docs/mrm-put-entry-inputs.md

EXTRA_DIST += tests/automated/check-mrm-entry-read-inputs.py docs/mrm-entry-read-inputs.md

EXTRA_DIST += tests/automated/check-mrm-get-entry-postconditions.py docs/mrm-get-entry-postconditions.md

EXTRA_DIST += tests/automated/check-mrm-put-entry-capacity.py docs/mrm-put-entry-capacity.md

EXTRA_DIST += tests/automated/check-mrm-header-byte-swap-order.py docs/mrm-header-byte-swap-order.md

EXTRA_DIST += tests/automated/check-mrm-index-list-failure.py docs/mrm-index-list-failure.md

EXTRA_DIST += tests/automated/check-mrm-index-string-bounds.py docs/mrm-index-string-bounds.md

EXTRA_DIST += tests/automated/check-mrm-rid-handle-bounds.py docs/mrm-rid-handle-bounds.md

EXTRA_DIST += tests/automated/check-mrm-pointer-list-cleanup.py docs/mrm-pointer-list-cleanup.md

EXTRA_DIST += tests/automated/check-mrm-pointer-list-init-cleanup.py docs/mrm-pointer-list-init-cleanup.md

EXTRA_DIST += tests/automated/check-mrm-pointer-list-free.py docs/mrm-pointer-list-free.md

EXTRA_DIST += tests/automated/check-mrm-pointer-list-search.py docs/mrm-pointer-list-search.md

EXTRA_DIST += tests/automated/check-mrm-pointer-list-resize.py docs/mrm-pointer-list-resize.md

EXTRA_DIST += tests/automated/check-mrm-pointer-list-shrink.py docs/mrm-pointer-list-shrink.md

EXTRA_DIST += tests/automated/check-mrm-open-recovery.py docs/mrm-open-recovery.md

EXTRA_DIST += tests/automated/check-mrm-close-delete.py docs/mrm-close-delete.md

EXTRA_DIST += tests/automated/check-mrm-clobber-close.py docs/mrm-clobber-close.md

EXTRA_DIST += tests/automated/check-mrm-no-clobber-close.py docs/mrm-no-clobber-close.md

EXTRA_DIST += tests/automated/check-mrm-pointer-list-append-state.py docs/mrm-pointer-list-append-state.md

EXTRA_DIST += tests/automated/check-mrm-pointer-list-content-state.py docs/mrm-pointer-list-content-state.md
EXTRA_DIST += tests/automated/check-mrm-index-parent-errors.py docs/mrm-index-parent-errors.md
EXTRA_DIST += tests/automated/check-mrm-index-preflight.py docs/mrm-index-preflight.md
EXTRA_DIST += tests/automated/check-mrm-index-collapse-copy-bounds.py docs/mrm-index-collapse-copy-bounds.md
EXTRA_DIST += tests/automated/check-mrm-confirm-node-space.py docs/mrm-confirm-node-space.md
EXTRA_DIST += tests/automated/check-mrm-index-collapse-status.py docs/mrm-index-collapse-status.md
EXTRA_DIST += tests/automated/check-mrm-index-pivot-scan.py docs/mrm-index-pivot-scan.md
EXTRA_DIST += tests/automated/check-mrm-resource-reload-bounds.py docs/mrm-resource-reload-bounds.md
EXTRA_DIST += tests/automated/check-mrm-resource-parent-rebind.py docs/mrm-resource-parent-rebind.md
EXTRA_DIST += tests/automated/check-mrm-fetch-setvalues-allocation.py docs/mrm-fetch-setvalues-allocation.md
EXTRA_DIST += tests/automated/check-mrm-wref-allocation.py docs/mrm-wref-allocation.md
EXTRA_DIST += tests/automated/check-mrm-register-names-allocation.py docs/mrm-register-names-allocation.md
EXTRA_DIST += tests/automated/check-mrm-widget-instance-allocation.py docs/mrm-widget-instance-allocation.md
EXTRA_DIST += tests/automated/check-mrm-widget-instance-input.py docs/mrm-widget-instance-input.md
EXTRA_DIST += tests/automated/check-mrm-pixmap-defer-bounds.py docs/mrm-pixmap-defer-bounds.md
EXTRA_DIST += tests/automated/check-mrm-widechar-allocation.py docs/mrm-widechar-allocation.md
EXTRA_DIST += tests/automated/check-mrm-header-version-allocation.py docs/mrm-header-version-allocation.md
EXTRA_DIST += tests/automated/check-mrm-safe-copy-allocation.py docs/mrm-safe-copy-allocation.md
EXTRA_DIST += tests/automated/check-mrm-hierarchy-cleanup.py docs/mrm-hierarchy-cleanup.md
EXTRA_DIST += tests/automated/check-mrm-resolved-path-output.py docs/mrm-resolved-path-output.md
EXTRA_DIST += tests/automated/check-mrm-context-realloc.py docs/mrm-context-realloc.md
EXTRA_DIST += tests/automated/check-mrm-error-allocation.py docs/mrm-error-allocation.md
EXTRA_DIST += tests/automated/check-mrm-buffer-pool-size.py docs/mrm-buffer-pool-size.md
EXTRA_DIST += tests/automated/check-mrm-writer-vector-input.py docs/mrm-writer-vector-input.md
EXTRA_DIST += tests/automated/check-mrm-writer-scalar-input.py docs/mrm-writer-scalar-input.md
EXTRA_DIST += tests/automated/check-mrm-topmost-index.py docs/mrm-topmost-index.md
EXTRA_DIST += tests/automated/check-mrm-cstring-stream-cleanup.py docs/mrm-cstring-stream-cleanup.md
EXTRA_DIST += tests/automated/check-mrm-callback-context-init.py docs/mrm-callback-context-init.md
EXTRA_DIST += tests/automated/check-mrm-callback-name-allocation.py docs/mrm-callback-name-allocation.md
EXTRA_DIST += tests/automated/check-mrm-literal-context-init.py docs/mrm-literal-context-init.md
EXTRA_DIST += tests/automated/check-mrm-child-context-init.py docs/mrm-child-context-init.md
EXTRA_DIST += tests/automated/check-mrm-widget-context-init.py docs/mrm-widget-context-init.md
EXTRA_DIST += tests/automated/check-mrm-literal-context-retain.py docs/mrm-literal-context-retain.md
EXTRA_DIST += tests/automated/check-mrm-callback-cleanup-bounds.py docs/mrm-callback-cleanup-bounds.md
EXTRA_DIST += tests/automated/check-mrm-callback-cleanup-bounds.py docs/mrm-callback-cleanup-bounds.md
EXTRA_DIST += tests/automated/check-mrm-writer-comment-input.py docs/mrm-writer-comment-input.md
EXTRA_DIST += tests/automated/check-mrm-writer-class-input.py docs/mrm-writer-class-input.md
EXTRA_DIST += tests/automated/check-mrm-writer-callback-routine.py docs/mrm-writer-callback-routine.md
EXTRA_DIST += tests/automated/check-mrm-writer-resource-callback-routine.py docs/mrm-writer-resource-callback-routine.md
EXTRA_DIST += tests/automated/check-mrm-writer-child-index.py docs/mrm-writer-child-index.md
EXTRA_DIST += tests/automated/check-mrm-writer-extra-args-validation.py docs/mrm-writer-extra-args-validation.md
EXTRA_DIST += tests/automated/check-mrm-writer-creation-callback-validation.py docs/mrm-writer-creation-callback-validation.md
EXTRA_DIST += tests/automated/check-mrm-writer-arglist-validation.py docs/mrm-writer-arglist-validation.md
EXTRA_DIST += tests/automated/check-mrm-writer-compressed-tag-validation.py docs/mrm-writer-compressed-tag-validation.md
EXTRA_DIST += tests/automated/check-mrm-writer-uncompressed-tag-validation.py docs/mrm-writer-uncompressed-tag-validation.md
EXTRA_DIST += tests/automated/check-mrm-writer-arg-value-validation.py docs/mrm-writer-arg-value-validation.md
EXTRA_DIST += tests/automated/check-mrm-writer-resource-ref-validation.py docs/mrm-writer-resource-ref-validation.md
EXTRA_DIST += tests/automated/check-mrm-writer-char8-vector-validation.py docs/mrm-writer-char8-vector-validation.md
EXTRA_DIST += tests/automated/check-mrm-writer-cstring-vector-validation.py docs/mrm-writer-cstring-vector-validation.md
EXTRA_DIST += tests/automated/check-mrm-writer-callback-arg-validation.py docs/mrm-writer-callback-arg-validation.md
EXTRA_DIST += tests/automated/check-mrm-writer-child-validation.py docs/mrm-writer-child-validation.md
EXTRA_DIST += tests/automated/check-mrm-writer-children-validation.py docs/mrm-writer-children-validation.md
EXTRA_DIST += tests/automated/check-mrm-writer-arg-value-rebind.py docs/mrm-writer-arg-value-rebind.md
EXTRA_DIST += tests/automated/check-mrm-writer-cstring-vector-rebind.py docs/mrm-writer-cstring-vector-rebind.md
EXTRA_DIST += tests/automated/check-mrm-callback-item-binding.py docs/mrm-callback-item-binding.md
EXTRA_DIST += tests/automated/check-mrm-class-comment-validation.py docs/mrm-class-comment-validation.md
EXTRA_DIST += tests/automated/check-mrm-writer-char8-vector-rebind.py docs/mrm-writer-char8-vector-rebind.md
EXTRA_DIST += tests/automated/check-mrm-widget-name-allocation.py docs/mrm-widget-name-allocation.md
EXTRA_DIST += tests/automated/check-mrm-reader-text-vector-validation.py docs/mrm-reader-text-vector-validation.md
EXTRA_DIST += tests/automated/check-mrm-safe-copy-null-sources.py docs/mrm-safe-copy-null-sources.md
EXTRA_DIST += tests/automated/check-mrm-convert-vector-validation.py docs/mrm-convert-vector-validation.md
EXTRA_DIST += tests/automated/check-mrm-convert-font-validation.py docs/mrm-convert-font-validation.md
EXTRA_DIST += tests/automated/check-mrm-convert-char8-null.py docs/mrm-convert-char8-null.md
EXTRA_DIST += tests/automated/check-mrm-convert-pointer-null.py docs/mrm-convert-pointer-null.md
EXTRA_DIST += tests/automated/check-mrm-convert-primitive-pointer-validation.py docs/mrm-convert-primitive-pointer-validation.md
EXTRA_DIST += tests/automated/check-mrm-callback-fixup-validation.py docs/mrm-callback-fixup-validation.md
EXTRA_DIST += tests/automated/check-mrm-callback-routine-offset.py docs/mrm-callback-routine-offset.md
EXTRA_DIST += tests/automated/check-mrm-fixup-widechar-null.py docs/mrm-fixup-widechar-null.md
EXTRA_DIST += tests/automated/check-mrm-createarg-color-fontfixup.py docs/mrm-createarg-color-fontfixup.md
EXTRA_DIST += tests/automated/check-mrm-createarg-resource-null.py docs/mrm-createarg-resource-null.md
EXTRA_DIST += tests/automated/check-mrm-fixup-fontlist-validation.py docs/mrm-fixup-fontlist-validation.md
EXTRA_DIST += tests/automated/check-mrm-arglist-reader-counts.py docs/mrm-arglist-reader-counts.md
EXTRA_DIST += tests/automated/check-mrm-arglist-offset-bounds.py docs/mrm-arglist-offset-bounds.md
EXTRA_DIST += tests/automated/check-mrm-widget-offset-bounds.py docs/mrm-widget-offset-bounds.md
EXTRA_DIST += tests/automated/check-mrm-create-tree-children-bounds.py docs/mrm-create-tree-children-bounds.md
EXTRA_DIST += tests/automated/check-mrm-create-tree-child-index-bounds.py docs/mrm-create-tree-child-index-bounds.md
EXTRA_DIST += tests/automated/check-mrm-arglist-record-bounds.py docs/mrm-arglist-record-bounds.md
EXTRA_DIST += tests/automated/check-mrm-widget-name-record-bounds.py docs/mrm-widget-name-record-bounds.md
EXTRA_DIST += tests/automated/check-mrm-callback-record-bounds.py docs/mrm-callback-record-bounds.md
EXTRA_DIST += tests/automated/check-mrm-set-widget-class-bounds.py docs/mrm-set-widget-class-bounds.md
EXTRA_DIST += tests/automated/check-mrm-callback-routine-string-bounds.py docs/mrm-callback-routine-string-bounds.md
EXTRA_DIST += tests/automated/check-mrm-arg-value-offset-bounds.py docs/mrm-arg-value-offset-bounds.md
EXTRA_DIST += tests/automated/check-mrm-arg-callback-record-bounds.py docs/mrm-arg-callback-record-bounds.md
EXTRA_DIST += tests/automated/check-mrm-arg-tag-string-bounds.py docs/mrm-arg-tag-string-bounds.md
EXTRA_DIST += tests/automated/check-mrm-callback-value-offset-bounds.py docs/mrm-callback-value-offset-bounds.md
EXTRA_DIST += tests/automated/check-mrm-resource-record-bounds.py docs/mrm-resource-record-bounds.md
EXTRA_DIST += tests/automated/check-mrm-resource-tag-lookup-failure.py docs/mrm-resource-tag-lookup-failure.md
EXTRA_DIST += tests/automated/check-mrm-related-tag-lookup-failure.py docs/mrm-related-tag-lookup-failure.md
EXTRA_DIST += tests/automated/check-mrm-related-tag-diagnostic.py docs/mrm-related-tag-diagnostic.md
EXTRA_DIST += tests/automated/check-mrm-icon-pixargs-bounds.py docs/mrm-icon-pixargs-bounds.md
EXTRA_DIST += tests/automated/check-mrm-pixmap-tag-failure.py docs/mrm-pixmap-tag-failure.md
EXTRA_DIST += tests/automated/check-mrm-related-count-type.py docs/mrm-related-count-type.md
EXTRA_DIST += tests/automated/check-mrm-compression-inputs.py docs/mrm-compression-inputs.md
EXTRA_DIST += tests/automated/check-mrm-table-fixup-inputs.py docs/mrm-table-fixup-inputs.md
EXTRA_DIST += tests/automated/check-mrm-table-offset-bounds.py docs/mrm-table-offset-bounds.md
EXTRA_DIST += tests/automated/check-mrm-openhierarchy-os-input.py docs/mrm-openhierarchy-os-input.md
EXTRA_DIST += tests/automated/check-mrm-hierarchy-name-inputs.py docs/mrm-hierarchy-name-inputs.md
EXTRA_DIST += tests/automated/check-mrm-hget-output-pointer.py docs/mrm-hget-output-pointer.md
EXTRA_DIST += tests/automated/check-mrm-idb-indexed-inputs.py docs/mrm-idb-indexed-inputs.md
EXTRA_DIST += tests/automated/check-mrm-find-indexed-output.py docs/mrm-find-indexed-output.md
EXTRA_DIST += tests/automated/check-mrm-rid-context.py docs/mrm-rid-context.md
EXTRA_DIST += tests/automated/check-mrm-resource-id-output.py docs/mrm-resource-id-output.md
EXTRA_DIST += tests/automated/check-mrm-open-name-input.py docs/mrm-open-name-input.md
EXTRA_DIST += tests/automated/check-mrm-memory-close.py docs/mrm-memory-close.md
EXTRA_DIST += tests/automated/check-mrm-open-output-init.py docs/mrm-open-output-init.md
EXTRA_DIST += tests/automated/check-mrm-buffer-null-source.py docs/mrm-buffer-null-source.md
EXTRA_DIST += tests/automated/check-mrm-decommit-record.py docs/mrm-decommit-record.md
EXTRA_DIST += tests/automated/check-mrm-decommit-all-failure.py docs/mrm-decommit-all-failure.md
EXTRA_DIST += tests/automated/check-mrm-close-decommit-failure.py docs/mrm-close-decommit-failure.md
EXTRA_DIST += tests/automated/check-mrm-buffer-output-init.py docs/mrm-buffer-output-init.md
EXTRA_DIST += tests/automated/check-mrm-resource-id-output-init.py docs/mrm-resource-id-output-init.md
EXTRA_DIST += tests/automated/check-mrm-open-write-output.py docs/mrm-open-write-output.md
EXTRA_DIST += tests/automated/check-mrm-writer-context-validation.py docs/mrm-writer-context-validation.md
EXTRA_DIST += tests/automated/check-mrm-writer-lookup-errors.py docs/mrm-writer-lookup-errors.md
EXTRA_DIST += tests/automated/check-mrm-data-context-null.py docs/mrm-data-context-null.md
EXTRA_DIST += tests/automated/check-mrm-context-valid-macro.py docs/mrm-context-valid-macro.md
EXTRA_DIST += tests/automated/check-mrm-validation-macros.py docs/mrm-validation-macros.md
EXTRA_DIST += tests/automated/check-mrm-widget-read-null.py docs/mrm-widget-read-null.md
EXTRA_DIST += tests/automated/check-mrm-interface-module-null.py docs/mrm-interface-module-null.md

EXTRA_DIST += tests/automated/check-mrm-fetch-literal-output.py docs/mrm-fetch-literal-output.md

EXTRA_DIST += tests/automated/check-mrm-literal-convenience-outputs.py docs/mrm-literal-convenience-outputs.md

EXTRA_DIST += tests/automated/check-mrm-icon-converter-inputs.py docs/mrm-icon-converter-inputs.md

EXTRA_DIST += tests/automated/check-mrm-icon-colortable-validation.py docs/mrm-icon-colortable-validation.md

EXTRA_DIST += tests/automated/check-mrm-icon-colortable-count.py docs/mrm-icon-colortable-count.md

EXTRA_DIST += tests/automated/check-mrm-icon-loader-inputs.py docs/mrm-icon-loader-inputs.md

EXTRA_DIST += tests/automated/check-xm-pixmap-hotspot-output.py docs/xm-pixmap-hotspot-output.md

EXTRA_DIST += tests/automated/check-xm-get-image-output.py docs/xm-get-image-output.md

EXTRA_DIST += tests/automated/check-xm-get-image-screen.py docs/xm-get-image-screen-validation.md

EXTRA_DIST += tests/automated/check-xme-pixmap-data-screen.py docs/xme-pixmap-data-screen-validation.md

EXTRA_DIST += tests/automated/check-xm-cache-allocation-failure.py docs/xm-cache-allocation-failure.md

EXTRA_DIST += tests/automated/check-xm-image-cache-init-failure.py docs/xm-image-cache-init-failure.md

EXTRA_DIST += tests/automated/check-xm-pixmap-table-initialization.py docs/xm-pixmap-table-initialization.md

EXTRA_DIST += tests/automated/check-xm-image-table-initialization.py docs/xm-image-table-initialization.md

EXTRA_DIST += tests/automated/check-xm-scaled-pixmap-entry.py docs/xm-scaled-pixmap-entry-allocation.md

EXTRA_DIST += tests/automated/check-xm-get-pixmap-data-validation.py docs/xm-get-pixmap-data-validation.md

EXTRA_DIST += tests/automated/check-xm-pixmap-set-initialization.py docs/xm-pixmap-set-initialization.md

EXTRA_DIST += tests/automated/check-xm-gc-cache-failure.py docs/xm-gc-cache-failure.md

EXTRA_DIST += tests/automated/check-xm-gc-callsite-rollback.py docs/xm-gc-callsite-rollback.md

EXTRA_DIST += tests/automated/check-xm-install-image-failure.py docs/xm-install-image-failure.md

EXTRA_DIST += tests/automated/check-xm-create-image-null.py docs/xm-create-image-null.md

EXTRA_DIST += tests/automated/check-xm-image-file-open-cleanup.py docs/xm-image-file-open-cleanup.md

EXTRA_DIST += tests/automated/check-xm-clean-pixmap-cache-null.py docs/xm-clean-pixmap-cache-null.md

EXTRA_DIST += tests/automated/check-xm-scaled-pixmap-table-null.py docs/xm-scaled-pixmap-table-null.md

EXTRA_DIST += tests/automated/check-xm-pixmap-compare-null.py docs/xm-pixmap-compare-null.md

EXTRA_DIST += tests/automated/check-xm-image-cache-string-null.py docs/xm-image-cache-string-null.md

EXTRA_DIST += tests/automated/check-xm-scratch-pixmap-failure.py docs/xm-scratch-pixmap-failure.md

EXTRA_DIST += tests/automated/check-xm-stippled-pixmap-failure.py docs/xm-stippled-pixmap-failure.md

EXTRA_DIST += tests/automated/check-xm-stippled-pixmap-race.py docs/xm-stippled-pixmap-race.md

EXTRA_DIST += tests/automated/check-xm-geometry-compare.py docs/xm-geometry-compare.md

EXTRA_DIST += tests/automated/check-xm-pixmap-data-optional-outputs.py docs/xm-pixmap-data-optional-outputs.md

EXTRA_DIST += tests/automated/check-xm-screen-lifecycle-failure.py docs/xm-screen-lifecycle-failure.md

EXTRA_DIST += tests/automated/check-xm-null-cursor-failure.py docs/xm-null-cursor-failure.md

EXTRA_DIST += tests/automated/check-xm-query-cursor-size-null.py docs/xm-query-cursor-size-null.md

EXTRA_DIST += tests/automated/check-xm-extutil-null.py docs/xm-extutil-null.md

EXTRA_DIST += tests/automated/check-xm-extutil-widget-null.py docs/xm-extutil-widget-null.md

EXTRA_DIST += tests/automated/check-xm-extutil-geometry-null.py docs/xm-extutil-geometry-null.md

EXTRA_DIST += tests/automated/check-xm-setvalues-args-null.py docs/xm-setvalues-args-null.md

EXTRA_DIST += tests/automated/check-xm-list-query-null.py docs/xm-list-query-null.md

EXTRA_DIST += tests/automated/check-xm-list-readonly-null.py docs/xm-list-readonly-null.md

EXTRA_DIST += tests/automated/check-xm-list-posbounds-null.py docs/xm-list-posbounds-null.md

EXTRA_DIST += tests/automated/check-xm-list-selection-null.py docs/xm-list-selection-null.md

EXTRA_DIST += tests/automated/check-xm-list-mutator-null.py docs/xm-list-mutator-null.md

EXTRA_DIST += tests/automated/check-xm-scrollbar-api-null.py docs/xm-scrollbar-api-null.md

EXTRA_DIST += tests/automated/check-xm-scale-api-null.py docs/xm-scale-api-null.md

EXTRA_DIST += tests/automated/check-xm-toggle-api-null.py docs/xm-toggle-api-null.md

EXTRA_DIST += tests/automated/check-xm-command-api-null.py docs/xm-command-api-null.md

EXTRA_DIST += tests/automated/check-xm-combobox-api-null.py docs/xm-combobox-api-null.md

EXTRA_DIST += tests/automated/check-xm-messagebox-child-null.py docs/xm-messagebox-child-null.md

EXTRA_DIST += tests/automated/check-xm-selectionbox-child-null.py docs/xm-selectionbox-child-null.md

EXTRA_DIST += tests/automated/check-xm-filesb-api-null.py docs/xm-filesb-api-null.md

EXTRA_DIST += tests/automated/check-xm-color-api-null.py docs/xm-color-api-null.md

EXTRA_DIST += tests/automated/check-xm-mainw-api-null.py docs/xm-mainw-api-null.md

EXTRA_DIST += tests/automated/check-xm-notebook-page-null.py docs/xm-notebook-page-null.md

EXTRA_DIST += tests/automated/check-xm-protocols-shell-null.py docs/xm-protocols-shell-null.md

EXTRA_DIST += tests/automated/check-xm-rcmenu-api-null.py docs/xm-rcmenu-api-null.md

EXTRA_DIST += tests/automated/check-xm-display-api-null.py docs/xm-display-api-null.md

EXTRA_DIST += tests/automated/check-xm-dropdown-api-null.py docs/xm-dropdown-api-null.md

EXTRA_DIST += tests/automated/check-xm-gadgetutil-null.py docs/xm-gadgetutil-null.md

EXTRA_DIST += tests/automated/check-xm-gmutils-query-null.py docs/xm-gmutils-query-null.md

EXTRA_DIST += tests/automated/check-xm-chcolor-null.py docs/xm-chcolor-null.md

EXTRA_DIST += tests/automated/check-xm-paned-getpanes-null.py docs/xm-paned-getpanes-null.md

EXTRA_DIST += tests/automated/check-xm-hierarchy-api-null.py docs/xm-hierarchy-api-null.md

EXTRA_DIST += tests/automated/check-xm-resind-null.py docs/xm-resind-null.md

EXTRA_DIST += tests/automated/check-xm-default-render-null.py docs/xm-default-render-null.md

EXTRA_DIST += tests/automated/check-notebook-geometry-init.py docs/notebook-geometry.md

EXTRA_DIST += tests/automated/check-fast-subclass-sequencing.py docs/fast-subclass.md

EXTRA_DIST += tests/automated/check-rowcolumn-menupost.py docs/rowcolumn-menupost.md

EXTRA_DIST += tests/automated/check-xcontext-zero.py docs/xcontext-null.md

EXTRA_DIST += tests/automated/check-mwm-bitmap-path-bounds.py docs/mwm-bitmap-path-bounds.md

EXTRA_DIST += tests/automated/check-protocol-list-bounds.py docs/protocol-list-bounds.md

EXTRA_DIST += tests/automated/check-latin-lower-bounds.py docs/latin-lower-bounds.md

EXTRA_DIST += tests/automated/check-font-family-bounds.py docs/font-family-bounds.md

EXTRA_DIST += tests/automated/check-reference-overview-links.py docs/reference-overview.md
