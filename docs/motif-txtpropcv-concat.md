# Text-property concatenation allocation

When Xlib returns multiple text items, `TextPropertyToSingleTextItem` now
checks the result buffer before concatenating them. Failure releases the Xlib
text list and returns `XNoMemory` instead of writing through null storage.

The regression check is
`tests/automated/check-txtpropcv-concat-allocation.py`.
