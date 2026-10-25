# CareVisual child-count bounds

`_XmNotifyChildrenVisual` traverses a composite widget's child array. The
loop now uses the Xt `Cardinal` child-count type, preserving the existing
care-parent-visual notification behavior while avoiding signed-index
wraparound for large child lists.
