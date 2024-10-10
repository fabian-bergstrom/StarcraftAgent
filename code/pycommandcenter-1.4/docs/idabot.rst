IDABot
======

.. class:: library.IDABot

   This is the basis of your bot. It contains all available managers and some
   methods to get you started.

   See :ref:`this page <gettingstarted>` for how to properly
   inherit from IDABot.

   Instances of managers:

   .. autoattribute:: base_location_manager
   .. autoattribute:: tech_tree
   .. autoattribute:: map_tools
   .. autoattribute:: building_placer

   Inherited methods:

   .. method:: IDABot.on_game_start(self)

      This method when Starcraft has stared, when you inherit it you have to
      call the parent's on_game_start method in order to make it work (see
      :ref:`gettingstarted`).

   .. method:: IDABot.on_step(self)

      This method is run on every tick of the game, when you inherit it you
      have to call the parent's on_step method in order to make it work (see
      :ref:`gettingstarted`).

   Methods:

   .. automethod:: get_all_units
   .. automethod:: get_my_units
   .. automethod:: get_player_race
   .. automethod:: send_chat
   .. automethod:: has_creep
   .. automethod:: move_camera

   .. method:: IDABot.ability_for_upgrade(self, UpgradeID)

      Ability that researches this upgrade

      Example: If we send in UPGRADE_ID.BANSHEECLOAK, it will return
      ABILITY_ID.RESEARCH_BANSHEECLOAKINGFIELD. In this case,
      the unit of scv has the ability for this. You can then use the
      unit.ability(AbilityID) on the scv for creating the upgrade.

   .. automethod:: upgrade_mineral_cost
   .. automethod:: upgrade_gas_cost
   .. automethod:: upgrade_research_time
   .. automethod:: effect_radius

   Attributes:

   .. autoattribute:: minerals
   .. autoattribute:: gas
   .. autoattribute:: current_supply
   .. autoattribute:: max_supply
   .. autoattribute:: current_frame
   .. autoattribute:: start_location
   ,, autoattribute:: start_locations

Debug
-----
.. class:: library.IDABot

   When developing AI-methods or when simply having a problem.
   The debug-methods are a great tool for speeding up the process.

   .. automethod:: debug_create_unit
   .. automethod:: debug_kill_unit
   .. automethod:: IDABot.debug_show_map(self)
   .. automethod:: IDABot.debug_fast_build(self)
   .. automethod:: debug_enemy_control
   .. automethod:: debug_fast_build
   .. automethod:: debug_ignore_food
   .. automethod:: debug_ignore_resource_cost
   .. automethod:: debug_give_all_resources
   .. automethod:: debug_god_mode
   .. automethod:: debug_ignore_mineral
   .. automethod:: debug_no_cooldowns
   .. automethod:: debug_give_all_tech
   .. automethod:: debug_give_all_upgrades
   .. automethod:: debug_set_score
   .. automethod:: debug_end_game

   .. method:: IDABot.debug_set_energy(self, Float, Unit)

      Set the amount (Float) of energy to the :class:`library.Unit`
      The maximum depends on the unit maxenergy.
      Note: This is not in percent of the unit energy.
      Same goes for life and shields.

   .. automethod:: debug_set_life

   .. automethod:: debug_set_shields


.. toctree::