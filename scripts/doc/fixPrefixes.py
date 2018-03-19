import sys
import io
import os.path
import fileinput

def get_problem_files(dir):
    return [name for name in os.listdir(dir)
            if os.path.isfile(os.path.join(dir, name))
            and name.startswith("_")]

def main():

    print ("""\
This script removes file with leading underscores in 
file name from an doxygen output
""")
    print(len(sys.argv))
    if (len(sys.argv) != 2 ):
        print ("""\

Usage:  fixPrefixes directory
""")
        sys.exit(0)

    folder=sys.argv[1];

    print ("Process folder: " + folder)
    problemfiles = get_problem_files(folder)
    for problem_filename in problemfiles:
        print (problem_filename)
    for root, dirs, files in os.walk(folder):
        for name in files:
            if (not name.endswith(".js") and not name.endswith(".html")): continue
            fullName = os.path.join(root, name)
            print(name);
            with open(fullName, 'r',encoding='utf8') as file:
                filedata = file.read()
            for problem_filename in problemfiles:
                print("...." + problem_filename)
                filedata = filedata.replace(problem_filename, "TTT"+problem_filename)
            with open(fullName, 'w',encoding='utf8') as file:
                file.write(filedata)
    for problem_filename in problemfiles:
        print (problem_filename)
        os.rename(os.path.join(folder,problem_filename), 
            os.path.join(folder,"TTT"+problem_filename))
    return 0

if __name__ == '__main__':
    status = main()
    sys.exit(status)