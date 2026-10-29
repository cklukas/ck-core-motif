# Container grid placement bounds

Container grid placement now compares candidate child rectangles with signed
wide content edges. During a resize, margins can temporarily exceed the
container allocation; the old unsigned subtraction could wrap and accept a
candidate that was outside the usable area.

Candidate origins and extents are widened before comparison, preserving normal
grid placement while making undersized containers safe.

The source contract is checked by:

```text
python3 tests/automated/check-container-place-grid-bounds.py
```
