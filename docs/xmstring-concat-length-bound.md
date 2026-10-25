# XmString optimized concatenation length bound

Optimized compound-string concatenation now checks the byte-count limit using
subtraction before adding the two unsigned lengths.  This prevents a wrapped
`a_len + b_len` from bypassing the compact-string limit and producing an
undersized result.
