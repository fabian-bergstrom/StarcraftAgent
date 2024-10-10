ReplayUnit
==========

.. class:: library.ReplayUnit

   An instance of the class Unit represents one unit in a replay. A ReplayUnit is a
   :class:`library.Unit` white some limitations.

   It is possible to use ReplayUnit as keys in a dictionary, which might be helpful
   for bookkeeping.

   Properties:

   .. autoattribute:: buffs

      Returns a list of BuffID

   .. autoattribute:: build_percentage
   .. autoattribute:: energy
   .. autoattribute:: facing

      Returns the direction the unit is facing

   .. autoattribute:: hit_points
   .. autoattribute:: max_hit_points
   .. autoattribute:: id
   .. autoattribute:: is_alive
   .. autoattribute:: is_blip

      Returns true if unit is a "blip" - a ping on the map.

   .. autoattribute:: is_being_constructed

      Returns build_progress > 0

   .. autoattribute:: is_burrowed
   .. autoattribute:: is_cloaked
   .. autoattribute:: is_completed

      Returns build_progress >= 1

   .. autoattribute:: is_flying
   .. autoattribute:: is_idle
   .. autoattribute:: is_powered
   .. autoattribute:: is_training
   .. autoattribute:: is_valid
   .. attribute:: ReplayUnit.player

      Returns the constant corresponding to player which this unit belongs to.
      See :ref:`playerconstants` for more information.

   .. autoattribute:: position
   .. autoattribute:: current_ability_id

   .. autoattribute:: progress

      Returns the progress of currently used ability (-1 if not using ability)

   .. autoattribute:: radius

      Retruns the radius of the unit

   .. autoattribute:: shields
   .. autoattribute:: tile_position
   .. autoattribute:: unit_type

      Returns the :class:`library.UnitType` of the unit

   .. autoattribute:: weapon_cooldown
   .. autoattribute:: is_carrying_minerals

      Returns if this unit is currently holding minerals


.. toctree::