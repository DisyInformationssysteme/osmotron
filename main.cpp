#include <iostream>
#include <fstream>

#include <osmium/area/assembler.hpp>
#include <osmium/area/multipolygon_collector.hpp>
#include <osmium/geom/wkt.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/index/map/sparse_mem_array.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>

typedef osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location> index_type;
typedef osmium::handler::NodeLocationsForWays<index_type> location_handler_type;

#define ELPP_NO_DEFAULT_LOG_FILE
#define ELPP_UNICODE
#include "easylogging++.h"
INITIALIZE_EASYLOGGINGPP


using namespace std;

class ExportToWKTHandler : public osmium::handler::Handler {

private:
    osmium::geom::WKTFactory<> m_factory;
    fstream fs;

    string toString(const osmium::TagList &list) {
        string tags = "[";
        for (auto& tag : list) {
            tags.append(tag.key());
            tags.append("=");
            tags.append(tag.value());
            tags.append(",");
        }
        tags += "]";
        replace(tags.begin(), tags.end(), ';', ':'); // to protect the csv format
        return tags;
    }

public:

    ExportToWKTHandler() {
        fs.open("result.csv", fstream::out);
        fs << "id;type;tag;wkt\n";
    }

    void node(const osmium::Node& node) {
        string wkt = m_factory.create_point(node);
        fs << to_string(node.id()) << ";" << "node" << ";" << toString(node.tags()) << ";" << wkt << "\n";
        LOG(TRACE) << 'n' << node.id() << ' ' << wkt;
    }

    void way(const osmium::Way& way) {
        try {
            string wkt = m_factory.create_linestring(way);
            fs << to_string(way.id()) << ";" << "way" << ";" << toString(way.tags()) << ";" << wkt << "\n";
            LOG(TRACE) << 'w' << way.id() << ' ' << wkt;
        } catch (osmium::geometry_error& e) {
            LOG(ERROR) << "Geometry with id " << way.id() << " is broken: " << e.what();
        }
    }

    void area(const osmium::Area& area) {
        try {
            string wkt = m_factory.create_multipolygon(area);
            fs << to_string(area.id()) << ";" << "area" << ";" << toString(area.tags()) << ";" << wkt << "\n";
            LOG(TRACE) << 'a' << area.id() << ' ' << wkt;
        } catch (osmium::geometry_error& e) {
            LOG(ERROR) << "Geometry with id " << area.id() << " is broken: " << e.what();
        }
    }

};

void exportWkt(string input_filename, osmium::area::MultipolygonCollector<osmium::area::Assembler>& collector) {
    TIMED_SCOPE(timerObj, "exportWkt");

    index_type index;
    location_handler_type location_handler(index);
    ExportToWKTHandler export_handler;

    osmium::io::Reader reader2(input_filename);
    osmium::apply(reader2, location_handler, export_handler, collector.handler([&export_handler](const osmium::memory::Buffer& buffer) {
        osmium::apply(buffer, export_handler);
    }));
}

int main(int argc, char* argv[]) {
    // Configure logger
    START_EASYLOGGINGPP(argc, argv);
    el::Configurations conf("logging.conf");
    el::Loggers::reconfigureAllLoggers(conf);

    if (argc != 2) {
        LOG(ERROR) << "Usage: " << argv[0] << " OSMFILE";
        exit(1);
    }

    string input_filename {argv[1]};
    osmium::area::Assembler::config_type assembler_config;
    osmium::area::MultipolygonCollector<osmium::area::Assembler> collector(assembler_config);

    LOG(INFO) << "Starting to process file " << argv[1];
    LOG(INFO) << "Checking relations for osm file...";
    osmium::io::Reader reader1(input_filename);
    collector.read_relations(reader1);

    LOG(INFO) << "Converting file content to WKT...";
    exportWkt(input_filename, collector);
    LOG(INFO) << "File " << argv[1] << " successfully converted (see result.csv).";
}
