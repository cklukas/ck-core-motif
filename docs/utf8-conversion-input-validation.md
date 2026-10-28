# UTF-8 conversion input validation

`_XmUtf8ToUcs2` now rejects a missing output-length pointer and a null input
buffer when bytes are requested. It returns `NULL` without dereferencing
invalid inputs. UTF-8 scalar boundaries are validated, including overlong and
surrogate encodings; malformed sequences and valid supplementary code points
that cannot be represented by UCS-2 produce one replacement character.
