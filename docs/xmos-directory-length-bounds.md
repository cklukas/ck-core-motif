# Xmos directory-entry length bounds

The shared `_XmOSGetDirEntries` path now keeps directory-entry lengths in
`size_t` from `strlen` through the cache helper. `AddEntryToCache` performs the
existing `SIZE_MAX`/`UINT_MAX` checks on that widened value before allocation,
so a long directory entry cannot narrow before validation.

The focused source regression is
`tests/automated/check-xmos-directory-lengths.py`.
