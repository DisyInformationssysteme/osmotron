#include <iostream>
#include <fstream>

#include <osmium/area/assembler.hpp>
#include <osmium/area/multipolygon_collector.hpp>
#include <osmium/io/any_input.hpp>
#include <boost/program_options.hpp>
#include <boost/algorithm/string.hpp>

#include "LogginProblemReporter.h"
#include "GeometryConverter.h"

INITIALIZE_EASYLOGGINGPP

namespace po = boost::program_options;

int main(int ac, char *av[]) {
    // Configure logger
    START_EASYLOGGINGPP(ac, av);
    el::Configurations conf("logging.conf");
    el::Loggers::reconfigureAllLoggers(conf);

    string outputFilename;
    string outputTypesRaw;
    set<string> outputTypes;
    string outputGeometriesRaw;
    set<string> outputGeometries;
    string inputFilename;

    po::options_description desc("Possible options");
    desc.add_options()
            ("help,h", "produce help message")
            ("output,o", po::value<string>(&outputFilename)->default_value("result.csv"), "output filename")
            ("type,t", po::value<string>(&outputTypesRaw)->default_value("node,way,area"), "output types (node, way, area). Multiple formats can be joined with a comma, e.g. 'way,area'")
            ("geoformat,gf", po::value<string>(&outputGeometriesRaw)->default_value("wkt,wkb,geojson"), "geometry output format (wkt, wkb, geojson). Multiple formats can be joined with a comma.")
            ("input,i", po::value<string>(&inputFilename)->required(), "input filename (required)");
    po::positional_options_description p;
    p.add("input", -1);

    try {
        po::variables_map vm;
        po::store(po::command_line_parser(ac, av).options(desc).positional(p).run(), vm);

        if (vm.count("help")) {
            cout << desc << endl;
            return 0;
        }

        po::notify(vm);

        if (inputFilename.empty()) {
            cerr << "No input filename specified!" << endl;
            return -1;
        }

        if (outputFilename.empty()) {
            cerr << "No output filename specified!" << endl;
            return -1;
        }

        boost::split(outputTypes, outputTypesRaw, boost::is_any_of(","));
        boost::split(outputGeometries, outputGeometriesRaw, boost::is_any_of(","));
    }
    catch (boost::program_options::required_option &e) {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        return -1;
    }
    catch (boost::program_options::error &e) {
        std::cerr << "ERROR: " << e.what() << std::endl << std::endl;
        return -1;
    }

    area::Assembler::config_type assembler_config;
    auto reporter = unique_ptr<area::ProblemReporter>(new LoggingProblemReporter());
    assembler_config.problem_reporter = reporter.get();
    assembler_config.check_roles = true;
    area::MultipolygonCollector<area::Assembler> collector(assembler_config);

    LOG(INFO) << "Starting to process file " << inputFilename;
    LOG(INFO) << "Checking relations for osm file...";
    io::Reader reader1(inputFilename);
    collector.read_relations(reader1);

    LOG(INFO) << "Converting file content...";
    GeometryConverter::exportGeometry(inputFilename, collector, outputFilename, outputTypes, outputGeometries);
    LOG(INFO) << "File " << inputFilename << " successfully converted (see " << outputFilename << ").";
}
