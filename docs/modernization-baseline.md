---
nav_exclude: true
---
# WP-00 modernization baseline

The WP-00 fixture is an installed-library consumer, not a source-tree unit
test.  It constructs a fixed representative hierarchy containing a Form,
Label, TextField, PushButton, ToggleButton, ScrolledWindow and nested Form.
It realizes the hierarchy, exercises keyboard focus traversal, resizes the
shell, drains the resulting Xt events and records final geometry and configure,
expose and focus activity.  It also captures the actual realized shell as a
PPM image.

Run it on Linux with an isolated Xvfb and a fresh installed prefix:

```sh
python3 tests/automated/run-modernization-baseline.py \
  --prefix /absolute/installed/prefix \
  --out /absolute/fresh/results
```

The runner compiles the consumer with the selected prefix's headers and
libraries, checks `ldd` for the selected `libXm.so.8`, sets an explicit
`Xft.dpi` resource through `XENVIRONMENT`, and records source/library hashes,
commands, timings and result/image hashes in `receipt.json`.  The output
directory is both the temporary workspace and the durable receipt location; it
must not be reused.

The 2026-10-24 AArch64 `uconsole` qualification passed at 96 and 144 dpi.
The fixture SHA256 was
`0968833e75d83e1e11b74084445964b4c9b822b2931e7aa22b003351d07a1b54`; the
installed `libXm.so.8` SHA256 was
`0c69b040374f3d929c0509160c7b51bda8aa4aa2382f6bf45f192f8d5c5b4333`.
Both runs produced result SHA256
`ed7ce47967763b19007bb1aa734fe3ca88e6700a4f402e41ce0b0ca1f8069796`
and image SHA256
`fd9be5b9f8f7d56a07273c77b8b966f55045434937ad582cb4f424b1209435a9`.

The baseline is intentionally not a performance target.  Its event counts
describe one server/widget configuration and are useful for comparison with a
later equivalent run.  They do not count internal geometry requests that never
produce an X event, and the PPM image is evidence for actual-window capture,
not an automated claim of visual quality.  Visual review, other X servers,
window managers, architectures, fonts and DPI contracts require separate
qualification.
