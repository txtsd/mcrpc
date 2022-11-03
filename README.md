# Minecraft Resource Pack Comparator

MCRPC checks your resource pack against any version of Minecraft to show resources missing from your pack for that version. It can also compare any two versions of Minecraft and show missing (added) files.

## Links

Project: <https://sr.ht/~txtsd/mcrpc/> <br>
Sources: <https://sr.ht/~txtsd/mcrpc/sources> <br>
Ticket Tracker: <https://todo.sr.ht/~txtsd/mcrpc> <br>
Mailing List: <https://lists.sr.ht/~txtsd/mcrpc> <br>

Mirrors: <br>
[Codeberg](https://codeberg.org/txtsd/mcrpc) <br>
[NotABug](https://notabug.org/txtsd/mcrpc) <br>
[GitLab](https://gitlab.com/txtsd/mcrpc) <br>
[GitHub](https://github.com/txtsd/mcrpc) <br>
[Bitbucket](https://bitbucket.org/txtsd/mcrpc) <br>

If sourcehut is not feasible, contribution is welcome from across mirrors.

## Installation and usage

Download a binary from the latest tag for your platform on [the refs page](https://git.sr.ht/~txtsd/mcrpc/refs).
Each tag has binaries attached.

NOTE: Release artifacts are built on GitHub due to cross-platform CI runner limitations on sourcehut.

Double-click it, or launch it via your terminal of choice.
Follow the prompts, and you will get a list of missing resources.

If you prefer running a python script over a binary, follow the development instructions instead.

## Development
Clone the repo

```
git clone https://git.sr.ht/~txtsd/mcrpc
cd mcrpc
```

You will need [`python`](https://www.python.org/) and [`python-poetry`](https://python-poetry.org/)

Install the dependencies into a virtual environment:

```
poetry install
```

Load the virtual environment created by `poetry`:

```
poetry shell
```

Finally, run

```
python mcrpc.py
```

## Screenshot
![screenshot](https://i.imgur.com/VyVocCv.png)
![screenshot](https://i.imgur.com/ONGGVN4.png)
