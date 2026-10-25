---
nav_exclude: true
---
# String-table resource conversion allocation

The string-table resource converter now checks its initial table, growth and
final reallocations, and every item string allocation. It releases partial
strings and the table on any failure, including an output-buffer size failure,
while preserving successful parsing and destroy ownership.
