/******************************************************************************
* Copyright (c) 2016, James W. O'Meara (james.w.omeara@gmail.com)
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
#include <chrono>

#include <pdal/pdal_test_main.hpp>

#include <pdal/util/FileUtils.hpp>
#include <pdal/StageFactory.hpp>

#include <pdal/PointView.hpp>
#include <pdal/pdal_defines.h>
#include <las/LasReader.hpp>
#include <reprojection/ReprojectionFilter.hpp>

#include "Support.hpp"

using namespace pdal;

Options getGeoWaveOptions()
{
    Options options;

    options.add("type", "geowave");
    options.add("zookeeper_url", "localhost:2181");
    options.add("instance_name", "geowave");
    options.add("username", "root");
    options.add("password", "password");
    options.add("feature_type_name", "PDAL_Point");

    return options;
}

void testReadWrite()
{
    using namespace std::chrono;
    unsigned long millis = system_clock::now().time_since_epoch() / milliseconds(1);

    {
        Options geowaveOptions = getGeoWaveOptions();
        geowaveOptions.add("table_namespace", "PDAL_Table_" + std::to_string(millis));

        // Read LAS data and write to GeoWave
        Options lasReadOpts;
        lasReadOpts.add("filename", Support::datapath("las/utm17.las"));

        LasReader reader;
        reader.setOptions(lasReadOpts);

        Options filterOptions;
        filterOptions.add("out_srs", "EPSG:4326");

        ReprojectionFilter filter;
        filter.setOptions(filterOptions);
        filter.setInput(reader);

        StageFactory f;
        Stage* geowaveWriter(f.createStage("writers.geowave"));
        geowaveWriter->setOptions(geowaveOptions);
        geowaveWriter->setInput(filter);

        PointTable table;
        geowaveWriter->prepare(table);
        geowaveWriter->execute(table);
    }

    {
        Options geowaveOptions = getGeoWaveOptions();
        geowaveOptions.add("table_namespace", "PDAL_Table_" + std::to_string(millis));
        geowaveOptions.add("bounds", "([-180,180],[-90,90],[0,100])");

        // Done - now read back.
        StageFactory f;
        Stage* geowaveReader(f.createStage("readers.geowave"));
        geowaveReader->setOptions(geowaveOptions);

        PointTable table2;
        geowaveReader->prepare(table2);
        PointViewSet viewSet = geowaveReader->execute(table2);
        EXPECT_EQ(viewSet.size(), 1U);
        PointViewPtr view = *viewSet.begin();

        using namespace Dimension;

        bool found = false;
        for (PointId idx = 0; idx < view->size(); idx++)
        {
            if (view->getFieldAs<double>(Id::Z, idx) == 170.62)
            {
                found = true;
                EXPECT_DOUBLE_EQ(view->getFieldAs<double>(Id::X, idx), -83.427559234389875);
                EXPECT_DOUBLE_EQ(view->getFieldAs<double>(Id::Y, idx), 39.012613812346736);
                EXPECT_DOUBLE_EQ(view->getFieldAs<double>(Id::Z, idx), 170.62);
                EXPECT_EQ(view->getFieldAs<uint16_t>(Id::Intensity, idx), 280);
                EXPECT_EQ(view->getFieldAs<uint8_t>(Id::ReturnNumber, idx), 0);
                EXPECT_EQ(view->getFieldAs<uint8_t>(Id::NumberOfReturns, idx), 6);
                EXPECT_EQ(view->getFieldAs<uint8_t>(Id::ScanDirectionFlag, idx), 0);
                EXPECT_EQ(view->getFieldAs<uint8_t>(Id::EdgeOfFlightLine, idx), 0);
                EXPECT_EQ(view->getFieldAs<uint8_t>(Id::Classification, idx), 2);
                EXPECT_FLOAT_EQ(view->getFieldAs<float>(Id::ScanAngleRank, idx), 0);
                EXPECT_EQ(view->getFieldAs<uint8_t>(Id::UserData, idx), 0);
                EXPECT_EQ(view->getFieldAs<uint16_t>(Id::PointSourceId, idx), 0);
                EXPECT_DOUBLE_EQ(view->getFieldAs<double>(Id::GpsTime, idx), 499450.80609205517);
            }
        }

        EXPECT_EQ(view->size(), 10);
        EXPECT_EQ(found, true);
    }
}


TEST(GeoWaveTest, readWrite)
{
    testReadWrite();
}
