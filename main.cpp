#include <iostream>
#include <fstream>

#include <osmium/area/assembler.hpp>
#include <osmium/area/multipolygon_collector.hpp>
#include <osmium/io/any_input.hpp>

#include "LogginProblemReporter.h"
#include "GeometryConverter.h"

INITIALIZE_EASYLOGGINGPP

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

    area::Assembler::config_type assembler_config;
    auto reporter = unique_ptr<area::ProblemReporter>(new LoggingProblemReporter());
    assembler_config.problem_reporter = reporter.get();
    assembler_config.check_roles = true;
    area::MultipolygonCollector<area::Assembler> collector(assembler_config);

    LOG(INFO) << "Starting to process file " << argv[1];
    LOG(INFO) << "Checking relations for osm file...";
    io::Reader reader1(input_filename);
    collector.read_relations(reader1);

    LOG(INFO) << "Converting file content...";
    GeometryConverter::exportGeometry(input_filename, collector);
    LOG(INFO) << "File " << argv[1] << " successfully converted (see result.csv).";
}
