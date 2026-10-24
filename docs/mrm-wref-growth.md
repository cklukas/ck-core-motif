# Mrm widget-reference growth

Widget-reference contexts now account for an empty reference vector without
unsigned underflow and grow until the complete descriptor and name heap fit.
Requests that exceed the resource-size or integer limits return `MrmTOO_MANY`
before resizing or copying the context.

The source-level regression is `tests/automated/check-mrm-wref-growth.py`.
