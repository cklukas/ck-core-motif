# XmString table conversion failures

`XmStringToXmStringTable` now clears its output pointer, rejects null input,
and rolls back already-created entries when copying or slicing fails.  It no
longer reports a successful one-entry table containing a null string.
