---
nav_exclude: true
---
# DropDown child geometry

DropDown child placement subtracts vertical spacing, shadow, borders, and
label/arrow widths from unsigned dimensions. During a collapsed resize those
subtractions could wrap, producing oversized child heights or unrelated
horizontal positions.

Placement now uses signed intermediates, clamps the available child height, and
saturates each child span after its border inset. Normal layouts retain their
existing geometry while transiently tiny controls remain bounded.

Geometry negotiation also distinguishes a refused parent height from a valid
width compromise. A refusal restores speculative child dimensions and returns
`XtGeometryNo`; it never reads an uninitialized reply or sends its fields to X.
Child preferred-size queries honor `request_mode`, using allocated geometry for
fields omitted by the child.

The native Linux/Xvfb regression is
`tests/automated/run-dropdown-negotiation.py`; it poisons partial query replies,
denies height and combined resize requests, and exercises FontSelector theme,
density, and scale transitions against the installed library.

Geometry negotiation also distinguishes a refused parent height from a valid
width compromise. A refusal restores speculative child dimensions and returns
`XtGeometryNo`; it never reads an uninitialized reply or sends its fields to X.
Child preferred-size queries honor `request_mode`, using allocated geometry for
fields omitted by the child.

The source contract is protected by
`tests/automated/check-dropdown-negotiation-contract.py`. A native Linux/Xvfb
runner can be added when an installed prefix is available for ABI-level
qualification.
