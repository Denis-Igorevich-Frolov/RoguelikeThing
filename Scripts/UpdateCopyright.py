import configparser
import os
from pathlib import Path


script_path = Path(__file__).resolve()

ini_path = script_path.parent.parent / "Config" / "DefaultGame.ini"
source_path = script_path.parent.parent / "Source"

config = configparser.ConfigParser()
config.read(ini_path, encoding="utf-8")

try:
    copyright_notice = config["/Script/EngineSettings.GeneralProjectSettings"]["CopyrightNotice"]
    print("Found CopyrightNotice: " + copyright_notice)
except:
    print("DefaultGame.ini not found.")

files_changed_count = 0

def update_copyright(file_path):
    global files_changed_count
    try:
        with open(file_path, 'r', encoding='utf-8') as file:
            lines = file.readlines()

            if lines and lines[0].strip().lstrip("\ufeff").startswith("//"):
                lines[0] = "// " + copyright_notice + '\n'
            else:
                print("WARNING: " + file_path + " doesn't start with a commentary, so it wasn't changed automatically.")

            with open(file_path, 'w', encoding='utf-8') as file:
                file.writelines(lines)
                files_changed_count += 1

    except Exception as e:
        print("ERROR: updating copyright in " + file_path)

for root, dirs, files in os.walk(source_path):
    for file in files:
        file_path = os.path.join(root, file)
        update_copyright(file_path)

print(str(files_changed_count) + " files updated")
input("Press enter to exit...")