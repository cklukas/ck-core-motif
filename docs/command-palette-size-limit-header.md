---
nav_exclude: true
---
# CommandPalette size-limit header

The CommandPalette text-combination allocation guard uses `SIZE_MAX` to reject
unrepresentable lengths. `CommandPalette.c` now includes `<stdint.h>`, making
that guard compile portably on platforms where `<string.h>` and the other
existing headers do not expose the macro.

The focused regression is
`tests/automated/check-command-palette-size-limit-header.py`.
