---
nav_exclude: true
---
# CommandPalette child allocation

CommandPalette creates a search box and command list during initialization.
Filtering, layout, activation, callbacks, and showing the palette now remain
safe when either internal child fails to allocate.

The contract is protected by
`tests/automated/check-command-palette-child-failure-guards.py`.
