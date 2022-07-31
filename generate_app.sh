#!/bin/bash

while getopts p:n: flag
do
    case "${flag}" in
        p) path=${OPTARG};;
		n) name=${OPTARG};;
    esac
done

echo "Generating app sources in location $path..."
echo "Header file: ${name}.cpp"
echo "Source file: ${name}.h"

PROJ_ROOT=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )

source_file="${name}.cpp"
header_file="${name}.h"

cd $path

set -o noclobber
cat <<EOT>> $header_file
#pragma once

#include "Engine.hpp"

class $name : public Application {
public:
	$name();
	~$name();

	// Executed when beginning the app
	void start() override;
	// Executed each frame
	void update(float dt) override;
	// Executed each frame for UI
	void imgui_update() override;
private:
	// All private variables...
};
EOT

cat <<EOT>> $source_file
#include "$name.h"

$name::$name(): Application("$name", 800, 600) { }
$name::~$name() { }

void $name::start() {
	// Executed when beginning the app
}

void $name::update(float dt) {
	// Executed each frame
}

void $name::imgui_update() {
	// Executed each frame for UI
}
EOT

echo "\n"
echo "Generating build files for the new app..."

cd $PROJ_ROOT/Release
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Release
cd ..
cd $PROJ_ROOT/Debug
cmake .. -DCMAKE_EXPORT_COMPILE_COMMANDS=1 -DCMAKE_BUILD_TYPE=Debug

echo "Done."

 
