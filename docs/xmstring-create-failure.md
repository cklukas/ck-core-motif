# XmString creation allocation failure

`_XmStringNCreate` now checks the result of `_XmStrCreate` for both optimized
and non-optimized representations before initializing headers or segments.
Allocation failure returns `NULL` cleanly instead of dereferencing an invalid
representation.
