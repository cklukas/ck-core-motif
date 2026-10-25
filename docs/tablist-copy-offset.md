# Tab-list copy offset validation

`XmTabListCopy` now validates the signed offset before deriving the default
copy count. Out-of-range offsets return `NULL` without allocation or traversal,
and magnitude calculation handles the most-negative `int` without invoking
undefined signed overflow. The same bounded magnitude helper is used by tab
lookup.
