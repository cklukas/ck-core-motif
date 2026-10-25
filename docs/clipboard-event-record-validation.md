# Clipboard event record validation

Clipboard client-message handling now validates retrieved format records and
header records before dereferencing them, including minimum record lengths and
callback-index bounds. It also releases records when callback state is absent.
Malformed or incomplete properties are ignored without invoking an invalid
callback or corrupting clipboard state.
