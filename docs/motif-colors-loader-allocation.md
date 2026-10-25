# Color selector allocation handling

The RGB color loader now keeps `ColorInfo` growth transactional and aborts
cleanly when a realloc or size calculation fails. Building the temporary
XmString list also checks its allocation before filling it. The existing color
selector state is left intact instead of dereferencing a null allocation.
