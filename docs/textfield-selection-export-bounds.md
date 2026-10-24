# TextField selection export bounds

TextField narrow selection export now treats zero or negative `mblen()` results
as conversion failure instead of allowing a non-progressing loop or unsigned
length underflow.  Wide selection export checks the element count before
multiplying by `sizeof(wchar_t)`.  Valid narrow and wide selections preserve
their existing contents and ownership behavior.

The selection timestamp regression now covers both TextField export formats.
