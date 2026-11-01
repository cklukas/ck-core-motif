# Changelog

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
- Use Autoconf's detected lexer library instead of assuming Linux's `libfl`.
  Represent WML string and token attributes explicitly; recognize
  `XrmResource` boolean attributes in resource and datatype definitions.
- Provide typed `MrmCreateProc`, `MrmCleanupProc`, `UilMessageProc` and
  `UilStatusProc` callbacks. UIL text buffers are borrowed `const char *`;
  callers must copy them before modification. Update the bundled callable
  compiler examples to use the typed interface.
- Make window-manager dispatch and argument parsers type-correct, including
  checked numeric keysyms and overflow rejection.
- Add native regressions for all six generator ABIs, shared/static aliases,
  toolkit aliases, Mrm and UIL callbacks, WML attributes, Mwm parser dispatch,
  string handling, native ABI identity and Unicode source maps.

See [Build and install](docs/build.md#modern-c-and-macos) for the platform
commands and qualification boundaries. macOS uses XQuartz for X11/Xt; desktop
integration, clipboard/input-method sessions and VoiceOver are separate
qualification work. Rebuild consumers against one coherent native ABI cohort.

The complete historical maintenance log remains in [CHANGES](CHANGES).
