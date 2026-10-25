# XmStringParseText initial allocation

`XmStringParseText` now checks creation of the initial tag component before
processing patterns and unmatched text.  A failed allocation releases the
process lock and returns null rather than continuing with a partial result.
