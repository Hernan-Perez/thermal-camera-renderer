# AMG8833 Thermal Camera Renderer

## Description

This Desktop App reads the serial output of an arduino with an AMG8833 Thermal Camera connected and displays that information as an image in realtime using OPENGL Libraries.</br></br>
The AMG8833 contains 64 sensors (grid of 8 x 8 sensors). The Thermal Camera library returns a buffer with the raw temperature value of each sensor in Celsius.</br></br>
The desktop app reads that buffer from the Arduino device and processes that information:
- Lowest sensor value gets assigned a blue color.
- Highest sensor value gets assigned a red color.
- Average sensor value gets assigned a green color.
- All in between values are assigned an interpolated color based on their value compared to the 3 points above.

Currently there are 2 display modes:</br>
- Display each sensor as a cube with their assigned color.</br>
- Display each sensor as a pixel with their assigned color, but each vertex of a cube has a basic interpolation of colors with their neighbor cubes.

</br>

![ezgif-3-e291a68bd8](https://user-images.githubusercontent.com/85197456/220471363-8e4f36cc-7961-4288-8f35-6459de88fd82.gif)

</br>
This is still an open project which I'm using as a practice to learn more about image processing techniques.
