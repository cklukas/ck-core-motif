---
nav_exclude: true
---
# XmComboBox API null-safety

The ComboBox compatibility helpers now reject null widgets before app-context
locking: add, delete, select, set, and update operations return safely.
`XmCombinationBoxGetValue` returns `NULL` for null or non-ComboBox widgets.
It also holds the application lock and returns `NULL` when its ComboBox or edit
child is entering teardown, rather than reaching a partially destroyed child.
Valid ComboBox behavior and child-list synchronization remain unchanged.
