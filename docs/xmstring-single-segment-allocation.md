# XmString single-segment allocation handling

`_XmStringSingleSegment` now validates copied tag, locale, and text component
results.  Failed component copies or default-tag allocation return `FALSE`
and clear the partial outputs instead of reporting a segment with a null
payload.
