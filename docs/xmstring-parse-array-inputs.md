# XmString parse-array inputs

`XmStringTableParseStringArray` now rejects a null source element before
calling the parser.  Already-created strings are released and the temporary
table is freed, preserving the wrapper's existing all-or-nothing failure
behavior.
