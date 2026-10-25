---
nav_exclude: true
---
# Mrm related-tag diagnostic handling

Related resource-tag lookup failures now report the related compressed code,
not the primary tag code, and stop the failed argument before later processing.
This makes diagnostics actionable and avoids retaining a partially initialized
related argument entry.

The source-level regression is
`tests/automated/check-mrm-related-tag-diagnostic.py`.
