---
nav_exclude: true
---
# Workspace property guards

Workspace property updates now validate screen and workspace state before
building `_DT_WORKSPACE_LIST` and `_DT_WORKSPACE_CURRENT`. An empty workspace
set publishes an empty list without treating `XtMalloc(0)` as an error, while
missing active state safely skips the current-workspace update.
