---
nav_exclude: true
---
# XmDatePicker

`XmDatePicker` is a compact date-only editor whose popup is an `XmCalendar`.
It shares the Calendar civil-date contract: proleptic Gregorian years 1–9999,
no time zone, and no conversion to an instant.  `XmDatePickerSetRange` rejects
an invalid or inverted range and rejects a range that would exclude the current
date.  A rejected update leaves the picker unchanged.

Pointer, Return, keypad Enter, and Space toggle the popup.  Choosing a valid
calendar date updates the picker and closes the popup.  The popup is an
override-redirect, non-modal shell and is positioned below the owner, clamped
to the screen.  `XmNdatePickerDateChangedCallback` is delivered after the
popup has closed; callback code may destroy the picker.  `XmNdatePickerOpenChangedCallback`
reports popup open/close transitions.

The compact field uses an unambiguous `YYYY-MM-DD` representation.  Locale
formatting and time-zone interpretation are intentionally outside this
date-only control and are separate work.
