Cross-platform solution on Cmake Windows
Used with Visual Studio 19 and cmake 3.9.2 in Release configuration


Steps to run Autotester:
 - Build using Cmake in Release configuration
 - Run: {build-directory}\src\autotester\autotester.exe {sourcefile_path} {queryfile_path} {outputfilename}.xml


To use tests found in the "Test09" directory:
 - Open the runAll.bat file provided in each test directory
 - Change the variable "buildpath" to the relative build path
 - run the .bat file


