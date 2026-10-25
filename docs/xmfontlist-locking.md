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

`XmFontListEntryGetFont` also rejects a null type-output pointer before
retrieving rendition resources, avoiding an invalid write on malformed calls.

`XmFontListGetNextFont` advances its context and retains a shared rendition
handle under lock, then retrieves the font and tag after unlocking so rendition
accessors do not recurse into the process lock without exposing a use-after-free
when another thread updates the table.

`XmFontListRemoveEntry` now retrieves rendition data before taking the table
mutation lock and uses a copied tag while removing the entry. This avoids
recursive locking and keeps the comparison stable if the table releases the
rendition during removal.
