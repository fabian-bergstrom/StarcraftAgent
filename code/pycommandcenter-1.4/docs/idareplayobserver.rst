IDAReplayObserver
=================

.. class:: library.IDAReplayObserver

   This is a class for following a replay.

   Inherited methods:

   .. method:: IDAReplayObserver.on_game_start(self)

      This method is called when a replay has started, when you inherit it you have to
      call the parent's on_game_start method in order to make it work (see
      :ref:`replays`).

   .. method:: IDAReplayObserver.on_step(self)

      This method is called on every tick of the replay, when you inherit it you
      have to call the parent's on_step method in order to make it work (see
      :ref:`replays`).

   .. method:: IDAReplayObserver.on_game_end(self)

      This method is called when the replay has ended, when you inherit it you have to
      call the parent's on_game_start method in order to make it work (see
      :ref:`replays`).

   .. method:: IDAReplayObserver.on_unit_created(self)

      This method is called when the an unit is created, that includes when an unit leaves a refinery. This only works if replay perspective is not set to 0.

   .. method:: IDAReplayObserver.on_unit_destroyed(self)

      This unit is called when a unit is destroyed.

   Methods:

   .. method:: IDAReplayObserver.get_all_units(self) -> List[library.ReplayUnit]

      Retrieves a list of all visible units

   .. method:: IDAReplayObserver.get_player_race(self, player_id) -> library.Race

      Returns the players race



.. toctree::