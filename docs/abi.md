# Native ABI cohort

Development version **2.4.0** starts native ABI epoch **5**. The suite's libtool
version is `5:0:0`, yielding `libXm.so.5`, `libMrm.so.5` and `libUil.so.5` on ELF
systems. There is no alias or compatibility shim for the preceding SONAME.

Adding a public function is not the only ABI concern. Cached Unicode maps,
text-field layout state and text-output cache state change native widget/source
records. The line-table width changes from `Dimension` to `int`, avoiding
16-bit truncation. Code that subclasses widgets, embeds native records or
accesses `P.h`/`I.h` members is a binary consumer of those layouts.

## Required rebuild

Rebuild the toolkit suite, custom widgets and subclass libraries, Mrm/Uil
integrations, resource-compiler consumers and applications against one exact
installed cohort. Remove stale generated objects first. Static linkage also
requires a rebuild. Never mix headers from one prefix with another prefix's
runtime library.

```c
#include <Xm/Xm.h>

if (XmToolkitABI() != XmABI_VERSION ||
    xmUseVersion != XmVersion) {
    /* Refuse this installation before creating native widgets. */
}
```

`XmVERSION`, `XmREVISION` and `XmUPDATE_LEVEL` describe source version;
`XmABI_VERSION` and `XmToolkitABI()` identify native layout epoch. Matching
numbers are necessary but insufficient: optional features and exact source
bytes can still differ. Prove compilation header paths and runtime-loaded
library paths, retain the configure manifest and hashes, and run the cohort
fixture against the installed prefix before selecting it.

Keep an older verified prefix isolated while its consumers remain in use.
Retire it after they have moved to the rebuilt, verified replacement. The
repository's root `ABI.md` is the source policy for this cohort.
