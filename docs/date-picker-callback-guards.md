---
nav_exclude: true
---

# DatePicker callback guards

DatePicker validates both its callback closure and the calendar widget before
accepting a date-change notification. Foreign or stale callback inputs are
ignored safely while normal popup and date-change behavior remains unchanged.
