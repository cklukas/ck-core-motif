# UTF-8 conversion input validation

`_XmUtf8ToUcs2` now rejects a missing output-length pointer and a null input
buffer when bytes are requested.  It returns `NULL` without dereferencing
invalid inputs; valid UTF-8 and malformed-sequence replacement behavior remains
unchanged.
