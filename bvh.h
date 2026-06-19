#ifndef BVH_H
#define BVH_H

#include "aabb.h"
#include "hittable.h"
#include "hittable_list.h"
#include "raytracer.h"
#include <memory>

class bvh_node : public hittable {

  public: 

    bvh_node(std::vector<shared_ptr<hittable>>& objects, size_t start, size_t end) {
    }

    bvh_node(hittable_list list) : bvh_node(list.objects, 0, list.objects.size()) {
    }

    bool hit(const ray& r, interval ray_t, hit_record& rec) const override {
      if (!bbox.hit(r, ray_t)) return false;

      bool hit_left = left->hit(r, ray_t, rec);
      bool hit_right = right->hit(r, ray_t, rec);

      return hit_left || hit_right;
    }


    private:
    aabb bbox;
    shared_ptr<hittable> left;
    shared_ptr<hittable> right;

};

#endif

