---
nav_exclude: true
---
# Focus-shell query null-widget guard

`XmeFocusIsInShell` now validates its widget before finding the top-level
shell. A null query therefore returns `False` without entering traversal or
application-context code, while valid focus checks retain their existing
behavior.

The focused regression is
`tests/automated/check-focus-shell-input.py`.
