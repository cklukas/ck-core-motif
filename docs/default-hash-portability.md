# Default hash portability

The default Motif hash function now mixes a widget or pointer key through
`uintptr_t` and bounds it before converting to the signed `XmHashValue` type.
Hash tables without a custom hasher therefore avoid implementation-defined
pointer narrowing on 64-bit systems.
