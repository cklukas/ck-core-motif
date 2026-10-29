---
nav_exclude: true
---
# DatePicker popup teardown

DatePicker calendar callbacks and public open/close operations can overlap
popup destruction. They now check popup and calendar availability and the
popup shell's `being_destroyed` state before popping down or destroying it.

The contract is protected by
`tests/automated/check-datepicker-teardown-guards.py`.
