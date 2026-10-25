---
nav_exclude: true
---
# Label baseline count bounds

Label and LabelGadget cache one extra baseline offset after the baselines
returned by the compound-string code.  Their overflow check now tests the
original `Cardinal` count before adding that extra element.  This avoids the
old `count + 1` wraparound being evaluated in the narrow `Cardinal` type and
then incorrectly accepted for reallocation.

The offset-adjustment loop also uses a `Cardinal` index, so a valid wide
baseline table cannot wrap a signed counter while applying the cached delta.

Counts that cannot be returned through the historical `int *line_count` API
are rejected before the `Cardinal` value is narrowed.

Normal baseline measurement and the public `XmWidgetGetBaselines` behavior are
unchanged.  The allocation regressions are covered by
`tests/automated/check-label-baseline-allocation.py` and
`tests/automated/check-labelg-baseline-allocation.py`.
