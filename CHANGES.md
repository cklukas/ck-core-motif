# Changelog

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
