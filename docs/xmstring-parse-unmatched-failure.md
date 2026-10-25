# XmStringParseText unmatched-component failure

`parse_unmatched` now reports component creation or concatenation failure
instead of silently leaving the input pointer unchanged.  `XmStringParseText`
unwinds the partial result and releases its process lock at each call site,
preventing allocation-failure loops and partial output.
