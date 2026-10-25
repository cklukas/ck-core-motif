---
nav_exclude: true
---

# ScrollFrame navigator-list growth

Navigator registration now checks cardinal and byte-size limits before adding
capacity. A wrapped slot count or allocation size is rejected without
publishing a corrupted list, while normal growth remains unchanged.

The focused regression check is
`tests/automated/check-scrollframe-nav-growth.py`.
