# XmComboBox API null-safety

The ComboBox compatibility helpers now reject null widgets before app-context
locking: add, delete, select, set, and update operations return safely.
`XmCombinationBoxGetValue` returns `NULL` for a null widget. Valid ComboBox
behavior and child-list synchronization remain unchanged.
