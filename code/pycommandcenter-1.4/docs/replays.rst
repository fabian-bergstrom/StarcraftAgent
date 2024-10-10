.. _replays:

Replays
=======
This page will describe two different techniques for handling replays. The
first technique parses the information saved in the replay file. The second
uses the information in the replay file to actually replay the game. The
different techniques have certain advantages and disadvantages. The main
one is that the reader is a lot faster while replaying the game provides
a lot more information.



SC2Reader
---------
SC2Reader_ is a library that can be used to implement the first technique.

Here is a short example that uses SC2Reader to extract the build order.


.. _SC2Reader: https://github.com/ggtracker/sc2reader

.. code-block:: python

    import sc2reader
    import json
    from sc2reader.factories import SC2Factory

    # The path to the replays
    PATH = "../Replays"

    def main():
        # Creates a generator of all the replays in the dictionary
        replays = sc2reader.load_replays(PATH, load_level=3)
        games = []
        # Loops through every relay
        for replay in replays:
            building_order = []
            if is_terran_vs_terran(replay):
                # Loops through every event
                for event in replay.events:
                    # Check if the event is that a building is crated
                    if type(event) == sc2reader.events.tracker.UnitInitEvent:
                        if event.unit.name not in building_order:
                            building_order.append(event.unit.name)
                games.append(building_order)
        return games


    def is_terran_vs_terran(replay):
        try:
            return sc2reader.constants.LOCALIZED_RACES[replay.players[0].play_race] == "Terran" \
               and sc2reader.constants.LOCALIZED_RACES[replay.players[1].play_race] == "Terran"
        except(KeyError):
            return False

    if __name__ == '__main__':
        games = main()
        text = json.dumps(games)
        print("Res: " + text)


SC2Reader has good documation_ that also describe what information
could be found. `What is in a Replay`_ and Events_ gives a good idea
if the information you want could be collected with SC2Reader.

.. _documation: https://sc2reader.readthedocs.io/en/latest/index.html
.. _What is in a Replay: https://sc2reader.readthedocs.io/en/latest/articles/whatsinareplay.html
.. _Events: https://sc2reader.readthedocs.io/en/latest/events/index.html

ReplayObserver
--------------
This is the second technique it is much like using a bot but with the difference
that no action can be preform just observations.


.. code-block:: python

    from library import *


    class MyObserver(ReplayObserver):
        def __init__(self):
            ReplayObserver.__init__(self)

        def on_game_start(self):
            ReplayObserver.on_game_start(self)

        def on_step(self):
            ReplayObserver.on_step(self)


    def main():

        coordinator = Coordinator(r"D:/StarCraft II/Versions/Base69232/SC2_x64.exe")
        if coordinator.load_replay_list("D:/Replays/"):
            observer = MyObserver()
            coordinator.add_replay_observer(observer)
            while coordinator.update():
                pass
        else:
            print("No replays found")


    if __name__ == "__main__":
        main()

.. toctree::