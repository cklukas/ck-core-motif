# Text selection source-string failure

The `XA_STRING`, compound-text, and UTF-8 selection conversion paths now check
the source string before calling `XmbTextListToTextProperty`.  A failed source
read or allocation returns an empty conversion cleanly instead of passing a
null string into the Xlib text encoder.

The focused source contract is checked by
`tests/automated/check-text-selection-source-failure.py`.
