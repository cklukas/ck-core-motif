---
nav_exclude: true
---

# Converter destructor input validation

Converter cleanup callbacks now return safely when Xt supplies no destination
value or address. Valid cached string, font, table, tab-list, and render-table
cleanup remains unchanged.

The focused regression check is
`tests/automated/check-converter-destructors.py`.
