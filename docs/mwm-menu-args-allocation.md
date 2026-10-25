---
nav_exclude: true
---

# MWM menu argument allocation

Formatted menu-function arguments now verify their `size_t` length before
converting it to Xt's `Cardinal` allocation size. Oversized formatting results
now return no argument buffer instead of wrapping the allocation request.
