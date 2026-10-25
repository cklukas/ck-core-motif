# Menu cascade traversal guard

Keyboard menu traversal can observe a popup shell while it is being torn down.
`FindNextMenuBarCascade` checked only that the shell pointer was non-null before
reading its first child, so an empty shell could crash traversal.

The function now requires at least one child before deriving the active cascade
button. With an empty shell it continues through the existing menu-bar search
fallback. The regression `tests/automated/check-menuutil-cascade-guard.py`
covers the lifecycle guard.
