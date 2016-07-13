#pragma once

#include <string>
#include <algorithm>
#include <fstream>
#include "b64/base64.h"

#include <osmium/geom/wkt.hpp>
#include <osmium/geom/wkb.hpp>
#include <osmium/geom/geojson.hpp>
#include <osmium/handler.hpp>

#define ELPP_NO_DEFAULT_LOG_FILE

#include "easylogging++.h"

using namespace std;
using namespace osmium;

class ExportHandler : public handler::Handler {

private:
    geom::WKTFactory<> wktFactory;
    geom::WKBFactory<> wkbFactory;
    geom::GeoJSONFactory<> gjsFactory;
    fstream fs;
    set<string> outputTypes;
    set<string> outputGeometries;

    string toString(const TagList &list) {
        string tags = "[";
        for (auto &tag : list) {
            tags.append(tag.key());
            tags.append("=");
            tags.append(tag.value());
            tags.append(",");
        }
        if (tags.length() > 1) {
            tags.pop_back();
        }
        tags += "]";

        // protect the csv format against breaking characters
        replace(tags.begin(), tags.end(), ';', ':');
        replace(tags.begin(), tags.end(), '\n', ' ');
        replace(tags.begin(), tags.end(), '\r', ' ');
        return tags;
    }

public:

    ExportHandler(string outputFileName, set<string> outputTypes, set<string> outputGeometries) : outputTypes(
            outputTypes), outputGeometries(outputGeometries) {
        fs.open(outputFileName, fstream::out);
        fs << "id;type;tag;";
        if (outputGeometries.count("wkt")) {
            fs << "wkt;";
        }
        if (outputGeometries.count("wkb")) {
            fs << "wkb;";
        }
        if (outputGeometries.count("geojson")) {
            fs << "geojson";
        }
        fs << "\n";
    }

    void node(const Node &node) {
        if (!outputTypes.count("node")) {
            return;
        }
        fs << to_string(node.id()) << ";" << "node" << ";" << toString(node.tags()) << ";";

        LOG(TRACE) << "node " << node.id();
        if (outputGeometries.count("wkt")) {
            string wkt = wktFactory.create_point(node);
            fs << wkt << ";";
            LOG(TRACE) << "\tWKT: " << wkt;
        }
        if (outputGeometries.count("wkb")) {
            auto rawWkb = wkbFactory.create_point(node);
            string wkb = base64_encode(rawWkb.c_str(), static_cast<int>(rawWkb.length()));
            fs << wkb << ";";
            LOG(TRACE) << "\tWKB: " << wkb;
        }
        if (outputGeometries.count("geojson")) {
            string gjs = gjsFactory.create_point(node);
            fs << gjs;
            LOG(TRACE) << "\tGJS: " << gjs;
        }
        fs << "\n";
    }

    void way(const Way &way) {
        if (!outputTypes.count("way")) {
            return;
        }
        try {
            fs << to_string(way.id()) << ";" << "way" << ";" << toString(way.tags()) << ";";

            LOG(TRACE) << "way " << way.id();
            if (outputGeometries.count("wkt")) {
                string wkt = wktFactory.create_linestring(way);
                fs << wkt << ";";
                LOG(TRACE) << "\tWKT: " << wkt;
            }
            if (outputGeometries.count("wkb")) {
                auto rawWkb = wkbFactory.create_linestring(way);
                string wkb = base64_encode(rawWkb.c_str(), static_cast<int>(rawWkb.length()));
                fs << wkb << ";";
                LOG(TRACE) << "\tWKB: " << wkb;
            }
            if (outputGeometries.count("geojson")) {
                string gjs = gjsFactory.create_linestring(way);
                fs << gjs;
                LOG(TRACE) << "\tGJS: " << gjs;
            }
            fs << "\n";
        } catch (geometry_error &e) {
            LOG(ERROR) << "Geometry with id " << way.id() << " is broken: " << e.what();
        }
    }

    void area(const Area &area) {
        if (!outputTypes.count("area")) {
            return;
        }
        try {
            string wkt = wktFactory.create_multipolygon(area);
            auto rawWkb = wkbFactory.create_multipolygon(area);
            string wkb = base64_encode(rawWkb.c_str(), static_cast<int>(rawWkb.length()));
            string gjs = gjsFactory.create_multipolygon(area);
            fs << to_string(area.id()) << ";" << "area" << ";" << toString(area.tags());

            LOG(TRACE) << "area " << area.id();
            if (outputGeometries.count("wkt")) {
                string wkt = wktFactory.create_multipolygon(area);
                fs << wkt << ";";
                LOG(TRACE) << "\tWKT: " << wkt;
            }
            if (outputGeometries.count("wkb")) {
                auto rawWkb = wkbFactory.create_multipolygon(area);
                string wkb = base64_encode(rawWkb.c_str(), static_cast<int>(rawWkb.length()));
                fs << wkb << ";";
                LOG(TRACE) << "\tWKB: " << wkb;
            }
            if (outputGeometries.count("geojson")) {
                string gjs = gjsFactory.create_multipolygon(area);
                fs << gjs;
                LOG(TRACE) << "\tGJS: " << gjs;
            }
            fs << "\n";
        } catch (geometry_error &e) {
            LOG(ERROR) << "Geometry with id " << area.id() << " is broken: " << e.what();
        }
    }
};