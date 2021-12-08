# Minecraft Resource Pack Comparator
MCRPC checks your resource pack against any version of Minecraft to show resources missing from your pack for that version.

## Installation and usage
Clone the repo
```
git clone https://github.com/txtsd/mcrpc.git
```

You will need [Python](https://www.python.org/) and [pipenv](https://github.com/pypa/pipenv)

Install the pipenv environment and dependencies:
```
pipenv install
pipenv shell
```

Or if you don't have/want pipenv
```
pip install -r requirements.txt
```
This will install requests.

Finally, run
```
python mcrpc.py
```

Follow the prompts, and you will get a list of missing resources.

##Screenshot
![screenshot](https://i.imgur.com/pc8xRpM.png)
