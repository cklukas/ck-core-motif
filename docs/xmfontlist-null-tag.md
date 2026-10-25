# Font-list helpers with missing tags

Font-list entries normally carry a charset tag, but a malformed or partially
initialized rendition can expose a null tag.  `XmFontListEntryGetTag` now
returns null without copying it, and `XmFontListGetNextFont` releases its
retained rendition and reports failure instead of passing the null value to
`XtNewString`.
