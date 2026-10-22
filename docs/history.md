# Fork history

The original Motif manuals and release notes describe the inherited toolkit.
The root `CHANGES` file records earlier fork work, including DPI-aware default
sizes for unlabeled toggle indicators, correct drag hotspot snap-back, explicit
X library linkage for tools and demos, and compiler prototype corrections.

## Maintenance foundation

The following changes precede the 2.4 development cohort:

| Source commit | Correction |
| --- | --- |
| `5b2ef86` | Generate Xm string definitions once under parallel make. |
| `8a03ed6` | Attach generated WML header dependencies to the object that consumes them. |
| `1f52842` | Declare terminating window-manager handlers with the Xt noreturn contract. |
| `791240f` | Enable Xft only after successful dependency probes. |
| `2b63cf1` | Correct selection timestamp item counts and release DataField encoding-probe storage. |

Selection lengths are protocol item counts: a format-32 timestamp is one
item, rather than `sizeof(Time)` items. This matters on platforms where native
`long` and the X protocol element size differ.

## Development 2.4 cohort

The next cohort introduces bounds-checked XIM preedit replacement, ICU Unicode
coordinate maps, coherent ICU/HarfBuzz text captures, native accessibility
providers and an explicit native ABI epoch. The [status page](status.md) lists
the validation limits. A source commit or a successful standalone fixture does
not establish complete widget or input-method acceptance.
