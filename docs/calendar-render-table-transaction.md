---
nav_exclude: true
---

# Calendar render-table updates

Calendar now replaces its render table transactionally. If copying the
requested table fails, the existing table remains owned and in use; the old
table is released only after a successful replacement.

The focused source contract is checked by
`tests/automated/check-calendar-render-table-transaction.py`.
