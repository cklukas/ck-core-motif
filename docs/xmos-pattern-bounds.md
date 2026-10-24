# Xmos pattern conversion bounds

The file-selection path converts a match pattern into a regular expression.
Escaping and wildcard expansion can require up to twice the input length, so
the old `XtCalloc(2, strlen(pattern) + 4)` expression could wrap its element
size for an extremely large resource string.  A missing pattern also reached
`strlen` directly.

The converter now rejects null input and checks the doubled allocation size
before calling Xt.  Normal pattern syntax and matching behavior are unchanged;
only unrepresentable or missing input is rejected safely.

`tests/automated/check-xmos-pattern-bounds.py` protects the input and arithmetic
contract.
