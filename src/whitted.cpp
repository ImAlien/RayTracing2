/*
 * @Author: Alien
 * @Date: 2023-03-01 22:55:22
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-02 11:45:18
 */
#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/sampler.h>
#include <nori/emitter.h>
#include <nori/bsdf.h>
NORI_NAMESPACE_BEGIN
class WhittedIntegrator : public Integrator {
 public:
  WhittedIntegrator(const PropertyList& props) {}
  Color3f Li(const Scene* scene, Sampler* sampler, const Ray3f& ray) const {
    Intersection its;
    Color3f color(0.0f);
    if (!scene->rayIntersect(ray, its)) {
      return color;//没碰到任何场景物体直接返回
    }
    Color3f Le(0.0f);
    if (its.mesh->isEmitter()) {//光源
      EmitterQueryRecord lRecE(ray.o, its.p, its.shFrame.n);
      Le = its.mesh->getEmitter()->eval(lRecE);
    }
    Color3f totLi(0.0f);
    if (its.mesh->getBSDF()->isDiffuse())
    for (auto light :scene->getEmitters()){
        EmitterQueryRecord lRec(its.p);
        Color3f Li = light->getEmitter()->sample(light, lRec, sampler);//光源上均匀采样
        if (scene->rayIntersect(lRec.shadowRay)) {//计算可见性
        Li = 0;
        }
        float cosTheta = Frame::cosTheta(its.shFrame.toLocal(lRec.wi));
        BSDFQueryRecord bRec(its.toLocal(-ray.d), its.toLocal(lRec.wi), ESolidAngle);
        Color3f fr = its.mesh->getBSDF()->eval(bRec);//计算BSDF
        if (cosTheta < 0) {//去掉朝下半球去的情况
        cosTheta = 0;
        }
        totLi += Li * fr* cosTheta;
    }
    else {//不是的话，递归
      BSDFQueryRecord bRec(its.toLocal(-ray.d));
      Color3f refColor = its.mesh->getBSDF()->sample(bRec, sampler->next2D());//采样一个出射方向
      if (sampler->next1D() < 0.95 && refColor.x() > 0.f) {//递归终止条件
        return Li(scene, sampler, Ray3f(its.p, its.toWorld(bRec.wo))) / 0.95 * refColor;
      } else {
        return Color3f(0.0f);
      } 
    }   
    return Le + totLi;
    //BSDFQueryRecord bRec(its.shFrame.toLocal(-ray.d));//入射方向是世界坐标，bsdf计算时在切线空间下
    //Color3f f = its.mesh->getBSDF()->sample(bRec, sampler->next2D());//就是公式中的fr
    //Color3f Li = 0;
    //Ray3f rayR = Ray3f(its.p, its.shFrame.toWorld(bRec.wo), 0.0001f);
    //Intersection itsR;
    //if (scene->rayIntersect(rayR, itsR)) {//出射方向有没有碰到物体
    //  if (itsR.mesh->isEmitter()) {
    //    EmitterQueryRecord lRec = EmitterQueryRecord(its.p, itsR.p, itsR.shFrame.n);
    //    Li = itsR.mesh->getEmitter()->eval(lRec);//出射方向正好是光源，也就是公式的Li
    //  }
    //}
    //return Le + Li * f;
  }
  std::string toString() const {
    return "WhittedIntegrator[]";
  }
};
NORI_REGISTER_CLASS(WhittedIntegrator, "whitted");
NORI_NAMESPACE_END