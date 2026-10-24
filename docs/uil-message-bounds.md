# UIL diagnostic message bounds

UIL diagnostic text is copied into a fixed 132-byte listing line.  The
abbreviation and message text can both come from localized or source-provided
strings, so the old `sprintf` call could overrun the listing buffer.

The message line now uses bounded `snprintf`.  Normal diagnostics retain their
format, while unusually long text is truncated at the listing boundary without
corrupting compiler state.

`tests/automated/check-uil-message-bounds.py` protects this contract.
