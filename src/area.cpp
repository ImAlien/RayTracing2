/*
 * @Author: Alien
 * @Date: 2023-03-01 22:47:57
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-01 22:50:10
 */
#include <nori/emitter.h>
#include <nori/sampler.h>
#include <nori/mesh.h>
NORI_NAMESPACE_BEGIN
class AreaLight : public Emitter {
 private:
  Color3f m_radiance;
 public:
  AreaLight(const PropertyList& propList) {
    m_radiance = propList.getColor("radiance");
  }

  Color3f eval(const EmitterQueryRecord& record) const override {
    const EmitterQueryRecord& lRec = record;
    return (lRec.n.dot(lRec.wi) < 0.0f) ? m_radiance : 0.0f;//法线和wi要面朝不同方向，这样光源正面才朝着着色点
  }

  Color3f getRadiance() const override {
    return m_radiance;
  }

  Color3f sample(const Mesh* mesh, EmitterQueryRecord& lRec, Sampler* sample) const override {
    auto sRec = mesh->sampleSurfaceUniform(sample);//均匀采样一个光源
    lRec.p = sRec.p;
    lRec.n = sRec.n;
    lRec.wi = (lRec.p - lRec.ref).normalized();
    lRec.shadowRay = Ray3f(lRec.ref, lRec.wi, Epsilon, (lRec.p - lRec.ref).norm() - Epsilon);
    lRec.pdf = pdf(mesh, lRec);
    if (lRec.pdf > 0.0f && !std::isnan(lRec.pdf) && !std::isinf(lRec.pdf)) {
      return eval(lRec) / lRec.pdf;//直接返回除以了pdf的值
    }
    return Color3f(0.0f);//返回0表示采样失败了
  }

  float pdf(const Mesh* mesh, const EmitterQueryRecord& lRec) const override {
    float cosTheta = lRec.n.dot(-lRec.wi);
    if (cosTheta > 0.0f) {
      //原本的pdf是定义在光源面积上的，但是渲染方程里是定义在立体角上
      //这里直接转换成立体角上的pdf了，之后有推导
      return mesh->getPdf().getNormalization() * (lRec.p - lRec.ref).squaredNorm() / cosTheta;
    }
    return 0.0f;
  }

  std::string toString() const override {
    return "Emitter[]";
  }
};
NORI_REGISTER_CLASS(AreaLight, "area")
NORI_NAMESPACE_END