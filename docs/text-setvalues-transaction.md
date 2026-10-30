---
nav_exclude: true
---

# FormSection and Skeleton text updates

FormSection header/footer strings and Skeleton labels are now copied before
their old values are released. Allocation failure preserves the currently
visible text instead of replacing it with a null or empty value.

The focused source contract is checked by
`tests/automated/check-text-setvalues-transaction.py`.
