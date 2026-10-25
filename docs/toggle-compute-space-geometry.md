# ToggleButton ComputeSpace geometry

`XmToggleButton` recomputes label positions when the widget is smaller than
its preferred content. The old centering and right-to-left placement paths
subtracted unsigned dimensions before assigning signed text positions, so a
collapsed button could wrap its label origin.

The calculations now convert each dimension before subtraction. Normal label
placement is unchanged while undersized buttons keep deterministic positions.
