# Icon-name hash portability

Icon-cache hashing now bounds its unsigned name mix before converting to
Motif's signed `XmHashValue` type. High-bit lengths or name bytes therefore do
not rely on implementation-defined signed narrowing.
