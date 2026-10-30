---
nav_exclude: true
---
# ComboBox selected-item import guards

Synthetic selected-item import now rejects null values and incomplete list or
edit-box children before lookup. A failed edit-box snapshot also returns
without comparing or updating the selection, preserving the existing state.
