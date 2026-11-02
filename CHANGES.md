# Changelog

## 2026-11-02 — Demo appearance and usable Airport flights

- Launch demos with the selected theme, density and scale; installed copies win
  over build-tree copies. Add `--open-demo` for direct gallery startup.
- Repaint wrapped demo descriptions after viewport resizing.
- Let Periodic respect inherited theme/density resources.
- Rework Airport into two usable airports with bounded gates, normal dragging,
  click/Fly and keyboard controls, real flight feedback, Help and safe transfers.
- Retire visual appearance/scale state through phase-one lifetime hooks, fixing
  the Periodic hierarchy teardown crash and surviving callback replacement.
- Add native pointer/keyboard, cross-process transfer and appearance qualification.
  See [Airport](docs/airport-demo.md) and
  [Demo launch appearance](docs/demo-launch-appearance.md).

## 2026-11-02 — Modern macOS UI audit

- Complete measured date/time/calendar controls, readable progress text, Xft
  DataField text, status/link colors, flat frames and disabled-to-normal tab
  contrast; preserve explicit application styling and classic appearance.
- Honor disjoint compound-string clips, preserving progress-text contrast.
- Make automatic scrolling and callback teardown safe when notifications remove
  navigators or immediately destroy their owning frame.
- Add whole-gallery light/dark state captures, font/clipping inventory and native
  regressions. See [Modern macOS UI audit](docs/modern-macos-ui-audit.md).

## 2026-11-02 — CDE WSM MWM-window lookup

- Use CDE's declared `_DtGetMwmWindow` helper for optional WSM startup rather
  than an obsolete session-manager symbol, preserving the established lookup
  and Xlib-status contract. See
  [MWM CDE workspace-manager window lookup](docs/mwm-cde-wsm-window-helper.md).

## 2026-11-02 — Xft color-cache allocation safety

- Keep the optional Xft color cache intact if growth fails: preserve its prior
  pointer and count while returning the current queried color for rendering.
- Extend the focused allocation regression; see
  [Render-table growth bounds](docs/xm-render-table-growth.md).

## 2026-11-01 — Text scratch line-table allocation safety

- Make narrow and multibyte hard-line update buffers failure-atomic and bound
  their capacity growth; a failed temporary allocation leaves reflow pending.
- Extend the focused source coverage; see
  [Text reflow scratch-table allocation](docs/text-reflow-scratch-table.md).

## 2026-11-01 — DataField highlight allocation safety

- Make internal highlight-list growth transactional: allocation failure or an
  unrepresentable Xt allocation size preserves the existing field state.
- Add a focused source regression and document the failure contract; see
  [DataField highlight-list allocation](docs/datafield-highlight-allocation.md).

## 2026-11-01 — Modern Breadcrumb Bar

- Use directional chevrons and comfortable spacing in Modern profiles.
- Keep the current label on the normal surface with an accent underline.
- Preserve explicit separators and Classic styling; refresh live palette colors
  and align RTL hit-test metrics with rendering.
- Qualify 24 native theme/scale/direction configurations, existing behavior,
  installed demos and the actual XQuartz window; see the
  [validation record](docs/validation/macos-breadcrumb-20261101.json) and
  [Breadcrumb Bar contract](docs/breadcrumb-bar.md).

## 2026-11-01 — Anchored popup examples and Modern Flyout layout

- Remove the Flyout/TeachingTip preview rectangle by keeping popup handles hidden.
- Anchor to the trigger layout; give Modern Flyout readable padding, a small gap
  and placement above the trigger near the screen bottom.
- Fix deferred popup destruction and expose `--open-example` for these examples.
- Qualify 397 focused native checks, existing gallery/dialog behavior, installed
  demos and the actual XQuartz window; see the
  [validation record](docs/validation/macos-flyout-20261101.json) and
  [Flyout contract](docs/flyout.md).

## 2026-11-01 — Smooth font previews and stable live layout

