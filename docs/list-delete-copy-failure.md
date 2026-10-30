---
nav_exclude: true
---
# XmListDeleteItems copy failure handling

`XmListDeleteItems` now treats its temporary item snapshot transactionally. If
an `XmString` copy fails, already-copied entries are released, the temporary
array is freed, the application lock is restored, and deletion returns before
any null snapshot entry is inspected.
