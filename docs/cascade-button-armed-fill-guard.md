# CascadeButton armed fill guard

The etched-in `XmCascadeButton` redisplay path fills the realized window when
its menu item is armed.  During resource replacement or a zero-sized geometry
transition, the arm GC or dimensions can be unavailable.

The fill request now validates those prerequisites.  Label exposure, cascade
pixmap drawing, and shadow rendering continue through their existing paths.
