# PyCommandCenter: Library for making bots for Starcraft II

PyCommandCenter is a slimmed down version of the popular Starcraft AI 
[CommandCenter](https://github.com/davechurchill/commandcenter) connected to 
Python.

Key differences:

* Library made for use with Python, made possible by binding existing C++ code 
  to Python using [pybind11](https://github.com/pybind/pybind11)
* No decision-making, only perception
* Improved TechTree, by importing data from the JSON files provided by 
  [sc2-gamedata](https://github.com/noorus/sc2-gamedata)
* Everything is built using cmake, allowing for one Visual studio project to 
  build PyCommandCenter together with all its dependencies

[Download page for Windows](https://gitlab.liu.se/starcraft-ai-course/pycommandcenter/-/tags)

[Wiki page describing how to use it](http://starcraft-ai-course.gitlab-pages.liu.se/pycommandcenter/index.html)

# Code example for making a bot

``` python
import os

from library import *


class MyAgent(IDABot):
    def __init__(self):
        IDABot.__init__(self)

    def on_game_start(self):
        IDABot.on_game_start(self)

    def on_step(self):
        IDABot.on_step(self)


def main():
    coordinator = Coordinator()
    bot1 = MyAgent()

    participant_1 = create_participants(Race.Terran, bot1)
    participant_2 = create_computer(Race.Random, Difficulty.Easy)

    coordinator.set_participants([participant_1, participant_2])

    coordinator.launch_starcraft()

    path = os.path.join(os.getcwd(), "maps", "InterloperTest.SC2Map")
    coordinator.start_game(path)

    while coordinator.update():
        pass


if __name__ == "__main__":
    main()
```

# How to build (Windows)

First you need to make sure you got all the build dependencies:

* cmake
* Visual Studio 2017 or later (earlier might work, but untested)
* git
* python 3.7 or later (earlier might work, but untested)

If you ever want to make any changes to the library, e.g. adding new features or making bug fixes, it's easier to fork the project before cloning the repository. For instructions on how to fork, see [the gitlab documentation](https://gitlab.liu.se/help/gitlab-basics/fork-project.md). 

Now, you are ready to build the python library:

1. Open up a terminal, download the source code using the command: `git clone --recurse-submodules https://gitlab.liu.se/course-starcraft-ai/pycommandcenter.git`
2. Next, open the repository in your file viewer and run the batch script 
   called `create-visual-studio-solution.bat` in order to use cmake to create
   a Visual studio solution
3. Open the Visual Studio solution located in the newly created directory 
   `build/`
4. The project called `library` should be selected as the default StartUp 
   project in the Solution Explorer (on the right side by default)
5. Change settings to Release and x64
6. Click "Local Windows Debugger" and it will start compiling
7. Visual Studio will open an error message telling you it cannot open the 
   resulting library file, which means it **successfully** created the library 
   file. The file will be located at `build\python-api-src\Release` and its 
   name will depend on the python version used.

# How to build (Linux, untested)

Same dependencies applies as for Windows, although you don't need Visual
Studio.

1. Open up a terminal, download the source code using the command: `git clone --recurse-submodules https://gitlab.liu.se/course-starcraft-ai/pycommandcenter.git`
2. Next, enter the directory and run the command `mkdir build` followed by `cd
   build` and `cmake ..` in order to create the makefiles needed for building the
   library.
3. Run `make` to build the project (use `make -j N` if you want to use N
   threads)

# How to use the library with PyCharm

See [separate page](pycharm.md).

# Building the documentation

1. Build the library binary as described above, the documentation uses the
   binary to automate some parts
2. Install [Sphinx](http://www.sphinx-doc.org)
3. Go to the `docs` subfolder,
4. If you are **not** running Visual Studio and building in Release mode you
   have to change row 17 of [conf.py](docs/conf.py) to match the location
   of the resulting `pyd` file
5. Build the documentation:
   * If you are using Windows, open a terminal, navigate to the docs/ folder
     and run the command `make.bat html`.
   * If you are using Linux, open a terminal, navigate to the docs/ folder and
     run the command `make html`

# Credits

CommandCenter is written by [David Churchill](http://www.cs.mun.ca/~dchurchill/), Assistant Professor of [Computer Science](https://www.cs.mun.ca/) at Memorial University, and organizer of the [AIIDE StarCraft AI
Competition](http://www.cs.mun.ca/~dchurchill/starcraftaicomp/).

CommandCenter is in turn based on the Blizzard's [StarCraft II AI 
API](https://github.com/Blizzard/s2client-api) and the architecture of 
[UAlbertaBot](https://github.com/davechurchill/ualbertabot/wiki).
