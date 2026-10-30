# DataGrid model replacement and edit cleanup

Replacing a DataGrid model is transactional: a malformed model or allocation
failure leaves the previous model and its active edit untouched. After a new
model has been copied successfully, any pending edit text and error string are
released and the edit state is reset. Repeated model replacement therefore does
not retain abandoned edit buffers until widget destruction.
