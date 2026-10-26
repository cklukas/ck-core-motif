# MWM workspace-list allocation cleanup

WSM allocates a per-client workspace list during `GetClientInfo`. Invalid
screen workspace metadata or a failed list allocation happens before the
client is registered, so the newly allocated `ClientData` remains the caller's
responsibility. Both error exits now release it before returning `NULL`.
