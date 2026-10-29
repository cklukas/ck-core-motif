---
nav_exclude: true
---
# Hierarchy disclosure-button allocation

Hierarchy nodes may have an optional open/close icon button. If Xt cannot
create that internal child, the node remains a valid hierarchy node: mapping
and callback setup are skipped, and later folder-pixmap updates ignore the
missing button. Other hierarchy operations continue without dereferencing a
null child, and a later reset can retry creation.

The contract is protected by
`tests/automated/check-hierarchy-disclosure-allocation.py`.
