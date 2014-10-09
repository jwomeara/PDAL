/******************************************************************************
* Copyright (c) 2014, Hobu Inc. (hobu@hobu.co)
*
* All rights reserved.
*
* Redistribution and use in source and binary forms, with or without
* modification, are permitted provided that the following
* conditions are met:
*
*     * Redistributions of source code must retain the above copyright
*       notice, this list of conditions and the following disclaimer.
*     * Redistributions in binary form must reproduce the above copyright
*       notice, this list of conditions and the following disclaimer in
*       the documentation and/or other materials provided
*       with the distribution.
*     * Neither the name of Hobu, Inc. or Flaxen Geo Consulting nor the
*       names of its contributors may be used to endorse or promote
*       products derived from this software without specific prior
*       written permission.
*
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
* "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
* LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS
* FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE
* COPYRIGHT OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT,
* INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING,
* BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS
* OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED
* AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
* OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT
* OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY
* OF SUCH DAMAGE.
****************************************************************************/

#include <cassert>
#include <sstream>

#include <pdal/drivers/las/VariableLengthRecord.hpp>

#include <pdal/SpatialReference.hpp>

#include "GeotiffSupport.hpp"

namespace pdal
{
namespace drivers
{
namespace las
{

ILeStream& operator>>(ILeStream& in, VariableLengthRecord& v)
{
    uint16_t reserved;
    uint16_t dataLen;

    in >> reserved;
    in.get(v.m_userId, 16);
    in >> v.m_recordId >> dataLen;
    in.get(v.m_description, 32);
    v.m_data.resize(dataLen);
    in.get(v.m_data);

    return in;
}


OLeStream& operator<<(OLeStream& out, const VariableLengthRecord& v)
{
    uint16_t reserved = 0xAABB;
    uint16_t dataLen;

    out << reserved;
    out.put(v.m_userId, 16);
    out << v.m_recordId << (uint16_t)v.dataLen();
    out.put(v.m_description, 32);
    out.put(v.data(), v.dataLen());

    return out;
}


ILeStream& operator>>(ILeStream& in, ExtVariableLengthRecord& v)
{
    uint16_t reserved;
    uint64_t dataLen;

    in >> reserved;
    in.get(v.m_userId, 16);
    in >> v.m_recordId >> dataLen;
    in.get(v.m_description, 32);
    v.m_data.resize(dataLen);
    in.get(v.m_data);

    return in;
}


OLeStream& operator<<(OLeStream& out, const ExtVariableLengthRecord& v)
{
    uint16_t reserved;
    uint64_t dataLen;

    out << (uint16_t)0;
    out.put(v.userId(), 16);
    out << v.recordId() << v.dataLen();
    out.put(v.description(), 32);
    out.put(v.data(), v.dataLen());

    return out;
}

} // namespace las
} // namespace drivers
} // namespace pdal

