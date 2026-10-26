# Text insert-string bounds

The `insert-string` action now validates each action parameter before using it
as an `XmTextBlock`. Null parameters are rejected, and strings larger than the
signed length accepted by the Motif text-source interface are rejected without
narrowing their size. The action rings the existing error bell and preserves
the document state for the rejected parameter; earlier parameters in the same
action remain compatible with the historical sequential behavior.

The focused source contract is checked by
`tests/automated/check-text-insert-string-bounds.py`.
