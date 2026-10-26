# MWM client title initialization

Client and icon titles are populated from window-manager properties and may be
replaced during later property notifications. `GetClientInfo` now initializes
both XmString handles to `NULL`, ensuring cleanup and replacement paths remain
safe when a client has no usable title property.
