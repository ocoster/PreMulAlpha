# Pre-Multiplied Alpha
Pre-Multiplied alpha blend mode is a blend mode has been around for a long time, but it seems to be re-discovered every few years.

The primary reason to typically use pre-multiplied alpaha is to get rid of back outlines when rendering.

For example, if you are rendering some leaves, you have an alpha channel indicating the leaf edges.
Rendeing with the standard blend mode (alpa, 1-alpha) resiults in color bleeding in around the leaf edges. (ie. black)

Most games resolve this by having artists fill in these background areas with a fill color (ie green).
*However* by pre multiplying the image offline and using pre-Multiplied alpha blend mode (1, 1-alpha) this in fill is not necessary.



## The 3 in one blend mode




| ![](Images/BlendModes.png) | 
|:--:| 
| *Blend  modes from Morgan McGuire presentation* |

## Reducing draw calls with Pre-Multiplied Alpha





## Links

http://www.realtimerendering.com/blog/gpus-prefer-premultiplication/
http://webglfundamentals.org/webgl/lessons/webgl-and-alpha.html
http://tomforsyth1000.github.io/blog.wiki.html#[[Premultiplied%20alpha%20part%202]]
https://developer.nvidia.com/content/alpha-blending-pre-or-not-pre
http://www.adriancourreges.com/blog/2017/05/09/beware-of-transparent-pixels/


## Tools
This project includes descriptions/examples and tools for using pre-multiplied alpha.
