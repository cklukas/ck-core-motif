# Xmos initial-character direction bounds

`_XmOSGetInitialCharsDirection` now validates its input/output pointers and
keeps byte counts in `size_t` until they are known to fit the public
`unsigned int` result. Wide-character multiplication is checked, and the
internal `Wcslen` helper handles a null input without undefined pointer
subtraction.

The focused source regression is
`tests/automated/check-xmos-initial-direction-bounds.py`.
