# Icon-cache path length bounds

`CheckDirCache` now retains directory, file, and cached-entry name lengths as
`size_t` while deriving them from paths and offset arrays. This prevents a
long path from narrowing through `int` before cache matching, allocation, or
`strncmp` calls; the stored legacy record layout remains unchanged.

The focused source regression is
`tests/automated/check-icon-cache-path-lengths.py`.
