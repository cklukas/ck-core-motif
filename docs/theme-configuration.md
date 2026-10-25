# Theme configuration

This fork does not ship the historical `XmThemes` implementation. The default
build therefore uses Motif resource defaults and application-provided resource
files. Passing `--enable-themes` now stops configuration with an explicit error
instead of silently producing a build without the requested feature.

This is separate from the supported additive appearance profiles in
`<Xm/Appearance.h>`. `XmSetAppearanceProfile` selects `classic`,
`modern-light`, `modern-dark` or `high-contrast` for a display before widgets
are created. The profiles contribute ordinary X resource fallbacks; explicit
widget values and application resource files retain precedence. Selection is
immutable for that display, so realized widgets are never changed behind an
application's back. See [appearance-profiles.md](appearance-profiles.md).

That preserves compatibility for normal builds while preventing a deployment
from claiming the historical theme implementation. A future implementation
may restore that option only by providing the files checked by `configure.ac`;
it would remain distinct from the current appearance-profile contract.
