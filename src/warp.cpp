/*
    This file is part of Nori, a simple educational ray tracer

    Copyright (c) 2015 by Wenzel Jakob
*/

#include <nori/warp.h>
#include <nori/vector.h>
#include <nori/frame.h>

NORI_NAMESPACE_BEGIN

Point2f Warp::squareToUniformSquare(const Point2f &sample) {
    return sample;
}

float Warp::squareToUniformSquarePdf(const Point2f &sample) {
    return ((sample.array() >= 0).all() && (sample.array() <= 1).all()) ? 1.0f : 0.0f;
}

float tent(float x) {
  return x < 0.5f ? sqrt(2.0f * x) - 1.0f : 1.0f - sqrt(2.0f - 2.0f * x);
}
Point2f Warp::squareToTent(const Point2f &sample) {
    //throw NoriException("Warp::squareToTent() is not yet implemented!");
    return Point2f(tent(sample.x()), tent(sample.y()));
}
float tentPdf(float t) {
  return t >= -1 && t <= 1 ? 1 - abs(t) : 0;
}
float Warp::squareToTentPdf(const Point2f &p) {
    //throw NoriException("Warp::squareToTentPdf() is not yet implemented!");
    return tentPdf(p.x()) * tentPdf(p.y());
}

Point2f Warp::squareToUniformDisk(const Point2f &sample) {
    //throw NoriException("Warp::squareToUniformDisk() is not yet implemented!");
    // f(r) = 2*r; F(r) = r^2 ; F^-1(r) = sqrt(r);
    float radius = std::sqrt(sample.x());
  float angle = sample.y() * (float)M_PI * 2;
  return Point2f(radius * cos(angle), radius * sin(angle));
}

float Warp::squareToUniformDiskPdf(const Point2f &p) {
    //throw NoriException("Warp::squareToUniformDiskPdf() is not yet implemented!");
    return std::sqrt(p.x() * p.x() + p.y() * p.y()) <= 1.0f ? INV_PI : 0.0f;
}

Vector3f Warp::squareToUniformSphere(const Point2f &sample) {
    //throw NoriException("Warp::squareToUniformSphere() is not yet implemented!");
    float phi = sample.x() * M_PI * 2;
    float theta = acos(1 - 2 * sample.y());
    float sinTheta = sin(theta);
    float cosTheta = cos(theta);
    float sinPhi = sin(phi);
    float cosPhi = cos(phi);
    return Vector3f(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);
}

float Warp::squareToUniformSpherePdf(const Vector3f &v) {
    //throw NoriException("Warp::squareToUniformSpherePdf() is not yet implemented!");
    return INV_FOURPI;
}

Vector3f Warp::squareToUniformHemisphere(const Point2f &sample) {
    float phi = sample.x() * M_PI * 2;
    float theta = acos(1 - sample.y());
    float sinTheta = sin(theta);
    float cosTheta = cos(theta);
    float sinPhi = sin(phi);
    float cosPhi = cos(phi);
    return Vector3f(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);
    //throw NoriException("Warp::squareToUniformHemisphere() is not yet implemented!");
}
float Warp::squareToUniformHemispherePdf(const Vector3f &v) {
    //throw NoriException("Warp::squareToUniformHemispherePdf() is not yet implemented!");
    return v.z() < 0 ? 0 : INV_TWOPI;
}

Vector3f Warp::squareToCosineHemisphere(const Point2f &sample) {
    //throw NoriException("Warp::squareToCosineHemisphere() is not yet implemented!");
    // float phi = sample.x() * M_PI * 2;
    // float theta = (acos(1 - 4*sample.y()))/2;
    // float sinTheta = sin(theta);
    // float cosTheta = cos(theta);
    // float sinPhi = sin(phi);
    // float cosPhi = cos(phi);
    // return Vector3f(sinTheta * cosPhi, sinTheta * sinPhi, cosTheta);
    Point2f bottom = squareToUniformDisk(sample);
  float x = bottom.x();
  float y = bottom.y();
  return Vector3f(x, y, sqrt(1 - x * x - y * y));
}

float Warp::squareToCosineHemispherePdf(const Vector3f &v) {
    //throw NoriException("Warp::squareToCosineHemispherePdf() is not yet implemented!");
    return v.z() < 0 ? 0 : v.z() * INV_PI;
}

Vector3f Warp::squareToBeckmann(const Point2f &sample, float alpha) {
    //throw NoriException("Warp::squareToBeckmann() is not yet implemented!");
    float phi = M_PI * 2 * sample.x();
  float theta = atan(sqrt(-alpha * alpha * log(1 - sample.y())));
  float cosPhi = cos(phi);
  float sinPhi = sin(phi);
  float cosTheta = cos(theta);
  float sinTheta = sin(theta);
  float x = sinTheta * cosPhi;
  float y = sinTheta * sinPhi;
  float z = cosTheta;
  return Vector3f(x, y, z);
}

float Warp::squareToBeckmannPdf(const Vector3f &m, float alpha) {
    //throw NoriException("Warp::squareToBeckmannPdf() is not yet implemented!");
    if (m.z() <= 0) {
        return 0;
    }
    float alpha2 = alpha * alpha;
    float cosTheta = m.z();
    float tanTheta2 = (m.x() * m.x() + m.y() * m.y()) / (cosTheta * cosTheta);
    float cosTheta3 = cosTheta * cosTheta * cosTheta;
    float azimuthal = INV_PI;
    float longitudinal = exp(-tanTheta2 / alpha2) / (alpha2 * cosTheta3);
    return azimuthal * longitudinal;
}

NORI_NAMESPACE_END