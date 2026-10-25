---
nav_exclude: true
---
# I18List column-width allocation rollback

I18List now allocates replacement column-width storage before releasing the
current array. Allocation failure leaves the existing widths intact, and an
initial allocation failure returns before any column writes through NULL.

The focused source regression is
`tests/automated/check-i18list-column-width-rollback.py`.
