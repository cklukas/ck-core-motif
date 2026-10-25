---
nav_exclude: true
---
# SelectionBox compound-text conversion

SelectionBox list selections received as COMPOUND_TEXT are converted to an
`XmString` before updating the text child. The converted value is now passed
to the textual trait, and conversion failures leave the operation incomplete
instead of updating the child with the raw transfer buffer. Partial textual
traits are ignored safely.
