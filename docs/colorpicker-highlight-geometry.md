# ColorPicker highlight geometry

ColorPicker draws a focus rectangle inset by its highlight thickness.  The
focus path now checks that both widget dimensions exceed twice the inset before
subtracting it, preventing unsigned geometry wraparound on very small pickers.
Normal focus painting is unchanged.
