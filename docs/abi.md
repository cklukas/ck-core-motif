# Native ABI cohort

Development version **2.4.0** uses native ABI epoch **8**. The suite's libtool
version is `8:0:0`, yielding `libXm.so.8`, `libMrm.so.8` and `libUil.so.8` on ELF
systems. There is no alias or compatibility shim for the preceding SONAME.

Adding a public function is not the only ABI concern. Cached Unicode maps,
text-field layout state and text-output cache state change native widget/source
records. The line-table width changes from `Dimension` to `int`, avoiding
16-bit truncation. Code that subclasses widgets, embeds native records or
accesses `P.h`/`I.h` members is a binary consumer of those layouts.

The first published 2.4 development cohort used epoch 5. Widget font-context
fields and public accessibility info/provider value fields change layouts again;
epoch 6 separates these consumers explicitly. Rebuild custom semantic providers,
not only applications and widget subclasses. No old-SONAME alias is provided.

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

## Epoch 8 incremental native Unicode source maps

TextField now owns an immutable Unicode source-map reference alongside its
layout capture. Native shared Text sources retain maps through approved edits.
The TextField private record changes size, so the whole toolkit and all native
consumers must rebuild for epoch 8. No alias or mixed epoch layout is provided.
The new map API retains unchanged LF-delimited paragraph bodies and exposes
construction-work accounting. Captures may retain old immutable maps across edits.
Fresh installed-prefix/runtime qualification is separate from source implementation.

## Epoch 7 native list and selection records

Native List items now own monotonic, non-reused identities, independent of
positions and labels. The native List record owns its next identity and coherent
change callback. Public accessibility info/provider records add selection
capability and typed selection callbacks. These change both native private
records and custom provider layout. Epoch 7 requires a coherent rebuild of
Xm/Mrm/Uil, subclasses, custom providers and applications. The earlier epoch 6
prefix remains valid only for its separately qualified consumers. No alias or
parallel compatibility layout is supplied. The fresh installed-prefix build and all 20 native ARM64 feature fixtures pass,
including the exact libXm.so.7 loader boundary and native list protocol/fault
checks. Nine focused ASan/UBSan fixtures also pass for the rebuilt toolkit, including
List protocol/lifetime and provider reentrancy. Xt/system dependencies are
uninstrumented and leak checking is disabled. Actual-application adoption and
x86_64 compilation/runtime qualification remain separate gates.
