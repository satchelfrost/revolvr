import argparse
import re
import os

def pascal_to_snake(s):
  # Insert an underscore before each uppercase letter
  # except for the first character
  return re.sub('(?<!^)(?=[A-Z])', '_', s).lower()

# Create argument parser
parser = argparse.ArgumentParser(prog='create_ritual', description='Generate RevolVR ritual')
parser.add_argument('ritual_name', help='Name of the C++ Ritual class to create')
parser.add_argument('-s', '--simple',
                    help='Do not include overridable methods such as Begin(), Update(), etc.',
                    action="store_true")
parser.add_argument('-v', '--verbose',
                    help='Prints line by line output of files created to standard out', action="store_true")
parser.add_argument('-f', '--force',
                    help='force file creation, even if it exists already', action="store_true")
parser.add_argument('-a', '--android_app_dir',
                    help='Automatically place in the android app folder', type=str, default="../../../app")
args = parser.parse_args()

# Save the file name
file_name = pascal_to_snake(args.ritual_name)

# Open up the ritual template file
with open('ritual_template', 'r') as f:
  template = f.read()

overridables_begin = '@overridables_begin'
overridables_end = '@overridables_end'
header_begin = '@header_begin'
header_end = '@header_end'
cpp_begin = '@cpp_begin'
cpp_end = '@cpp_end'

# Get the content between the @overridables_begin and @overridables_end placeholders
overridables_content = template.split(overridables_begin)[1].split(overridables_end)[0]

# cpp definitions for overridables
overridables_definitions = ""
lines = overridables_content.splitlines()
for i in range(1, len(lines)):
    line = re.sub(r'\s+virtual void\s+', '', lines[i])
    line = re.sub(r'\s+override;', '', line)
    overridables_definitions += "\nvoid " + args.ritual_name + "::" + line + " {}\n"

# Get the content between the @header_begin and @header_end placeholders
header_content = template.split(header_begin)[1].split(header_end)[0]
header_content = header_content[1:] # remove newline

# Get the content between the @cpp_begin and @cpp_end placeholders
cpp_content = template.split(cpp_begin)[1].split(cpp_end)[0]
cpp_content = cpp_content[1:] # remove newline

# Replace the placeholders in the header content with the provided class name
header_content = header_content.replace('@class_name', args.ritual_name)
if args.simple:
  header_content = header_content.replace('@overridables', '')
else:
  header_content = header_content.replace('@overridables', overridables_content)

# Replace the placeholders in the CPP content with the provided class name and header file name
cpp_content = cpp_content.replace('@class_name', args.ritual_name).replace('@file_name', f"<{file_name}.h>")
if args.simple:
  cpp_content = cpp_content.replace('@overridables', '')
else:
  cpp_content = cpp_content.replace('@overridables', overridables_definitions)

# verbose printing
if args.verbose:
  print(header_content)
  print(cpp_content)

# Write file
def write_files(cpp_file, header_file):
    with open(header_file, 'w') as f:
      f.write(header_content)
      print(f"{header_file} written")
    with open(cpp_file, 'w') as f:
      f.write(cpp_content)
      print(f"{cpp_file} written")

# Change directory if necessary
header_file = f"include/{file_name}.h"
cpp_file = f"src/main/cpp/{file_name}.cpp"
cwd = os.getcwd()
if args.android_app_dir:
    if args.android_app_dir == parser.get_default('android_app_dir'):
        print(f"Using default android directory {args.android_app_dir}")
    else:
        print(f"Using specified android directory {args.android_app_dir}")
    parent_dir = os.path.abspath(os.path.join(cwd, args.android_app_dir))
    os.chdir(parent_dir)

# Write the header and cpp files
files_exist = os.path.isfile(header_file) or os.path.isfile(cpp_file)
if (files_exist and args.force) or (not files_exist):
    write_files(cpp_file, header_file)
else:
  print(f'Files "{header_file}" and "{cpp_file}" already exist. Override with --force or -f.')
