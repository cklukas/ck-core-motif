# PanedWindow keyboard sash batching

`XmPanedWindow` batches keyboard sash movements during the display's multi-click
interval. Opposite directions can cancel the accumulated movement to zero while
the batch's timeout remains pending. The pending timeout ID now determines
whether a new timeout is needed. Returning the movement total to zero no longer
allows the next key to schedule a second timeout and overwrite the owned ID.

A cancelled batch leaves pane sizes unchanged. Further keys in the same interval
contribute to that batch normally; after it fires, the next key starts a fresh
batch. Destroying the PanedWindow cancels its single pending sash timeout. The
existing batching interval, default and large increments, orientation behavior,
action parameters, resources, instance layouts and native ABI remain unchanged.
This correction applies to `XmPanedWindow` (`PanedW.c`), not the separate
`XmPaned` widget.

## Native regression

`tests/automated/check-paned-keyboard.c` invokes the real sash `SashAction` action
in horizontal and vertical PanedWindows, with default and large increments.
It checks repeated cancellation, cancellation followed by movement, subsequent
batches, exact pane sizes and destruction with a pending batch. Timer scheduling
and cancellation are observed through wrappers that delegate to the real Xt
functions; actual timeouts run in an isolated Xvfb event loop. This fixture does
not claim physical-display or application-level keyboard acceptance.

Compile and run against an absolute installed prefix on Linux, with a writable
task temporary directory:

```sh
export TMPDIR=/absolute/task/tmp
prefix=/absolute/toolkit/prefix
cc -D_GNU_SOURCE -std=c11 -Wall -Wextra -Werror -g -O2 \
  -I"$prefix/include" -L"$prefix/lib" -Wl,-rpath,"$prefix/lib" \
  tests/automated/check-paned-keyboard.c -lXm -lXt -lX11 -ldl \
  -o "$TMPDIR/check-paned-keyboard"
LD_LIBRARY_PATH="$prefix/lib" xvfb-run -a "$TMPDIR/check-paned-keyboard"
```

Native AArch64 validation passes 1,072 checks. The unchanged prior library fails
the same fixture on its duplicate-timeout assertion. The independent fixed build
also passes the installed ABI check, the existing SpinBox, List Unicode and DPI
regressions, and the 630-check button activation/timer regression.
