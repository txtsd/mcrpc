import glob
import hashlib
import json
import os
import shutil
import sys
from distutils.dir_util import copy_tree
from pprint import pprint
from zipfile import ZipFile

import requests

if sys.platform.startswith('linux'):
    PATH_DOTMC = os.path.join(os.path.expanduser('~'), '.minecraft')
elif sys.platform.startswith('darwin'):
    PATH_DOTMC = os.path.join(os.path.expanduser('~'), 'Library', 'Application Support', 'minecraft')
elif sys.platform.startswith('win32'):
    PATH_DOTMC = os.path.join(os.path.expandvars(r'%APPDATA%'), '.minecraft')

PATH_RP_DIR = os.path.join(PATH_DOTMC, 'resourcepacks')
PATH_CACHE = '.cache'
PATH_TARGET = os.path.join(PATH_CACHE, 'target')
PATH_DEFAULT = os.path.join(PATH_CACHE, 'default')
PATH_CLIENT = os.path.join(PATH_CACHE, 'client.jar')
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


def choose_mcv():
    json_vm = SESSION.get(LINK_VM)
    json_vm.raise_for_status()
    json_vm = json_vm.json()

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


def setup_compare_folders(path_rp, rp):
    if os.path.exists(PATH_TARGET):
        shutil.rmtree(PATH_TARGET)
    if os.path.exists(PATH_DEFAULT):
        shutil.rmtree(PATH_DEFAULT)

    if path_rp[-4:] == ".zip":
        with ZipFile(path_rp) as zip1:
            for file in zip1.namelist():
                if file.startswith('assets/'):
                    zip1.extract(file, path=PATH_TARGET)
    else:
        copy_tree(os.path.join(path_rp, 'assets'), os.path.join(PATH_TARGET, 'assets'))

    with ZipFile(PATH_CLIENT) as zip2:
        for file in zip2.namelist():
            if file.startswith('assets/'):
                zip2.extract(file, path=PATH_DEFAULT)


def compare(rp, mcv):
    path_rp = os.path.join(PATH_RP_DIR, rp)

    json_version = SESSION.get(mcv['url'])
    json_version.raise_for_status()
    json_version = json_version.json()

    link_client = json_version['downloads']['client']['url']
    sha_client = json_version['downloads']['client']['sha1']
    sha1 = hashlib.sha1()
    sha_calculated = None

    if not os.path.exists(PATH_CACHE):
        os.makedirs(PATH_CACHE)

    if os.path.exists(PATH_CLIENT):
        with open(PATH_CLIENT, 'rb') as f:
            while True:
                data = f.read(BUF_SIZE)
                if not data:
                    break
                sha1.update(data)
        sha_calculated = sha1.hexdigest()

    if not sha_calculated == sha_client:
        with open(PATH_CLIENT, 'wb') as f:
            print('Downloading client.jar…')
            download = SESSION.get(link_client)
            f.write(download.content)

    setup_compare_folders(path_rp, rp)

    dir_left = os.path.join(PATH_DEFAULT, '**', '*.*')
    dir_right = os.path.join(PATH_TARGET, '**', '*.*')

    files_left = glob.glob("{}".format(dir_left), recursive=True)
    files_right = glob.glob("{}".format(dir_right), recursive=True)
    missing_files = [x for x in files_left if x not in files_right]

    print('\nMissing files:')
    for file in sorted(missing_files):
        print("  {}".format(file.replace('.cache/default/assets/', '')))


if __name__ == '__main__':
    compare(choose_rp(), choose_mcv())
