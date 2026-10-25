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
EXTRA_DIST += tests/automated/check-dialog-name-bounds.py docs/dialog-name-bounds.md
EXTRA_DIST += tests/automated/check-editres-error-bounds.py docs/editres-error-bounds.md
EXTRA_DIST += tests/automated/check-spinbox-name-bounds.py docs/spinbox-name-bounds.md
EXTRA_DIST += tests/automated/check-column-label-name-bounds.py docs/column-label-name-bounds.md
EXTRA_DIST += tests/automated/check-xmos-init-path-bounds.py docs/xmos-init-path-bounds.md
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
EXTRA_DIST += tests/automated/check-xlibre-dpi-contract.c tests/automated/check-xlibre-output-selection.c tests/automated/run-xlibre-dpi-contract.py docs/xlibre-dpi-contract.md
EXTRA_DIST += tests/automated/check-textdest-location-lifetime.py docs/motif-textdest-location-lifetime.md
EXTRA_DIST += tests/automated/check-textsel-timestamp-allocation.py docs/motif-textsel-timestamp.md
EXTRA_DIST += tests/automated/check-textdrop-record-allocation.py docs/motif-textdrop-record.md
EXTRA_DIST += tests/automated/check-textsel-string-value-allocation.py docs/motif-textsel-string-value.md
EXTRA_DIST += tests/automated/check-textsel-compound-value-allocation.py docs/motif-textsel-compound-value.md
EXTRA_DIST += tests/automated/check-textsel-utf8-value-allocation.py docs/motif-textsel-utf8-value.md
EXTRA_DIST += tests/automated/check-textsel-targets-allocation.py docs/motif-textsel-targets.md
EXTRA_DIST += tests/automated/check-textsel-text-value.py docs/motif-textsel-text-value.md
EXTRA_DIST += tests/automated/check-filesb-value-buffer-allocation.py docs/motif-filesb-value-buffer.md
EXTRA_DIST += tests/automated/check-filesb-item-filter-allocation.py docs/motif-filesb-item-filter.md
EXTRA_DIST += tests/automated/check-filesb-resource-filter-mask.py docs/motif-filesb-resource-filter-mask.md
EXTRA_DIST += tests/automated/check-filesb-filter-mask-update.py docs/motif-filesb-filter-mask-update.md
EXTRA_DIST += tests/automated/check-filesb-qualified-mask-allocation.py docs/motif-filesb-qualified-mask.md
EXTRA_DIST += tests/automated/check-filesb-directory-list-allocation.py docs/motif-filesb-directory-list-allocation.md
EXTRA_DIST += tests/automated/check-filesb-list-allocation.py docs/motif-filesb-list-allocation.md
EXTRA_DIST += tests/automated/check-filesb-empty-text.py docs/motif-filesb-empty-text.md
EXTRA_DIST += tests/automated/check-filesb-convert-allocation.py docs/motif-filesb-convert.md
EXTRA_DIST += tests/automated/check-textsel-primary-state-allocation.py docs/motif-textsel-primary-state.md
EXTRA_DIST += tests/automated/check-list-drag-target-allocation.py docs/motif-list-drag-targets.md
EXTRA_DIST += tests/automated/check-list-translation-allocation.py docs/motif-list-translations.md
EXTRA_DIST += tests/automated/check-list-element-allocation.py docs/motif-list-elements.md
EXTRA_DIST += tests/automated/check-xmstring-table-parse-allocation.py docs/motif-xmstring-table-parse.md
EXTRA_DIST += tests/automated/check-xmstring-table-unparse-allocation.py docs/motif-xmstring-table-unparse.md
EXTRA_DIST += tests/automated/check-iconfile-hash-fallback.py docs/motif-iconfile-cache-table.md
EXTRA_DIST += tests/automated/check-iconfile-cache-copies.py docs/motif-iconfile-cache-copies.md
EXTRA_DIST += tests/automated/check-iconfile-leaf-allocation.py docs/motif-iconfile-cache-leaf.md
EXTRA_DIST += tests/automated/check-iconfile-dir-allocation.py docs/motif-iconfile-cache-directory.md
EXTRA_DIST += tests/automated/check-iconfile-key-allocation.py docs/motif-iconfile-cache-key.md
EXTRA_DIST += tests/automated/check-iconfile-entry-allocation.py docs/motif-iconfile-cache-entry.md
EXTRA_DIST += tests/automated/check-iconbutton-stipple-allocation.py docs/motif-iconbutton-stipple.md
EXTRA_DIST += tests/automated/check-xmstring-table-concat-input.py docs/motif-xmstring-table-concat.md
EXTRA_DIST += tests/automated/check-xmstring-rendition-input.py docs/motif-xmstring-rendition.md
EXTRA_DIST += tests/automated/check-xmstring-table-split.py docs/motif-xmstring-table-split.md
EXTRA_DIST += tests/automated/check-xmstring-tablist-create.py docs/motif-xmstring-tablist.md
EXTRA_DIST += tests/automated/check-xmstring-tab-growth-allocation.py docs/motif-xmstring-tab-growth.md
EXTRA_DIST += tests/automated/check-scrollframe-nav-growth.py docs/motif-scrollframe-nav-growth.md
EXTRA_DIST += tests/automated/check-scrollframe-add-input.py docs/motif-scrollframe-add-input.md
EXTRA_DIST += tests/automated/check-scrollframe-remove-input.py docs/motif-scrollframe-remove-input.md
EXTRA_DIST += tests/automated/check-scrollframe-update-guards.py docs/motif-scrollframe-update-guards.md
EXTRA_DIST += tests/automated/check-resconvert-token-allocation.py docs/motif-resconvert-token.md
EXTRA_DIST += tests/automated/check-txtpropcv-generated-strings.py docs/motif-txtpropcv-generated-strings.md
EXTRA_DIST += tests/automated/check-txtpropcv-plain-table.py docs/motif-txtpropcv-plain-table.md
EXTRA_DIST += tests/automated/check-txtpropcv-empty-plain-table.py docs/motif-txtpropcv-empty-plain-table.md
EXTRA_DIST += tests/automated/check-txtpropcv-asn1-table.py docs/motif-txtpropcv-asn1-table.md
EXTRA_DIST += tests/automated/check-txtpropcv-empty-asn1-table.py docs/motif-txtpropcv-empty-asn1-table.md
EXTRA_DIST += tests/automated/check-txtpropcv-segment-copy.py docs/motif-txtpropcv-segment-copy.md
EXTRA_DIST += tests/automated/check-txtpropcv-empty-segment.py docs/motif-txtpropcv-empty-segment.md
EXTRA_DIST += tests/automated/check-txtpropcv-segment-allocation.py docs/motif-txtpropcv-segment.md
EXTRA_DIST += tests/automated/check-txtpropcv-growth-allocation.py docs/motif-txtpropcv-growth.md
EXTRA_DIST += tests/automated/check-txtpropcv-value-allocation.py docs/motif-txtpropcv-value.md
EXTRA_DIST += tests/automated/check-txtpropcv-concat-allocation.py docs/motif-txtpropcv-concat.md
EXTRA_DIST += tests/automated/check-txtpropcv-useable-table.py docs/motif-txtpropcv-useable-table.md
EXTRA_DIST += tests/automated/check-txtpropcv-final-allocation.py docs/motif-txtpropcv-final.md
EXTRA_DIST += tests/automated/check-txtpropcv-asn1-allocation.py docs/motif-txtpropcv-asn1.md
EXTRA_DIST += tests/automated/check-txtpropcv-compound-table.py docs/motif-txtpropcv-compound-table.md
EXTRA_DIST += tests/automated/check-txtpropcv-string-table-allocation.py docs/motif-txtpropcv-string-table.md
EXTRA_DIST += tests/automated/check-iconv-output-allocation.py docs/motif-resencod-iconv.md
EXTRA_DIST += tests/automated/check-resencod-prefix-allocation.py docs/motif-resencod-prefix.md
EXTRA_DIST += tests/automated/check-resencod-octet-allocation.py docs/motif-resencod-octets.md
EXTRA_DIST += tests/automated/check-resencod-registry-allocation.py docs/motif-resencod-registry.md
EXTRA_DIST += tests/automated/check-resencod-context-allocation.py docs/resencod-direction-stack.md
EXTRA_DIST += tests/automated/check-resencod-direction-stack.py
EXTRA_DIST += tests/automated/check-resencod-segment-allocation.py docs/motif-resencod-segments.md
EXTRA_DIST += tests/automated/check-icon-cache-growth-allocation.py docs/icon-directory-cache.md
EXTRA_DIST += tests/automated/check-traversal-node-allocation.py docs/traversal-node-allocation.md
EXTRA_DIST += tests/automated/check-traversal-row-allocation.py docs/traversal-row-allocation.md
EXTRA_DIST += tests/automated/check-traversal-shrink-allocation.py
EXTRA_DIST += tests/automated/check-traversal-tablist-allocation.py docs/traversal-tab-list.md
EXTRA_DIST += tests/automated/check-clip-window-translation-allocation.py docs/clip-window-translations.md
EXTRA_DIST += tests/automated/check-hierarchy-growth-allocation.py docs/hierarchy-allocation.md
EXTRA_DIST += tests/automated/check-scrollframe-callbacks.py docs/motif-scrollframe-callbacks.md
EXTRA_DIST += tests/automated/check-container-trait-callbacks.py docs/motif-container-trait-callbacks.md
EXTRA_DIST += tests/automated/check-text-source-progress.py docs/motif-text-source-progress.md
EXTRA_DIST += tests/automated/check-navigator-slider-callbacks.py docs/motif-navigator-slider-callbacks.md
EXTRA_DIST += tests/automated/check-combobox-text-trait.py docs/motif-combobox-text-trait.md
EXTRA_DIST += tests/automated/check-icong-container-trait.py docs/motif-icong-container-trait.md
EXTRA_DIST += tests/automated/check-spinb-text-trait.py docs/motif-spinb-text-trait.md
EXTRA_DIST += tests/automated/check-textout-navigator-trait.py docs/motif-textout-navigator-trait.md
EXTRA_DIST += tests/automated/check-text-textual-trait.py docs/motif-text-textual-trait.md
EXTRA_DIST += tests/automated/check-selectionbox-compound-text.py docs/motif-selectionbox-compound-text.md
EXTRA_DIST += tests/automated/check-xmstring-segment-tags.py docs/motif-xmstring-segment-tags.md
EXTRA_DIST += tests/automated/check-colors-loader-allocation.py docs/motif-colors-loader-allocation.md
EXTRA_DIST += tests/automated/check-fonts-family-allocation.py docs/motif-fonts-family-allocation.md
EXTRA_DIST += tests/automated/check-fonts-size-list-allocation.py docs/motif-fonts-size-list-allocation.md
EXTRA_DIST += tests/automated/check-combobox-child-args.py docs/motif-combobox-child-args.md
EXTRA_DIST += tests/automated/check-txtpropcv-single-allocation.py docs/motif-txtpropcv-single.md
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
EXTRA_DIST += tests/cpp/motif_support.hpp tests/cpp/support_lifecycle.cpp \
              tests/automated/run-cpp-support-prototype.py \
              docs/cpp-support-library-prototype.md
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
EXTRA_DIST += tests/automated/check-mrm-writer-resource-ref-validation.py docs/mrm-writer-resource-ref-validation.md
