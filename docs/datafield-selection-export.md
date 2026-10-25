---
nav_exclude: true
---
# DataField selection export conversion

When exporting a multibyte DataField selection, the conversion loop now treats
zero or negative `mblen()` results as a conversion failure instead of treating
them as progress.  This prevents an invalid locale sequence from causing an
infinite loop or unsigned length underflow.  Valid multibyte selections retain
their existing exported text and ownership behavior.

The `check-datafield-wcs` regression covers valid UTF-8 selection export after
the wide access-text conversion checks.
