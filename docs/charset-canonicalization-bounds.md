---
nav_exclude: true
---
# Charset canonicalization bounds

The private charset canonicalizer accepts application-derived strings.  Its
legacy ISO detection path assumed a non-null pointer and used an unbounded
format write into a fixed-size canonical name buffer.

Null input now returns `NULL` safely, and the fixed ISO form uses bounded
formatting.  Existing ASCII, ISO, and pass-through canonical names are
unchanged.

`tests/automated/check-charset-canonicalize-bounds.py` protects these paths.
