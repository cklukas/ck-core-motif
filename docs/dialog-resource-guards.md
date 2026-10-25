# DialogShell resource merge guard

DialogShell class initialization now checks the merged resource-table size and
allocation. On failure it skips the optional merge and still runs the normal
class-initialization hook, avoiding a null write or integer-overflowed request.
