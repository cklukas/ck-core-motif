---
nav_exclude: true
---
# UIL expanded filename capacity

UIL source control blocks accept paths up to 255 bytes plus a terminating NUL.
The symbol-table root and include records previously declared their copied
`full_file_name` fields as 255 bytes, so the maximum accepted path could
overflow those fields when copied with the existing API.

Both internal fields now have 256 bytes, matching the source control buffer.
Existing filenames remain valid and the symbol-table API shape is unchanged;
the regression `tests/automated/check-uil-full-name-capacity.py` keeps the
capacities aligned.
