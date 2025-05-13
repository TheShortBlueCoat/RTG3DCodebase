#pragma once
#include "glm/glm.hpp"  
#include "glm/gtc/matrix_transform.hpp" 
#include "glm/gtc/type_ptr.hpp"

using namespace glm;

#include <string>
#include <fstream>
#include <iostream>

using namespace std;

//base class for a light
class Light
{
public:
	Light();
	~Light() {}

	//load from mainfest
	virtual void Load(ifstream& _file);

	//tick this light
	virtual void Tick(float _dt);

	//Getters and Setters
	void SetName(string _name) { m_name = _name; }
	string GetName() { return m_name; }
	void SetTye(string _type) { m_type = _type; }
	string GetType() { return m_type; }

	vec3 GetCol() { return m_col; }
	vec3 GetAmb() { return m_amb; }
	vec3 GetPos() { return m_pos; }

	void SetPos(const glm::vec3& pos) { m_pos = pos; }
	void SetCol(const glm::vec3& col) { m_col = col; }
	void SetAmb(const glm::vec3& amb) { m_amb = amb; }

	//set my shader values
	//base version: if name of light is LG
	//sets up shader values for LGpos LGcol & LGamb
	//position, main colour and ambient colour for this light
	virtual void SetRenderValues(unsigned int _prog);

protected:
	string m_name;
	string m_type;

	vec3 m_pos; // position of the light
	vec3 m_col; // colour of the light
	vec3 m_amb; // ambient colour of the light

};