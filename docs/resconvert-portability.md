---
nav_exclude: true
---
# Resource converter portability

The resource converters now make their established positive, bounded integer
values explicit at byte-count and copy-size boundaries. String spans, cardinal
table capacities, default-render-table cache growth and atom-list copies retain
their native unsigned widths until Xt's checked `Cardinal` allocation boundary.
The redundant charset-table overflow branch was removed because the bounded
counter cannot reach the condition on supported hosts; the subsequent
size-based allocation checks remain authoritative.

The focused source contract is covered by
`tests/automated/check-resconvert-portability.py`.
