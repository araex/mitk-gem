content=`cat ${1}`
parsed=${content##*enabled_plugins}
parsed=${parsed%%)}
cmake_args="-DMITK_BUILD_ALL_PLUGINS:BOOL=0"
for line in $parsed; do
	tmp=${line%\"}
	tmp=${tmp#\"}
	cmake_args="${cmake_args} -DMITK_BUILD_${tmp}:BOOL=1"
done
echo "Generated CMake arguments: $cmake_args"