- Use antialiased Xft outlines for Modern Font Selector previews, preserving
  the selected family, style and point size across theme and scale changes.
- Align Family and Size editor baselines with their labels.
- Complete segmented layout after appearance changes so Icons remains visible.
- Preserve Classic rendering and explicit control dimensions. See the
  [behavior and native regression](docs/font-selector-modern.md).
- Qualify 351 focused native checks, existing geometry/rendering regressions,
  34 installed demos and the actual XQuartz gallery; see the
  [validation record](docs/validation/macos-font-selector-20261101.json).

## 2026-11-01 — Usable Dialog Shell gallery example

- Fit the complete launcher label and demonstrate Dialog Shell with populated
  dialog content, a Close action and the managed-child lifecycle.
- Own and center the example on the gallery window, preserving screen bounds,
  and qualify repeated open/close behavior across themes and scales.
- Add `--open-example` for inspection and native page acceptance. See
  [shell examples](docs/gallery-shell-examples.md).
- Qualify 18 configurations, 54 open/close cycles and the actual XQuartz
  window; see the [validation record](docs/validation/macos-dialog-shell-20261101.json).

## 2026-11-01 — Modern Cascade buttons

- Give Cascade widget and gadget modern padding, flat selected surfaces and
  clean generated submenu chevrons, preserving Classic and explicit resources.
- Make theme ownership queries include live gadgets, so their selected colors
  follow the palette while explicit application resources remain authoritative.
- Reuse supplied gallery menu bars, create one for generic hosts, and remove
  the modern menu bar's default outer shadow. See the
  [appearance contract](docs/cascade-button-appearance.md).
- Qualify 186 native cases and the actual XQuartz window; see the
  [validation record](docs/validation/macos-cascade-20261101.json).

## 2026-11-01 — Modern Avatar rendering

- Render modern Avatars as smooth tinted circles with larger initials and
  corners matching the parent surface, including tiled backgrounds.
- Preserve Classic rendering, image cropping and explicit application colors
  and fonts; release cached Xft targets before freeing initials pixmaps.
- Add native pixel, theme, scale and recreation coverage; see [Avatar](docs/avatar.md).

## 2026-11-01 — Connected Outline branches

- Make Outline lines meet visible node borders through their focus margins,
  matching Tree's endpoint behavior, while preserving active focus rings.
- Keep endpoints correct after repaint, resize and collapse/expand; scale modern
  indentation for larger disclosure buttons. See [the contract](docs/outline-connections.md).

## 2026-11-01 — Modern Container rows and gadget lifetime

- Fix stale appearance state when Xt reuses a destroyed gadget's address;
  revisited Container previews retain modern colors and flat styling.
- Improve modern Container padding and icon spacing and remove connector lines,
  while preserving Classic styling and application overrides.
- Qualify repeated gallery creation, rendered surfaces, vector icons and live
  theme/scale changes; see the [native record](docs/validation/macos-container-20261101.json).

## 2026-11-01 — Modern dialog actions and complete Mac demos

- Size short modern dialog actions to at least 80 logical pixels (64 compact),
  preserving long labels, Classic geometry and the natural-size opt-out.
- Correct default-action highlight reporting and a ColorSelector declaration
  rejected by current Clang.
- Initialize optional workspace demo data pointers; native Mac copy/move/link
  integration now passes instead of trapping when icon data is absent.
- Include all 34 gallery demo executables and runtime resources in the native
  SVG-enabled Mac test installation. See [build guidance](docs/build.md).

## 2026-11-01 — Native macOS SVG gallery

- Qualify the native C23 Mac gallery with `--enable-svg` and librsvg2.62.3.
  Modern themes then have all nine vector dialog icons instead of the optional
  renderer’s legacy bitmap fallback and missing-icon placeholders.
- Verify vector scaling, cache/override behavior and toolbar backgrounds across
  normal, pressed and disabled states, four themes and two scales; retain the
  historical SVG-disabled results separately. See the
  [qualification record](docs/validation/macos-svg-20261101.json).

