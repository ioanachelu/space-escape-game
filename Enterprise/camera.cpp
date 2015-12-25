#include "camera.h"

Camera::Camera(){
}
Camera::~Camera(){
}

void Camera::init(){
position = Vector3D(0,15,0);
forward = Vector3D(0,-1,0);
up = Vector3D(0,0,1);
right = Vector3D(-1,0,0);
}

void Camera::translate_Forward(float dist){
	position += (forward * dist);
}
void Camera::translate_Up(float dist){
	position += (up * dist);
}
void Camera::translate_Right(float dist){
	position += (right * dist);
}




void Camera::rotateFPS_OY(float angle){
	std::cout << forward.x << " " << forward.y << " " << forward.z << std::endl;
	std::cout << right.x << " " << right.y << " " << right.z <<std::endl;
	Vector3D new_forward, new_right;
	
	new_forward = forward * cos(angle) + right * sin (angle);
	new_right = forward.CrossProduct(up) ;
	forward = new_forward;
	right = new_right;
}
void Camera::rotateFPS_OX(float angle){
	Vector3D new_forward, new_up;
	new_forward = forward * cos(angle) + up * sin (angle);
	new_up = forward.CrossProduct(right) * (-1) ;
	forward = new_forward;
	up = new_up;

}
void Camera::rotateFPS_OZ(float angle){
	Vector3D new_right, new_up;
	new_right = right * cos(angle) + up * sin (angle);
	new_up = right.CrossProduct(forward) ;
	right = new_right;
	up = new_up;

}
void Camera::rotateTPS_OY(float angle, float dist_to_interes){
	Vector3D new_forward, new_right;
	new_forward = forward * cos(angle) + right * sin (angle);
	new_right = forward.CrossProduct(up) ;
	position = position +  forward * dist_to_interes - new_forward * dist_to_interes ;
	forward = new_forward;
	right = new_right;
}
void Camera::rotateTPS_OX(float angle, float dist_to_interes){
	/*Vector3D new_forward, new_up;
	new_forward = forward * cos(angle) + up * sin (angle);
	new_up = forward.CrossProduct(right) * (-1) ;

	position = position +  forward * dist_to_interes - new_forward * dist_to_interes ;
	forward = new_forward;
	up = new_up;*/
	translate_Up(dist_to_interes);
	rotateFPS_OX(angle);
	translate_Up(-dist_to_interes);
	
	//trans
}
void Camera::rotateTPS_OZ(float angle, float dist_to_interes){
	Vector3D new_right, new_up;
	new_right = right * cos(angle) + up * sin (angle);
	new_up = right.CrossProduct(forward) ;
	position = position +  right * dist_to_interes - new_right * dist_to_interes ;
	right = new_right;
	up = new_up;
}


void Camera::render(){
	Vector3D center = position + forward;
	gluLookAt(	position.x, position.y, position.z, 
				center.x, center.y, center.z,
				up.x, up.y, up.z);
}