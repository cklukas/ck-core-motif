# Text drag-distance bounds

The selection drag threshold calculation now widens coordinate differences to
`int64_t` before taking their magnitude. Extreme event coordinates therefore
cannot overflow the signed `int` arithmetic previously used by `abs()`, and
drag classification remains deterministic.

The focused source contract is checked by
`tests/automated/check-text-drag-distance-bounds.py`.
