---
nav_exclude: true
---
# Generated XmString parse-table substitutes

Generated tab and newline mappings now require successful temporary compound
strings and copied substitutes before entering the shared cache. Failed
substitute allocation rolls back the partial table instead of caching a
mapping that can never be applied.

The regression check is
`tests/automated/check-xmstring-generated-table-substitutes.py`.
