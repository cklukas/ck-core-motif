# XmString concatenation line-copy failure handling

When a non-mutating `XmStringConcat` copies the first line from its second
operand, the allocation can fail. The concatenation now releases both owned
operands and returns `NULL` before it records the missing line or uses it as a
Compound String entry.

The focused source-contract regression check is
`tests/automated/check-xmstring-concat-line-copy-failure.py`.
