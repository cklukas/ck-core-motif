# TabList simple modify

`XmTabbedStackListSimpleModify` now initializes the attribute record from its
`label_string` argument before calling the general modify operation. This
restores the helper’s documented behavior and avoids reading an uninitialized
label value.

The focused regression is `tests/automated/check-tablist-simple-modify.py`.
