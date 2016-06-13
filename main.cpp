#include <iostream>

#include <osmium/area/assembler.hpp>
#include <osmium/area/multipolygon_collector.hpp>
#include <osmium/geom/wkt.hpp>
#include <osmium/io/any_input.hpp>
#include <osmium/index/map/sparse_mem_array.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>

typedef osmium::index::map::SparseMemArray<osmium::unsigned_object_id_type, osmium::Location> index_type;
typedef osmium::handler::NodeLocationsForWays<index_type> location_handler_type;

using namespace std;

class ExportToWKTHandler : public osmium::handler::Handler {

    osmium::geom::WKTFactory<> m_factory;

public:

    void node(const osmium::Node& node) {
        std::cout << 'n' << node.id() << ' ' << m_factory.create_point(node) << "\n";
    }

    void way(const osmium::Way& way) {
        try {
            std::cout << 'w' << way.id() << ' ' << m_factory.create_linestring(way) << "\n";
        } catch (osmium::geometry_error&) {
            // ignore broken geometries (such as ways with only a single node)
        }
    }

    void area(const osmium::Area& area) {
        try {
            std::cout << 'a' << area.id() << ' ' << m_factory.create_multipolygon(area) << "\n";
        } catch (osmium::geometry_error&) {
            // ignore broken geometries (such as illegal multipolygons)
        }
    }

};

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " OSMFILE\n";
        exit(1);
    }

    std::string input_filename {argv[1]};

    osmium::area::Assembler::config_type assembler_config;
    osmium::area::MultipolygonCollector<osmium::area::Assembler> collector(assembler_config);

    std::cerr << "Pass 1...\n";
    osmium::io::Reader reader1(input_filename);
    collector.read_relations(reader1);
    std::cerr << "Pass 1 done\n";

    index_type index;
    location_handler_type location_handler(index);

    std::cerr << "Pass 2...\n";
    ExportToWKTHandler export_handler;
    osmium::io::Reader reader2(input_filename);
    osmium::apply(reader2, location_handler, export_handler, collector.handler([&export_handler](const osmium::memory::Buffer& buffer) {
        osmium::apply(buffer, export_handler);
    }));
    std::cerr << "Pass 2 done\n";
}