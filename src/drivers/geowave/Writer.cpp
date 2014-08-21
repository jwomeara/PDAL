/******************************************************************************
* Copyright (c) 2011, Howard Butler, hobu.inc@gmail.com
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

#include <pdal/drivers/geowave/Writer.hpp>
#include <pdal/Algorithm.hpp>
#include <pdal/PointBuffer.hpp>
#include <pdal/pdal_macros.hpp>

#include <iostream>
#include <algorithm>
#include <map>

#include <boost/algorithm/string.hpp>
#include <boost/algorithm/string/erase.hpp>

#include <jace/Jace.h>
using jace::java_cast;
using jace::java_new;

#include "jace/JNIException.h"
using jace::JNIException;

#include "jace/VirtualMachineShutdownError.h"
using jace::VirtualMachineShutdownError;

#include "jace/OptionList.h"
using jace::OptionList;
using jace::Option;
using jace::ClassPath;
using jace::Verbose;
using jace::CustomOption;

#include <jace/StaticVmLoader.h>
using jace::StaticVmLoader;

#ifdef _WIN32
  #include "jace/Win32VmLoader.h"
  using jace::Win32VmLoader;
#else
  #include "jace/UnixVmLoader.h"
  using ::jace::UnixVmLoader;
#endif

#include "jace/proxy/java/lang/String.h"
using jace::proxy::java::lang::String;

#include "jace/proxy/mil/nga/giat/geowave/accumulo/JaceConnector.h"
using jace::proxy::mil::nga::giat::geowave::accumulo::JaceConnector;

#ifdef USE_PDAL_PLUGIN_GEOWAVE
MAKE_WRITER_CREATOR(templateWriter, pdal::drivers::geowave::Writer)
CREATE_WRITER_PLUGIN(temp, pdal::drivers::geowave::Writer)
#endif

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

namespace pdal
{
namespace drivers
{
namespace geowave
{

struct FileStreamDeleter
{

    template <typename T>
    void operator()(T* ptr)
    {
        ptr->flush();
        FileUtils::closeFile(ptr);
    }
};

int Writer::createJvm()
{
	try
	{
		StaticVmLoader loader(JNI_VERSION_1_2);

		OptionList options;
		options.push_back(ClassPath(TOSTRING(GEOWAVE_RUNTIME_JAR)));
		options.push_back(ClassPath(TOSTRING(JACE_RUNTIME_JAR)));
		options.push_back(CustomOption("-Xmx128M")); // TODO: update with actual options

		jace::createVm(loader, options);
	}
	catch (VirtualMachineShutdownError&)
	{
		std::cout << "The JVM was terminated in mid-execution. " << std::endl;
		return -2;
	}
	catch (JNIException& jniException)
	{
		std::cout << "An unexpected JNI error has occured: " << jniException.what() << std::endl;
		return -2;
	}
	catch (std::exception& e)
	{
		std::cout << "An unexpected C++ error has occurred: " << e.what() << std::endl;
		return -2;
	}

	JaceConnector jc = java_new<JaceConnector>();

	jc.printMessage(java_new<String>("This is a C++ String printed in Java"));
}

Options Writer::getDefaultOptions()
{
	std::cout << "Geowave Writer: getDefaultOptions" << std::endl;
    return Options();
}

void Writer::processOptions(const Options& ops)
{
	std::cout << "Geowave Writer: processOptions" << std::endl;
}

void Writer::ready(PointContext ctx)
{
	std::cout << "Geowave Writer: ready" << std::endl;
}

void Writer::write(const PointBuffer& data)
{
	std::cout << "Geowave Writer: write" << std::endl;
}

}
}
} // namespaces
