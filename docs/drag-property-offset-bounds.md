---
nav_exclude: true
---

# Validate DragBS property offsets

DragBS atom and target-list readers now verify the serialized `heap_offset`
falls after the fixed header/table records and within the received property.
Malformed external-owner properties are rejected before table reconstruction,
while the existing protocol layout remains unchanged.
