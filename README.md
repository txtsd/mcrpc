# Minecraft Resource Pack Comparator
MCRPC checks your resource pack against any version of Minecraft to show resources missing from your pack for that version. It can also compare any two versions of Minecraft and show missing (added) files.

## Installation and usage

Download the correct binary from the [latest release](https://github.com/txtsd/mcrpc/releases/latest) for your platform.
NOTE: Builds on sourcehut are not available yet

Double-click it, or launch it via your terminal of choice.
Follow the prompts, and you will get a list of missing resources.

## Development
Clone the repo
```
git clone https://git.sr.ht/~txtsd/mcrpc
```

You will need [Python](https://www.python.org/)

Install the dependencies:
```
pip install -r requirements.txt
```

Or if you have [pipenv](https://github.com/pypa/pipenv) (preferred), install the pipenv environment and dependencies:
```
pipenv install
pipenv shell
```

Finally, run
```
python mcrpc.py
```



## Screenshot
![screenshot](https://i.imgur.com/VyVocCv.png)
![screenshot](https://i.imgur.com/ONGGVN4.png)
