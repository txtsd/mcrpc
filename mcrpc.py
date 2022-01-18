import glob
import hashlib
import json
import os
import re
import shutil
import sys
from distutils.dir_util import copy_tree
from pprint import pprint
from zipfile import ZipFile

import requests

PLATFORM = sys.platform
if PLATFORM.startswith('linux'):
    PATH_DOTMC = os.path.join(os.path.expanduser('~'), '.minecraft')
elif PLATFORM.startswith('darwin'):
    PATH_DOTMC = os.path.join(os.path.expanduser('~'), 'Library', 'Application Support', 'minecraft')
elif PLATFORM.startswith('win32'):
    PATH_DOTMC = os.path.join(os.path.expandvars(r'%APPDATA%'), '.minecraft')

PATH_RP_DIR = os.path.join(PATH_DOTMC, 'resourcepacks')
PATH_CACHE = '.cache'
PATH_TARGET = os.path.join(PATH_CACHE, 'target')
PATH_DEFAULT = os.path.join(PATH_CACHE, 'default')
PATH_CLIENT_LOW = os.path.join(PATH_CACHE, 'client_low.jar')
PATH_CLIENT_HIGH = os.path.join(PATH_CACHE, 'client_high.jar')
LINK_VM = 'https://launchermeta.mojang.com/mc/game/version_manifest_v2.json'
BUF_SIZE = 65536
SESSION = requests.Session()


def choose_rp():
    list_rp = os.listdir(PATH_RP_DIR)
    list_rp_checked = []

    list_rp.sort()

    print('\nSelect the Resource Pack you want to compare:')

    for item in list_rp:
        if item[-4:] == '.zip':
            with ZipFile(os.path.join(PATH_RP_DIR, item), 'r') as zip1:
                if 'pack.mcmeta' in zip1.namelist():
                    list_rp_checked.append(item)
        elif os.path.isdir(os.path.join(PATH_RP_DIR, item)):
            if os.path.isfile(os.path.join(PATH_RP_DIR, item, 'pack.mcmeta')):
                list_rp_checked.append(item)

    if len(list_rp_checked) == 0:
        print('No valid resource packs. Install one and try again.')
        exit(1)

    for i, item in enumerate(list_rp_checked, 1):
        print("  [{}] {}".format(i, item))
    print('Resource Pack choice: ', end='')

    choice_rp_num = input()
    choice_rp = None

    for i, item in enumerate(list_rp_checked, 1):
        if i == int(choice_rp_num):
            choice_rp = item

    return(choice_rp)


def choose_mcv(low=None):
    json_vm = SESSION.get(LINK_VM)
    json_vm.raise_for_status()
    json_vm = json_vm.json()

    if low == True:
        print('\nSelect the lower Minecraft Version:')
    elif low == False:
        print('\nSelect the higher Minecraft Version:')
    else:
        print('\nSelect the Minecraft Version you want to compare against:')
    for i, item in enumerate(json_vm['versions'], 1):
        print("  [{}] {}".format(i, item['id']))
        if i == 25:
            break
    print('Minecraft Version choice: ', end='')

    choice_mcv_num = input()
    choice_mcv = None

    for i, item in enumerate(json_vm['versions'], 1):
        if i == int(choice_mcv_num):
            choice_mcv = item

    return(choice_mcv)


def setup_compare_folders(path_rp=None, mc2=False):
    if os.path.exists(PATH_TARGET):
        shutil.rmtree(PATH_TARGET)
    if os.path.exists(PATH_DEFAULT):
        shutil.rmtree(PATH_DEFAULT)

    if not mc2:
        if path_rp[-4:] == ".zip":
            with ZipFile(path_rp) as zip1:
                for file in zip1.namelist():
                    if file.startswith('assets/'):
                        zip1.extract(file, path=PATH_TARGET)
        else:
            copy_tree(os.path.join(path_rp, 'assets'), os.path.join(PATH_TARGET, 'assets'))
    else:
        with ZipFile(PATH_CLIENT_LOW) as zip2:
            for file in zip2.namelist():
                if file.startswith('assets/'):
                    zip2.extract(file, path=PATH_TARGET)

    with ZipFile(PATH_CLIENT_HIGH) as zip3:
        for file in zip3.namelist():
            if file.startswith('assets/'):
                zip3.extract(file, path=PATH_DEFAULT)

def get_client(mcv, low=False):
    json_version = SESSION.get(mcv['url'])
    json_version.raise_for_status()
    json_version = json_version.json()

    link_client = json_version['downloads']['client']['url']
    sha_client = json_version['downloads']['client']['sha1']
    sha1 = hashlib.sha1()
    sha_calculated = None

    if not os.path.exists(PATH_CACHE):
        os.makedirs(PATH_CACHE)

    if low == True:
        path_client = PATH_CLIENT_LOW
    else:
        path_client = PATH_CLIENT_HIGH

    if os.path.exists(path_client):
        with open(path_client, 'rb') as f:
            while True:
                data = f.read(BUF_SIZE)
                if not data:
                    break
                sha1.update(data)
        sha_calculated = sha1.hexdigest()

    if not sha_calculated == sha_client:
        with open(path_client, 'wb') as f:
            print('Downloading client.jar…')
            download = SESSION.get(link_client)
            f.write(download.content)


def compare(rp, mcv):
    if type(rp) is dict:
        # Comparing 2 minecraft versions
        get_client(rp, low=True)
        get_client(mcv)
        setup_compare_folders(mc2=True)
        pass
    else:
        # Comparing a resource pack against a minecraft version
        path_rp = os.path.join(PATH_RP_DIR, rp)
        get_client(mcv)
        setup_compare_folders(path_rp)
        pass

    dir_left = os.path.join(PATH_DEFAULT, '**', '*.*')
    dir_right = os.path.join(PATH_TARGET, '**', '*.*')

    files_left = glob.glob("{}".format(dir_left), recursive=True)
    files_right = glob.glob("{}".format(dir_right), recursive=True)

    if PLATFORM.startswith('linux') or PLATFORM.startswith('darwin'):
        pattern = re.compile(".*/assets/(.*)")
    elif PLATFORM.startswith('win32'):
        pattern = re.compile(".*\\assets\\(.*)")

    for i, file in enumerate(files_left):
        files_left[i] = pattern.sub("\g<1>", file)
    for i, file in enumerate(files_right):
        files_right[i] = pattern.sub("\g<1>", file)

    missing_files = [x for x in files_left if x not in files_right]

    print('\nMissing files:')
    for file in sorted(missing_files):
        print("  {}".format(file))
    print('\nTotal missing files:', len(missing_files))


if __name__ == '__main__':

    while True:
        print('\n[1] Compare a resource pack against a minecraft version')
        print('[2] Compare a minecraft version against another minecraft version')
        print('Choice: ', end='')

        choice_logic = input()

        if choice_logic == '1':
            compare(choose_rp(), choose_mcv())
            break
        elif choice_logic == '2':
            compare(choose_mcv(low=True), choose_mcv(low=False))
            break
        else:
            print('\nInvalid choice!')

    print('\nExit? (y/N)')
    while True:
        if input() == 'y' or input() == 'Y':
            exit(0)
