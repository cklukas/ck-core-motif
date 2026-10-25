# IconG right-to-left icon placement

The exported IconG icon-position helper now computes right-to-left small and
large icon origins in signed arithmetic before returning a `Position`.  Icons
that exceed a narrow gadget remain off-edge instead of wrapping to a distant
positive coordinate.

The source-level regression is `tests/automated/check-icong-rtl-icon.py`.
