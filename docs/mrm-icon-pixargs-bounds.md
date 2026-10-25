# Deferred icon argument bounds

`Urm__CW_CreateArglist` defers icon-image arguments until a widget has been
created so it can supply the display and colour parameters needed to build a
pixmap. The deferred storage is a fixed compatibility buffer with ten entries.
The ordinary icon-image path now checks that capacity before loading or storing
another icon, matching the existing check for literal-backed icons and bitmap
files. Excess malformed arguments are reported and skipped instead of writing
past the local array.
