---
nav_exclude: true
---
# Automatic ScrolledWindow wheel input

`XmAUTOMATIC` ScrolledWindow instances scroll plain work-area content with the
mouse wheel. This includes labels, ordinary controls and descendants created
after realization. The library supplies fallback Xt translations; the gallery
does not install application-specific wheel handlers.

- Buttons 4/5 move vertically by three scrollbar increments.
- Shift plus buttons 4/5, or horizontal wheel buttons 6/7, moves horizontally.
- Control, Mod1 (typically Alt), and Mod4 (typically Super) combinations are
  reserved for application behavior.
- Values clamp to the scrollbar range. A gesture at a bound does not emit an
  unchanged value callback. Reversed processing directions are honored.
- Insensitive widgets and non-editable scrollbars do not scroll. Native modal
  admission applies.

The nearest scroll frame owns the gesture. An application-defined frame blocks
fallback routing to an outer frame. List, Text, ScrollBar, TreeView and DataGrid
keep their own input behavior. Nested automatic frames do not also scroll their
parents, including when their range is exhausted.

Xt can merge an existing press action with a longer fallback press/release
sequence. A per-frame Xt action hook records whether an existing action handled
the gesture; the fallback then leaves it alone. This preserves both press-only
and press/release application bindings without inspecting Xt's private
translation structures. The hook is removed when the frame is destroyed.
Scrollbar value updates use the existing scroll-frame callback path. No frame
or scrollbar state is accessed after the update's callbacks, which may destroy
either widget.

The [Xt translation contract](https://www.x.org/archive/current/doc/libXt/intrinsics.pdf)
defines augmentation and ancestor action lookup. The implementation registers
its action in the ScrolledWindow class and augments descendants through the
public create hook; it adds no global application action table.

## Native regression

Run on Linux with Xvfb and the XTest development package:

```sh
python3 tests/automated/run-scrolled-wheel.py --prefix PREFIX --out OUTPUT
```

The fixture sends real XTest pointer/button input. It checks ordinary and late
children, vertical/horizontal movement, bounds and reversed direction, custom
press-only and press/release bindings, primary-button activation, nested frames,
exact native List/Text movement without outer scrolling, disabled/read-only
controls, and destruction from a value callback. Classic, modern light and
modern dark each run at 1x, 1.5x and 2x. Build/runtime identities are recorded by
the runner. ScrolledWindow's new private fields belong to the pending ABI 9
cohort; native subclasses must be rebuilt.
