# XmString table conversion transactions

`XmStringTableParseStringArray` and `XmStringTableUnparse` now treat the
result table transactionally. If an element conversion fails, previously
created XmStrings or output buffers are freed along with the table and the
functions return `NULL`; successful conversions are unchanged.
