---
nav_exclude: true
---
# SM client ID property handling

`ProcessSmClientID` receives the `SM_CLIENT_ID` value as an X property.  X
property data is length-delimited and is not required to include a C-string
terminator.  The reader now rejects empty or oversized values, copies valid
bytes into an Xt-managed buffer, appends a terminator, and releases the raw
Xlib buffer.  Existing consumers can therefore continue using the stored
value as a normal string without reading past the property.
