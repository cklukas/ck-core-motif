# XmString creation allocation failure

`_XmStringNCreate` now checks the result of `_XmStrCreate` for both optimized
and non-optimized representations before initializing headers or segments.
Allocation failure returns `NULL` cleanly instead of dereferencing an invalid
representation.

The same path now keeps the measured length in `size_t` and rejects a source
whose byte count cannot be represented by the legacy `unsigned int` segment
field before allocating or copying it.
