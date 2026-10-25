---
nav_exclude: true
---
# StackWindow guards

Window restacking can race client withdrawal and workspace list teardown.
`StackWindow` now validates the workspace, screen, target entry, and optional
stack reference before reading client windows.  Bottom stacking also returns
when the screen list is empty, avoiding a null tail dereference.  Valid X
restacking behavior is unchanged.
