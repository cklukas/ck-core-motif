# ScrollFrame navigator addition input validation

`_XmSFAddNavigator` now rejects a null frame, navigator, or scroll-frame state
before trait lookup and callback registration.  This keeps navigator setup
safe during partial initialization and teardown.

The regression check is
`tests/automated/check-scrollframe-add-input.py`.
