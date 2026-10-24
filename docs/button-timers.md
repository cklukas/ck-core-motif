# Keyboard activation and release timers

ArrowButton, ArrowButtonGadget and push-enabled DrawnButton keep one pending
release-animation timeout during repeated keyboard activation. This matches
PushButton and PushButtonGadget. Each activation still delivers its activation
callback; repeated input does not replace the only stored timeout ID and leave
older animation callbacks behind.

Destruction cancels the pending timeout before the widget retires. Once a
release timeout fires and clears its ID, a later activation can schedule the
next animation normally. Public resource types, callback data, instance layouts
and the native ABI remain unchanged.

## Native regression

`tests/automated/check-button-timers.c` exercises all five button classes. For
each class it performs repeated native keyboard activation actions, checks callback
counts and the single scheduled timeout, destroys the button, and dispatches
real timers after destruction. It repeats the lifecycle three times per class.
This fixture checks the activation/timer contract; it does not qualify every
menu, pointer-grab or physical-display interaction.

Compile and run on Linux against one absolute installed prefix, using an
explicit writable task temporary directory:

```sh
export TMPDIR=/absolute/task/tmp
prefix=/absolute/toolkit/prefix
cc -D_GNU_SOURCE -std=c11 -Wall -Wextra -Werror -g -O2 \
  -I"$prefix/include" -L"$prefix/lib" -Wl,-rpath,"$prefix/lib" \
  tests/automated/check-button-timers.c -lXm -lXt -lX11 -ldl \
  -o "$TMPDIR/check-button-timers"
LD_LIBRARY_PATH="$prefix/lib" xvfb-run -a "$TMPDIR/check-button-timers"
```

Native AArch64 validation passes 630 checks across the five classes. The same
fixture fails against the unchanged prior library on the duplicate-timeout
assertion. The independent fixed build also passes the installed ABI check and
the existing SpinBox, List Unicode and DPI regressions.
