#!bin/bash

setConan()
{

	conan install .  --output-folder=build --build=missing --profile=$1

}

setCMAKE()
{
	mkdir build
	cd build
	cmake .. -DCMAKE_TOOLCHAIN_FILE=./build/$1/generators/conan_toolchain.cmake -DCMAKE_BUILD_TYPE=$1
}

echo "0) Build as release"
echo "1) Build as debug"
echo "2) Build as conan ONLY"
echo "3) Delete build cache files"

read option

if [ $option != 0 ] && [ $option != 1 ] && [ $option != 2 ] && [ $option != 3 ]; then
echo "ERROR: OPTION NOT ALLOWED"
exit 1;
fi

if [ $option -eq 0 ];then
	setConan default
	setCMAKE Release
	exit 0
fi

if [ $option -eq 1 ];then
	setConan debug
	setCMAKE Debug
	exit 0
fi

if [ $option -eq 2 ];then
	echo "This way the project will be build with the debug profile"
	setConan debug
	exit 0
fi

if [ $option -eq 3 ];then

	rm -rf build CMakeUserPresets.json
	exit 0
fi

