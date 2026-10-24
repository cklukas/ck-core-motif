# DataField insert-string action bounds

The `insert-string` action now checks the complete action parameter before
copying it into the fixed-size conversion buffer.  Oversized parameters are
ignored safely, while normal action strings retain their existing filtering,
conversion, replacement, and callback behavior.

`tests/automated/check-datafield-insert-action.c` exercises the oversized
parameter case against the installed library.
