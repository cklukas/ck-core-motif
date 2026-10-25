# ComboBox edit-field width bounds

ComboBox edit-field width and height now use signed available-space arithmetic
after borders, highlights, and dropdown arrow extents.  Collapsed controls
keep a one-pixel child geometry instead of wrapping to a huge unsigned
dimension.

The source-level regression is `tests/automated/check-combobox-edit-width.py`.
