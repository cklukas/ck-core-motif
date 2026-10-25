# XmFontList constructor locking

`XmFontListCreate`, `XmFontListCreate_r`, `XmFontListAdd`,
`XmFontListEntryCreate`, `XmFontListEntryCreate_r`,
`XmFontListEntryLoad`, and
`XmFontListAppendEntry`, `XmFontListEntryGetFont`,
`XmFontListEntryGetTag`, and `XmeRenderTableGetDefaultFont` now delegate synchronization to the charset, rendition,
and render-table helpers instead of holding a process or application lock
across those calls.  This avoids
recursive locking when the helpers establish their own display/application
context while preserving the existing validation, ownership, and public
return-value behavior.

The source contract is covered by
`tests/automated/check-xmfontlist-locking.py`; the installed library build and
the existing Xft font-lifetime fixture qualify the changed call path.

`XmFontListGetNextFont` advances its context index while the process lock is
held, then retrieves the font and tag after unlocking so rendition accessors do
not recurse into that lock.

`XmFontListRemoveEntry` retrieves rendition data before taking the table
mutation lock and uses a copied tag while removing the entry. This avoids
recursive locking and keeps the comparison stable if the table releases the
rendition during removal.
