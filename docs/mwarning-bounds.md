# Mwm menu-warning bounds

Mwm’s menu diagnostics include resource-defined menu names. The warning helper
now always emits a bounded message through `snprintf`; names longer than the
fixed diagnostic buffer are truncated safely instead of suppressing the
warning altogether. Existing short warning text is unchanged.

`tests/automated/check-mwarning-bounds.py` guards this behavior.
