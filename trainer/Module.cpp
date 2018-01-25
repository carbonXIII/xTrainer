/*
 * Module.cpp
 *
 *  Created on: Jan 5, 2018
 *      Author: shado
 */

#include "Module.h"
#include "MemoryManager.h"

#include <unordered_set>
#include <memory>

void reverse(Size s, void* in, void* out){
	int n = (int)s;
	char* _in = (char*)in; char* _out = (char*)out;
	for(int i = 0; i < n; i++){
		_out[i] = _in[n-i-1];
	}
}

Conversion::Conversion(void (*from) (Size,void*,void*), void (*to) (Size,void*,void*)): from(from), to(to) {}

Property::Property(const char* name, Property::Address addr, Size s, int typeFlags, const Conversion& converter):
	isFP(typeFlags & FLOATING_POINT), isSgned(typeFlags & SIGNED),
	addr(addr), converter(converter), s(s), name(name)
{
	if(isFP && s < Size::_32BIT){//floats must be at least 32 bit
		s = Size::_32BIT;
	}
}

void Property::init(Module* parent){
	this->parent = parent;
	parent->attachProperty(name, this);
}

bool Property::isFloatingPoint() const{
	return isFP;
}

bool Property::isSigned() const{
	return isSgned;
}

size_t Property::getAddr() const{
	return (addr.reference != 0) ? (size_t)(*addr.reference) + addr.offset : addr.offset;
}

int Property::getSize() const{
	return (int)s;
}

Property* Property::dependency() const{
	return addr.reference;
}

void Property::write(){
	char* out = new char[s];
	if(isFP){
		if(s == Size::_32BIT){
			float* o = (float*)out;
			*o = data.fpValue;
		}else{
			double* o = (double*)out;
			*o = data.fpValue;
		}
	}else{
		if(!isSgned){
			if(s == Size::_8BIT){
				uint8_t* o = (uint8_t*)out;
				*o = data.intValue;
			}else if(s == Size::_16BIT){
				uint16_t* o = (uint16_t*)out;
				*o = data.intValue;
			}else if(s == Size::_32BIT){
				uint32_t* o = (uint32_t*)out;
				*o = data.intValue;
			}else{
				uint64_t* o = (uint64_t*)out;
				*o = data.intValue;
			}
		}else{
			if(s == Size::_8BIT){
				int8_t* o = (int8_t*)out;
				*o = (sign * 2 - 1) * data.intValue;
			}else if(s == Size::_16BIT){
				int16_t* o = (int16_t*)out;
				*o = (sign * 2 - 1) * data.intValue;
			}else if(s == Size::_32BIT){
				int32_t* o = (int32_t*)out;
				*o = (sign * 2 - 1) * data.intValue;
			}else{
				int64_t* o = (int64_t*)out;
				*o = (sign * 2 - 1) * data.intValue;
			}
		}
	}

	if(converter.to != nullptr){
		char* out2 = new char[s];
		converter(s,out,out2);
		delete [] out;
		out = out2;
	}

	parent->write(this, out);
	delete [] out;
}

void Property::update(){
	if(parent == 0)return;
	char* val = new char[s];
	parent->read(this,val);
	if(val == 0)return;

	if(converter.from != nullptr){
		char* val2 = new char[s];
		converter(s,val,val2,1);
		delete [] val;
		val = val2;
	}

	if(isFP){
		if(s == Size::_32BIT){
			float* o = (float*)val;
			data.fpValue = *o;
		}else{
			double* o = (double*)val;
			data.fpValue = *o;
		}
	}else{
		if(!isSgned){
			if(s == Size::_8BIT){
				uint8_t* o = (uint8_t*)val;
				data.intValue = *o;
			}else if(s == Size::_16BIT){
				uint16_t* o = (uint16_t*)val;
				data.intValue = *o;
			}else if(s == Size::_32BIT){
				uint32_t* o = (uint32_t*)val;
				data.intValue = *o;
			}else{
				uint64_t* o = (uint64_t*)val;
				data.intValue = *o;
			}
		}else{
			if(s == Size::_8BIT){
				int8_t* o = (int8_t*)val;
				if(*o < 0){
					sign = 1;
					data.intValue = -*o;
				}else{
					sign = 0;
					data.intValue = *o;
				}
			}else if(s == Size::_16BIT){
				int16_t* o = (int16_t*)val;
				if(*o < 0){
					sign = 1;
					data.intValue = -*o;
				}else{
					sign = 0;
					data.intValue = *o;
				}
			}else if(s == Size::_32BIT){
				int32_t* o = (int32_t*)val;
				if(*o < 0){
					sign = 1;
					data.intValue = -*o;
				}else{
					sign = 0;
					data.intValue = *o;
				}
			}else{
				int64_t* o = (int64_t*)val;
				if(*o < 0){
					sign = 1;
					data.intValue = *o;
				}else{
					sign = 0;
					data.intValue = *o;
				}
			}
		}
	}

	delete [] val;
}

void Module::getProperties(std::insert_iterator<std::set<Property*>> iter){
	for(auto it = props.begin(); it != props.end(); it++){
		iter = it->second;
	}
}

void Module::attachProperty(const char* name, Property* prop){
	props[std::string(name)] = prop;
}

void Module::write(Property* prop, char* str){
	parent->write(prop->getAddr(), str, prop->getSize());
}

void Module::read(Property* prop, char* str){
	parent->read(prop->getAddr(), str, prop->getSize());
}

Module::Module(MemoryManager* parent): parent(parent) {}

