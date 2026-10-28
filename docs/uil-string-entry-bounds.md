# UIL string-entry bounds

The UIL lexer’s string-value constructor uses an `int` length and allocates a
terminator byte.  It now rejects negative lengths and `INT_MAX` before
evaluating `length + 1`, preventing signed overflow from reaching `XtCalloc`.

Normal string values retain their existing representation and allocation.
