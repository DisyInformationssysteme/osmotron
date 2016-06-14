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
        replace(tags.begin(), tags.end(), ';', ':'); // to protect the csv format
        return tags;
    }

public:

    ExportHandler() {
        fs.open("result.csv", fstream::out);
        fs << "id;type;tag;wkt;wkb;geojson\n";
    }

    void node(const Node &node) {
        string wkt = wktFactory.create_point(node);
        auto rawWkb = wkbFactory.create_point(node);
        string wkb = base64_encode(rawWkb.c_str(), static_cast<int>(rawWkb.length()));
        string gjs = gjsFactory.create_point(node);
        fs << to_string(node.id()) << ";" << "node" << ";" << toString(node.tags()) << ";" << wkt << ";" << wkb <<
        ";" << gjs << "\n";
        LOG(TRACE) << 'n' << node.id() << ' ' << wkt << ";" << wkb << ";" << gjs;
    }

    void way(const Way &way) {
        try {
            string wkt = wktFactory.create_linestring(way);
            auto rawWkb = wkbFactory.create_linestring(way);
            string wkb = base64_encode(rawWkb.c_str(), static_cast<int>(rawWkb.length()));
            string gjs = gjsFactory.create_linestring(way);
            fs << to_string(way.id()) << ";" << "way" << ";" << toString(way.tags()) << ";" << wkt << ";" << wkb <<
            ";" << gjs << "\n";
            LOG(TRACE) << 'w' << way.id() << ' ' << wkt << ";" << wkb << ";" << gjs;
        } catch (geometry_error &e) {
            LOG(ERROR) << "Geometry with id " << way.id() << " is broken: " << e.what();
        }
    }

    void area(const Area &area) {
        try {
            string wkt = wktFactory.create_multipolygon(area);
            auto rawWkb = wkbFactory.create_multipolygon(area);
            string wkb = base64_encode(rawWkb.c_str(), static_cast<int>(rawWkb.length()));
            string gjs = gjsFactory.create_multipolygon(area);
            fs << to_string(area.id()) << ";" << "area" << ";" << toString(area.tags()) << ";" << wkt << ";" << wkb <<
            ";" << gjs << "\n";
            LOG(TRACE) << 'a' << area.id() << ' ' << wkt << ";" << wkb << ";" << gjs;
        } catch (geometry_error &e) {
            LOG(ERROR) << "Geometry with id " << area.id() << " is broken: " << e.what();
        }
    }
};