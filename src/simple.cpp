/*
 * @Author: Alien
 * @Date: 2023-03-01 14:31:13
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-01 15:26:16
 */
#include <nori/integrator.h>
#include <nori/scene.h>
#include <nori/vector.h>
#include <nori/color.h>
 
NORI_NAMESPACE_BEGIN
 
class SimpleIntegrator : public Integrator {
public:
    SimpleIntegrator(const PropertyList &props) {
        m_lightEnergy = props.getColor("energy");
        m_lightPos = props.getPoint("position");
        //std::cout << "Parameter value was : " << m_myProperty << std::endl;
    }
 
    /// Compute the radiance value for a given ray. Just return green here
    Color3f Li(const Scene *scene, Sampler *sampler, const Ray3f &ray) const {

        Intersection its;
        if (!scene->rayIntersect(ray, its)) {
        return Color3f(0.0f);
        }
        const auto& frame = its.shFrame;
        Point3f x = its.p;
        Point3f p = m_lightPos;
        auto wo = p - x;
        auto woDir = wo.normalized();
        float visiblity = 1;//可见性检查，起始点稍微增加一点点防止自遮挡
        if (scene->rayIntersect(Ray3f(x + wo * 0.00001f, wo))) {
        visiblity = 0;
        }
        auto cosTheta = frame.cosTheta(frame.toLocal(woDir));//frame的cosTheta函数是切线空间下计算
        auto s = std::pow((x - p).norm(), 2.0f);
        auto li = (m_lightEnergy / (4 * M_PI * M_PI)) * ((std::max(0.0f, cosTheta)) / s) * visiblity;
        return li;
        // //环境光
        // Intersection its;
        // if (!scene->rayIntersect(ray, its)) {
        // return Color3f(0.0f);
        // }
        // const auto& frame = its.shFrame;
        // auto rng = sampler->next2D();//单位正方形内均匀分布的点
        // auto h = Warp::squareToCosineHemisphere(rng);//变换到单位半球余弦上
        // auto p = frame.toWorld(h);//因为是切线空间中的点，所以要变换到世界空间
        // auto pN = p.normalized();
        // int visiblity = 0;
        // if (!scene->rayIntersect(Ray3f(its.p + pN * 0.00001f, pN))) {
        // visiblity = 1;
        // }
        // auto cosTheta = frame.cosTheta(frame.toLocal(p));
        // return Color3f(float(visiblity)*cosTheta/M_PI);
    }
 
    /// Return a human-readable description for debugging purposes
    std::string toString() const {
        return tfm::format(
            "SimpleIntegrator[\n"
            "  myProperty = \"%s\"\n"
            "]"
        );
    }
protected:
    Point3f m_lightPos;
    Color3f m_lightEnergy;
};
 
NORI_REGISTER_CLASS(SimpleIntegrator, "simple");
NORI_NAMESPACE_END