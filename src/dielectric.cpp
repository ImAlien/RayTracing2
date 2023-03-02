/*
 * @Author: Alien
 * @Date: 2023-03-01 11:55:29
 * @LastEditors: Alien
 * @LastEditTime: 2023-03-02 11:39:44
 */
/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include <nori/bsdf.h>
#include <nori/frame.h>

NORI_NAMESPACE_BEGIN

/// Ideal dielectric BSDF
class Dielectric : public BSDF {
public:
    Dielectric(const PropertyList &propList) {
        /* Interior IOR (default: BK7 borosilicate optical glass) */
        m_intIOR = propList.getFloat("intIOR", 1.5046f);

        /* Exterior IOR (default: air) */
        m_extIOR = propList.getFloat("extIOR", 1.000277f);
    }

    Color3f eval(const BSDFQueryRecord &) const {
        /* Discrete BRDFs always evaluate to zero in Nori */
        return Color3f(0.0f);
    }

    float pdf(const BSDFQueryRecord &) const {
        /* Discrete BRDFs always evaluate to zero in Nori */
        return 0.0f;
    }

    static Vector3f refract(const Vector3f& wi, const Vector3f& n, float eta) {
        float cosThetaI = wi.dot(n);
        if (cosThetaI < 0)
            eta = 1.0f / eta;
        float cosThetaTSqr = 1 - (1 - cosThetaI * cosThetaI) * (eta * eta);
        if (cosThetaTSqr <= 0.0f)
            return Vector3f(0.0f);
        float sign = cosThetaI >= 0.0f ? 1.0f : -1.0f;
        return n * (-cosThetaI * eta + sign * sqrt(cosThetaTSqr)) + wi * eta;
    }
    Color3f sample(BSDFQueryRecord &bRec, const Point2f &sample) const {
        //throw NoriException("Unimplemented!");
        bRec.measure = EDiscrete;
        float cosThetaI = Frame::cosTheta(bRec.wi);
        float kr = fresnel(cosThetaI, m_extIOR, m_intIOR);
        if (sample.x() < kr) {
        bRec.wo = Vector3f(-bRec.wi.x(), -bRec.wi.y(), bRec.wi.z());//反射结果
        bRec.eta = 1.f;
        return Color3f(1.0f);
        } else {
        //bRec.eta = cosThetaI >= 0 ? m_extIOR / m_intIOR : m_intIOR / m_extIOR;
        //Normal3f n = cosThetaI < 0 ? Normal3f(0.f, 0.f, -1.f) : Normal3f(0.f, 0.f, 1.f);
        //cosThetaI = abs(cosThetaI);
        //float cosThetaO = sqrt(1 - bRec.eta * bRec.eta * fmax(0.f, 1.f - cosThetaI * cosThetaI));
        //bRec.wo = bRec.eta * -bRec.wi + (bRec.eta * cosThetaI - cosThetaO) * n;
        //bRec.wo.normalize();
        //return Color3f(bRec.eta * bRec.eta);
        //2021.12.2更新
        Vector3f n = Vector3f(0.0f, 0.0f, 1.0f);
        float factor = m_intIOR / m_extIOR;
        if (Frame::cosTheta(bRec.wi) < 0.f) {//从内部往外部折射
            factor = m_extIOR / m_intIOR;
            n.z() = -1.0f;
        }
        //折射公式
        bRec.wo = refract(-bRec.wi, n, factor);
        bRec.eta = m_intIOR / m_extIOR;
        return Color3f(1.0f);
        }
    }

    std::string toString() const {
        return tfm::format(
            "Dielectric[\n"
            "  intIOR = %f,\n"
            "  extIOR = %f\n"
            "]",
            m_intIOR, m_extIOR);
    }
private:
    float m_intIOR, m_extIOR;
};

NORI_REGISTER_CLASS(Dielectric, "dielectric");
NORI_NAMESPACE_END
