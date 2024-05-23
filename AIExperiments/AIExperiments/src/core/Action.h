#pragma once
#include<string>

class Action
{
public:
	Action();
	Action(const std::string& name, const std::string& type);

	const std::string& Name() const;
	const std::string& Type() const;

private:
	std::string m_name;
	std::string m_type;
};
