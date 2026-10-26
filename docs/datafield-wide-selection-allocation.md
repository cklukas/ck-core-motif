# DataField wide-selection allocation

`XmDataFieldGetSelectionWcs` now checks both the wide-character allocation
width and the returned pointer before conversion and terminator writes.  An
unavailable selection buffer returns `NULL` through the existing locked API
path without dereferencing null storage.

The focused source contract is checked by
`tests/automated/check-datafield-wide-selection-allocation.py`.
