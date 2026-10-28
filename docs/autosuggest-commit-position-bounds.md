---
nav_exclude: true
---
# AutoSuggestBox committed-position bounds

`XmAutoSuggestBox` converts the selected 1-based List position to a 0-based
`Cardinal` before committing a suggestion. The callback path now rejects a
non-positive position before subtracting one, preventing malformed or stale
selection data from becoming a very large item index.

The focused source regression is
`tests/automated/check-autosuggest-commit-position-bounds.py`.
