# Mrm legacy font-list allocation

Legacy font-list conversion now checks the variable-size `XtMalloc` result
before fixup and replacement of the resource-context buffer. Allocation
failure returns `MrmFAILURE` without losing the original buffer; valid font
lists retain their existing conversion behavior.

The source-level regression is
`tests/automated/check-mrm-fontlist-allocation.py`.
