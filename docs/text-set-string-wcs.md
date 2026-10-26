# Text wide-string setter

`XmTextSetStringWcs` now treats a null wide-string input as an empty value and
checks character-count, byte-size, and allocation limits before calling
`wcstombs`.  Unrepresentable or unavailable conversion storage leaves the
existing widget value unchanged rather than dereferencing null storage.

The focused source contract is checked by
`tests/automated/check-text-set-string-wcs.py`.
