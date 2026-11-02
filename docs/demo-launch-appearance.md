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
