import os

MIT_LICENSE_HEADER = """\
/********************************************************************/
/*                                                                  */
/*                            MIT License                           */
/*                                                                  */
/*  Copyright (c) 2022-present Reese Gallagher, Cristhian De La Paz */
/*                                                                  */
/*  This code is licensed under the MIT license (MIT)               */
/*                                                                  */
/*  (http://opensource.org/licenses/MIT)                            */
/*                                                                  */
/********************************************************************/
"""

def add_license_header(filename, header):
    with open(filename, 'r+') as file:
        content = file.read()
        if header.strip() not in content: # check for existing header
            file.seek(0, 0)
            file.write(header + '\n' + content)
            print(f"Added license to {filename}")

def add_license_to_files(directory):
    for root, dirs, files in os.walk(directory):
        for file in files:
            if file.endswith('.cpp') or file.endswith('.h'):
                file_path = os.path.join(root, file)
                add_license_header(file_path, MIT_LICENSE_HEADER)

current_dir = os.getcwd()
parent_dir = os.path.dirname(current_dir)
os.chdir(parent_dir)
add_license_to_files('include')
add_license_to_files('src')
