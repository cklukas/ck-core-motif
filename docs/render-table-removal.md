---
nav_exclude: true
---
# Render-table removal

Shared render tables are copied before removal so other handles remain
unchanged. The copy now checks the table body, handle, and each rendition
allocation; a failure releases partial state and returns the original table.
