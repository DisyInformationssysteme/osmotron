#define CATCH_CONFIG_MAIN

#include <iostream>
#include <fstream>

#include <osmium/area/assembler.hpp>
#include <osmium/area/multipolygon_collector.hpp>
#include <boost/tokenizer.hpp>

#include "catch.hpp"
#include "../GeometryConverter.h"

INITIALIZE_EASYLOGGINGPP

using namespace boost;
typedef tokenizer<escaped_list_separator<char>> Tokenizer;
typedef vector<vector<string>> CSV;

string testFolder = "../test/resources/";

CSV ReadResultCSV() {
    ifstream in("result.csv");
    if (!in.is_open()) {
        throw runtime_error("Unable to open result file!");
    };

    CSV lines;
    string fullLine;
    while (getline(in, fullLine)) {
        Tokenizer tok(fullLine, escaped_list_separator<char>('\\', ';', '\"'));
        vector<string> splittedLine;
        splittedLine.assign(tok.begin(), tok.end());
        lines.push_back(splittedLine);
    }
    return lines;
}

TEST_CASE("Test file problems", "[input problems]") {
    area::Assembler::config_type assembler_config;
    area::MultipolygonCollector<area::Assembler> collector(assembler_config);

    SECTION("Missing file") {
        string input = "missingFile.osm";
        REQUIRE_THROWS(GeometryConverter::exportGeometry(input, collector));
    }

    SECTION("Empty file") {
        string input = testFolder + "empty.osm";
        REQUIRE_THROWS(GeometryConverter::exportGeometry(input, collector));
    }
}

TEST_CASE("Test simple files", "[simple input]") {
    remove("result.csv");
    area::Assembler::config_type assembler_config;
    area::MultipolygonCollector<area::Assembler> collector(assembler_config);

    SECTION("Empty file / Header test") {
        string input = testFolder + "minimal.osm";
        GeometryConverter::exportGeometry(input, collector);
        auto csv = ReadResultCSV();
        REQUIRE(csv.size() == 1);
        REQUIRE(csv[0][0] == "id");
        REQUIRE(csv[0][1] == "type");
        REQUIRE(csv[0][2] == "tag");
        REQUIRE(csv[0][3] == "wkt");
        REQUIRE(csv[0][4] == "wkb");
        REQUIRE(csv[0][5] == "geojson");
    }

    SECTION("Single node file") {
        string input = testFolder + "simpleNode.osm";

        GeometryConverter::exportGeometry(input, collector);
        auto csv = ReadResultCSV();
        REQUIRE(csv.size() == 2);
        REQUIRE(csv[1][0] == "42");
        REQUIRE(csv[1][1] == "node");
        REQUIRE(csv[1][2] == "[tagKey=tagValue]");
        REQUIRE(csv[1][3] == "POINT(8.2 48.1)");
        REQUIRE(csv[1][4] == "AQEAAABmZmZmZmYgQM3MzMzMDEhA");
        REQUIRE(csv[1][5] == "{type:Point,coordinates:[8.2,48.1]}");
    }

    remove("result.csv");
}