# Text cursor-scroll delta bounds

`ScrollCursorVertically` now computes the difference between current and
requested line indices in `int64_t`, then saturates it to the signed `int`
range required by `XmTextScroll`. Very large line tables therefore cannot
wrap the scroll direction when the legacy API receives the delta.

The focused source contract is checked by
`tests/automated/check-text-scroll-cursor-delta-bounds.py`.
