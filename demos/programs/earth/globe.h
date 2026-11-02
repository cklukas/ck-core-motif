/* Continuous orthographic globe projection. One geographic texture, no frames. */
#ifndef EARTH_GLOBE_H
#define EARTH_GLOBE_H
#include <math.h>
#define EARTH_TAU 6.28318530717958647692

typedef struct {
    float u, v, light, coverage;
} GlobePoint;

static double globe_advance(double angle, double elapsed, int tenths_rpm)
{
    double result = fmod(angle + elapsed * tenths_rpm * EARTH_TAU / 600.0,
                         EARTH_TAU);
    return result < 0 ? result + EARTH_TAU : result;
}

static GlobePoint globe_project(double x, double y, double radius)
{
    GlobePoint p = {0, 0, 0, 0};
    double distance = sqrt(x*x + y*y);
    double coverage = radius - distance + 0.5;
    double nx, ny, z, diffuse;
    if (coverage <= 0) return p;
    p.coverage = coverage < 1 ? coverage : 1;
    /* Clamp the partial edge pixel to the surface of the sphere. */
    if (distance > radius) { x *= radius / distance; y *= radius / distance; }
    nx = x / radius; ny = -y / radius;
    z = sqrt(fmax(0, 1 - nx*nx - ny*ny));
    p.u = 0.5 + atan2(nx, z) / EARTH_TAU;
    p.v = 0.5 - asin(fmax(-1, fmin(1, ny))) / (EARTH_TAU / 2);
    diffuse = fmax(0, -0.35*nx + 0.25*ny + 0.9027735*z);
    p.light = 0.38 + 0.62 * diffuse;
    return p;
}

/* Bilinear interpolation wraps longitude and clamps the poles. */
static void globe_sample(const unsigned char *rgb, int width, int height,
                         double u, double v, double color[3])
{
    double px = (u - floor(u)) * width;
    double py = fmax(0, fmin(height - 1, v * (height - 1)));
    int x0 = (int)px, x1 = (x0 + 1) % width;
    int y0 = (int)py, y1 = y0 + 1 < height ? y0 + 1 : y0;
    double fx = px - x0, fy = py - y0;
    int c;
    for (c = 0; c < 3; ++c) {
        double a = rgb[(y0*width+x0)*3+c]*(1-fx) + rgb[(y0*width+x1)*3+c]*fx;
        double b = rgb[(y1*width+x0)*3+c]*(1-fx) + rgb[(y1*width+x1)*3+c]*fx;
        color[c] = a*(1-fy) + b*fy;
    }
}
#endif
