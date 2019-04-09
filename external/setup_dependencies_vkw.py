import os
from shutil import copyfile
from subprocess import call

def create_paths_if_invalid(*paths):
    for path in paths:
        if not os.path.exists(path):
            os.mkdir(path)


dirname = os.path.dirname(__file__)
proj_path_vulkan = os.path.join(dirname, 'Vulkan')

if os.name == 'nt':
    os_path_vulkan = os.environ['VULKAN_SDK']
    call(['robocopy', os.path.join(os_path_vulkan, "include"), os.path.join(proj_path_vulkan, "include"), "/E"])
    call(['robocopy', os.path.join(os_path_vulkan, "Lib"), os.path.join(proj_path_vulkan, "lib64"), "/E"])
    call(['robocopy', os.path.join(os_path_vulkan, "Lib32"), os.path.join(proj_path_vulkan, "lib32"), "/E"])
else:
    create_paths_if_invalid(proj_path_vulkan)
    print("Not working for any other OS other than Windows")









