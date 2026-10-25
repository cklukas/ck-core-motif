# UIL source filename capacity

UIL symbol records preserve both the source spelling and the expanded path.
The source spelling fields were 255 bytes even though callers can provide a
255-byte name, leaving no room for the terminating NUL during the existing
copies.

The root and include `file_name` fields now use 256 bytes, matching the
expanded-name capacity and preserving maximum-length input safely. The
regression `tests/automated/check-uil-file-name-capacity.py` keeps both fields
aligned.
