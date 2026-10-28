# UIL empty filename scanning

`open_source_file` derives the directory portion of a source filename before
opening it.  The old loop formed `&c_file_name[len - 1]` before checking
whether `len` was nonzero, so an empty filename produced a pointer before the
string object.  The scan now indexes from the end only while `len > 0`, which
keeps malformed or empty input within the object bounds.
