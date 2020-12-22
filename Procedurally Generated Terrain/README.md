# Procedurally Generated Terrain


[![PGT Video](terrain_youtube.PNG)](https://www.youtube.com/watch?v=LP7ggmH3EM8&feature=youtu.be)

Video Link: https://www.youtube.com/watch?v=LP7ggmH3EM8&feature=youtu.be



Procedurally Generated Terrain is a multi-step process and I had to learn each individual step. The steps involved include: Perlin Noise, Octaves, Frequency and Lacunarity, Meshing, and Character Movement (for the camera).

# Perlin Noise

Perlin noise is at the center of most procedurally generated terrain techniques, fire, water, and other procedural functions. Perlin noise is a type of coherent noise, meaning that it changes gradually, which is perfect for procedural generation because of its smooth curve. Perlin noise can be used on 2D and 3D applications. Generating random numbers could accomplish the same purpose as Perlin noise, however, using a random number generation isn’t as smooth or as clean as Perlin noise.
Perlin Noise Random Numbers


Source: https://cdn.kastatic.org/ka-perseus-images/0fd97fc7ab7ac5a7670935f1695d2a0c614e5252.png
https://cdn.kastatic.org/ka-perseus-images/81e9d201147cd09f1b78f9541993d8460355eb3e.png
https://www.khanacademy.org/computing/computer-programming/programming-natural-simulations/programming-noise/a/perlin-noise

# Octaves, Frequency, Lacunarity

These three variables determine how the noise looks by changing different values of the noise. In a nutshell, octaves influence how many levels of detail the noise has; lacunarity determines the frequency of the octaves (the detail of each octave); persistence determines the amplitude, or the shape, of each octave; and the scale is the zoom in / zoom out factor of the Perlin noise. The octaves are combined together to form surfaces, or layers, of noise. It’s easy to conceptualize these octave layers by thinking of the octaves as the levels on a topographic map (at least that’s how I visualized it all).
Image Source: https://i.pinimg.com/originals/56/4c/a7/564ca726d48c6b952f6141cf44f6f0a7.gif

# Coloring

Coloring in Unity wasn’t particularly difficult. I simply took the height values and assigned a color to any values within a certain range. I could have fine tuned the numbers, and added more colors, for a more realistic and visually appealing product. I searched through the heights, and then updated the colors based on the height of the noise.

# Mesh

Thankfully, this wasn’t my first experience with meshing, and I understood the principles from previous projects and homework. The idea of meshing on this terrain map is somewhat complicated, but it involves calculating triangle vertices, performing vector calculations on those vertices and then normalizing the values. The hardest part of this section was determining how to connect it to Unity and map it work. I had to create several additional materials and scripts to get it to work, but in the end it did. Fundamentally, the triangles that are added to the mesh are at
1) (theVertexIndex, theVertexIndex + width + 1, theVertexIndex + width)
AND
2) (vertexIndex + width + 1, vertexIndex, vertexIndex + 1)

