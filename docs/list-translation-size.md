# List translation allocation size

List class initialization concatenates two translation tables before parsing
them.  The combined size is now computed in `size_t` with an overflow check
before stack or heap allocation, keeping the initialization path safe if the
translation sources grow.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-list-translation-size.py
```
