# UIL lexer string assembly

Primitive strings may span multiple linked lexer buffers.  Their cumulative
length is stored in a signed `int` for the historical semantic-value ABI.  The
lexer now checks each fixed-buffer addition before accumulating it and stops
token construction if the semantic string node or payload cannot be created.
This prevents signed wraparound and null-value dereferences during long or
allocation-constrained inputs.
