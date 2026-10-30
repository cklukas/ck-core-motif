# Trait hash portability

Trait-table hashing now folds object and trait-name pointers into a
`uintptr_t`, then bounds the result before converting to Motif's signed
`XmHashValue` type. This avoids implementation-defined narrowing of 64-bit
pointer bits while preserving deterministic table lookup behavior.
