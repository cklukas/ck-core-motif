# Region operation capacity growth

Region union, intersection, and subtraction operations initially reserve twice
the larger source rectangle count.  The old `MAX(...)*2` expression could
overflow the `long` region-size field or the Xt `Cardinal` allocation size
before the rectangle array was allocated.

The operation now validates the doubled capacity against both limits and
returns without mutating the destination when the request is unrepresentable.
Normal region operations keep their existing capacity and coalescing behavior.
The source regression in
`tests/automated/check-region-operation-capacity.py` protects the checks.
