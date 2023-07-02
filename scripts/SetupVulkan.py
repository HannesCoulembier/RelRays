import os
import subprocess
import sys
from pathlib import Path

import Utils

from io import BytesIO
from urllib.request import urlopen

class VulkanRequirements:
	requiredVulkanVersion = "1.3"
	vulkanSubVersion = "250.0"
	vulkanDirectory = "./LoFox/vendor/VulkanSDK"

	@classmethod
	def Validate(self):
		if (not self.CheckVulkanSDK()):
			print("Vulkan SDK not installed correctly. Aborting.")
			return False

		if (not self.CheckVulkanSDKDebugLibs()):
			print("\nNo Vulkan SDK debug libs found. Install Vulkan SDK with debug libs. Aborting.")
			return False

		return True

	@classmethod
	def CheckVulkanSDK(self):
		vulkanSDK = os.environ.get("VULKAN_SDK")
		if (vulkanSDK is None):
			print("\nYou don't have the Vulkan SDK installed!")
			if not self.InstallVulkanSDK():
				return False
		else:
			print(f"\nLocated Vulkan SDK at {vulkanSDK}")

		if (self.requiredVulkanVersion not in vulkanSDK):
			print(f"You don't have the correct Vulkan SDK version! (Engine requires {self.requiredVulkanVersion})")
			return self.InstallVulkanSDK()

		print(f"Correct Vulkan SDK located at {vulkanSDK}")
		return True

	@classmethod
	def InstallVulkanSDK(self):
		permissionGranted = False
		while not permissionGranted:
			reply = str(input(f"Would you like to install VulkanSDK {self.requiredVulkanVersion}? [Y/N]: ")).lower().strip()[:1]
			if reply == 'n':
				return False
			permissionGranted = (reply == 'y')

		vulkanInstallURL = f"https://sdk.lunarg.com/sdk/download/{self.requiredVulkanVersion}.{self.vulkanSubVersion}/windows/VulkanSDK-{self.requiredVulkanVersion}.{self.vulkanSubVersion}-Installer.exe"
		vulkanPath = f"{self.vulkanDirectory}/VulkanSDK-{self.requiredVulkanVersion}.{self.vulkanSubVersion}-Installer.exe"
		print(f"Downloading {vulkanInstallURL} to {vulkanPath}")
		Utils.DownloadFile(vulkanInstallURL, vulkanPath)
		print("Running Vulkan SDK installer...")
		os.startfile(os.path.abspath(vulkanPath))
		print("Re-run this script after installation!")
		quit()

	@classmethod
	def CheckVulkanSDKDebugLibs(self):
		vulkanSDK = os.environ.get("VULKAN_SDK")
		shadercdLib = Path(f"{vulkanSDK}/Lib/shaderc_sharedd.lib")

		return shadercdLib.exists()

if __name__ == "__main__":
	VulkanConfig.Validate()