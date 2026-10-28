# Frame title baseline count

Frame title layout uses the baselines returned by `XmWidgetGetBaselines` for
baseline-aligned titles.  A successful query must still be treated as empty
when it returns no baseline array or a zero count; indexing `nlines - 1` in
that case would underflow before the fallback geometry can be used.  The
layout now requires both a non-null array and a positive count before reading
the first or last baseline.

The regression contract is checked by
`tests/automated/check-frame-baseline-count.py`.
