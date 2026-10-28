# Accessibility memo-table capacity bounds

`memo_resize` now validates its requested bucket capacity at the allocation
boundary, including the `XtCalloc` element-size conversion.  Invalid or
overflowing capacities fail before allocation, so future callers cannot rely
on a caller-side check to keep the memo table's byte count representable.

The source-contract regression is
`tests/automated/check-accessibility-memo-capacity-bounds.py`.
