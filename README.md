# osmotron

This is a small command line tool that takes a OSM ([OpenStreetMap](https://www.openstreetmap.org)) file as input and creates a list of contained geometries as output.
The output contains the geometries in the following formats: WKT, WKB (base64 encoded), GeoJson

In addition, it logs all possible geometry errors it encounters and warn about problems with the OSM data.

This tool requires [Boost](http://www.boost.org/) and the [Osmium](http://osmcode.org/libosmium/) library to compile.
For more details, have a look at the [dependencies](https://github.com/osmcode/libosmium/wiki/Libosmium-dependencies) required by Osmium.