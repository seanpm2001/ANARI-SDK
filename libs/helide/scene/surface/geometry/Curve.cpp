// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Curve.h"
// std
#include <numeric>

namespace helide {

Curve::Curve(HelideGlobalState *s) : Geometry(s)
{
  m_embreeGeometry =
      rtcNewGeometry(s->embreeDevice, RTC_GEOMETRY_TYPE_ROUND_LINEAR_CURVE);
}

void Curve::commit()
{
  Geometry::commit();

  cleanup();

  m_index = getParamObject<Array1D>("primitive.index");
  m_vertexPosition = getParamObject<Array1D>("vertex.position");
  m_vertexRadius = getParamObject<Array1D>("vertex.radius");
  m_vertexAttributes[0] = getParamObject<Array1D>("vertex.attribute0");
  m_vertexAttributes[1] = getParamObject<Array1D>("vertex.attribute1");
  m_vertexAttributes[2] = getParamObject<Array1D>("vertex.attribute2");
  m_vertexAttributes[3] = getParamObject<Array1D>("vertex.attribute3");
  m_vertexAttributes[4] = getParamObject<Array1D>("vertex.color");

  if (!m_vertexPosition) {
    reportMessage(ANARI_SEVERITY_WARNING,
        "missing required parameter 'vertex.position' on curve geometry");
    return;
  }

  if (m_index)
    m_index->addCommitObserver(this);
  m_vertexPosition->addCommitObserver(this);

  const float *radius =
      m_vertexRadius ? m_vertexRadius->beginAs<float>() : nullptr;
  m_globalRadius = getParam<float>("radius", 1.f);

  const auto numSegments =
      m_index ? m_index->size() : m_vertexPosition->size() / 2;

  {
    auto *vr = (float4 *)rtcSetNewGeometryBuffer(embreeGeometry(),
        RTC_BUFFER_TYPE_VERTEX,
        0,
        RTC_FORMAT_FLOAT4,
        sizeof(float4),
        m_vertexPosition->size());

    const auto *begin = m_vertexPosition->beginAs<float3>();
    const auto *end = m_vertexPosition->endAs<float3>();
    uint32_t rID = 0;
    std::transform(begin, end, vr, [&](const float3 &v) {
      return float4(v, radius ? radius[rID++] : m_globalRadius);
    });
  }

  if (m_index) {
    rtcSetSharedGeometryBuffer(embreeGeometry(),
        RTC_BUFFER_TYPE_INDEX,
        0,
        RTC_FORMAT_UINT,
        m_index->data(),
        0,
        sizeof(uint32_t),
        numSegments);
  } else {
    auto *idx = (uint32_t *)rtcSetNewGeometryBuffer(embreeGeometry(),
        RTC_BUFFER_TYPE_INDEX,
        0,
        RTC_FORMAT_UINT,
        sizeof(uint32_t),
        numSegments);
    std::iota(idx, idx + numSegments, 0);
  }

  rtcCommitGeometry(embreeGeometry());
}

float4 Curve::getAttributeValueAt(
    const Attribute &attr, const Ray &ray) const
{
  if (attr == Attribute::NONE)
    return DEFAULT_ATTRIBUTE_VALUE;

  auto attrIdx = static_cast<int>(attr);
  auto *attributeArray = m_vertexAttributes[attrIdx].ptr;
  if (!attributeArray)
    return Geometry::getAttributeValueAt(attr, ray);

  auto idx = m_index ? *(m_index->dataAs<uint32_t>() + ray.primID) : ray.primID;

  auto a = readAttributeArrayAt(attributeArray, idx + 0);
  auto b = readAttributeArrayAt(attributeArray, idx + 1);

  return a + (b - a) * ray.u;
}

void Curve::cleanup()
{
  if (m_index)
    m_index->removeCommitObserver(this);
  if (m_vertexPosition)
    m_vertexPosition->removeCommitObserver(this);
}

} // namespace helide
