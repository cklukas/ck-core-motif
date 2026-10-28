# Accessibility selection counts

List selection counts are stored as `size_t` internally but exposed through an
integer accessibility API.  `XmAccessibilitySelectionCount` now uses the
shared saturating conversion helper, preventing implementation-defined
wraparound for an unusually large native selection set.
