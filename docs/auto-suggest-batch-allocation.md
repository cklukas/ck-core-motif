---
nav_exclude: true
---
# AutoSuggestBox batch allocation

`XmAutoSuggestBoxSetSuggestions` now checks the item-array byte count against
host `size_t` and Xt's `Cardinal` width, then checks the allocation and every
ID/label copy before publishing the batch. Failed installation releases
partial items and leaves the list in an explicit no-results state instead of
dereferencing a null array.

The focused source check is
`tests/automated/check-auto-suggest-string-copy-guards.py`.
