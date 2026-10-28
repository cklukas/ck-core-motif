---
nav_exclude: true
---
# DataField modify-verify result allocation

DataField narrow and wide modify-verify callback results now check the
allocation before copying or converting the returned buffer. Callback records
and lengths are validated first. Allocation failure clears the temporary result
and follows the existing cleanup path, so it cannot become a null write or
leak the callback-conversion buffer. The wide result size is formed in
`size_t` before narrowing to Xt's `Cardinal`.

Regression coverage: `tests/automated/check-datafield-modify-verify-result-allocation.py`.
