---
nav_exclude: true
---

# UIL object-comment allocation bounds

Object comments are accumulated as C strings but Xt allocation sizes use
`Cardinal`. The semantic action now rejects a comment length that cannot fit
that allocator width before adding the terminator and narrowing the size.

Regression: `tests/automated/check-uil-comment-allocation-bounds.py`
