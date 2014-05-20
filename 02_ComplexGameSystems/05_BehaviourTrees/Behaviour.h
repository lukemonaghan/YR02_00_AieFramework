#pragma once

#include <vector>

class Agent;

class Behaviour {
public:
	Behaviour() { }
	virtual ~Behaviour() { }

	virtual bool excecute(Agent* a_agent) = 0;
};

class Composite : public Behaviour {
public:
	Composite() { }
	virtual ~Composite() { 
		for (auto child : m_children){
			delete child;
		}
	}

	void addChild(Behaviour* a_behaviour) {m_children.push_back(a_behaviour);}

protected:
	std::vector<Behaviour*> m_children;
};

class Selector : public Composite {
public:
	Selector() { }
	virtual ~Selector() { }

	virtual bool excecute(Agent* a_agent){
		for ( auto child : m_children){
			if (child->excecute(a_agent) == true){
				return true;
			}
		}
		return false;
	}
};

class Sequence : public Composite {
public:
	Sequence() { }
	virtual ~Sequence() { }

	virtual bool excecute(Agent* a_agent){
		for ( auto child : m_children){
			if (child->excecute(a_agent) == false){
				return false;
			}
		}
		return true;
	}
};

