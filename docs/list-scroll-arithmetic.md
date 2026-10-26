# List scrollbar arithmetic

The List widget now computes scroll candidates in `int64_t` before applying
the scrollbar minimum and maximum bounds.  This prevents signed `int`
overflow when an application supplies extreme scrollbar values or increments,
while preserving the normal one-increment scrolling behavior.

The focused source-contract check is:

```sh
python3 tests/automated/check-list-scroll-arithmetic.py
```
