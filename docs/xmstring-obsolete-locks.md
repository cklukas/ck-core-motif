# Legacy XmString compatibility lock boundaries

The compatibility APIs `XmStringNConcat`, `XmStringNCopy`, and
`XmStringByteCompare` delegate to modern conversion, copy, and concatenation
functions that manage the Motif process lock themselves. Their redundant outer
locks were removed to prevent recursive-lock deadlocks; ownership and return
values remain unchanged.
