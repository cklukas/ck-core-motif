---
nav_exclude: true
---
# XmCalendar

`XmCalendar` is an additive, date-only calendar widget.  Its value is an
`XmCalendarDate` civil date (`year`, `month`, `day`); it is never converted to
an instant and the widget never consults the host time zone or daylight-saving
rules.  Years are in the inclusive range 1 through 9999 and use the
proleptic Gregorian calendar, including the 400-year leap-year rule.

The initial value is 1970-01-01.  Applications can set a value with
`XmCalendarSetDate` and an inclusive range with `XmCalendarSetRange`.  Both
operations validate before changing state.  A range change is rejected if it
would exclude the current date, so a failed update has no partial effect.
The default range is 0001-01-01 through 9999-12-31.

`XmNcalendarWeekStart` is an unsigned weekday number using Sunday = 0 through
Saturday = 6.  `XmNcalendarRtl` mirrors the day columns and reverses horizontal
keyboard movement; it does not change the civil date or the week-start rule.
The heading and weekday labels use deterministic English fallback labels.  A
future locale provider may replace those labels without changing date
semantics.

Arrow keys move by one day or one week, Page Up/Down move by one month while
clamping the day to the target month, and Home/End select the first/last day of
the current month.  Return activates the current date.  Pointer activation
selects a day and a second activation invokes the activate callback.  Dates
outside the inclusive range remain visible but are not selectable.

`XmNcalendarDateChangedCallback` and `XmNcalendarActivateCallback` receive an
`XmCalendarCallbackStruct`.  Callback code may destroy the widget; the widget
does not access its instance after invoking a callback.

This first implementation deliberately keeps the public contract bounded and
date-only.  DatePicker and TimePicker controls, locale providers, and timezone
or daylight-saving interpretation are separate work packages.
