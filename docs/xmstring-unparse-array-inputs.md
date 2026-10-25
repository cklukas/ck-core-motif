# XmString unparse-array inputs

`XmStringTableUnparse` now rejects a null source element before converting it.
Any prior converted values and the temporary output array are released, so
callers receive an all-or-nothing result instead of a partial table.
