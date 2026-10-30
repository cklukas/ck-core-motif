# ImageCache hash portability

Image and GC cache hash values now bound their 32-bit mixes before converting
to Motif's signed `XmHashValue` type. This avoids implementation-defined
high-bit narrowing while preserving deterministic cache lookup behavior.
