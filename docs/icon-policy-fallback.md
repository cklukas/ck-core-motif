---
nav_exclude: true
---
# Icon-box policy fallback

Icon-box initialization now handles a missing or invalid scrollbar display
policy without calling string functions on `NULL` or writing into an unknown
resource buffer.  It uses a writable static `all` fallback and keeps the
existing `all`, `vertical`, and `horizontal` policy values unchanged.

`tests/automated/check-icon-policy-fallback.py` protects this path.
