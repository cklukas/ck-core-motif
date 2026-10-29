---
nav_exclude: true
---
# Sidebar toggle child allocation

Sidebar expansion and collapse now tolerate a missing toggle button after an
internal child allocation failure. Focus traversal and label updates are
skipped safely while content state and callbacks continue to be maintained.

The contract is protected by
`tests/automated/check-sidebar-toggle-child-guards.py`.
