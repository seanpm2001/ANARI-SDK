// Copyright 2022 The Khronos Group
// SPDX-License-Identifier: Apache-2.0

#include "Sampler.h"

namespace helide {

Sampler::Sampler(HelideGlobalState *s) : Object(ANARI_SAMPLER, s) {}

Sampler *Sampler::createInstance(std::string_view subtype, HelideGlobalState *s)
{
  return (Sampler *)new UnknownObject(ANARI_SAMPLER, s);
}

} // namespace helide

HELIDE_ANARI_TYPEFOR_DEFINITION(helide::Sampler *);
