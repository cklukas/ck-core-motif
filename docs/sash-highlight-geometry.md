# Sash highlight geometry

Sash focus painting fills or clears the widget interior after applying twice
the primitive shadow thickness. The highlight and unhighlight paths now skip
the operation when that inset consumes the widget, preventing unsigned width
or height wraparound on very small sashes. Normal sash focus painting is
unchanged.
