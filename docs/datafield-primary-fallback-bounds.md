---
nav_exclude: true
---

# DataField primary-selection fallback bounds

The raw wide primary-selection fallback now rejects lengths that cannot pass
through the existing `int` replacement API or cannot be multiplied safely for
an Xt `Cardinal` allocation. Rejection restores temporary selection state and
releases the callback request records before returning.

The focused source contract is checked by
`tests/automated/check-datafield-primary-fallback-bounds.py`.
