Constants
=========

See also UNIT_TYPEID, UPGRADE_ID and ABILITY_ID for more constants.

.. _playerconstants:

Player constants
----------------

The following constants are used when referring to a player:

.. autoclass:: library.PLAYER_SELF
.. autoclass:: library.PLAYER_ENEMY
.. autoclass:: library.PLAYER_NEUTRAL
.. autoclass:: library.PLAYER_ALLY

These are internally represented as integers, but these constants should be 
used instead to avoid confusion.

Difficulty
----------

.. autoclass:: library.Difficulty
   :members:
   :undoc-members:

AIBuild
-------

.. autoclass:: library.AIBuild
   :members:
   :undoc-members:

Race
----

.. class:: library.Race

   The following three values represent actual races:
   
   .. attribute:: Race.Protoss
   .. attribute:: Race.Terran
   .. attribute:: Race.Zerg

   The following attribute means a randomly selected race from the three above:

   .. attribute:: Race.Random


.. toctree::