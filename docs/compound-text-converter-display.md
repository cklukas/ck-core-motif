# Compound-text converter display input

The public compound-text converter wrappers now reject a null `Display *`
before attempting conversion diagnostics. A failed conversion previously
passed that pointer to `XtDisplayToApplicationContext`, which could crash for
direct malformed calls. Valid Xt converter calls retain their normal warnings
and conversion behavior.

Regression coverage: `tests/automated/check-compound-text-converter-display.py`.
