# Appearance-profile contract

WP-01 adds a small, additive selector without replacing Xt resources or
mutating realized widgets:

```c
#include <Xm/Appearance.h>

XmSetAppearanceProfile(display, XmAPPEARANCE_CLASSIC);
```

The selector is per `Display` and should be called after the display is opened
but before creating application widgets. The selected profile is stored in that display's
existing X resource database. `XmGetAppearanceProfile` reports the selected
profile; an unselected display behaves as `classic`. Unknown enum values are
rejected. The current package intentionally defines only `classic`, which
preserves existing Motif defaults and makes no visual changes.

Profile precedence is unchanged and testable: explicit widget arguments win
over ordinary X resources, and ordinary X resources win over toolkit defaults.
Future profiles must add only documented fallback values and must preserve this
ordering. Profile selection does not provide live style mutation; applications
must select a profile before creating widgets.

The installed consumer regression is
`tests/automated/run-appearance-profile.py`. It verifies default/selected
`classic`, rejection of an unknown value, and ordinary-versus-explicit width
precedence against the installed library under Xvfb.
