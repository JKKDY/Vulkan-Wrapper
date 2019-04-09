import os
from shutil import copytree
from shutil import copyfile

dirname = os.path.dirname(__file__)
assimp_libs64 = os.path.join(dirname, 'build64', 'code')
assimp_libs32 = os.path.join(dirname, 'build', 'code')


def maybe_copy(src, dst):
    if not os.path.exists(dst) and os.path.exists(src):
        copytree(src, dst)


if not os.path.exists(os.path.join(dirname, 'include', 'assimp', 'config.h')):
    copyfile(os.path.join(dirname, 'build', 'include', 'assimp', 'config.h'),
             os.path.join(dirname, 'include', 'assimp', 'config.h'))
    
maybe_copy(os.path.join(assimp_libs32, 'Debug'), os.path.join(dirname, 'Debug', 'Win32'))
maybe_copy(os.path.join(assimp_libs32, 'Release'), os.path.join(dirname, 'Win32'))
maybe_copy(os.path.join(assimp_libs64, 'Debug'), os.path.join(dirname, 'Debug', 'x64'))
maybe_copy(os.path.join(assimp_libs64, 'Release'), os.path.join(dirname, 'x64'))



