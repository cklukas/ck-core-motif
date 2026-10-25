# PushButton default-shadow geometry

When erasing the default-button shadow, `XmPushButton` derives the fill area
from the widget size and an emphasis adjustment.  A zero highlight thickness
could make that adjustment negative, and a tiny widget could then pass wrapped
width or height values to the fill helper.

The adjustment is now clamped to zero and the fill is skipped unless both
dimensions contain the requested inset.  Normal default-button rendering is
unchanged while collapsed buttons avoid oversized erase operations.
