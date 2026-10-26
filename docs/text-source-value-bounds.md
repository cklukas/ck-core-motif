# Text-source value extraction bounds

`_XmStringSourceGetValue` now validates every block returned by the text source
before copying it into the result buffer.  It rejects non-progressing or
oversized blocks and reports multibyte-to-wide-character conversion errors
instead of terminating a partially written result.  Successful extraction
keeps the existing byte and wide-character representations.

The focused source contract is checked by
`tests/automated/check-text-source-value-bounds.py`.
