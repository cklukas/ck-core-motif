---
nav_exclude: true
---

# MessageBox getter class guards

MessageBox’s message, symbol-pixmap, OK-label, Cancel-label, and Help-label
synthetic getters now validate the widget class before accessing private child
state. Invalid callback inputs use the existing return path, while valid
MessageBox reads preserve their current child-resource behavior.

The focused source-contract check is
`python3 tests/automated/check-messagebox-getter-class-guards.py`.
