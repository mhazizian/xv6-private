import fileinput
def change_space_to_tab(filename):
	for line in fileinput.FileInput(filename,inplace=1):
	    print(line.replace("  ","\t"), end="")

import os
rootdir = '.'
extensions = ('.c', '.h', '.cpp', '.hpp')

for subdir, dirs, files in os.walk(rootdir):
    for file in files:
        ext = os.path.splitext(file)[-1].lower()
        if ext in extensions:
        	change_space_to_tab(os.path.join(subdir, file))
            # print (os.path.join(subdir, file))