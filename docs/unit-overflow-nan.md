---
nav_exclude: true
---
# Unit conversion NaN overflow

The internal unit-conversion overflow check now rejects NaN values as well as
values outside the signed integer range. NaN comparisons otherwise evaluate
false, which could allow an invalid parsed value to reach integer conversion.
Finite values retain the existing rounding and range behavior.

`tests/automated/check-unit-overflow-nan.py` protects this contract.
