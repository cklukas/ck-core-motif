# DataField wide selection export bounds

`XmDataFieldGetSelectionWcs` now checks the selection length before multiplying
by `sizeof(wchar_t)`.  A range that cannot be represented by the allocation
size fails cleanly and releases the application lock; valid wide selections
retain their existing contents and termination behavior.

The DataField wide-text regression covers both UTF-8 selection export and WCS
selection export after rebuilding the library.
