# Xmfuncs compatibility macros

`Xmfuncs.h` supplies legacy `bcopy` and `bzero` mappings on ANSI C systems
which lack those historical interfaces. Modern platform headers may already
provide fortified macro versions of the same interfaces. The Motif mappings now
only apply when a platform has not already defined them, preserving the
platform's bounds-aware implementation and avoiding macro-redefinition warnings
in strict macOS builds.

This does not alter the legacy fallback on systems which do not provide either
name. The focused source contract is:

```sh
python3 tests/automated/check-xmfuncs-compatibility-macros.py
```
