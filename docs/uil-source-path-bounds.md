---
nav_exclude: true
---
# UIL source-path length bounds

Source-file lookup composes the main-file directory, include directories, and
the requested filename into a fixed path buffer. The composition checks each
length with subtraction-based bounds before adding lengths, so an unusually
large include-directory string cannot wrap the arithmetic and bypass the
buffer check. Existing absolute-path and include-search ordering is unchanged.

The contract is covered by
`tests/automated/check-uil-source-path-bounds.py`.
