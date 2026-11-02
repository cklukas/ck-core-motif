# MWM title-font metric validation

Title bars store their measured text height as an unsigned value, but X font
ascent and descent are signed integers.  MWM now adds the metrics in `long`
and accepts the result only when it is positive and representable as the title
height.  Invalid metrics use the window manager's existing 16-pixel robustness
fallback and emit a diagnostic, preventing signed arithmetic from wrapping
into an oversized title bar.
