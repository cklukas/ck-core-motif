---
nav_exclude: true
---
# UTF-8 resource conversion length accounting

`XmString` resource conversion may expand a source string when converting it
to UTF-8.  The output buffer is appended using the converted byte count, and
the running length must advance by that same count.  Advancing by the source
length leaves the output cursor inside the converted data, so a following
segment can overwrite bytes or produce a truncated text property.

The conversion path now records the converted length once, checks the
`unsigned int` and aggregate-length bounds, and uses that value for both the
append and the cursor update.
