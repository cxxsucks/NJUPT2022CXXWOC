#ifndef JSON_H
#define JSON_H

#include <list>
#include <string>
#include <utility>

struct jsonUnit;

typedef enum { FIELD, ARRAY, OBJECT } jsonType;

struct jsonField {
	enum {
		STRING, INTEGER, REAL, BOOLEAN
	} type;
    union DATA{
		std::string string;
		long long integer;
		long double real;
		bool boolean;

        DATA() : string(NULL) {}
        DATA(std::string str) : string(str) {}
        ~DATA(){}
	} data;

    jsonField(std::string str) : type(STRING), data(str) { }

	jsonField(long long _integer) : type(INTEGER) {
		data.integer = _integer;
	}

	jsonField(long double _real) : type(REAL) {
		data.real = _real;
	}

	jsonField(bool _boolean) : type(BOOLEAN) {
		data.boolean = _boolean;
	}
};

typedef std::pair<std::string, jsonUnit*> jsonMember;

typedef std::pair<std::string, std::list<jsonMember*>> jsonObject;

typedef std::pair<std::string, std::list<jsonUnit*>> jsonArray;

struct jsonUnit{
	jsonType type;
	union DATA{
		jsonField* field;
		jsonArray* array;
		jsonObject* object;

		DATA() : object(nullptr) {}
	} data;

	jsonUnit(jsonType _type) : type(_type), data() {}
};

const jsonObject& read(std::string path);

#endif
