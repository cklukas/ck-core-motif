# MWM debug error-code formatting

The debug X-error handler converts the request opcode to text before querying
the X request database. That conversion now uses `snprintf` and clears the
temporary key if formatting reports an error, keeping the diagnostic path
bounded without changing normal error reporting.

The focused source contract is checked with:

```sh
python3 tests/automated/check-mwm-error-code-format.py
```
