# RelRays
# Sync with LoFox commits
To sync your RelRays clone with any LoFox commits, you need to add LoFox as an upstream repository. This can be done using `git remote add upstream https://github.com/HannesCoulembier/LoFox`. To check if LoFox is correctly set as an upstream remote, use `git remove -v`.

# LoFox (TODO: Update)
LoFox is an engine that provides a simple renderer using Vulkan.
## How to install LoFox:
### Requirements:
- Operating system: Windows (10-11)
- Have [git](https://git-scm.com/downloads) installed. When installing, you can leave everything to default and just click "next" until you get to the end of the installation wizard. You can check if git is installed already by typing `git --version` in the command line (if it complains about git being an unrecognized command, you don't have git installed).
- Have [Visual Studio](https://visualstudio.microsoft.com/downloads/) installed.
### Installation:
- Go to the folder where you want to install LoFox. Open the command line in that directory (make sure you run as administrator). Type `git clone --recursive https://github.com/HannesCoulembier/LoFox.git LoFox` in the command line and press enter. This will clone the LoFox repository with all its submodules into a folder named LoFox.

- In the new LoFox folder you will find a folder called "scripts". In there, you will find a bat-file called "SetupProject.bat". When you run this (double click), it will check if you have the correct version of python, ask to install premake (type "y") and check if you have the correct version of the VulkanSDK installed.\
\
If you don't have the VulkanSDK installed or an older version, it will ask to install Vulkan and **rerun the script again afterwards**. This will run the Vulkan SDK installer. Again, while installing, you can leave everything to default and just click "next" until you get to the end of the installation wizard.\
\
When everything is installed correctly, the setup script will update your submodules (this should do nothing as you have just freshly cloned them when cloning the repository). Afterwards, it will run premake to generate the Visual Studio solution files.

- In the root directory (the folder with "scripts", "Sandbox", "LoFox" and "vendor" folders), there should now be a "LoFox.sln" solution file (along with LoFox.vcxproj, LoFox.vcxproj.filters and LoFox.vcxproj.user). If Visual Studio is correctly installed, it should have the Visual Studio logo in the icon. Double click this file to open the solution (project).
### Running the engine:
- First compile the engine (`ctr + b` is a standard keybind for that)
- To run the engine press `F5` or the green play button named "Local Windows Debugger".
- Congratulations, you should now see a big triangle on the screen!
