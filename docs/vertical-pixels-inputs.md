# Vertical pixel conversion input contract

`XmeToVerticalPixels` now rejects a null widget or output value pointer before
screen/unit lookup, returning `XmSYNTHETIC_LOAD`, matching the existing
horizontal conversion contract.  Valid vertical conversions are unchanged.
