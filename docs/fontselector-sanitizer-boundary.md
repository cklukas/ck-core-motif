# FontSelector sanitizer boundary

The installed widget regression was run with the AArch64 sanitizer-built
`libXm` on `uconsole` on 2026-10-25. Locale, SpinBox, DPI, Unicode List,
ClipWindow and MenuBar fixtures passed. The FontSelector fixture reached its
normal teardown but exited nonzero because LeakSanitizer reported one 160-byte
allocation:

```text
XCreateGC -> XtAllocateGC -> Label.c:SetNormalGC
  -> DropDown.c:CreateChildren -> FontS.c:CreateFamilyBox
```

This is a system `libX11`/`libXt` allocation path, not an allocation made by
FontSelector's private data structures. A temporary `XtAllocateGC` /
`XtReleaseGC` observer showed that every Motif label destroy path releases its
normal and insensitive GCs, while one shared Xt GC-cache entry remains at
process exit. The observer was diagnostic only and is not part of the toolkit.

The result is retained as an open dependency-ownership boundary: the fixture
remains a failure under strict leak detection, and no suppression,
leak-detection disablement or false toolkit pass is permitted. Closing this
gate requires either an instrumented matching Xt/Xlib qualification or a
reproducer that demonstrates a missing `libXm` release. Until then, the
toolkit-wide sanitizer matrix must not be advertised as clean.
