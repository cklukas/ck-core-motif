# XmFontList constructor locking

`XmFontListCreate` and `XmFontListAdd` now delegate synchronization to charset,
rendition, and render-table helpers instead of holding a process/app lock across
those calls. Their validation and ownership behavior remain unchanged.
