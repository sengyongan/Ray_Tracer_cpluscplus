#ifndef AABB_H
#define AABB_H

#include "interval.h"
#include "../render/ray.h"

/* 轴对齐包围盒 */
class aabb
{
public:
    interval x, y, z; /* 3轴方向的最小最大边界 */

    aabb() {} // The default AABB is empty, since intervals are empty by default.

    aabb(const interval &x, const interval &y, const interval &z)
        : x(x), y(y), z(z)
    {
        pad_to_minimums();
    }

    aabb(const point3 &a, const point3 &b) /* 通过两个点确定包围盒xyz */
    {
        x = (a[0] <= b[0]) ? interval(a[0], b[0]) : interval(b[0], a[0]);
        y = (a[1] <= b[1]) ? interval(a[1], b[1]) : interval(b[1], a[1]);
        z = (a[2] <= b[2]) ? interval(a[2], b[2]) : interval(b[2], a[2]);
        pad_to_minimums();
    }
    aabb(const aabb &box0, const aabb &box1) /* 根据两个包围盒求新的包围盒 */
    {
        x = interval(box0.x, box1.x);
        y = interval(box0.y, box1.y);
        z = interval(box0.z, box1.z);
    }
    const interval &axis_interval(int n) const
    {
        if (n == 1)
            return y;
        if (n == 2)
            return z;
        return x;
    }

    bool hit(const ray &r, interval ray_t) const /* 光线和包围盒求交 */
    {
        const point3 &ray_orig = r.origin();
        const vec3 &ray_dir = r.direction();

        for (int axis = 0; axis < 3; axis++)
        {
            const interval &ax = axis_interval(axis);
            const double adinv = 1.0 / ray_dir[axis];

            auto t0 = (ax.min - ray_orig[axis]) * adinv;
            auto t1 = (ax.max - ray_orig[axis]) * adinv;

            if (t0 < t1)
            {
                if (t0 > ray_t.min)
                    ray_t.min = t0;
                if (t1 < ray_t.max)
                    ray_t.max = t1;
            }
            else
            {
                if (t1 > ray_t.min)
                    ray_t.min = t1;
                if (t0 < ray_t.max)
                    ray_t.max = t0;
            }

            if (ray_t.max <= ray_t.min)
                return false;
        }
        return true;
    }
    int longest_axis() const /* 返回边界框最长轴的索引。 */
    {

        if (x.size() > y.size())
            return x.size() > z.size() ? 0 : 2;
        else
            return y.size() > z.size() ? 1 : 2;
    }

    static const aabb empty, universe;

private:
    void pad_to_minimums()
    {
        // Adjust the AABB so that no side is narrower than some delta, padding if necessary.调整AABB，使任何边都不会比delta窄，必要时可以填充。

        double delta = 0.0001;
        if (x.size() < delta)
            x = x.expand(delta);
        if (y.size() < delta)
            y = y.expand(delta);
        if (z.size() < delta)
            z = z.expand(delta);
    }
};
aabb operator+(const aabb &bbox, const vec3 &offset);

aabb operator+(const vec3 &offset, const aabb &bbox);
#endif