# Color object property serialization

Color-object properties encode numeric color fields as hexadecimal text before
updating the resource database. The parser uses a fixed 256-byte scratch buffer;
its numeric serialization now uses `snprintf` with that capacity, preserving the
property format while preventing an oversized value representation from writing
past the buffer.

`tests/automated/check-color-object-property-bounds.py` guards the serialization
paths.
