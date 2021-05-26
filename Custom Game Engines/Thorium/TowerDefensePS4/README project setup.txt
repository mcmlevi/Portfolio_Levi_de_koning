To run the engine 3 small setup variables must be set

in the TowerDefensePS4 Project settings go to the Debugging tab and set
Working Directory to -> $(SolutionDir)bin\$(ProjectName)\$(Platform)-$(Configuration)\

in the Thorium project settings go to Debugging tab and set 
Working Directory to -> $(SolutionDir)bin\$(ProjectName)\$(Platform)-$(Configuration)\

Our project uses copy scripts to move assets and libs into the final build folder so if these settings are not set it won't be able to find the .lib files or won't be able to load the assets.
Also make sure that the buildscripts folder is pulled from perforce. otherwise the build will fail as post build command rely on the copy script in the post build events.

Lastly make sure to set the TowerDefensePS4 as startup project.