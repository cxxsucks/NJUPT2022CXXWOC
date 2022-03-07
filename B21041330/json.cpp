#include "json.h"

#include <algorithm>
#include <fstream>
#include <sstream>

#define dot '.'
#define eCapital 'E'
#define eLowercase 'e'
#define TRUE #true
#define FALSE #false

#define strSymbol '\"'
#define colon ':'
#define comma ','
#define objectFront '{'
#define objectBack '}'
#define arrayFront '['
#define arrayBack ']'

class jsonNode : public jsonUnit {
public:
	jsonNode* parent;

	jsonNode(jsonType _type) : jsonUnit(_type), parent(nullptr) {}
};

inline bool isvalid(char ch) {
	switch (ch) {
	case strSymbol:
	case colon:
	case comma:
	case objectFront:
	case objectBack:
	case arrayFront:
	case arrayBack:
		return true;
	}
	return false;
}

void eraseSpaces(std::string& str) {
	int num = 0;
	auto i = str.begin();
	while (isspace(*i++))
		num++;
	str.erase(0, num);
	num = 0;
	auto j = str.rend();
	while (isspace(*j++))
		num++;
	str.erase(num);
}

jsonField* recognize(std::string src) {
	if (src == "true") 
		return new jsonField(true);
	if (src == "false")
		return new jsonField(false);
	if (src.find(dot) != std::string::npos && src.find(eCapital) != std::string::npos && src.find(eLowercase) != std::string::npos) {
		std::istringstream stream(src);
		long double result;
		stream >> result;
		return new jsonField(result);
	}
	std::istringstream stream(src);
	long long result;
	stream >> result;
	return new jsonField(result);
}

jsonNode* link(jsonMember* member, jsonNode* parent) {
	parent->data.object->second.push_back(member);
	if (member->second->type != FIELD)
		parent = (jsonNode*)member->second;
	return parent;
}

jsonNode* link(jsonNode* child, jsonNode* parent) {
	if (!parent)
		return child;
	if (parent->type == OBJECT)
		return link(new jsonMember(NULL, child), parent);
	parent->data.array->second.push_back(child);
	if (child->type != FIELD)
		parent = child;
	return parent;
}

jsonNode* findNext(jsonNode* current) {
	jsonNode* parent = current->parent;
	if (!parent)
		return current;
	if (parent->type == OBJECT) {
		auto& container = parent->data.object->second;
		auto i = find_if(container.begin(), container.end(), [current](jsonMember* i) {return i->second == current; });
		if (++i != container.end())
			return (jsonNode*)(*i)->second;
		return nullptr;
	}
	auto& container = parent->data.array->second;
	auto i = find(container.begin(), container.end(), current);
	if (++i != container.end())
		return (jsonNode*)*i;
	return nullptr;
}

jsonNode* next(jsonNode* current) {
	jsonNode* result;
	while (!(result = findNext(current)))
		current = current->parent;
	return result;
}

const jsonObject& read(std::string path) {
	std::ifstream stream;
	int i = 0, j = 0;
	std::list<std::string> target;
	jsonNode* current = nullptr;
	std::string buffer;

	stream.open(path);
	target.push_back("");
	while (getline(stream, buffer)) 
		while(j < buffer.length()) {
			while (!isvalid(buffer[j]) && j < buffer.length())
				j++;
            target.push_back(buffer.substr(i, j - 1));
			eraseSpaces(target.back());
			target.emplace_back(1, buffer[j]);
			i = ++j;
		}
	
	for (auto i = target.begin(); i != target.end(); i++) {
		while (i++->empty())
			;
		if (i->size() == 1)
			switch (i->front()) {
			case objectFront: {
				jsonNode* nov = new jsonNode{ OBJECT };
				jsonObject* obj = new jsonObject;
				nov->data.object = obj;
				current = link(nov, current);
				break;
			}
			case objectBack:case arrayBack: 
				current = next(current); break;
			case arrayFront: {
				jsonNode* nov = new jsonNode{ ARRAY };
				jsonArray* arr = new jsonArray;
				nov->data.array = arr;
				current = link(nov, current);
				break;
			}
			case strSymbol: {
				auto key = ++i;
				while (i->size() == 1 && i->front() != strSymbol)
					i++;
				if (i->front() == colon) 
					switch (i->front()) {
					case objectFront: {
						jsonNode* nov = new jsonNode{ OBJECT };
						jsonObject* obj = new jsonObject;
						obj->first = move(*key);
						nov->data.object = obj;
						current = link(nov, current);
						break;
					}
					case arrayFront: {
						jsonNode* nov = new jsonNode{ ARRAY };
						jsonArray* arr = new jsonArray;
						arr->first = move(*key);
						nov->data.array = arr;
						current = link(nov, current);
						break;
					}
					case objectBack:case arrayBack:
						current = next(current); break;
					case strSymbol: {
						jsonNode* unit = new jsonNode(FIELD);
						jsonField* field = new jsonField(*++i);
						unit->data.field = field;
						jsonMember* nov = new jsonMember(move(*key), unit);
						current = link(nov, current);
						i++;
						break;
					}
					case comma:break;
					default: {
						jsonNode* unit = new jsonNode(FIELD);
						jsonField* field = recognize(*++i);
                        unit->data.field = field;
						jsonMember* nov = new jsonMember(move(*key), unit);
						current = link(nov, current);
					}
					}
				else {
					jsonNode* nov = new jsonNode{ FIELD };
					jsonField* field = new jsonField(*++i);
					nov->data.field = field;
					current = link(nov, current);
				}
				break;
			}
			}
		else {
			jsonNode* nov = new jsonNode(FIELD);
			nov->data.field = recognize(*i);
			current = link(nov, current);
		}
	}

	return *current->data.object;
}
