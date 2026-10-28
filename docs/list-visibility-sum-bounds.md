# List visibility-sum bounds

List visibility checks compare the top row plus the visible-row count in
`long long` arithmetic.  This prevents signed `int` wraparound from making a
row appear visible after extreme geometry or resource values.

The focused source-contract regression is:

```sh
PYTHONDONTWRITEBYTECODE=1 python3 tests/automated/check-list-visibility-sum-bounds.py
```
