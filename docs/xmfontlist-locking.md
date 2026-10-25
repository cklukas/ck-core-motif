# XmFontList constructor locking

`XmFontListCreate`, `XmFontListAdd`, `XmFontListEntryCreate`,
`XmFontListCreate_r`, `XmFontListEntryCreate_r`,
`XmFontListEntryLoad`,
`XmFontListAppendEntry`, `XmFontListEntryGetFont`, and
`XmFontListEntryGetTag` now delegate synchronization to charset, rendition, and
render-table helpers instead of
holding a process/app lock across those calls. Their validation and ownership
behavior remain unchanged.
