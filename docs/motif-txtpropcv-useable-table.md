# Usable-text table allocation

The locale, text, string and standard ICC conversion styles now check the
`useable_text` pointer table allocation before filling it. Non-empty tables
return `BadAlloc` with the application lock balanced instead of entering the
conversion loop with null storage.

The regression check is
`tests/automated/check-txtpropcv-useable-table.py`.
