import sys
import subprocess
import os
import platform

from SetupPython import PythonRequirements
if not PythonRequirements.Validate():
	print("Failed to setup LoFox project.")
	sys.exit()

from SetupPremake import PremakeRequirements
from SetupVulkan import VulkanRequirements

# Change from scripts directory to root
os.chdir('../')

premakeInstalled = PremakeRequirements.Validate()

if not VulkanRequirements.Validate():
	print("Failed to setup LoFox project.")
	sys.exit()

print("\nUpdating submodules:")
subprocess.call(["git", "submodule", "update", "--init", "--recursive"])

if (premakeInstalled):
	if platform.system() == "Windows":
		print("\nRunning premake...")
		subprocess.call([os.path.abspath("./scripts/WinGenProjects.bat"), "nopause"])
	print("\nSetup completed!")
else:
	print("Benga requires Premake to generate project files.")