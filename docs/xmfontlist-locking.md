# XmFontList constructor locking

`XmFontListCreate`, `XmFontListAdd`, `XmFontListEntryCreate`,
`XmFontListCreate_r`, `XmFontListEntryCreate_r`,
`XmFontListEntryLoad`,
`XmFontListAppendEntry`, `XmFontListEntryGetFont`, and
`XmFontListEntryGetTag`, and `XmeRenderTableGetDefaultFont` now delegate
synchronization to charset, rendition, and
render-table helpers instead of
holding a process/app lock across those calls. Their validation and ownership
behavior remain unchanged.

`XmFontListGetNextFont` advances its context under lock, then retrieves the
font and tag after unlocking so rendition accessors do not recurse into the
process lock.

`XmFontListRemoveEntry` now retrieves rendition data before taking the table
mutation lock and uses a copied tag while removing the entry. This avoids
recursive locking and keeps the comparison stable if the table releases the
rendition during removal.
