/*
 * Module.h
 *
 *  Created on: Jan 5, 2018
 *      Author: shado
 */

#ifndef TRAINER_MODULE_H_
#define TRAINER_MODULE_H_

#include "../tools/tools.h"

#include <map>
#include <set>

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

struct Conversion{
	void (*from) (Size,void*,void*) = nullptr;
	void (*to) (Size,void*,void*) = nullptr;

	Conversion(void (*from) (Size,void*,void*), void (*to) (Size,void*,void*));

	inline void operator () (Size s, void* in, void* out, bool reverse = false){
		reverse ? from(s,in,out) : to(s,in,out);
	}
};

const Conversion REVERSE = Conversion(reverse,reverse);
const Conversion NULL_CONVERTER = Conversion(0,0);

class Module;
struct Property{
	struct Address{
		Address(size_t offset = 0, Property* reference = 0): offset(offset), reference(reference) {};
		size_t offset;
		Property* reference;
	};

friend class Module;
	Property(): converter(NULL_CONVERTER) {}
	Property(const char* name, Property::Address addr, Size s, int typeFlags = 0, const Conversion& converter = NULL_CONVERTER);
	void init(Module* parent);

	bool isFloatingPoint() const;
	bool isSigned() const;
	size_t getAddr() const;
	int getSize() const;
	Property* dependency() const;

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
class Module {
friend class Memory;
friend struct Property;
public:
	Module() {};

	void write(Property* prop, char* str);
	void read(Property* prop, char* str);

	Property& operator[] (std::string name){
		return *props[name];
	}

	virtual void init() {};
	void getProperties(std::insert_iterator<std::set<Property*>> it);

protected:
	Module(MemoryManager* parent);

	void attachProperty(const char* name, Property* prop);

	std::map<std::string, Property*> props;

	virtual ~Module() {};

private:
	MemoryManager* parent = 0;
};

#endif /* TRAINER_MODULE_H_ */
