---
nav_exclude: true
---
# Wide-string text search bounds

`XmTextFindStringWcs` now counts the search string in `size_t`, checks the
wide-to-multibyte allocation against both `size_t` and Xt's `Cardinal` limit,
and handles a null search string or allocation failure without calling
`wcstombs`. The normal search and public return contract are unchanged.

The source contract is checked by:

```sh
python3 tests/automated/check-text-find-wcs-bounds.py
```
