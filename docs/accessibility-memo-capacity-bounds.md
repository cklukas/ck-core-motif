# Accessibility memo-table capacity bounds

`memo_resize` now validates its requested bucket capacity at the allocation
boundary, including the `XtCalloc` element-size conversion and the power-of-two
invariant required by the bucket bitmask. Invalid or overflowing capacities
fail before allocation, so future callers cannot rely on caller-side checks to
keep the memo table structurally valid.

The source-contract regression is
`tests/automated/check-accessibility-memo-capacity-bounds.py`.
