# IconFile cache comparator input guards

The icon-name cache comparator now validates both entry pointers and their key
strings before comparing them. Valid cache keys retain pointer and string
comparison behavior; malformed optional cache state now reports no match
instead of dereferencing null data.

The focused regression is `tests/automated/check-icon-cache-comparator.py`.
