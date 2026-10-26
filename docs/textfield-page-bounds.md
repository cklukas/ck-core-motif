# TextField paging bounds

`PageLeft` and `PageRight` now validate the available content width before
performing page movement. Margin, shadow, and highlight totals are widened
before subtraction, and a widget with no usable viewport returns without
changing its horizontal offset.

The focused source contract is checked by
`tests/automated/check-textfield-page-bounds.py`.
