# DT window-management request cleanup

`GetDtWmRequest` keeps the X property buffer between calls while it extracts
the NUL-separated requests.  If the property is absent, the X request fails,
or the returned format is not 8-bit, the buffer must still be released before
the static state is reset.  The invalid-property path now frees that buffer,
preventing a leak each time a malformed request is received.
