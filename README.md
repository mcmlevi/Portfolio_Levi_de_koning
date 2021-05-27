# What is this Repository?
This is a Repository containing projects I worked on during my time at Breda University of applied sciences. Some of these projects are projects I worked on alone and some projects were made in a bigger group. The goal of this repo is to showcase some of the projects I worked on and what my contributions were. As my portfolio website doesn't realy allow for that like github does.

This Repo Contains the Following Projects:

- CPU raytracers
- Wacky Noodle
- Thorium and Frac custom Game engines

# Custom Game Engines
Year two's main focus was custom game engines. For this year there where 2 main game engines we worked on mainly Thorium and Frac. Thorium spanned for 8 weeks during the first semester of year 2 and Frac has been in development in one way or another for the third semester of year 2 and will run till june 2021. With the final goal releasing a game on itch.io

These projects aren't fully compilable due to not being able to share all needed files due to NDA so they can only be used as a reference on the github page itself. 

## Thorium
![screen](https://user-images.githubusercontent.com/39926733/119725216-fa01c780-be6f-11eb-9c34-e966f7acc416.PNG)

Thorium was developed with a team of 6 programmers and 1 artist, 2 Gameplay Programmers , 2 engine programmers and 1 graphics programmer. During this project I worked mainly on the engine itself and supporting other developers wherever neccesairy. Thorium was a great learning opertunity,tought me a lot about design and scalability. these points are also why thorium in the end failed as a project. We managed to write a game on it, however development became harder and harder as we made some descision that made the code base not great to use and time preasure, made us not able to, fix these issues. However all these problems served as a learning oppertunity, for the 2nd generation of custom game engines Frac. Where I took the things I learned and tried to write an engine that, was easier to use and scalable. and avoid the pitfalls I walked in the first time when writing a project of this scale something I had never done before. and never with such a big team. 

Some of the Features of Thorium:
- Obj model loading
- Audio support using Fmod
- partial PS4 support
- custom memory allocators using https://github.com/mtrebi/memory-allocators and expended to support the ps4. 

### What did I do for the project?
As mentioned above I mainly focused on the engine and almost had no hands in the engine and gameplay code. I also wrote the imput handling and had my hands in some of the core files in our engine such as the event dispatcher which was used for systems to communicate. I also implemented the audio system based of Bojan Endrovski system and adapted it to fit in a ECS and our engine. I also wrote most of the IMGUI ui in the engine. such as the hiarchy tool, and component "inspector" 
Some of the the files I worked on for this project:
- [Message logger](https://github.com/mcmlevi/Portfolio_Levi_de_koning/blob/main/Custom%20Game%20Engines/Thorium/TowerDefensePS4/Projects/Thorium/include/Core/MessageLogger.h)
- [input handler](https://github.com/mcmlevi/Portfolio_Levi_de_koning/blob/main/Custom%20Game%20Engines/Thorium/TowerDefensePS4/Projects/Thorium/include/Events/InputHandler.h)
- [event dispatcher](https://github.com/mcmlevi/Portfolio_Levi_de_koning/blob/main/Custom%20Game%20Engines/Thorium/TowerDefensePS4/Projects/Thorium/include/Events/EventMessenger.h)
- [memory manager](https://github.com/mcmlevi/Portfolio_Levi_de_koning/blob/main/Custom%20Game%20Engines/Thorium/TowerDefensePS4/Projects/Thorium/include/Memory/MemoryManager.h)
## Frac
![Frac](https://user-images.githubusercontent.com/39926733/119726440-6630fb00-be71-11eb-9012-ef0e85ad9642.png)

Frac is the second generation custom game engine and is the combined effort of 7 programmers, 2 graphics programmers, 2 engine programmer, 1 tools / engine programmer and 2 gameplay programmers. I served as the main engine programmer for this project. The group consists of 50/50 programmers those who worked on thorium and those who worked on another tower defense engine made for the nintendo switch. As neither team had all programmers left that worked on the orgininal engine and we all agreed both engines either had  issues or where to complex to use. So we decided to start over with a new engine That hopefully would be clear for all teamembers how to use and would be an improvement from it's predecessor. This engine also got nummerous improvements over the old engine. 

- Certain bugs that where present in the old engines didn't exist in this one mainly the transforms.
- Debug line renderer
- Text renderer
- GLTF model loading with a PBR shaders
- improved scalability
- nitendo switch support 

For this engine I also tried applying knowledge I have learned from books such as clean architecture and data oriented design. as well as programming patterns and using 3D game engine design as a tool with some engine design problems such as tackling input as well as engine entry / construction.

### What did I work on?
My main role this block was designing the engine and writing some of the harder systems that required templates to work. I also tried to assist other programmers to write code in Frac and help them getting used to Frac. And help them with any issues that arose. We used Flecs as an ECS that, had some intresting quirks and problems so helping programmers go trough that minefield took a suprising amouth of time. 

For the ECS we wanted to write a wrapper so we didn't expose flecs directly into the engine. Looking back at it, was the wrong descision. To not expose flecs to the engine seemed like a good idea and was supported by ideas and principles written in clean achitecture. However for this project it ended up resulting in a large chunck of maintanance and provided very little benefit. I basically designed and wrote the entire wrapper around flecs. Although it turned out this wasn't the best desicion after using it to make a game, it was something we agreed on as a team would be needed It provided plenty of challanges and learning opportunities.
- [ECS wrapper](https://github.com/mcmlevi/Portfolio_Levi_de_koning/blob/main/Custom%20Game%20Engines/Frac/FracEngine/include/Core/ECSWrapper.h)
- [Querry wrapper](https://github.com/mcmlevi/Portfolio_Levi_de_koning/blob/main/Custom%20Game%20Engines/Frac/FracEngine/include/Core/Query.h)
- [System wrapper](https://github.com/mcmlevi/Portfolio_Levi_de_koning/blob/main/Custom%20Game%20Engines/Frac/FracEngine/include/Core/System.h)

For this project I also wrote the first version for quite some systems that other programmers took up and adapted and improved. For example in Thorium I wrote a wildcard system that was used as inspiration for the newer version in Frac written by Danny Kruiswijk. I also wrote the first version of the text renderer during the second semester of the 2nd year that got picked up by Dion van der wateren and got expanded and improved to fit the needs of Frac. 
# CPU RayTracer

![DOF](https://user-images.githubusercontent.com/39926733/119718849-5e208d80-be68-11eb-9e7a-fb83c2e16846.png)
### Project Information
- This Project was made during the second semester in my first year at BUAS.
- The project lasted for 8 weeks.
- It was a solo project.
- This project was developed with windows in mind. It might run on linux but I have never attempted that.
### Project Description
For this project we had to make a simple Raytracer capable of rendering spheres, planes and boxes using transforms to be rotated. These features I managed to accomplish very early in the block and I spend some more time on extra features that, I thought would be intresting. Texture Support, normal mapping, multithreading and a BVH. I also added some simple UI to display me information. I also worked on some tooling to help me debug problems such as a BVH visualizer and a build in tool to make screenshots. The raytracer is whitted style with some more fancy tricks when you turn on the "screen-shot mode" such as soft shadows and depth of field.
### What did I work on
![dragon](https://user-images.githubusercontent.com/39926733/119720065-d0de3880-be69-11eb-9b18-9cfcae51c711.png)

At the time, while doing/making the project I was most proud of the following things:
- Optimizing the math libary
- Implementing multithreading
- Implementing the BVH
- implementing more path traced features such as Depth of field and soft shadows.
### Reflection on the project
This Project allowed me to explore the things I realy, liked as, I was allowed complete freedom this block and I ended up being done with the required material rather quickly. So I could experiment with features that intrested me, such as multithreading and low level optimalizations using SIMD. Looking back at my implementations of these, they where naive at best, but they did pave the path to learn more about these topics and learning more about these topics in my free time with books such as C++ Concurrency in Action by Anthony Williams and Data oriented design. 
### Future work and further development:
In my free time I am / was working on a new raytracer that focused even more on speed dropping, other things such as primatives. In the end this turned out to be a failure however some of the things in this small project where quite cool I think. The performance improvement was very significant Compared to my old project I was able to run high poly objects in a much better frame time with similar features. I contribute this mainly to an improvement in the BVH and code structure and experienced gained, during my 2 years at Breda University. I also made some other improvements such as caching Bounding volume hierarchies to generate them them near instantaniously. As I was using binning to generate high quality BVHs of models. Although binning is rather fast with the amouth of tests I was doing this would still be too long to make programm quick to load and debug. So caching the BVH reduced loading times from 2-3 minutes on debug to less than a second on debug. ( for some reason MSVS realy didn't like the BVH creation and on clang this process was much faster from 2-3 minutes to a few seconds on debug) I also implemented skyboxes in this new raytracer.

# Wacky noodle
![WackyNoodle_Promo_WithLogo](https://user-images.githubusercontent.com/39926733/119739684-df851980-be82-11eb-9d5f-ee175833957a.png)
Wacky noodle is a game we released on [itch.io](https://buas.itch.io/wacky-noodle), This Project was made in unreal engine over the course of 8 weeks. It has local multiplayer support. The team consisted of 2 programmers, 3 artists and 4 designers.  

Have you ever dreamed of being a great jousting knight?
Dashing around on your noble steed and knocking other honorable knights of their horses?

Of course you have! But horses are expensive, so a pig will do. 

Knock your friends off their piggy by poking them with a pool noodle.

TIME TO GET BRUTAL! SO GRAB YOUR NOODLE!

### What did I work on?
I mainly worked on the multiplayer, writing the gamemodes and making sure everything was compatible with UE4 build in lan support. The [code provided](https://github.com/mcmlevi/Portfolio_Levi_de_koning/tree/main/WackyNoodle) has all been written by me. I also worked on the customization of the character and saving the player data such as last outfit and last used name. For this project I also wrote a custom tool for designers to track and record player positions during the match and play them back later in the tool to view the match on a timeline and see events such as player deaths and movement of the players over time. This tool was used to indentify chokepoints and improve map usage by making less travelled spots more intresting with things such as power ups or opening up the map.

![tool](https://user-images.githubusercontent.com/39926733/119740283-f4ae7800-be83-11eb-8ea3-5beac867982c.png)

