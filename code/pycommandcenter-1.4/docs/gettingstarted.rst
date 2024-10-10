.. _gettingstarted:

Getting started
===============

The goal of this page is to get you quickly started using the API. The 
following code block is the bare minimum you need to start Starcraft II and run 
your own bot.

An entire AI template project is available here_, it contains the code below
and more.

.. _here: https://gitlab.liu.se/starcraft-ai-course/sc2-python-bot

.. code-block:: python

   import os

   from typing import Optional
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
   
       coordinator.set_real_time(True)
       coordinator.set_participants([participant_1, participant_2])
   
       coordinator.launch_starcraft()
       path = os.path.join(os.getcwd(), "maps", "InterloperTest.SC2Map")
       coordinator.start_game(path)
   
       while coordinator.update():
           pass
   
   
   if __name__ == "__main__":
        main()

Now, let us break it down piece by piece to understand it.

.. code-block:: python

   from library import *

This imports everything from the library into your namespace. 

Next, we need to define our bot.

.. code-block:: python

   class MyAgent(IDABot):
       def __init__(self):
           IDABot.__init__(self)
   
       def on_game_start(self):
           IDABot.on_game_start(self)
   
       def on_step(self):
           IDABot.on_step(self)

A bot which plays Starcraft is defined as a subclass to the class :class:`library.IDABot` which
contains some help code in order to make implementing your bit more straightforward.

If we look closer at our newly created bot, it has three methods which are all 
run at different times. The method ``__init__`` is the constructor, which is 
called whenever the bot is first created.

The method ``on_game_start`` method is run when the game starts. Lastly, the 
method ``on_step`` is run on every time step of the game and thus where most of 
the decision making is going to be implemented.

Moving on, we have the code which sets up a game of Starcraft II:

.. code-block:: python

   def main():
       coordinator = Coordinator()
       bot1 = MyAgent()
   
       participant_1 = create_participants(Race.Terran, bot1)
       participant_2 = create_computer(Race.Random, Difficulty.Easy)
   
       coordinator.set_real_time(True)
       coordinator.set_participants([participant_1, participant_2])
       coordinator.launch_starcraft()

       path = os.path.join(os.getcwd(), "maps", "InterloperTest.SC2Map")
       coordinator.start_game(path)
   
       while coordinator.update():
           pass
   
   
   if __name__ == "__main__":
       main()

First we create a coordinator, this is the object we use to start Starcraft.
On the line after that the bot is constructed (the constructor is called).
The rest of the function sets a few settings and then starts Starcraft.

There are a few changes you might want to do here:

Remove or comment out the following line:

.. code-block:: python

   coordinator.set_real_time(True)

This line will make the game run at the same speed as humans play the game. 
However, your bot might will probably be able to play the game faster. If you 
remove or comment out this line the game will run as fast as possible, only 
waiting for your bot to return from `on_step`.

If you want to train your agent on certain type of bots. For example, If we
want the opponent to focus on air strategy. You can add the
following line to the bot.

.. code-block:: python

   participant_2 = create_computer(Race.Random, Difficulty.Easy, AIBuild.Air)

We can also play two bots against each other by changing the row:

.. code-block:: python

   participant_2 = create_computer(Race.Random, Difficulty.Easy)

to:

.. code-block:: python

   bot2 = SomeOtherBot()
   participant_2 = create_participant(Race.Terran, bot2)

where ``SomeOtherBot`` is a bot defined in the same way as ``MyAgent``.

.. toctree::