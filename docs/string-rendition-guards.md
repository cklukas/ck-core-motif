# Compound-string rendition copy guards

Rendition-tag arrays returned by `_XmStringGetSegment` are now built through a
single checked helper. Array allocation and each copied tag are validated; a
failure frees earlier tags and returns no partial rendition output for either
copying or borrowed-tag mode.
