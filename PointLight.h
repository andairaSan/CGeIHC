#pragma once
#include "Light.h"

class PointLight : public Light
{
public:
	PointLight();
	PointLight(GLfloat red, GLfloat green, GLfloat blue,
		GLfloat aIntensity, GLfloat dIntensity,
		GLfloat xPos, GLfloat yPos, GLfloat zPos, //aquí se  modifica para que la luz se mueva con la luz de la lámpara 
		GLfloat con, GLfloat lin, GLfloat exp);
	//para que el main pueda decirle a la luz a dónde moverse
	void SetPos(glm::vec3 pos) {
		position = pos;
	}
	//para modificar la atenuación
	void SetAttenuation(GLfloat con, GLfloat lin, GLfloat exp) {
		constant = con;
		linear = lin;
		exponent = exp;
	} 

	void UseLight(GLfloat ambientIntensityLocation, GLfloat ambientcolorLocation,
		GLfloat diffuseIntensityLocation, GLfloat positionLocation,
		GLfloat constantLocation, GLfloat linearLocation, GLfloat exponentLocation);

	~PointLight();

protected:
	glm::vec3 position;
	GLfloat constant, linear, exponent;
};
