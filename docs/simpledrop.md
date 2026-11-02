# Simple drag and drop demo

`simpledrop` demonstrates copying a number through Motif drag and drop.
Set the slider, then hold the left mouse button on **Drag number: …** below
it, move into **drop here**, and release. Each successful drop creates a label
at the drop location. The source label follows slider changes; a drag captures
the number at its start. Middle-button dragging also works on that label or
on the scale's numeric value above the slider.

The destination accepts `STRING`, `COMPOUND_TEXT` and `PIXMAP` from compatible
Motif sources. It is not a desktop file-drop receiver. Help → Tutorial explains
the gestures. Widget resource names remain stable (`drop_here` is the resource
name, while its displayed label is “drop here”).

## Regression validation

Run `python3 tests/automated/run-simpledrop.py --prefix /absolute/install
--out /absolute/fresh-results` on macOS with XQuartz and Xvfb installed. Use a
per-task directory on the prescribed temporary volume. The test compiles the
actual demo implementation and uses XTest pointer events on an isolated Xvfb
display to complete primary- and middle-button transfers. It checks the current
slider value, resulting dropped labels and destination title. The receipt
records source hashes, commands, timings and outcomes.
