# Vulkan

reference: https://vulkan-tutorial.com/ & https://github.com/SaschaWillems/Vulkan & https://renderdoc.org/

The problem has been solved.
My view matrix is wrong. The initial matrix should be identity matrix.
--------------------------
I meet a problem. I load a 3d model and put an texture on the model. The model is a plane, just 2 triangle. And the texture is 256x256 32bit bmp file. But the result always loses the right-bottom corner.
![alt text](problem1.jpg)

I change to draw wireframe. It happens the same problem.
![alt text](problem2.jpg)

Even I just draw a triangle. It does, too.
![alt text](problem3.jpg)

But if I move the tiangle to left. The problem seems to be sloved.
![alt text](problem4.jpg)

If I move the tiangle to right. The problem seems to become worse.
![alt text](problem5.jpg)
