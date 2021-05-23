import os

from . import launch


def getFrameFiles(frameid):
    if launch.g_iopath is None:
        return ()
    dirpath = os.path.join(launch.g_iopath, '{:06d}'.format(frameid))
    if not os.path.isdir(dirpath):
        return ()
    res = []
    try:
        for name in os.listdir(dirpath):
            path = os.path.join(dirpath, name)
            name, ext = os.path.splitext(os.path.basename(path))
            if os.path.exists(path):
                res.append((name, ext, path))
    except FileNotFoundError:
        pass
    return tuple(res)


def getFrameCount(max_frameid):
    if launch.g_iopath is None:
        return 0
    for frameid in range(max_frameid):
        dirpath = os.path.join(launch.g_iopath, '{:06d}'.format(frameid))
        if not os.path.isdir(dirpath):
            return frameid
    return max_frameid
