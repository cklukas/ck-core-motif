# DataField wide callback bridge bounds

When a character modify-verify callback is followed by a wide callback,
DataField copies callback-owned text into a temporary wide buffer.  Both the
callback-provided length and the original insertion length are now validated
before adding the terminator or multiplying by `sizeof(wchar_t)`.

Invalid or unrepresentable callback lengths fail the edit without issuing an
undersized allocation.
