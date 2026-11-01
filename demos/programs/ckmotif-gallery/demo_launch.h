#ifndef GALLERY_DEMO_LAUNCH_H
#define GALLERY_DEMO_LAUNCH_H
#include <stddef.h>
#include <sys/types.h>
/* Resolve only configured build/install locations; never search PATH or invoke a shell. */
int gallery_demo_path(const char *build_root, const char *install_root,
                      const char *build_dir, const char *install_dir,
                      const char *binary, char *path, size_t size);
/* Child inherits display/environment, runs beside its assets. Returns exec errors synchronously. */
int gallery_demo_launch(const char *path, pid_t *pid);
#endif
