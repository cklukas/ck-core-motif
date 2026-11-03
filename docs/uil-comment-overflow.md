# UIL comment overflow handling

The UIL lexer preserves consecutive comments in a dynamically grown buffer.
If the incoming text or the required buffer size cannot fit the supported Xt
allocation and signed lexer limits, the lexer now reports the diagnostic and
returns `UILEOF` immediately.

The verified input and output lengths are then copied directly with `memcpy`,
with the newline and terminator written at their exact offsets.  This prevents
a returning Xt error handler from allowing wrapped size arithmetic to reach an
unbounded concatenation routine.
