#include "json.h"
#include "preload.h"

#define FILE "setting.json"

imgScene* preload() {
	auto content = read(FILE);
}