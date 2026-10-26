# FileSelectionBox directory length bounds

The FileSelectionBox file and directory search paths now retain directory
prefix lengths from `strlen` as `size_t` while filtering and slicing returned
names. This avoids narrowing a long directory prefix to `unsigned` before
path indexing and compound-string generation.

The focused source regression is
`tests/automated/check-filesb-directory-lengths.py`.
