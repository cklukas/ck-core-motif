---
nav_exclude: true
---
# FontSelector text-error callback guard

The text error-removal callback now returns safely when invoked without a
FontSelector instance, before reading the current-text state.
