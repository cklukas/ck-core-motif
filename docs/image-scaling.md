# Image scaling bounds

Image-to-pixmap conversion validates source rectangles, scaled dimensions,
row strides, and intermediate allocation sizes before creating the destination
image or coordinate tables. Invalid or unrepresentable image dimensions are
rejected cleanly, while ordinary scaling continues to use the existing cache
and rendering APIs.

Validated positive dimensions, depths, and color-symbol counts are converted
explicitly at X11/Xt API boundaries so strict conversion builds cannot hide a
sign or width change.

Failure exits also use braced lock-release blocks, so builds where the process
lock macro is empty remain unambiguous and warning-free.

The scaler also rejects a destination origin whose vertical extent cannot be
represented by the signed X11 coordinate type. This keeps the strip upload's
`dest_y + min_y` calculation defined while preserving valid off-screen origins.

The native regression exercises malformed image dimensions and calls the
private scaler with values that would previously overflow its legacy
coordinate and allocation arithmetic:

```sh
python3 tests/automated/run-image-regressions.py /absolute/path/to/installed-prefix /absolute/fresh/results
```

The fixture starts with a real initialized `XImage` and non-null pixel data.
Seventeen cases vary malformed fields and source/destination rectangles
independently and check that the destination stays unchanged. A positive
twofold checkerboard scaling case compares every destination pixel against an
independent expected value; this catches implementations that simply return
for every request. Earlier null-data-only calls did not reach the intended
dimension validation branches.

The runner verifies the actual loaded `libXm` path, fingerprints the source
and library before and after execution, and writes commands, logs and timing
to `receipt.json`. It compiles only the fixture and uses a fresh Xvfb display.
This covers the recorded corpus and valid ZPixmap scaling; it does not claim
complete decoder, color-depth or allocator-failure qualification.

On 2026-11-01 the corpus passed on rpi4 against an isolated snapshot of the
ABI9 library with SHA-256
`cb73f5a93257004b4509b09184782f17a99bdf583bf299cd5765b1540a4faa7a`.
The loaded-library assertion and all pixel checks passed. This evidence is
specific to that snapshot and does not qualify later library changes.
