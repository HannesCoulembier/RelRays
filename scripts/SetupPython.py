import sys
import subprocess
import importlib.util as importLib_util

class PythonRequirements:

	requiredPackages = ["requests"]

	@classmethod
	def Validate(self):
		print("Validating python installment:")

		if not self.ValidateVersion(3, 3):
			print("Cannot complete setup with current python version. Aborting.")
			return False

		print("Checking for necessary packages:")
		for packageName in self.requiredPackages:
			if not self.ValidatePackage(packageName):
				print(f"Cannot complete setup without '{packageName}' package installed. Aborting.")
				return False


		print("Python installment is sufficient.")
		return True

	@classmethod
	def ValidateVersion(cls, versionMajor, versionMinor):
		if sys.version is not None:
			print("Python version {0:d}.{1:d}.{2:d} detected.".format(sys.version_info.major, sys.version_info.minor, sys.version_info.micro))
			if sys.version_info.major < versionMajor or (sys.version_info.major == versionMajor and sys.version_info.minor < versionMinor):
				print(f"Python version too low, expected version {versionMajor}.{versionMinor} or higher.")
				return False
			return True
		print("Python version is None.")
		return False

	@classmethod
	def ValidatePackage(cls, packageName):
		if importLib_util.find_spec(packageName) is None:
			print(f"Package {packageName} is not installed.")
			return self.InstallPackage(packageName)
		print(f"Found package '{packageName}'.")
		return True

	@classmethod
	def InstallPackage(cls, packageName):
		permissionGranted = False
		while not permissionGranted:
			reply = str(input(f"Would you like to install Python package '{packageName}'? [Y/N]: ")).lower().strip()[:1]
			if reply == 'n':
				return False
			permissionGranted = (reply == 'y')

		print(f"Installing {packageName} module...")
		subprocess.check_call(['python', '-m', 'pip', 'install', packageName])

		return self.ValidatePackage(packageName)

if __name__ == "__main__":
	PythonRequirements.Validate()