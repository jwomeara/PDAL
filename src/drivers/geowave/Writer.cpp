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
  const std::string os_pathsep(";");
#else
  #include "jace/UnixVmLoader.h"
  using ::jace::UnixVmLoader;
  const std::string os_pathsep(":");
#endif

#include "jace/proxy/java/lang/String.h"
using jace::proxy::java::lang::String;

#include "jace/proxy/mil/nga/giat/geowave/accumulo/BasicAccumuloOperations.h"
using jace::proxy::mil::nga::giat::geowave::accumulo::BasicAccumuloOperations;

#define STRINGIFY(x) #x
#define TOSTRING(x) STRINGIFY(x)

namespace pdal
{
namespace drivers
{
namespace geowave
{

Options Writer::getDefaultOptions()
{
	std::cout << "Geowave Writer: getDefaultOptions" << std::endl;
	Options options;

    Option zookeeperUrl("zookeeperUrl", "", "The comma-delimited URLs for all zookeeper servers, this will be directly used to instantiate a ZookeeperInstance");
    Option instanceName("instanceName", "", "The zookeeper instance name, this will be directly used to instantiate a ZookeeperInstance");
    Option username("username", "", "The username for an account to establish an Accumulo connector");
    Option password("password", "", "The password for the account to establish an Accumulo connector");
	Option tableNamespace("tableNamespace", "", "An optional string that is prefixed to any of the table names");

    options.add(zookeeperUrl);
    options.add(instanceName);
    options.add(username);
    options.add(password);
	options.add(tableNamespace);

    return options;
}

void Writer::processOptions(const Options& ops)
{
	std::cout << "Geowave Writer: processOptions" << std::endl;

	zookeeperUrl = ops.getValueOrThrow<std::string>("zookeeperUrl");
	instanceName = ops.getValueOrThrow<std::string>("instanceName");
	username = ops.getValueOrThrow<std::string>("username");
	password = ops.getValueOrThrow<std::string>("password");
	tableNamespace = ops.getValueOrDefault<std::string>("tableNamespace", "");
}

void Writer::ready(PointContext ctx)
{
	std::cout << "Geowave Writer: ready" << std::endl;

	int status = createJvm();
	if (status == 0)
		std::cout << "JVM Creation Successful" << std::endl;
	else
		std::cout << "JVM Creation Failed: Error ["  << status << "]" << std::endl;
	ctx;
}

void Writer::write(const PointBuffer& data)
{
	std::cout << "Geowave Writer: write" << std::endl;
	
	BasicAccumuloOperations accumuloOperations;
	
	try
	{
		accumuloOperations = java_new<BasicAccumuloOperations>(
				java_new<String>(zookeeperUrl),
				java_new<String>(instanceName),
				java_new<String>(username),
				java_new<String>(password),
				java_new<String>(tableNamespace));
	}	
	catch (JNIException& jniException)
	{
		std::cout << "An unexpected JNI error has occured: " << jniException.what() << std::endl;
		return;
	}
	catch (std::exception& e)
	{
		std::cout << "An unexpected C++ error has occurred: " << e.what() << std::endl;
		return;
	}

	if (accumuloOperations.tableExists(java_new<String>("glare_data")))
		std::cout << "glare_data exists!" << std::endl;
	else
		std::cout << "glare_data does not exist!" << std::endl;

	data;
}

int Writer::createJvm()
{
	try
	{
		StaticVmLoader loader(JNI_VERSION_1_2);

		std::string jaceClasspath = TOSTRING(JACE_RUNTIME_JAR);
		std::string geowaveClasspath = TOSTRING(GEOWAVE_RUNTIME_JAR);

		OptionList options;
		//options.push_back(CustomOption("-Xcheck:jni"));
		//options.push_back(Verbose (Verbose::JNI));
		//options.push_back(Verbose (Verbose::CLASS));
		options.push_back(ClassPath(jaceClasspath + os_pathsep + geowaveClasspath));

		jace::createVm(loader, options);
	}
	catch (VirtualMachineShutdownError&)
	{
		std::cout << "The JVM was terminated in mid-execution. " << std::endl;
		return -1;
	}
	catch (JNIException& jniException)
	{
		std::cout << "An unexpected JNI error has occured: " << jniException.what() << std::endl;
		return -2;
	}
	catch (std::exception& e)
	{
		std::cout << "An unexpected C++ error has occurred: " << e.what() << std::endl;
		return -3;
	}

	return 0;
}

}
}
} // namespaces
