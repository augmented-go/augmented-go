from glob import glob

# is need to be able to use the c++11 features of the v120_CTP_Nov2012 compiler (why are we not using gcc anyway :( )
# will be run in the build directory

proj_files = glob("*/*.vcxproj")
for file in proj_files:
    print("modifying: " + file)
    with open(file, "r+") as f:
         old = f.read() # read everything in the file
         f.seek(0) # rewind
         f.write(old.replace("<PlatformToolset>v110</PlatformToolset>", "<PlatformToolset>v120_CTP_Nov2012</PlatformToolset>"))