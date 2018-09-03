/*
 * Module.h
 *
 *  Created on: Jan 5, 2018
 *      Author: Jordan Richards
 */

#ifndef TRAINER_MODULE_H_
#define TRAINER_MODULE_H_

#include "../tools/tools.h"

#include <map>
#include <set>

namespace xtrainer{

//Specifies the size of a numerical property
enum class Size {
	_8BIT = 1,
	_16BIT = 2,
	_32BIT = 4,
	_64BIT = 8
};

enum : int{
	FLOATING_POINT = 0x1,
	SIGNED = 0x2
};


extern void reverse(Size s, void* in, void* out);

//TODO: add support for string properties
//TODO: add support for PropertyArrays with syntax " memoryManger["module"]["array"][0] "

//A converter that can convert a numerical property to the format it is stored in the foreign process and the reverse
struct Conversion{
	void (*from) (Size,void*,void*) = nullptr;
	void (*to) (Size,void*,void*) = nullptr;

	Conversion(void (*from) (Size,void*,void*), void (*to) (Size,void*,void*));

	inline void operator () (Size s, void* in, void* out, bool reverse = false){
		reverse ? from(s,in,out) : to(s,in,out);
	}
};

//Built in Conversion function for reversing Endianness (convenient for dealing with emulators)
const Conversion REVERSE = Conversion(reverse,reverse);

//Built in Conversion that does nothing (i.e. the property will store the number in the same way it was read)
const Conversion NULL_CONVERTER = Conversion(0,0);

class Module;

//Properties allow us to retrieve variables from foreign process memory, supporting pointer resolution (using another property as an offset)
struct Property{
	//Address of a location in foreign process memory
	//if reference is supplied, it is offset by the value of the referenced property
	struct Address{
		Address(size_t offset = 0, Property* reference = 0): offset(offset), reference(reference) {};
		size_t offset;
		Property* reference;
	};
	//TODO: change one of the names of the two classes named Address, or make them compatible

friend class Module;
	Property(): converter(NULL_CONVERTER) {}
	Property(const char* name, Property::Address addr, Size s, int typeFlags = 0, const Conversion& converter = NULL_CONVERTER);
	void init(Module* parent);//post initialization (to prevent the address of the parent from being wrong after it's creation)
	//TODO: implement automatic post-init using an array of Properties to initialize (to avoid chance of user error)

	bool isFloatingPoint() const;
	bool isSigned() const;
	address_t getAddr() const;
	int getSize() const;
	Property* dependency() const;//returns the dependency of the property (or nullptr if it exists)

	//set the property to some other value
	template <typename T>
	Property& operator =(T&& t){
		if(isFP){
			data.fpValue = t;
		}else{
			if(t < 0){
				sign = 1;
				data.intValue = -t;
			}else{
				sign = 0;
				data.intValue = t;
			}
		}
		if(parent != nullptr)write();
		return *this;
	}

	//convert the property to a usable type
	template <typename T>
	inline operator T(){
		return T(isFP ? data.fpValue : data.intValue);
	}

	void update();

private:
	void write();

	Module* parent = nullptr;

	Property::Address addr;
	Size s = Size::_64BIT;
	const char* name;

	//A union of a double and integer is used to support both values depending on the variable "isFP" (terrible hack)
	union{
		double fpValue;
		size_t intValue;
	} data;
	bool sign = 0;

	bool isFP = false;
	bool isSgned = false;

	Conversion converter;
};

class MemoryManager;

//Modules contain and manage properties
class Module {
friend struct Property;
public:
	Module() {};

	//Read and write properties in memory (provided the byte array contents of the Property)
	void write(Property* prop, char* str);
	void read(Property* prop, char* str);

	//retrieve a property by its name
	Property& operator[] (std::string name){
		return *props[name];
	}

	virtual void init() {};//overload this init function to post-init

	//inserts properties into a set for use with MemoryManger::updateModules()
	void getProperties(std::insert_iterator<std::set<Property*>> it);

protected:
	Module(MemoryManager* parent);

	void attachProperty(const char* name, Property* prop);

	std::map<std::string, Property*> props;

	virtual ~Module() {};

private:
	MemoryManager* parent = 0;
};

}//namespace xtrainer

#endif /* TRAINER_MODULE_H_ */
