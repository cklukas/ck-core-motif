# SelectionBox compound-text conversion

SelectionBox list selections received as COMPOUND_TEXT are converted to an
`XmString` before updating the text child. The converted value is passed to
the textual trait, and conversion failures leave the operation incomplete
instead of updating the child with the raw transfer buffer. Partial textual
traits are ignored safely.

The transfer buffer is released after every successful conversion request,
including a conversion failure; insertion-position updates occur only after a
value was actually delivered to the text child. The source-level regression
check is `tests/automated/check-selectionbox-compound-text.py`.
