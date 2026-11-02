---
nav_exclude: true
---
# Appearance of gallery-launched demos

Every **Open** action snapshots the gallery's current appearance profile, density
and effective widget scale. The child receives standard Xt command-line resources:

- `ckCoreMotif.appearanceProfile`: classic, modern-light, modern-dark or high-contrast.
- `ckCoreMotif.appearanceDensity`: comfortable or compact.
- `Xft.dpi`: effective scale multiplied by 96, including Automatic display scaling.

Xt reads these values before creating the demo's controls. The launcher does not
change the parent's environment, user resource files or server resource database.
Applications retain authority over their explicit widget resources and semantic
colors. Existing child windows retain their startup selection: close and reopen
an example to apply changed gallery settings.

Installed executables under `PREFIX/share/Xm` take precedence over configured
build-tree copies. An absent installed target may use the build tree as a
convenience for development. Resolution never searches PATH or invokes a shell.
The child runs beside its assets and inherits the display and augmented UIDPATH.
Missing-app notices still show exact installed/build locations.

For direct interactive startup:

```sh
ckmotif-gallery --appearance modern-dark --scale 1.5 --open-demo airport
```

`--open-demo` accepts a catalogue executable name and opens the Demo Applications
page before launching it. Invalid or missing names fail before creating windows.

`run-gallery-demo-launch.py` checks resolution precedence, literal execution,
child-only arguments, error handling and reaping. `run-airport-behavior.py` uses
real XTest input on a separate Xvfb display to verify the actual child profile,
density and scale when launched through the production gallery, including the
Automatic fallback to scale 1 on displays without an explicit DPI resource.

Wrapped demo introductions and descriptions repaint after viewport resizing.
`run-gallery-demo-reflow.py` observes an externally resized compact window on an
isolated Xvfb server and checks visible text pixels without forcing exposure.

## Earth window and rotation speed

Earth uses `XmGetAppearanceProfile`, as other standalone demos do, to select
its rendering. Modern light/dark project a single 2048×1024 NASA world map onto
a sphere using bilinear sampling, surface lighting and an antialiased edge.
The original `terre.xbm` is a strip of thirty 64×64 monochrome frames; it is
retained only for classic/high-contrast rendering. No sequence of prerendered
modern globe frames is stored. The XPM texture loads once, without networking
or optional JPEG/PNG decoder dependencies. Its source and attribution are in
`demos/programs/earth/MAP-LICENSE`.

The decorated window can be moved and resized. Its canvas is black, and the
globe stays centered and uniformly scaled. Right-clicking the globe toggles
**Rotation (RPM; 0 pauses)** below the globe, in the same window. Hiding the
slider gives its space back to the globe; no second shell is created. The
slider uses normal theme colors and its preferred height, with a scale-aware
32-pixel track for a comfortable drag target. Only the globe area is black.

Left-clicking without dragging stops rotation. Dragging left/right turns the
globe directly and release sets the direction and speed from the last 120 ms
of motion (bounded to ±20 RPM). Holding it still for 150 ms before release
stops it. Press immediately freezes the displayed orientation and cancels the
timer; even a late callback cannot advance the globe during pointer control.
Scroll up to enlarge the globe or down to shrink it in fine 2% steps (5%–200%). The zoom
center is always the center of the globe area, even in a non-square window.
The slider updates to the release speed. A pointer grab handles release outside
the window; Escape cancels the gesture.

The timer targets **30 FPS** by default, independently of rotation speed. A
monotonic clock sets the continuous angle; late presentation deadlines are
skipped without catch-up bursts. Default rotation is **0.5 RPM**, one turn in
two minutes. The slider spans -20.0 to 20.0 RPM in 0.1 increments. Zero pauses
and negative values reverse. Pausing and destruction cancel the owned timeout.

`earth -speed 3 -fps 60` requests 0.3 RPM at 60 FPS. The integer `speed`
resource is in tenths of RPM (-200..200); `fps` is 1..120. Both clamp values
outside their ranges. Rendering cost and display transport can limit delivered
FPS; elapsed-time rotation remains correct when frames are skipped.

Native regression command (requires Xvfb and an installed toolkit):

```sh
python3 tests/automated/run-earth.py --prefix /path/to/install --out "$TMPDIR/earth-check"
```

Checks cover modern-light, modern-dark and classic: real 30 FPS cadence,
wall-clock rotation, pause/resume/reverse, delayed dispatch without bursts,
slider toggle/geometry and native thumb dragging, direct spin/click-to-stop, black margins, high-resolution texture detail,
interpolation/wrapping, an unshaped managed shell and resized rendering.
