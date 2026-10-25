---
nav_exclude: true
---
# UIL WMD path formatting

The UIL database search path expands several `%s` components from environment
variables and installation directories.  The old allocation counted the
template bytes and substituted strings but omitted the terminating byte; the
subsequent `sprintf` could therefore write one byte past the allocation, with
larger path arithmetic also vulnerable to overflow.

`init_wmd_path` now measures the expanded format with `snprintf`, allocates the
exact result plus its terminator, and performs the bounded write.  Normal WMD
and application-resource search order is unchanged, including the absolute
database path and environment overrides.

`tests/automated/check-uil-path-formatting.py` protects the sizing and bounded
formatting contract.
