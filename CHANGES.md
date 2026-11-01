# Changelog

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
