---
nav_exclude: true
---

# XIM preedit overstrike recovery width

Overstrike recovery derives its replacement and recovery ranges from
`XmTextPosition`, but the legacy callback stored them in `int` and accumulated
the recovered multibyte byte count in another `int`. Large text positions or
long recovery spans could therefore truncate before allocation. Recovery now
keeps positions in `XmTextPosition` and byte totals in `size_t`, while retaining
the existing allocation limits.

`tests/automated/check-text-preedit-recovery-width.py` protects the conversion
and accumulation widths.
