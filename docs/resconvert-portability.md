---
nav_exclude: true
---
# Resource converter portability

The resource converters now make their established positive, bounded integer
values explicit at byte-count and copy-size boundaries. String spans, cardinal
table capacities, default-render-table cache growth and atom-list copies retain
their native unsigned widths until Xt's checked `Cardinal` allocation boundary.
Charset-table conversion also checks its pointer-table multiplication before
evaluating it: a maximum-width comma-separated value can reach an entry count
that would otherwise overflow the table byte calculation before the later
allocation-width checks run.

The focused source contract is covered by
`tests/automated/check-resconvert-portability.py`.
