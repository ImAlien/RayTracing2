/*
 * @Author: Alien
 * @Date: 2023-03-01 15:28:52
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-01 16:06:49
 */
#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/vector.h>
#include <nori/color.h>
#include <nori/warp.h>
 
NORI_NAMESPACE_BEGIN
 
class AoIntegrator : public Integrator {
public:
    AoIntegrator(const PropertyList &props) {
        //std::cout << "Parameter value was : " << m_myProperty << std::endl;
    }
 
    /// Compute the radiance value for a given ray. Just return green here
    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const { 
        //环境光
        Intersection its;
        if (!scene->rayIntersect(ray, its)) {
            return Color3f(0.0f);
        }
        const auto& frame = its.shFrame;
        auto rng = sampler->next2D();//单位正方形内均匀分布的点
        auto h = Warp::squareToCosineHemisphere(rng);//变换到单位半球余弦上
        auto dir = frame.toWorld(h);//因为是切线空间中的点，所以要变换到世界空间
        auto pN = dir.normalized();
        int visiblity = 0;
        if (!scene->rayIntersect(Ray3f(its.p + pN * 0.00001f, pN))) {
            visiblity = 1;
        }
        auto cosTheta = frame.cosTheta(h);
        if(cosTheta <0) return Color3f(0.0f);
        return Color3f(visiblity * cosTheta/M_PI);
    }
 
    /// Return a human-readable description for debugging purposes
    std::string toString() const {
        return tfm::format(
            "AoIntegrator[\n"
            "  myProperty = \"%s\"\n"
            "]"
        );
    }
};
 
NORI_REGISTER_CLASS(AoIntegrator, "ao");
NORI_NAMESPACE_END