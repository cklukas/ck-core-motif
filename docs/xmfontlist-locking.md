# XmFontList constructor locking

`XmFontListCreate`, `XmFontListAdd`, `XmFontListEntryCreate_r`,
`XmFontListEntryLoad`, and
`XmFontListAppendEntry`, `XmFontListEntryGetFont`, and
`XmFontListEntryGetTag` now delegate synchronization to the charset, rendition,
and render-table helpers instead of holding a process or application lock
across those calls.  This avoids
recursive locking when the helpers establish their own display/application
context while preserving the existing validation, ownership, and public
return-value behavior.

The source contract is covered by
`tests/automated/check-xmfontlist-locking.py`; the installed library build and
the existing Xft font-lifetime fixture qualify the changed call path.
