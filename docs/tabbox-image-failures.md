---
nav_exclude: true
---
# Tab-box image rotation failure handling

The tab-box image rotation helper now checks its pixel-buffer allocation and
both `XCreateImage` calls. On failure it releases the buffer and returns
without attempting pixel writes, preventing low-memory conditions from
becoming NULL dereferences.

The focused regression is
`tests/automated/check-tabbox-image-failures.py`.
