# Gallery startup appearance and scale

The gallery applies an explicitly requested appearance profile and display
scale immediately after opening its display and before creating child
widgets. This keeps screenshot runs deterministic and preserves the Motif
contract: `XmApplyAppearanceProfile` is a live transaction for an existing
hierarchy, while startup selection belongs to `XmSetAppearanceProfile`.

Applying a profile after realization caused legacy widgets that cache graphics
contexts to rebuild those contexts while painting. On the rpi4 Xvfb capture
path that could produce an X `BadGC` and leave the documentation screenshot
set incomplete. The startup path avoids that lifetime hazard while retaining
the public profile and scale APIs.

The ordering contract is checked by:

```sh
python3 tests/automated/check-gallery-startup-profile.py
```
