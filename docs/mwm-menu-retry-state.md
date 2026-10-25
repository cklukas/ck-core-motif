# MWM menu retry state

`CreateMenuWidget` marks a menu specification while recursively constructing
its children.  Allocation failures now clear that marker before returning, so
a later attempt can retry the menu instead of being falsely rejected as menu
recursion.  Successful construction and recursion detection remain unchanged.
