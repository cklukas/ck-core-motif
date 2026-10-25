# ComboBox scrolled-list geometry

ComboBox scrolled-list layout now computes available width and height in
signed arithmetic and clamps collapsed dimensions to one pixel.  Large border,
highlight, or margin settings cannot wrap the list geometry into huge
unsigned dimensions.

The source-level regression is
`tests/automated/check-combobox-list-geometry.py`.
