// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "array/Array2D.h"

namespace helide {

bool isCompact(const Array2DMemoryDescriptor &d)
{
  return (d.byteStride1 == 0 || d.byteStride1 == anari::sizeOf(d.elementType))
      && (d.byteStride2 == 0 || d.byteStride2 == anari::sizeOf(d.elementType));
}

Array2D::Array2D(HelideGlobalState *state, const Array2DMemoryDescriptor &d)
    : Array(ANARI_ARRAY2D, state, d)
{
  if (!isCompact(d))
    throw std::runtime_error("2D strided arrays not yet supported");

  m_size[0] = d.numItems1;
  m_size[1] = d.numItems2;

  initManagedMemory();
}

size_t Array2D::totalSize() const
{
  return size(0) * size(1);
}

size_t Array2D::size(int dim) const
{
  return m_size[dim];
}

uint2 Array2D::size() const
{
  return uint2(uint32_t(size(0)), uint32_t(size(1)));
}

void Array2D::privatize()
{
  makePrivatizedCopy(size(0) * size(1));
}

} // namespace helide

HELIDE_ANARI_TYPEFOR_DEFINITION(helide::Array2D *);
