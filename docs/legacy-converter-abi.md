---
nav_exclude: true
---
# Legacy resource converter ABI and output sizing

The IconButton, ButtonBox, and Column resource converters now use the full
`XtTypeConverter` callback signature and are registered without incompatible
function-pointer casts. Existing resource spellings, values, and cache modes
are unchanged.

The ButtonBox fill-option converter also now checks the caller's output buffer
before replacing `to->size` with the required size. An undersized destination
is reported through the standard Xt size-negotiation result instead of being
written through.

The focused regression check is
`tests/automated/check-legacy-converter-abi.py`.
