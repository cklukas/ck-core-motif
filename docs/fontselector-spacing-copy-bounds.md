---
nav_exclude: true
---
# FontSelector spacing-field bounds

The fixed-width spacing fields in `FontData` and `FamilyInfo` now use the
existing bounded `StoreString` helper for every assignment. This preserves
the XLFD spacing values while preventing malformed or future-longer values
from overrunning the three-character storage.
