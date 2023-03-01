/*
 * @Author: Alien
 * @Date: 2023-03-01 11:55:29
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-01 22:47:32
 */
/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#pragma once

#include <nori/object.h>

NORI_NAMESPACE_BEGIN

/**
 * \brief Superclass of all emitters
 */
struct EmitterQueryRecord {
  Point3f ref;//表示着色点
  Point3f p;//光源上的点
  Normal3f n;//法线
  Vector3f wi;//从着色点ref到光源p的方向
  float pdf;//采样点p的pdf
  Ray3f shadowRay;//检测遮挡的射线

  EmitterQueryRecord(const Point3f& ref) : ref(ref) {}

  EmitterQueryRecord(const Point3f& ref, const Point3f& p, const Normal3f& n) : ref(ref), p(p), n(n) {
    wi = (p - ref).normalized();
  }
};
class Emitter : public NoriObject {
public:
    virtual ~Emitter() {}
    virtual Color3f eval(const EmitterQueryRecord& record) const = 0;
    virtual Color3f getRadiance() const = 0;
    virtual float pdf(const Mesh* mesh, const EmitterQueryRecord& lRec) const = 0;
    virtual Color3f sample(const Mesh* mesh, EmitterQueryRecord& lRec, Sampler*) const = 0;
    /**
     * \brief Return the type of object (i.e. Mesh/Emitter/etc.) 
     * provided by this instance
     * */
    EClassType getClassType() const { return EEmitter; }
};

NORI_NAMESPACE_END
