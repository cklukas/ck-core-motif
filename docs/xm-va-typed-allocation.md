Typed varargs argument-list allocation
=======================================

The private `_XmVaToTypedArgList` helper now initializes both output values,
rejects non-positive or unrepresentable capacities before byte multiplication,
and returns an empty result when Xt cannot allocate the typed-argument array.
This prevents malformed internal counts from wrapping the allocation size or
causing a null-array write while preserving the existing successful conversion
format.
