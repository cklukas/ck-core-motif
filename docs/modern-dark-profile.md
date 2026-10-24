# `modern-dark` appearance profile

`XmAPPEARANCE_MODERN_DARK` is an opt-in resource-backed profile selected before
widgets are created.  It supplies dark surfaces (`#202124`), light text
(`#f1f3f4`), restrained shadow colors, blue focus (`#8ab4f8`), dark arm/
selection surfaces (`#3c4043`) and a disabled foreground (`#9aa0a6`).  The
toggle-button class maps that disabled color through its insensitive rendering
path while preserving the darker general shadow default.  It uses
the same immutable Xrm precedence rules as `modern-light` and
`high-contrast`; explicit application resources remain authoritative.

The installed fixture creates real Label, PushButton, ToggleButton, TextField
and ScrollBar controls, realizes them under Xvfb, checks dark/disabled colors,
keyboard focus traversal and explicit white-button precedence, and verifies
that an application-supplied label pixmap remains the same X resource.  It
does not recolor or rescale application pixmaps.

The profile is qualified on the AArch64 `uconsole` Linux host with the current
Xft-enabled installed prefix.  Other desktops, compositors, image formats,
and platform theme bridges remain outside this claim.
