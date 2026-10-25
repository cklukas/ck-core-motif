# Final text-property conversion buffer

After converting each item to usable text, the locale, text, string, and
standard ICC styles allocate one null-separated result buffer.  The allocation
is now checked; failure frees every per-item string, releases the pointer
table, balances the application lock, and returns `BadAlloc`.

The regression check is
`tests/automated/check-txtpropcv-final-allocation.py`.
