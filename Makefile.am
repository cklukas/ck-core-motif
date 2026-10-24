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
EXTRA_DIST += tests/automated/check-string-table-conversion.c tests/automated/run-string-table-regressions.py docs/string-table-conversion.md
EXTRA_DIST += tests/automated/check-cardinal-list-conversion.c tests/automated/run-cardinal-list-regressions.py docs/cardinal-list-conversion.md
EXTRA_DIST += tests/automated/check-xmstring-table-conversion.c tests/automated/run-xmstring-table-regressions.py docs/xmstring-table-conversion.md
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
