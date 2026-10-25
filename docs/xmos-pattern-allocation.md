# Xmos pattern allocation

`GetFixedMatchPattern` now checks its output-buffer allocation before writing
the regular-expression prefix and converted pattern. Allocation failure
returns `NULL`, matching its existing invalid-input behavior without changing
successful pattern conversion.
