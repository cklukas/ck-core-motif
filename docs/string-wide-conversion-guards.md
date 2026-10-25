---
nav_exclude: true
---
# Compound-string wide-text conversion guard

`XmStringGetNextSegment` now checks its multibyte conversion buffer before
calling `wcstombs`. Allocation failure returns `FALSE`, clears the text and
tag outputs, and does not pass a null buffer to the conversion routine.
