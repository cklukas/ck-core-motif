---
nav_exclude: true
---
# Mwm command-tree bounds

Nested menu command names are assembled from resource-controlled fragments.
The construction now uses `size_t` arithmetic with overflow checks and bounded
`snprintf` calls. A missing tree name is treated as an empty component, and an
unrepresentable command is skipped safely instead of overflowing an integer or
heap buffer.

`tests/automated/check-mwm-menutree-bounds.py` guards this construction.
