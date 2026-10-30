---
nav_exclude: true
---
# RowColumn synthetic string export guards

Menu-post, label-string, and tear-off-title synthetic exports now validate the
widget and output pointers before reading RowColumn state or copying strings.
Malformed resource callbacks return safely without changing normal exported
values.
