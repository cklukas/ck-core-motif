---
nav_exclude: true
---
# XmString null parse patterns

`match_pattern` now treats mappings with a null pattern as non-matches after
handling the special direction-change marker. Invalid mapping input no longer
reaches wide-character dereferences or `strlen`.

The regression check is
`tests/automated/check-xmstring-match-pattern.py`.
