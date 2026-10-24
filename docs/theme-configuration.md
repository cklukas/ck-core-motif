# Theme configuration

This fork does not ship the historical XmThemes implementation. The default
build therefore uses Motif resource defaults and application-provided resource
files. Passing `--enable-themes` now stops configuration with an explicit error
instead of silently producing a build without the requested feature.

That preserves compatibility for normal builds while preventing a deployment
from claiming theme support that is not present. A future theme implementation
can restore the option by providing the files checked by `configure.ac`.
