---
nav_exclude: true
---
# FontSelector fixed/proportional refresh guards

The spacing refresh now validates the selector, font state, and family table
before resolving family data or changing spacing flags. Missing optional
spacing toggles remain supported, while a partially initialized selector
returns safely.
