#include <iostream>
#include <fstream>

#include "easylogging++.h"

#include <osmium/area/assembler.hpp>
#include <osmium/area/multipolygon_collector.hpp>
#include <osmium/geom/wkt.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/index/map/sparse_mem_array.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>

typedef osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location> index_type;
typedef osmium::handler::NodeLocationsForWays<index_type> location_handler_type;

INITIALIZE_EASYLOGGINGPP

using namespace std;

class ExportToWKTHandler : public osmium::handler::Handler {

    osmium::geom::WKTFactory<> m_factory;

public:

    void node(const osmium::Node& node) {
        LOG(TRACE) << 'n' << node.id() << ' ' << m_factory.create_point(node);
    }

    void way(const osmium::Way& way) {
        try {
            LOG(TRACE) << 'w' << way.id() << ' ' << m_factory.create_linestring(way);
        } catch (osmium::geometry_error&) {
            // ignore broken geometries (such as ways with only a single node)
        }
    }

    void area(const osmium::Area& area) {
        try {
            LOG(TRACE) << 'a' << area.id() << ' ' << m_factory.create_multipolygon(area);
        } catch (osmium::geometry_error&) {
            // ignore broken geometries (such as illegal multipolygons)
        }
    }

};

int main(int argc, char* argv[]) {
    // Configure logger
    el::Configurations conf("logging.conf");
    el::Loggers::reconfigureAllLoggers(conf);

    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " OSMFILE\n";
        LOG(FATAL) << "Usage: " << argv[0] << " OSMFILE";
        exit(1);
    }

    std::string input_filename {argv[1]};

    osmium::area::Assembler::config_type assembler_config;
    osmium::area::MultipolygonCollector<osmium::area::Assembler> collector(assembler_config);

    LOG(INFO) << "Starting to process file " << argv[1];
    LOG(INFO) << "Checking relations for osm file...";
    osmium::io::Reader reader1(input_filename);
    collector.read_relations(reader1);

    index_type index;
    location_handler_type location_handler(index);

    LOG(INFO) << "Converting file content to WKT...";
    ExportToWKTHandler export_handler;
    osmium::io::Reader reader2(input_filename);
    osmium::apply(reader2, location_handler, export_handler, collector.handler([&export_handler](const osmium::memory::Buffer& buffer) {
        osmium::apply(buffer, export_handler);
    }));
    LOG(INFO) << "File " << argv[1] << " successfully converted.";
}