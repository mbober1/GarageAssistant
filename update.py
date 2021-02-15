import shutil, os
Import("env", "projenv")

dir = "firmware/"
shutil.rmtree(dir, True)
os.makedirs(dir)
dir += str(projenv["UNIX_TIME"])
dir += ".bin"
shutil.copyfile(".pio/build/nodemcuv2/firmware.bin", dir)
print(dir)

with open('version', 'w') as f:
    f.write(str(projenv["UNIX_TIME"]))