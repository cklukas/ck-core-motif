# aicon directory-population safety

The historical `aicon` Container demonstration now uses typed C23 callback and
helper declarations.  Its directory scanner no longer alters the caller's
file-selection text, bounds the assembled icon path and icon-list allocation,
closes the directory stream, and releases the temporary text returned by the
file-selection widget.  Empty container selections are ignored safely before
trying to use their pixmap; failed pixmap geometry queries and empty directory
results no longer feed invalid values into the display or child-management
paths.

The focused source-contract check is:

```sh
python3 tests/automated/check-aicon-c23-directory-safety.py
```
