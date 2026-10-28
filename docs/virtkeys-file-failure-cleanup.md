---
nav_exclude: true
---

# Atomic virtual-key file loading

When a user virtual-key binding file cannot be read completely, the loader
now releases any partially assembled string and clears the output pointer
before returning failure.  This ensures `LoadVendorBindings` proceeds to its
normal alias or built-in fallback instead of publishing truncated bindings.
The same cleanup applies if trimming the successfully read buffer cannot be
allocated.

Regression: `tests/automated/check-virtkeys-file-failure-cleanup.py`
