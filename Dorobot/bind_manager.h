#pragma once
#include "pch.h"
#include <string>
#include <map>
#include <vector>
#include <unordered_set>

enum BIND_TYPE { BIND_TYPE_NONE, BIND_TYPE_HOLD, BIND_TYPE_TOGGLE };
struct Bind
{
	Bind(const std::string& name, int key, BIND_TYPE type) : name(name), key(key), type(type), id(idCounter) { idCounter++; }
	Bind() : id(idCounter) { idCounter++; };
	BIND_TYPE type;
	std::string name;
	int key;
	bool active = false;
	int id;
	static int idCounter;
};

class BindManager
{
public:
	BindManager(Dorobot* doroBot);
	~BindManager();
	void cycle();
	void addBind(const std::string& name, int key);
	void deleteBind(const Bind& bind);
	void registerBindName(const std::string& name, BIND_TYPE type);
	bool bindActive(const std::string& name);
	std::vector<std::string> getPossibleBindNames() const;
	std::vector<Bind> binds;
	void setBindName(Bind& bind, const std::string& name);
	int getCurrentPressedKey() const;
	std::string getNameForKey(int key) const;

private:
	std::vector<int> allowedKeys {};
	std::map<std::string, BIND_TYPE> possibleBinds
	{
	};
	std::unordered_set<int> previousPressedKeys;
	Dorobot* doroBot;
	void setupAllowedKeys();

};
