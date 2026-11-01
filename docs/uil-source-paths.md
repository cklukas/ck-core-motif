---
nav_exclude: true
---
# UIL source-path output bounds

UIL stores the resolved filename of an included source in a legacy fixed-size
symbol-table field. `src_open_file` therefore receives the field capacity from
its caller and copies the resolved path with explicit truncation and a trailing
NUL. A path longer than that historical field remains usable for opening and
diagnostics through the file-control block; only the fixed-size bookkeeping
copy is truncated, matching the existing root-module record behavior.

The focused regression compiles the production copy helper with strict warnings
and ASan/UBSan. It checks a 2048-byte resolved path, an ordinary path, null
input, and a zero-sized destination:

```sh
python3 tests/automated/run-uil-source-path-copy-unit.py --out /absolute/fresh/results
```
