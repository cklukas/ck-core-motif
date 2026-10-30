---
nav_exclude: true
---

# DatePicker and TimePicker render-table updates

DatePicker and TimePicker now treat render-table replacement transactionally.
When copying a requested table fails, the existing table remains owned and in
use, and the setter reports no render-table change. The previous implementation
freed the valid table before checking the replacement result.

The focused source contract is checked by
`tests/automated/check-picker-render-table-transaction.py`.
