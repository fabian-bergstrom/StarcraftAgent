.. _contributing:

Contributors guide
==================

The goal of this page is to give you some guidance regarding how to make changes to the library and its documentation.

Here's a brief overview of the process:

1. Fork the repository
2. Make changes
3. Document the new features / changes you have made
4. Commit and push to your fork
5. Create a Merge request
6. Read the comments on your merge request and make the necessary changes

The rest of the document gives some more details on the individual steps.

If anything is unclear, don't hesitate to contact any lab assistant or create an issue describing the problem.

Forking the repository
----------------------

The code is hosted on `LiU's gitlab <https://gitlab.liu.se/starcraft-ai-course/pycommandcenter/>`_. For instructions on how to fork a repository see `Gitlab's documentation <https://gitlab.liu.se/help/gitlab-basics/fork-project.md>`_.

This will result in your own copy of the repository, where you can make changes. This copy can be used as any other repository on gitlab, but has the additional feature of creating merge requests back to the original repository.

Making changes
--------------

This is done just as any other project. Please try to use the same code standard. Avoid making changes which breaks other student's code.

All code related to binding the C++ code to Python can be found in the subdirectory *python-api-src*. The project uses pybind11 to make the bindings from C++ to Python, see `their extensive documentation <https://pybind11.readthedocs.io/en/stable/>`_ on how to use it.

The code is heavily based on the original `CommandCenter <https://github.com/davechurchill/commandcenter>`_, all code which originally comes from this library lies in the subdirectory *src*.

The source code for all additional dependencies (including pybind11) are registerd as git submodules in the subdirectory *lib*.

The system is built using cmake, see the file `CMakeLists.txt <https://gitlab.liu.se/starcraft-ai-course/pycommandcenter/blob/master/CMakeLists.txt>`_ in the project root.

If a function already exist but you can't use it in python, check and see if it's exist in the library files (pybind).

#. Create a declaration of your function in the header file.
#. Create a definition of your function in the source file.
#. Depending on which object you decided to extend, you should also add this function to the library file. The library is the pybind, making it possible to use your function in python.
#. Update the documentation. There are instructions on how you build and test the documentation.

Example:
We want to add function to unit, returning a bool depending on if it's holding a mineral. We have discovered a function in the SC2 API containing this information (sc2_client.ccp). This is a helper function, it doesn't belong to any object.

#. In unit.h: bool isCarryingMinerals() const;
#. In unit.cpp: bool Unit::isCarryingMinerals() const { return sc2::IsCarryingMinerals(\*m_unit); }
#. We can access m_unit in the unit file and with sc2::Function() we can access any function in the blizzard API that doesn't belong to any object. The same goes for any object, for example sc2::Point3D makes us able to access the Point3D object.
#. In lib_unit.cpp: .def_property_readonly("is_carrying_minerals", &Unit::isCarryingMinerals)
#. In the folder named docs we update the documentation. In this case, we update the file unit.rst.

Common problems:

#. The return in python is a memory address: Make sure that it returns a correct type.
#. The compiler complains about files I have not even touched: Make sure that the startUp is library and you have release x64. If you just added a new function in pybind, check it.
#. The bot crashes whit a exit code -1073741819 (0xC0000005): Make sure that you dont read null values.

Document the new features / changes you have made
-------------------------------------------------

The documentation can be found in the *docs* subdirectory. The documentation is done using Sphinx which auto-generates some parts of the documentation based on the generated Python library. For steps on how to build the documentation see the `README file <https://gitlab.liu.se/starcraft-ai-course/pycommandcenter/blob/master/README.md>`_ in the project root.

The documentation is written in reStructuredText, a quick-guide can be found `here <http://docutils.sourceforge.net/docs/user/rst/quickref.html>`_. 

Note that documenting changes you make to the library is required to have them merged with the main repository.

Create a Merge request
----------------------

A merge request can be created by going into your fork's project page, selecting *Merge Requests* on the left-hand side and pressing the *New merge request* button.

The source branch should be branch which you have made your changes on and the target branch should be the *master* branch of *course-starcraft/pycommandcenter*.

Read the comments on your merge request and make the necessary changes
----------------------------------------------------------------------

In this step someone will review your changes and make comments. These comments might ask you to make changes to the code or documentation. These changes must be done before the merge request is accepted. Therefore it is important to make merge requests several days before eventual deadlines.

Once the merge request has been accepted the code is now part of PyCommandCenter. Congratulations and thank you for your contribution to the project!

.. toctree::