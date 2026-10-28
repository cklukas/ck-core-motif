# Compound-text converter inputs

`XmCvtTextToXmString` now returns `False` when its Xt converter input or output
record is absent. The previous implementation dereferenced those records
before checking the contained text pointer. Valid converter calls retain their
existing conversion and warning behavior.

Regression coverage: `tests/automated/check-compound-text-converter-inputs.py`.
