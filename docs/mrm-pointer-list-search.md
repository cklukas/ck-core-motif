# Mrm pointer-list string search

`UrmPlistFindString` now skips null pointer slots before calling `strcmp`.
This keeps searches safe when callers append a null pointer or reuse a list
whose contents were cleared.
