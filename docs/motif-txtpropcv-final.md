# Final text-property conversion buffer

The locale, text, string and standard ICC conversion styles now check the
final null-separated result buffer. Allocation failure releases every
usable-text item and the temporary table, balances the application lock, and
returns `BadAlloc`.

The regression check is
`tests/automated/check-txtpropcv-final-allocation.py`.
