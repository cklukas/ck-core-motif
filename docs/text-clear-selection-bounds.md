# Text clear-selection bounds

The `ClearSelection` action replaces selected text with spaces.  It now
rejects negative, reversed, or too-wide source ranges before converting the
range to the `int` width required by `XmTextBlock`.  It also checks the
temporary selected-string allocation before scanning it.  Invalid or
unrepresentable requests use the existing bell/error path instead of
truncating a range or dereferencing null storage.

The focused source contract is checked by
`tests/automated/check-text-clear-selection-bounds.py`.
