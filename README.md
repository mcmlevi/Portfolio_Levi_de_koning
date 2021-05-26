# What is this reposotory?
This is a reposotory containing projects I worked on during my time at Breda University of applied sciences. Some of these projects Are projects I worked on alone and some projects where made in a bigger group. The goal of this repo is to showcase some of the projects I worked on and what my contributions where. As my portfolio website doesn't realy allow for that like github does.

This Repo Contains the Following Projects:

- CPU raytracers
- Wacky Noodle
- Thorium and Frac custom Game engines

# CPU RayTracer

![DOF](https://user-images.githubusercontent.com/39926733/119718849-5e208d80-be68-11eb-9e7a-fb83c2e16846.png)
### Project Information
- This Project was made during the second semester during my first year at BUAS.
- The project lasted for 8 weeks.
- The group size was 1 person
- This project was developed with windows in mind. It might run on linux but I have never attempted that.
### Project Description
For this project we had to make a simple Raytracer capable of rendering spheres, planes and boxes using transforms to be rotated. These features I managed to accomplish very early in the block and I spend some more time on extra features that I thought would be intresting. Texture Support, normal mapping, multithreading and a BVH. I also added some simple UI to display me information. I also worked on some tooling to help me debug problems such as a BVH visualizer and a build in tool to make screenshots to make screenshots. The raytracer is whitted style with some more fancy tricks when you turn on the "screen-shot mode" such as soft shadows and depth of field.
### What did I work on
![dragon](https://user-images.githubusercontent.com/39926733/119720065-d0de3880-be69-11eb-9b18-9cfcae51c711.png)

The following things are what at the time of making the project I was the most proud of:
- Optimizing the math libary
- Implementing multithreading
- Implementing the BVH
- implementing more path traced features such as Depth of field and soft shadows.
### Reflection on the project
This Project allowed me to explore the things I realy liked as I was allowed complete freedom this block and I ended up being done with the required material rather quickly. So I could experiment with features thatintrested me, such as multithreading and low level optimalizations using SIMD. Looking back at my implementations of these they where naive at best but they did pave the path to learn more about these topics and learning more about these topics in my free time with books such as C++ Concurrency in Action by Anthony Williams and Data oriented design. 
