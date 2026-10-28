---
nav_exclude: true
---

# UIL name-hash width

On 64-bit-long platforms, the name hash combines machine words with masks up
to all 64 bits. The mask and accumulator storage now use `unsigned long`, so
the all-ones mask is represented without a signed overflow conversion while
preserving the hash algorithm and its public behavior.

Regression: `tests/automated/check-uil-name-hash-width.py`
