---
nav_exclude: true
---

# Text output allocation

TextOut backward scans now validate the character count before calculating the
temporary wide-buffer size. The previous signed `(num_chars + 1)` arithmetic
could overflow before reaching the allocator; the checked `size_t` calculation
now fails the scan safely instead.

Regression coverage:

```text
python3 tests/automated/check-text-output-allocation.py
```
