#include <iostream>
#include <fstream>

#include <osmium/area/assembler.hpp>
#include <osmium/area/multipolygon_collector.hpp>

#include <osmium/io/any_input.hpp>
#include <osmium/index/map/sparse_mem_array.hpp>
#include <osmium/handler/node_locations_for_ways.hpp>

#include "LogginProblemReporter.h"
#include "ExportHandler.h"

using namespace std;
using namespace osmium;

typedef index::map::SparseMemArray<unsigned_object_id_type, Location> index_type;
typedef handler::NodeLocationsForWays<index_type> location_handler_type;

class GeometryConverter {
public:
    static void exportGeometry(string input_filename, area::MultipolygonCollector<area::Assembler> &collector) {
        TIMED_SCOPE(timerObj, "exportGeometry");

        index_type index;
        location_handler_type location_handler(index);
        ExportHandler export_handler;

        io::Reader reader(input_filename);
        apply(reader, location_handler, export_handler,
              collector.handler([&export_handler](const memory::Buffer &buffer) { apply(buffer, export_handler); }));
    }
};