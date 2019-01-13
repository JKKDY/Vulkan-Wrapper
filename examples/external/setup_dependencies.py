import os
from subprocess import call
from urllib .request import urlretrieve
from shutil import copytree
import zipfile

windows = os.name == 'nt'

# urls & paths
# if windows:
url_cmake = 'https://github.com/Kitware/CMake/releases/download/v3.13.2/cmake-3.13.2-win64-x64.zip'
url_glfw32 = 'https://github.com/glfw/glfw/releases/download/3.2.1/glfw-3.2.1.bin.WIN32.zip'
url_glfw64 = 'https://github.com/glfw/glfw/releases/download/3.2.1/glfw-3.2.1.bin.WIN64.zip'

dirname = os.path.dirname(__file__)
path_assimp = os.path.join(dirname, 'assimp')
path_glfw = os.path.join(dirname, 'glfw')
path_cmake_exe = os.path.join(dirname, os.path.basename(url_cmake)[:-4], 'bin', 'cmake.exe')

git_glm = 'https://github.com/g-truc/glm.git'
git_stb = 'https://github.com/nothings/stb.git'
git_gli = 'https://github.com/g-truc/gli.git'
git_glfw = 'https://github.com/glfw/glfw.git'
git_assimp = 'https://github.com/assimp/assimp.git'
git_tinyobjloader = 'https://github.com/syoyo/tinyobjloader.git'


def maybe_mkdir(path):
    if not os.path.exists(path):
        os.mkdir(path)


def git_clone(repo, directory):
    real_dir = os.path.join(directory, os.path.basename(repo)[0:-4])
    if not os.path.exists(real_dir):
        os.mkdir(real_dir)
    if len(os.listdir(real_dir)) == 0:
        try:
            import git
        except ImportError:
            if windows:
                call(['git', 'clone', repo, real_dir])
        else:
            git.Git(directory).clone(repo)
    return os.path.basename(repo)[0:-4]


git_clone(git_glm, dirname)
git_clone(git_stb, dirname)
git_clone(git_gli, dirname)
git_clone(git_tinyobjloader, dirname)

if windows:
    def maybe_copy(src, dst):
        if not os.path.exists(dst):
            copytree(src, dst)


    def maybe_download(url, dst):
        if not os.path.exists(dst):
            urlretrieve(url, dst)

    # cmake
    maybe_download(url_cmake, os.path.join(dirname, os.path.basename(url_cmake)))

    if not os.path.exists(os.path.join(dirname, os.path.basename(url_cmake)[:-4])):
        with zipfile.ZipFile(os.path.join(dirname, os.path.basename(url_cmake)), 'r') as zip_ref:
            zip_ref.extractall(dirname)
             
    # glfw
    maybe_mkdir(path_glfw)
    extract32 = os.path.join(path_glfw, 'glfw-3.2.1.bin.WIN32')
    extract64 = os.path.join(path_glfw, 'glfw-3.2.1.bin.WIN64')
    maybe_download(url_glfw32, extract32 + '.zip')
    maybe_download(url_glfw64, extract64 + '.zip')

    # unzip
    if not os.path.exists(extract32):
        with zipfile.ZipFile(extract32 + '.zip', 'r') as zip_ref:
            zip_ref.extractall(path_glfw)

    if not os.path.exists(extract64):
        with zipfile.ZipFile(extract64 + '.zip', 'r') as zip_ref:
            zip_ref.extractall(path_glfw)

    # copy libraries and headers
    maybe_copy(os.path.join(extract32, 'include/GLFW'), os.path.join(path_glfw, 'include', 'glfw'))
    maybe_copy(os.path.join(extract32, 'lib-vc2015'), os.path.join(path_glfw, 'Win32'))
    maybe_copy(os.path.join(extract64, 'lib-vc2015'), os.path.join(path_glfw, 'x64'))

    # assimp
    assimp_name = git_clone(git_assimp, path_assimp)

    # copy headers
    print(os.path.join(path_assimp, assimp_name, 'include', 'assimp'))
    maybe_copy(os.path.join(path_assimp, assimp_name, 'include', 'assimp'),
               os.path.join(path_assimp, 'include', 'assimp'))

    # build
    maybe_mkdir(os.path.join(path_assimp, 'build'))
    maybe_mkdir(os.path.join(path_assimp, 'build64'))
    if len(os.listdir(os.path.join(path_assimp, 'build'))) == 0:
        call([path_cmake_exe, '-S', os.path.join(path_assimp, assimp_name), '-B', os.path.join(path_assimp, 'build')])
    if len(os.listdir(os.path.join(path_assimp, 'build64'))) == 0:
        call([path_cmake_exe, '-G', 'Visual Studio 15 2017 Win64', '-S', os.path.join(path_assimp, assimp_name),
              '-B', os.path.join(path_assimp, 'build64')])

