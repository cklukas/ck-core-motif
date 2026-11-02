# DNDDemo: drawing and drag-and-drop

DNDDemo demonstrates Motif's Uniform Transfer Model: color transfer onto a
rectangle, rectangle move/copy, valid/invalid drop feedback, and cancellation.

- Drag with the left button on empty canvas to create a rectangle. A dashed
  black/white outline follows the pointer and survives exposure. Release sets
  the final corner; a zero-width or zero-height gesture creates nothing.
- Press a named color swatch to choose the color for new rectangles. The
  current choice is shown below the palette. Black is the initial color.
- Drag a swatch onto an existing rectangle to paint it. Dropping a color onto
  empty canvas is invalid and leaves the canvas unchanged.
- Drag an existing rectangle to move it. Hold Ctrl to copy it instead. Both
  left and middle buttons initiate transfers; a middle button is not required.
- Escape cancels drawing or a transfer. The original rectangle survives a
  cancelled move.

The original demo created only black rectangles and required middle-button
transfers. Clicking a swatch previously did not select a drawing color. The
controls now explain themselves in the window and use primary-button input.
The preview uses retained geometry and ordinary drawing, rather than XOR state
shared with the permanent rectangle graphics context.

## Transfer contracts

`XmeDragSource` asks for `_MOTIF_EXPORT_TARGETS` locally before creating its drag
context. `_XmConvertHandler` permits that local negotiation without a context;
external selection requests still require a live context. The local-conversion
flag is consumed on entry so a rejected conversion cannot affect the next one.

A format-32 selection length counts items, not bytes. DNDDemo exports one
native `unsigned long` for a pixel and six for rectangle data (x, y, width,
height, color, pixmap). It decodes these items explicitly instead of treating a
packed CARD32 structure as a native-long array. This works on both ILP32 and
LP64. The receiver checks the type, format, count and rectangle dimensions.
It requests only an advertised target, owns and frees its callback state, and
requests source deletion only after successfully receiving a moved rectangle.

## Regression validation

Run from a native X11 development environment with Xvfb and libXtst:

```sh
python3 tests/automated/run-dnd-demo.py --prefix /absolute/install \
  --out /absolute/task-temporary-directory/dnd-demo
```

On macOS, use a fresh output below a mounted `/Volumes/S8T/tmp` or
`/Volumes/PRO-BLADE/tmp`. The runner compiles the actual demo source into a
fixture and drives XTEST pointer/key events on an isolated display. It tests
local negotiation and rejected contextless external requests, preview pixel
readback before release and after exposure, color transfer, move, Ctrl-copy,
selected drawing color, reverse-direction drawing, and Escape cancellation.
It runs classic, modern-light and modern-dark profiles and records source hashes,
commands, elapsed time, output and runtime library identity in `receipt.json`.
Native XQuartz inspection is separate from this automated fixture.
