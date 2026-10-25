---
nav_exclude: true
---
# Float-unit conversion output validation

`_XmConvertFloatUnitsToIntUnits` now rejects a conversion when either output
pointer is missing, before selecting a unit scale or writing a result. This
keeps the internal conversion helper safe for callers that cannot provide an
optional result and preserves the existing `False` failure contract.

`tests/automated/check-float-units-outputs.py` protects this input-validation
behavior.
