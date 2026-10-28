---
nav_exclude: true
---

# Xft cache initialization

When the Xft draw cache grows, all newly allocated slots are now zeroed in
every build. This prevents uninitialized display/window pairs from being
treated as cached draws when the optional `FIX_1449` macro is not defined.