## 2026-11-01 — Buffered painting and direct gallery selection

- Paint toolkit-owned full frames through normal Xt expose methods, using DBE
  when supported and pixmap fallback otherwise, preserving application window
  IDs, widget record layouts and application drawing callbacks.
- Retire Xft draw-cache entries before buffer release and DBE swaps. A native
  regression reproduced missing labels at frame three after resize; the fixed
  build retains contrasting text through all 24 frames under DBE, pixmap and
  direct modes.
- Replace gallery Theme/Scale cycling buttons with selection ComboBoxes and
  prepare preview roots before exchanging managed children.
- Correct UTF-8 hard-line reads across wide storage and gap-buffer edits so the
  Source pane can reach its final line.

## 2026-11-01 — Standalone workspaces and demo launching

- Enable the standalone MWM workspace protocol and restore Wsm's window
  membership, geometry, visibility, command menus, persistence and reconnect paths.
- Add the gallery's Demo Applications catalogue with descriptions, Open buttons,
  exact missing-app paths and `--enable-demos` build guidance.
- Correct empty selection acknowledgements, parentless-shell traversal geometry,
  command-tree cleanup and UIL custom-widget reference flags exercised by demos.


## 2026-11-01 — Toolbar icon surfaces

- Match toolbar action icons to each button’s normal, pressed and disabled
  surfaces, including overflow entries and live theme/scale changes. This
  removes the lighter rectangles caused by using the toolbar background.
- Add the borrowed `XmToolbarGetActionButton` accessor without changing widget
  records. Keep image ownership with the application.
- Reproduce the original mismatch and verify real gallery-factory pixels across
  four profiles and two scales; see [toolbar documentation](docs/toolbar.md).

## 2026-11-01 — Modern C and native macOS builds

- Build the toolkit libraries, WML generators, UIL compiler, `mwm` and modern
  gallery with current Clang in C23 mode on macOS and Linux.
- Replace old-style definitions and untyped callback tables with explicit
  prototypes. Generate modern function-ABI declarations, use the standard
  wide-character functions, and initialize static atomics without the removed
  `ATOMIC_VAR_INIT` macro. Use a standard-C ASCII check for Paned sash actions.
- Export the existing deprecated widget functions, class pointers and class
  records as actual Mach-O aliases. The aliases preserve code/storage identity
  in both static archives and shared libraries.
- Include configuration before generated WML scanner system headers so POSIX
  declarations remain available in strict C23 mode.
- Use Autoconf's detected lexer library instead of assuming Linux's `libfl`.
  Represent WML string and token attributes explicitly; recognize
  `XrmResource` boolean attributes in resource and datatype definitions.
- Provide typed `MrmCreateProc`, `MrmCleanupProc`, `UilMessageProc` and
  `UilStatusProc` callbacks. UIL text buffers are borrowed `const char *`;
  callers must copy them before modification. Update the bundled callable
  compiler examples to use the typed interface.
- Select the UIL parser semantic type independently of legacy platform macros
  that strict C modes no longer define.
- Make window-manager dispatch and argument parsers type-correct, including
  checked numeric keysyms and overflow rejection.
- Add native regressions for all six generator ABIs, shared/static aliases,
  toolkit aliases, Mrm and UIL callbacks, WML attributes, Mwm parser dispatch,
  string handling, native ABI identity and Unicode source maps.

Validated native arm64 macOS and Linux C23 builds and their display-independent
regressions, GCC C23 Linux consumers, and 619 selected source checks. The
[qualification record](docs/modern-c-portability-validation.json) identifies the
tested commit, features and loaded library bytes.

See [Build and install](docs/build.md#modern-c-and-macos) for the platform
commands and qualification boundaries. macOS uses XQuartz for X11/Xt; desktop
integration, clipboard/input-method sessions and VoiceOver are separate
qualification work. Rebuild consumers against one coherent native ABI cohort.

The complete historical maintenance log remains in [CHANGES](CHANGES).
