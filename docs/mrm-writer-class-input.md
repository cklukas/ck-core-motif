# Mrm writer class input

`UrmCWRSetClass` now rejects a null user-defined class name before measuring or
copying it.  Empty names continue to return the existing `MrmNO_CLASS_NAME`
error, while valid class names and predefined class codes retain their normal
behavior.

The source-level regression is
`tests/automated/check-mrm-writer-class-input.py`.
