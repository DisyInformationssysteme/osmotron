# Osmotron

<a href="http://www.boost.org/users/license.html"><img src="https://img.shields.io/badge/license-Boost-blue.svg"></a>
[![Build Status](https://travis-ci.org/DisyInformationssysteme/osmotron.svg?branch=master)](https://travis-ci.org/DisyInformationssysteme/osmotron)

This is a small command line tool that takes a OSM ([OpenStreetMap](https://www.openstreetmap.org)) file as input and creates a list of contained geometries as output.
The output contains the geometries in the following formats: WKT, WKB (base64 encoded), GeoJSON

In addition, it logs all possible geometry errors it encounters and warn about problems with the OSM data.

This tool requires [Boost](http://www.boost.org/) and the [Osmium](http://osmcode.org/libosmium/) library to compile.
For more details, have a look at the [dependencies](https://github.com/osmcode/libosmium/wiki/Libosmium-dependencies) required by Osmium.

---
TL;DR:
### OSM -> WKT, WKB, GeoJSON
