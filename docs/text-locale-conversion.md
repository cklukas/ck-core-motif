---
nav_exclude: true
---
# Locale-text conversion failure state

`_XmTextToLocaleText` now initializes its optional `success` result to
`False` and validates the widget and source value before obtaining atoms from
the display.  Unsupported encodings, null inputs, and conversion failures
therefore cannot leave a caller with stale success state or trigger a display
access through a null widget.

The focused regression is
`tests/automated/check-text-locale-conversion.py`.
