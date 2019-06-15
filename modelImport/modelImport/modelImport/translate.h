#pragma once
#ifndef  TRANSLATE_H
#define TRANSLATE_H
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
class Translate{
private:
	float MovementSpeed;
	float ScaleSpeed;
	float RotateSpeed;
	float Scale;
	float Rotate;
	bool beR, beB;
	glm::vec3 position;
public:
	bool startR;
	Translate(float ms,float ss,float rs,glm::vec3 po=glm::vec3(0.0f,0.0f,0.0f),float s=1.0f,float r=0.0f):position(po),Scale(s),Rotate(r) {
		MovementSpeed = ms;
		ScaleSpeed = ss;
		RotateSpeed = rs;
		beR = true;
		beB = true;
		startR = false;
	}

	glm::mat4 getModelMatrix(glm::mat4 ro) {
		glm::mat4 model = glm::mat4(1.0f);
		model = ro;
		model = glm::translate(model, position);
		model = glm::scale(model, glm::vec3(Scale, Scale, Scale));
		return model;
	}
	void ProcessKeyboard(const std::string translateType,float deltaTime)
	{
		if (translateType == "MOVE") {
			float velocity = MovementSpeed * deltaTime;
			position.x += beR ? velocity : (-velocity);
			if (position.x >= 1.0) {
				beR = false;
			}
			else if (position.x <= -1) {
				beR = true;
			}
		}
		if (translateType == "SCALE") {
			float velocity = ScaleSpeed * deltaTime;
			Scale += beB ? velocity : (-velocity);
			if (Scale >= 2.0f) beB = false;
			else if (Scale < 0.1) beB = true;
		}
		if (translateType == "ROTATE") {
			startR = true;
		}
		
	}
};

#endif //
