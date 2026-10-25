---
nav_exclude: true
---
# VendorShell count bounds

VendorShell `SetValues` and `GetValuesHook` now iterate Xt argument counts
with `Cardinal` counters, and `Resize` uses a `Cardinal` child index for the
composite child array. This preserves resource and child processing while
avoiding signed-counter wraparound at large valid counts.
