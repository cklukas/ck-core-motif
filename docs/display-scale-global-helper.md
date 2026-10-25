# Keep the display-scale helper interface consistent

The display-scale implementation has two roles: reading the global Xft DPI
resource and exposing the established `_XmGetDpiScale` helper.  The resource
reader is now named `_XmGetGlobalDpiScale`, matching its callers in the Xlibre
backend and scale transaction code.  `_XmGetDpiScale` remains as the existing
wrapper, preserving the internal compatibility entry point.

The shared private header declares both helpers so each translation unit sees
the same return type and parameter contract.  This removes the duplicate
definition and implicit-declaration errors that otherwise prevent a clean
build on modern compilers.
