#ifndef HITTABLE_LIST_H
#define HITTABLE_LIST_H

#include "aabb.h"
#include "hittable.h"
#include "vec3.h"
#include <vector>


class hittable_list : public hittable {
  public:
    std::vector<shared_ptr<hittable>> objects;
     
    hittable_list() {}
    hittable_list(shared_ptr<hittable> object) {add(object); }

    void clear() { objects.clear(); }

    void add(shared_ptr<hittable> object) {
      objects.push_back(object);
      bbox = aabb(bbox, object->bounding_box());
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
      hit_record temp_rec;
      bool hit_anything = false;
      auto closest_so_far = ray_t.max;
      // iterator over the objects 
      for (const auto& object : objects) {
        if (object->hit(r, interval(ray_t.min, closest_so_far), temp_rec)) {
          hit_anything = true;
          closest_so_far = temp_rec.t;
          rec = temp_rec;
        }
      }

      return hit_anything;
    }

    void translate(const vec3& v) override {
      bbox = aabb();
      for (const auto& object : objects) {
        object->translate(v);
        bbox = aabb(bbox, object->bounding_box());
      }
    }

    // in this function, the translate's manage the bounding box
    // each rotation ends with a translation (and recalculation) of the items bounding box)
    void rotate_axis(double x, double y, double z, const point3& p) override {
      for (const auto& object : objects) {
        object->rotate_axis(x, y, z, p);
      }
    }

    aabb bounding_box() const override { return bbox; }

  private:
    aabb bbox;
};

#endif
