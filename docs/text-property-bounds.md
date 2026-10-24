# Text property conversion bounds

`XmCvtXmStringTableToTextProperty` now rejects negative counts and non-empty
tables without a table pointer. Its compound-string conversion writes the final
terminator within the allocated buffer instead of advancing past the allocation.
The source contract is covered by
`tests/automated/check-text-property-bounds.py`.
