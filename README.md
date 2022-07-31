Engine type of thing that is built while learning OpenGL.

Sponza Model:
![Sponza Model + point lights](gallery/img.jpeg)
Mandelbrot Set:

<img src="gallery/giphy.gif" width="400px">
xdd

made possible thanks to learnopengl.com.

To create a new Application, type:
`sh generate_app.sh -p YOUR_APP_LOCATION -n YOUR_APP_NAME`
Note that `YOUR_APP_LOCATION` must be in one of the directories that CMake knows, such as
`apps/`
`src/`
`include/`
Example:
`sh generate_app.sh -p apps/learnopengl -n DepthTest` will create 2 files: `DepthTest.h` and `DepthTest.cpp` which is a boilerplate code for a new application.
Also the script will generate the necessary build files both in `Release` and `Debug`.

Once you have made your app, you can go to `src/main.cpp` and instantiate your app.

