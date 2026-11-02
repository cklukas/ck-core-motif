#define _POSIX_C_SOURCE 200809L
#include "demo_launch.h"
#include <errno.h>
#include <fcntl.h>
#include <stdint.h>
#include <math.h>
extern char **environ;
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <unistd.h>

static int executable(const char *path)
{
    struct stat info;
    return stat(path, &info) == 0 && S_ISREG(info.st_mode) && access(path, X_OK) == 0;
}

int gallery_demo_path(const char *build_root, const char *install_root,
                      const char *build_dir, const char *install_dir,
                      const char *binary, char *path, size_t size)
{
    if (!build_root || !install_root || !build_dir || !install_dir || !binary || !path || !size)
        return EINVAL;
    int length = snprintf(path, size, "%s/%s/%s", install_root, install_dir, binary);
    if (length < 0 || (size_t)length >= size) return ENAMETOOLONG;
    if (executable(path)) return 0;
    length = snprintf(path, size, "%s/%s/%s", build_root, build_dir, binary);
    if (length < 0 || (size_t)length >= size) return ENAMETOOLONG;
    if (executable(path)) return 0;
    /* Keep the missing-app notice anchored to the installed location. */
    length = snprintf(path, size, "%s/%s/%s", install_root, install_dir, binary);
    return length < 0 || (size_t)length >= size ? ENAMETOOLONG : ENOENT;
}

static int launch(const char *path, const char *profile, const char *density,
                  double scale, pid_t *pid)
{
    char profile_resource[96], density_resource[96], dpi_resource[96];
    if (profile) {
        if ((strcmp(profile,"classic") && strcmp(profile,"modern-light") &&
             strcmp(profile,"modern-dark") && strcmp(profile,"high-contrast")) ||
            !density || (strcmp(density,"comfortable") && strcmp(density,"compact")) ||
            !isfinite(scale) || scale <= 0.0 || !isfinite(scale*96.0)) return EINVAL;
        snprintf(profile_resource,sizeof(profile_resource),"ckCoreMotif.appearanceProfile: %s",profile);
        snprintf(density_resource,sizeof(density_resource),"ckCoreMotif.appearanceDensity: %s",density);
        snprintf(dpi_resource,sizeof(dpi_resource),"Xft.dpi: %.17g",scale*96.0);
    }
    int channel[2], error = 0;
    if (!path || path[0] != '/' || !pid) return EINVAL;
    char *directory = strdup(path);
    if (!directory) return ENOMEM;
    char *slash = strrchr(directory, '/');
    if (!slash) { free(directory); return EINVAL; }
    *slash = '\0';
    const char *inherited = getenv("UIDPATH");
    if (!inherited) inherited = "";
    size_t directory_size = strlen(directory), inherited_size = strlen(inherited);
    if (inherited_size > SIZE_MAX - 128 || directory_size > (SIZE_MAX - inherited_size - 128) / 4) { free(directory); return ENAMETOOLONG; }
    size_t uid_size = 4 * directory_size + inherited_size + 128;
    char *uid = malloc(uid_size);
    size_t entries = 0;
    while (environ && environ[entries]) ++entries;
    char **environment = calloc(entries + 2, sizeof(char *));
    if (!uid || !environment) { free(uid); free(environment); free(directory); return ENOMEM; }
    snprintf(uid, uid_size, "UIDPATH=%s%s%s/%%U:%s/%%L/uid/%%U:%s/%%l/uid/%%U:%s/C/uid/%%U",
             inherited, *inherited ? ":" : "", directory, directory, directory, directory);
    size_t used = 0;
    for (size_t i = 0; i < entries; ++i)
        if (strncmp(environ[i], "UIDPATH=", 8)) environment[used++] = environ[i];
    environment[used] = uid;
    if (pipe(channel) != 0) { error = errno; free(environment); free(uid); free(directory); return error; }
    if (fcntl(channel[1], F_SETFD, FD_CLOEXEC) < 0) {
        error = errno; close(channel[0]); close(channel[1]); free(environment); free(uid); free(directory); return error;
    }
    pid_t child = fork();
    if (child == 0) {
        close(channel[0]);
        if (chdir(*directory ? directory : "/") == 0) {
            char *const arguments[] = {(char *)path, profile ? "-xrm" : NULL,
                profile_resource, "-xrm", density_resource, "-xrm", dpi_resource, NULL};
            execve(path, arguments, environment);
        }
        error = errno;
        ssize_t written;
        do { written = write(channel[1], &error, sizeof(error)); } while (written < 0 && errno == EINTR);
        _exit(127);
    }
    if (child < 0) error = errno;
    close(channel[1]);
    free(directory);
    free(environment);
    free(uid);
    if (child >= 0) {
        ssize_t count;
        do { count = read(channel[0], &error, sizeof(error)); } while (count < 0 && errno == EINTR);
        if (count < 0) error = errno;
        if (error) { while (waitpid(child, NULL, 0) < 0 && errno == EINTR) {} }
        else *pid = child;
    }
    close(channel[0]);
    return error;
}

int gallery_demo_launch(const char *path, pid_t *pid)
{
    return launch(path, NULL, NULL, 0.0, pid);
}

int gallery_demo_launch_with_appearance(const char *path, const char *profile,
                                        const char *density, double scale, pid_t *pid)
{
    if (!profile) return EINVAL;
    return launch(path, profile, density, scale, pid);
}
