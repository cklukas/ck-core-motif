# Label baseline count bounds

Label and LabelGadget cache one extra baseline offset after the baselines
returned by the compound-string code.  Their overflow check now tests the
original `Cardinal` count before adding that extra element.  This avoids the
old `count + 1` wraparound being evaluated in the narrow `Cardinal` type and
then incorrectly accepted for reallocation.

Normal baseline measurement and the public `XmWidgetGetBaselines` behavior are
unchanged.  The allocation regressions are covered by
`tests/automated/check-label-baseline-allocation.py` and
`tests/automated/check-labelg-baseline-allocation.py`.
