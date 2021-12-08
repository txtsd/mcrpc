# Minecraft Resource Pack Comparator
MCRPC checks your resource pack against any version of Minecraft to show resources missing from your pack for that version.

## Installation and usage
Clone the repo
```
git clone https://github.com/txtsd/mcrpc.git
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

Follow the prompts, and you will get a list of missing resources.

## Screenshot
![screenshot](https://i.imgur.com/pc8xRpM.png)
