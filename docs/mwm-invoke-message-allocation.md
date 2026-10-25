# MWM invoke-message allocation

The workspace-management `SendInvokeMessage` path now checks its fixed-size
message allocation before packing the command and client window identifiers.
Under allocation pressure it returns without sending a malformed message,
while successful invocations retain the existing wire format and callback
behavior.

The source-level regression is
`tests/automated/check-mwm-invoke-message-allocation.py`.
