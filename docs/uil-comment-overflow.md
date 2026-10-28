# UIL comment overflow handling

The UIL lexer preserves consecutive comments in a dynamically grown buffer.
If the incoming text or the required buffer size cannot fit the supported Xt
allocation and signed lexer limits, the lexer now reports the diagnostic and
returns `UILEOF` immediately.

This prevents a returning Xt error handler from allowing wrapped size
arithmetic to reach `strcat`.
