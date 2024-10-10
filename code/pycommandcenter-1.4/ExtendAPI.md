# Extending the API
This is a guide for extending the API and adding more functionality to the courses TDDE25 and TDDD92.

If a function already exist but you can't use it in python, check and see if it's exist in the library files (pybind).

1. Create a declaration of your function in the header file.
2. Create a definition of your function in the source file.
3. Depending on which object you decided to extend, you should also add this function to the library file. The library is the pybind, making it possible to use your function in python.
4. Update the documentation. There are instructions on how you build and test the documentation.


Example:
We want to add function to unit, returning a bool depending on if it's holding a mineral. We have discovered a function in the SC2 API containing this information (sc2_client.ccp). This is a helper function, it doesn't belong to any object.

1. In unit.h: ```bool isCarryingMinerals() const;```
2. In unit.cpp: ```bool Unit::isCarryingMinerals() const
{
	return sc2::IsCarryingMinerals(*m_unit);
}```
We can access m_unit in the unit file and with sc2::Function() we can access any function in the blizzard API that doesn't belong to any object. The same goes for any object, for example sc2::Point3D makes us able to access the Point3D object.
3. In lib_unit.cpp: ```.def_property_readonly("is_carrying_minerals", &Unit::isCarryingMinerals)```
4. In the folder named docs we update the documentation. In this case, we update the file unit.rst.

Common problems:
1. The return in python is a memory address:
    Make sure that it returns a correct type.
2. The compiler complains about files I have not even touched:
   Make sure that the startUp is library and you have release x64. If you just added a new function in pybind, check it.
