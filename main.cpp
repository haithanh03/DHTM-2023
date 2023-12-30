#pragma region Khai báo, Khởi tạo

/*Chương trình chiếu sáng Blinn-Phong (Phong sua doi) cho hình lập phương đơn vị, điều khiển quay bằng phím x, y, z, X, Y, Z.*/

#include "Angel.h"  /* Angel.h là file tự phát triển (tác giả Prof. Angel), có chứa cả khai báo includes glew và freeglut*/
#include "../Models/all_models.h"
#include <cstring>
#include <iostream>

// remember to prototype
void initGPUBuffers(GLuint vao, GLuint buffer, int size, vec4 vertices[], vec3 normals[], vec4 ambients[], vec4 diffuses[], vec4 speculars[]);
void shaderSetup(void);
void display(void);
void keyboard(unsigned char key, int x, int y);

using namespace std;

GLuint program;
mat4 model;
mat4 instance;

vec4 eye(-30, 10, 0, 1), at(0, 0, 0, 1), up(0, 1, 0, 1);
GLfloat l = -3, r = 3, b = -3, t = 3, zN = 6, zF = 100;
mat4 view_mat;
mat4 move_mat;
GLfloat matXY[2] = {0, 90};
vec4 mat_dinhhuong(1, 0, 0, 1);

GLfloat theta[3] = { 0, 0, 0 };
GLfloat dr = 5;

GLuint model_loc, projection_loc, view_loc, light_a_loc, light_d_loc, light_s_loc, light_pos_loc, light_shine_loc;
GLuint flipX_loc, flipY_loc, flipZ_loc;
GLuint isCustomMaterial_loc, customMaterialAmbient_loc, customMaterialDiffuse_loc, customMaterialSpecular_loc;

// -------------Bước 1: Khai báo VAO, VBO-------------
GLuint vao_matTroi, vao_xeTai_Dau, vao_xeTai_Cube, vao_xeTai_BanhXe, vao_xeTai_Cua, vao_xeTai_DenSau, vao_xeTai_GatNuoc, vao_xeTai_Guong, vao_xeTai_Kinh, vao_xeTai_NoiThat1, vao_xeTai_TayNam, vao_xeTai_TrucGuong1, vao_xeTai_TrucGuong2, vao_xeTai_Chan1, vao_xeTai_Chan2;
GLuint vao_coiXayThan, vao_coiXayTuabin, vao_coiXayCanhQuat;
GLuint vao_battery, vao_than_battery;
GLuint vao_than, vao_la, vao_canh1, vao_canh2, vao_canh3;
GLuint vao_house, vao_cuachinh, vao_tayNamCuaChinh, vao_cuaSo1, vao_caighe, vao_khungfridge, vao_ngantulanh1, vao_ngantulanh2, vao_canhtulanh1, vao_canhtulanh2, vao_fan, vao_bed;

GLuint buffer_matTroi, buffer_xeTai_Dau, buffer_xeTai_Cube, buffer_xeTai_BanhXe, buffer_xeTai_Cua, buffer_xeTai_DenSau, buffer_xeTai_GatNuoc, buffer_xeTai_Guong, buffer_xeTai_Kinh, buffer_xeTai_NoiThat1, buffer_xeTai_TayNam, buffer_xeTai_TrucGuong1, buffer_xeTai_TrucGuong2, buffer_xeTai_Chan1, buffer_xeTai_Chan2;
GLuint buffer_coiXayThan, buffer_coiXayTuabin, buffer_coiXayCanhQuat;
GLuint buffer_battery, buffer_than_battery;
GLuint buffer_than, buffer_la, buffer_canh1, buffer_canh2, buffer_canh3; GLuint buffer_house, buffer_cuachinh, buffer_tayNamCuaChinh, buffer_cuaSo1, buffer_caighe, buffer_khungfridge, buffer_ngantulanh1, buffer_ngantulanh2, buffer_canhtulanh1, buffer_canhtulanh2, buffer_fan, buffer_bed;

const int spotLightCount = 18;
vec4 spotLightPos[spotLightCount];

const int xetai_cube_size = 6 * 4 * 2 + 6 * 10, xetai_banhxe_size = 36 * 3 * 2 + 36 * 6 * 9, coixaythan_size = 24 * 3 * 2 + 24 * 6 * 4;
const int coixaytuabin_size = 16 * 3 * 2 + 16 * 6 * 3, coixaycanhquat_size = 6 * 2 + 6 * 4;
const int NumPoints = xetai_cube_size + xetai_banhxe_size + coixaythan_size + coixaytuabin_size + coixaycanhquat_size;
vec4 points[NumPoints];
vec3 normals[NumPoints];
vec4 ambient[NumPoints];
vec4 diffuse[NumPoints];
vec4 specular[NumPoints];
int index = 0;

vec3* pointInCircle(float radius, int soDiem, float z)
{
	vec3* point = new vec3[soDiem];
	
	float doLech = 360 / soDiem * DegreesToRadians;
	float deg = 360;
	for (int i = 0; i < soDiem; i++) {
		float x = cos(deg) * radius;
		float y = sin(deg) * radius;
		point[i] = vec3(x, y, z);
		deg -= doLech;
	}
	return point;
}

//Nguoc chieu kiem dong ho
void taoDiemTamGiac(vec3 pointA, vec3 pointB, vec3 pointC, int pxaPhiaTruoc)
{
	vec3 u = pointB - pointA, v = pointC - pointA;
	normals[index] = normalize(cross(u, v)) * pxaPhiaTruoc;
	points[index] = vec4(pointA); index++;

	u = pointC - pointB, v = pointA - pointB;
	normals[index] = normalize(cross(u, v)) * pxaPhiaTruoc;
	points[index] = vec4(pointB); index++;

	u = pointA - pointC, v = pointB - pointC;
	normals[index] = normalize(cross(u, v)) * pxaPhiaTruoc;
	points[index] = vec4(pointC); index++;
}

/*
	C	D
	A	B
*/
void taoDiemTuGiac(vec3 pointA, vec3 pointB, vec3 pointC, vec3 pointD, int pxaPhiaTruoc)
{
	taoDiemTamGiac(pointA, pointB, pointC, pxaPhiaTruoc);
	taoDiemTamGiac(pointB, pointD, pointC, pxaPhiaTruoc);
}

void taoDiemHinhChop(vec3* point, vec3 tam, int soDiem, int pxaPhiaTruoc)
{
	for (int i = 0; i < soDiem - 1; i++) {
		taoDiemTamGiac(tam, point[i + 1], point[i], pxaPhiaTruoc);
	}
	taoDiemTamGiac(tam, point[0], point[soDiem - 1], pxaPhiaTruoc);
}

void taoDiemMatPhang(vec3* point1, vec3* point2, int soDiem, int pxaPhiaTruoc)
{
	for (int i = 0; i < soDiem - 1; i++) {
		taoDiemTuGiac(point1[i], point1[i + 1], point2[i], point2[i + 1], pxaPhiaTruoc);
	}
}

void taoDiemTruTron(vec3* point1, vec3* point2, int soDiem, int pxaPhiaTruoc)
{
	taoDiemMatPhang(point1, point2, soDiem, pxaPhiaTruoc);
	taoDiemTuGiac(point1[soDiem - 1], point1[0], point2[soDiem - 1], point2[0], pxaPhiaTruoc);
}

/*11 va 12 la cung z1 - 21 va 22 la cung z2*/
void taoDiemHinhKhoi(float* x11, float* y11, float* z11, float* x12, float* y12, float* z12, float* x21, float* y21, float* z21, float* x22, float* y22, float* z22, int soDiem)
{
	vec3* point11 = new vec3[soDiem];	//z1
	vec3* point12 = new vec3[soDiem];	//z1
	vec3* point21 = new vec3[soDiem];	//z2
	vec3* point22 = new vec3[soDiem];	//z2
	for (int i = 0; i < soDiem; i++) {
		point11[i] = vec3(x11[i], y11[i], z11[i]);
		point12[i] = vec3(x12[i], y12[i], z12[i]);
		point21[i] = vec3(x21[i], y21[i], z21[i]);
		point22[i] = vec3(x22[i], y22[i], z22[i]);
	}
	taoDiemMatPhang(point11, point12, soDiem, 1);
	taoDiemMatPhang(point21, point22, soDiem, -1);

	vec3* point1 = new vec3[soDiem * 2];
	vec3* point2 = new vec3[soDiem * 2];
	int i = 0, index1 = 0;
	while (i < soDiem) {
		point1[index1] = point11[i];
		point2[index1] = point21[i];
		index1++; i++;
	}
	int j = soDiem - 1;
	while (j >= 0) {
		point1[index1] = point12[j];
		point2[index1] = point22[j];
		index1++; j--;
	}
	taoDiemTruTron(point1, point2, soDiem * 2, -1);
}

// HOP
const int soDiemHop = 5;
void initCube()
{
	index = 0;
	float x1[]{
		-0.5, -0.25, 0, 0.25, 0.5
	};
	float y1[]{
		-0.5, -0.5, -0.5, -0.5, -0.5
	};
	float z1[]{
		0.5, 0.5, 0.5, 0.5, 0.5, 0.5
	};
	float x2[]{
		-0.5, -0.25, 0, 0.25, 0.5
	};
	float y2[]{
		0.5, 0.5, 0.5, 0.5, 0.5, 0.5
	};
	float z2[]{
		-0.5, -0.5, -0.5, -0.5, -0.5
	};
	taoDiemHinhKhoi(x1, y1, z1, x2, y2, z1, x1, y1, z2, x2, y2, z2, soDiemHop);
}

void initCanhQuat() {
	index = 0;
	float x1[]{
		-0.4, 0.4
	};
	float y1[]{
		0, 0
	};
	float z1[]{
		0.15, 0.15
	};
	float x2[]{
		-1, 1
	};
	float y2[]{
		9, 9
	};
	float z2[]{
		-0.15, -0.15
	};
	taoDiemHinhKhoi(x1, y1, z1, x2, y2, z1, x1, y1, z2, x2, y2, z2, 2);
}

// HINH TRU
void initCustomCylinder(float radius[], float z[], int soVong, int soDiemTrenVong) {
	index = 0;
	vec3* point1 = pointInCircle(radius[0], soDiemTrenVong, z[0]);
	taoDiemHinhChop(point1, vec3(0, 0, point1[0].z), soDiemTrenVong, 1);
	vec3* point2;
	for (int i = 1; i < soVong; i++) {
		point2 = pointInCircle(radius[i], soDiemTrenVong, z[i]);
		taoDiemTruTron(point1, point2, soDiemTrenVong, 1);
		point1 = point2;
	}
	taoDiemHinhChop(point1, vec3(0, 0, point1[soVong - 1].z), soDiemTrenVong, -1);
}

// HINH BANH XE
void initBanhXe()
{
	index = 0;
	float radius[]{
		0.2149, 0.2401, 0.2744, 0.3659, 0.39991, 0.39991, 0.3659, 0.2744, 0.2401, 0.2149
	};
	float z[]{
		0.07163, 0.105, 0.115, 0.115, 0.1, -0.1, -0.115, -0.115, -0.105, -0.07163
	};
	initCustomCylinder(radius, z, 10, 36);
}

void initCoiXayThan() {
	index = 0;
	float radius[]{
		0.375, 0.42188, 0.49219, 0.61875, 0.74
	};
	float z[]{
		16, 14, 12, 8, 0
	};
	initCustomCylinder(radius, z, 5, 24);
}

void initCoiXayTuabin() {
	index = 0;
	float radius[]{
		0.44052, 0.49158, 0.46188, 0.3
	};
	float z[]{
		0.75, 0.58437, 0, -0.75
	};
	initCustomCylinder(radius, z, 4, 16);
}

void initGPUBuffers(GLuint vao, GLuint buffer, int size, vec4 vertices[], vec3 normals[], vec4 ambients[], vec4 diffuses[], vec4 speculars[])
{
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);

	int partition1 = sizeof(vec4) * size;
	int partition2 = partition1 + sizeof(vec3) * size;
	int partition3 = partition2 + sizeof(vec4) * size;
	int partition4 = partition3 + sizeof(vec4) * size;
	int partition5 = partition4 + sizeof(vec4) * size;

	glBufferData(GL_ARRAY_BUFFER, partition5, NULL, GL_STATIC_DRAW);

	glBufferSubData(GL_ARRAY_BUFFER, 0, partition1, vertices);
	glBufferSubData(GL_ARRAY_BUFFER, partition1, partition2 - partition1, normals);
	glBufferSubData(GL_ARRAY_BUFFER, partition2, partition3 - partition2, ambients);
	glBufferSubData(GL_ARRAY_BUFFER, partition3, partition4 - partition3, diffuses);
	glBufferSubData(GL_ARRAY_BUFFER, partition4, partition5 - partition4, speculars);

	// -----------------Định vị trong GPU buffer-----------------
	// Khởi tạo thuộc tính vị trí đỉnh từ vertex shader
	GLuint loc_vPosition = glGetAttribLocation(program, "vPosition");
	glEnableVertexAttribArray(loc_vPosition);
	glVertexAttribPointer(loc_vPosition, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0));

	GLuint loc_vNormal = glGetAttribLocation(program, "vNormal");
	glEnableVertexAttribArray(loc_vNormal);
	glVertexAttribPointer(loc_vNormal, 3, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(partition1));

	GLuint loc_MaterialAmbient = glGetAttribLocation(program, "MaterialAmbient");
	glEnableVertexAttribArray(loc_MaterialAmbient);
	glVertexAttribPointer(loc_MaterialAmbient, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(partition2));

	GLuint loc_MaterialDiffuse = glGetAttribLocation(program, "MaterialDiffuse");
	glEnableVertexAttribArray(loc_MaterialDiffuse);
	glVertexAttribPointer(loc_MaterialDiffuse, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(partition3));

	GLuint loc_MaterialSpecular = glGetAttribLocation(program, "MaterialSpecular");
	glEnableVertexAttribArray(loc_MaterialSpecular);
	glVertexAttribPointer(loc_MaterialSpecular, 4, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(partition4));
	// --------------------------------------------------------------------
}


void shaderSetup(void)
{
	// Nạp các shader và sử dụng chương trình shader
	program = InitShader("vshader1.glsl", "fshader1.glsl");   // hàm InitShader khai báo trong Angel.h
	glUseProgram(program);

	
	//initBanhXe();
	// Tạo VAO - vertex array object
	glGenVertexArrays(1, &vao_matTroi);
	glGenVertexArrays(1, &vao_xeTai_Dau);
	glGenVertexArrays(1, &vao_xeTai_Cube);
	glGenVertexArrays(1, &vao_xeTai_BanhXe);
	glGenVertexArrays(1, &vao_xeTai_Cua);
	glGenVertexArrays(1, &vao_xeTai_DenSau);
	glGenVertexArrays(1, &vao_xeTai_GatNuoc);
	glGenVertexArrays(1, &vao_xeTai_Guong);
	glGenVertexArrays(1, &vao_xeTai_Kinh);
	glGenVertexArrays(1, &vao_xeTai_NoiThat1);
	glGenVertexArrays(1, &vao_xeTai_TayNam);
	glGenVertexArrays(1, &vao_xeTai_TrucGuong1);
	glGenVertexArrays(1, &vao_xeTai_TrucGuong2);
	glGenVertexArrays(1, &vao_xeTai_Chan1);
	glGenVertexArrays(1, &vao_xeTai_Chan2);

	glGenVertexArrays(1, &vao_coiXayThan);
	glGenVertexArrays(1, &vao_coiXayTuabin);
	glGenVertexArrays(1, &vao_coiXayCanhQuat);

	glGenVertexArrays(1, &vao_battery);
	glGenVertexArrays(1, &vao_than_battery);

	glGenVertexArrays(1, &vao_than);
	glGenVertexArrays(1, &vao_la);
	glGenVertexArrays(1, &vao_canh1);
	glGenVertexArrays(1, &vao_canh2);
	glGenVertexArrays(1, &vao_canh3);

	glGenVertexArrays(1, &vao_house);
	glGenVertexArrays(1, &vao_cuachinh);
	glGenVertexArrays(1, &vao_tayNamCuaChinh);
	glGenVertexArrays(1, &vao_cuaSo1);
	glGenVertexArrays(1, &vao_caighe);
	glGenVertexArrays(1, &vao_khungfridge);
	glGenVertexArrays(1, &vao_ngantulanh1);
	glGenVertexArrays(1, &vao_ngantulanh2);
	glGenVertexArrays(1, &vao_canhtulanh1);
	glGenVertexArrays(1, &vao_canhtulanh2);
	glGenVertexArrays(1, &vao_fan);
	glGenVertexArrays(1, &vao_bed);

	// Tạo buffer object
	glGenBuffers(1, &buffer_matTroi);
	glGenBuffers(1, &buffer_xeTai_Dau);
	glGenBuffers(1, &buffer_xeTai_Cube);
	glGenBuffers(1, &buffer_xeTai_BanhXe);
	glGenBuffers(1, &buffer_xeTai_Cua);
	glGenBuffers(1, &buffer_xeTai_DenSau);
	glGenBuffers(1, &buffer_xeTai_GatNuoc);
	glGenBuffers(1, &buffer_xeTai_Guong);
	glGenBuffers(1, &buffer_xeTai_Kinh);
	glGenBuffers(1, &buffer_xeTai_NoiThat1);
	glGenBuffers(1, &buffer_xeTai_TayNam);
	glGenBuffers(1, &buffer_xeTai_TrucGuong1);
	glGenBuffers(1, &buffer_xeTai_TrucGuong2);
	glGenBuffers(1, &buffer_xeTai_Chan1);
	glGenBuffers(1, &buffer_xeTai_Chan2);

	glGenBuffers(1, &buffer_coiXayThan);
	glGenBuffers(1, &buffer_coiXayTuabin);
	glGenBuffers(1, &buffer_coiXayCanhQuat);


	glGenBuffers(1, &buffer_battery);
	glGenBuffers(1, &buffer_than_battery);

	glGenBuffers(1, &buffer_than);
	glGenBuffers(1, &buffer_la);
	glGenBuffers(1, &buffer_canh1);
	glGenBuffers(1, &buffer_canh2);
	glGenBuffers(1, &buffer_canh3);

	glGenBuffers(1, &buffer_house);
	glGenBuffers(1, &buffer_cuachinh);
	glGenBuffers(1, &buffer_tayNamCuaChinh);
	glGenBuffers(1, &buffer_cuaSo1);
	glGenBuffers(1, &buffer_caighe);
	glGenBuffers(1, &buffer_khungfridge);
	glGenBuffers(1, &buffer_ngantulanh1);
	glGenBuffers(1, &buffer_ngantulanh2);
	glGenBuffers(1, &buffer_canhtulanh1);
	glGenBuffers(1, &buffer_canhtulanh2);
	glGenBuffers(1, &buffer_fan);
	glGenBuffers(1, &buffer_bed);

	initCube();
	initGPUBuffers(vao_xeTai_Cube, buffer_xeTai_Cube, xetai_cube_size, points, normals, NULL, NULL, NULL);
	initBanhXe();
	initGPUBuffers(vao_xeTai_BanhXe, buffer_xeTai_BanhXe, xetai_banhxe_size, points, normals, NULL, NULL, NULL);
	initGPUBuffers(vao_matTroi, buffer_matTroi, mattroi_size, mattroi_vertices, mattroi_normals, mattroi_ambients, mattroi_diffuses, mattroi_speculars);
	initGPUBuffers(vao_xeTai_Dau, buffer_xeTai_Dau, xetai_dau_size, xetai_dau_vertices, xetai_dau_normals, xetai_dau_ambients, xetai_dau_diffuses, xetai_dau_speculars);
	initGPUBuffers(vao_xeTai_NoiThat1, buffer_xeTai_NoiThat1, xetai_noithat1_size, xetai_noithat1_vertices, xetai_noithat1_normals, xetai_noithat1_ambients, xetai_noithat1_diffuses, xetai_noithat1_speculars);
	initGPUBuffers(vao_xeTai_TrucGuong1, buffer_xeTai_TrucGuong1, xetai_trucguong1_size, xetai_trucguong1_vertices, xetai_trucguong1_normals, xetai_trucguong1_ambients, xetai_trucguong1_diffuses, xetai_trucguong1_speculars);
	initGPUBuffers(vao_xeTai_TrucGuong2, buffer_xeTai_TrucGuong2, xetai_trucguong2_size, xetai_trucguong2_vertices, xetai_trucguong2_normals, xetai_trucguong2_ambients, xetai_trucguong2_diffuses, xetai_trucguong2_speculars);
	initGPUBuffers(vao_xeTai_Guong, buffer_xeTai_Guong, xetai_guong_size, xetai_guong_vertices, xetai_guong_normals, xetai_guong_ambients, xetai_guong_diffuses, xetai_guong_speculars);
	initGPUBuffers(vao_xeTai_Chan1, buffer_xeTai_Chan1, xetai_chan1_size, xetai_chan1_vertices, xetai_chan1_normals, xetai_chan1_ambients, xetai_chan1_diffuses, xetai_chan1_speculars);
	initGPUBuffers(vao_xeTai_Chan2, buffer_xeTai_Chan2, xetai_chan2_size, xetai_chan2_vertices, xetai_chan2_normals, xetai_chan2_ambients, xetai_chan2_diffuses, xetai_chan2_speculars);
	initGPUBuffers(vao_xeTai_DenSau, buffer_xeTai_DenSau, xetai_densau_size, xetai_densau_vertices, xetai_densau_normals, xetai_densau_ambients, xetai_densau_diffuses, xetai_densau_speculars);
	initGPUBuffers(vao_xeTai_GatNuoc, buffer_xeTai_GatNuoc, xetai_gatnuoc_size, xetai_gatnuoc_vertices, xetai_gatnuoc_normals, xetai_gatnuoc_ambients, xetai_gatnuoc_diffuses, xetai_gatnuoc_speculars);
	initGPUBuffers(vao_xeTai_Cua, buffer_xeTai_Cua, xetai_cua_size, xetai_cua_vertices, xetai_cua_normals, xetai_cua_ambients, xetai_cua_diffuses, xetai_cua_speculars);
	initGPUBuffers(vao_xeTai_Kinh, buffer_xeTai_Kinh, xetai_kinh_size, xetai_kinh_vertices, xetai_kinh_normals, xetai_kinh_ambients, xetai_kinh_diffuses, xetai_kinh_speculars);
	initGPUBuffers(vao_xeTai_TayNam, buffer_xeTai_TayNam, xetai_taynam_size, xetai_taynam_vertices, xetai_taynam_normals, xetai_taynam_ambients, xetai_taynam_diffuses, xetai_taynam_speculars);

	initCoiXayThan();
	initGPUBuffers(vao_coiXayThan, buffer_coiXayThan, coixaythan_size, points, normals, NULL, NULL, NULL);
	initCoiXayTuabin();
	initGPUBuffers(vao_coiXayTuabin, buffer_coiXayTuabin, coixaytuabin_size, points, normals, NULL, NULL, NULL);
	initCanhQuat();
	initGPUBuffers(vao_coiXayCanhQuat, buffer_coiXayCanhQuat, coixaycanhquat_size, points, normals, NULL, NULL, NULL);

	initGPUBuffers(vao_battery, buffer_battery, battery_size, battery_vertices, battery_normals, battery_ambients, battery_diffuses, battery_speculars);
	initGPUBuffers(vao_than_battery, buffer_than_battery, than_battery_size, than_battery_vertices, than_battery_normals, than_battery_ambients, than_battery_diffuses, than_battery_speculars);

	initGPUBuffers(vao_than, buffer_than, than_size, than_vertices, than_normals, than_ambients, than_diffuses, than_speculars);
	initGPUBuffers(vao_la, buffer_la, la_size, la_vertices, la_normals, la_ambients, la_diffuses, la_speculars);
	initGPUBuffers(vao_canh1, buffer_canh1, canh1_size, canh1_vertices, canh1_normals, canh1_ambients, canh1_diffuses, canh1_speculars);
	initGPUBuffers(vao_canh2, buffer_canh2, canh2_size, canh2_vertices, canh2_normals, canh2_ambients, canh2_diffuses, canh2_speculars);
	initGPUBuffers(vao_canh3, buffer_canh3, canh3_size, canh3_vertices, canh3_normals, canh3_ambients, canh3_diffuses, canh3_speculars);

	initGPUBuffers(vao_house, buffer_house, house_size, house_vertices, house_normals, house_ambients, house_diffuses, house_speculars);
	initGPUBuffers(vao_cuachinh, buffer_cuachinh, cuachinh_size, cuachinh_vertices, cuachinh_normals, cuachinh_ambients, cuachinh_diffuses, cuachinh_speculars);
	initGPUBuffers(vao_tayNamCuaChinh, buffer_tayNamCuaChinh, taynamcuachinh_size, taynamcuachinh_vertices, taynamcuachinh_normals, taynamcuachinh_ambients, taynamcuachinh_diffuses, taynamcuachinh_speculars);
	initGPUBuffers(vao_cuaSo1, buffer_cuaSo1, cuaso1_size, cuaso1_vertices, cuaso1_normals, cuaso1_ambients, cuaso1_diffuses, cuaso1_speculars);
	initGPUBuffers(vao_caighe, buffer_caighe, caighe_size, caighe_vertices, caighe_normals, caighe_ambients, caighe_diffuses, caighe_speculars);
	initGPUBuffers(vao_khungfridge, buffer_khungfridge, khungfridge_size, khungfridge_vertices, khungfridge_normals, khungfridge_ambients, khungfridge_diffuses, khungfridge_speculars);
	initGPUBuffers(vao_ngantulanh1, buffer_ngantulanh1, ngantulanh1_size, ngantulanh1_vertices, ngantulanh1_normals, ngantulanh1_ambients, ngantulanh1_diffuses, ngantulanh1_speculars);
	initGPUBuffers(vao_ngantulanh2, buffer_ngantulanh2, ngantulanh2_size, ngantulanh2_vertices, ngantulanh2_normals, ngantulanh2_ambients, ngantulanh2_diffuses, ngantulanh2_speculars);
	initGPUBuffers(vao_canhtulanh1, buffer_canhtulanh1, canhtulanh1_size, canhtulanh1_vertices, canhtulanh1_normals, canhtulanh1_ambients, canhtulanh1_diffuses, canhtulanh1_speculars);
	initGPUBuffers(vao_canhtulanh2, buffer_canhtulanh2, canhtulanh2_size, canhtulanh2_vertices, canhtulanh2_normals, canhtulanh2_ambients, canhtulanh2_diffuses, canhtulanh2_speculars);
	initGPUBuffers(vao_fan, buffer_fan, fan_size, fan_vertices, fan_normals, fan_ambients, fan_diffuses, fan_speculars);
	initGPUBuffers(vao_bed, buffer_bed, bed_size, bed_vertices, bed_normals, bed_ambients, bed_diffuses, bed_speculars);


	light_a_loc = glGetUniformLocation(program, "AmbientLight");
	light_d_loc = glGetUniformLocation(program, "DiffuseLight");
	light_s_loc = glGetUniformLocation(program, "SpecularLight");
	light_pos_loc = glGetUniformLocation(program, "LightPosition");
	light_shine_loc = glGetUniformLocation(program, "Shininess");
	flipX_loc = glGetUniformLocation(program, "FlipX");
	flipY_loc = glGetUniformLocation(program, "FlipY");
	flipZ_loc = glGetUniformLocation(program, "FlipZ");
	isCustomMaterial_loc = glGetUniformLocation(program, "IsCustomMaterial");
	customMaterialAmbient_loc = glGetUniformLocation(program, "CustomMaterialAmbient");
	customMaterialDiffuse_loc = glGetUniformLocation(program, "CustomMaterialDiffuse");
	customMaterialSpecular_loc = glGetUniformLocation(program, "CustomMaterialSpecular");

	model_loc = glGetUniformLocation(program, "Model");
	projection_loc = glGetUniformLocation(program, "Projection");
	view_loc = glGetUniformLocation(program, "View");

	glEnable(GL_DEPTH_TEST);
	glClearColor(0.7, 0.7, 0.7, 1);        
}
#pragma endregion

#pragma region Draw Objects

GLfloat phi = 0, phiTrucGuong2 = 0, phiGuong = 0, phiGatNuoc = 0, phiCuaTrai = 0, phiCuaPhai = 0, phiTayNamPhai = 0, phiTayNamTrai = 0;
GLfloat phiBanhTruoc = 0, phiBanh = 0; 
GLfloat dKinhTrai = 0, dKinhPhai = 0;

bool isGatNuoc = false;
GLfloat chieuGatNuoc = -1;

GLfloat speed = 0;
vec3 dirXetai = vec3(0, 0, 3.25), dirBanh;
vec3 dirDichuyen;
vec3 posXetai = vec3(0, 0, 0);
GLfloat phiXetai = 0;
GLfloat timer = 0;

GLfloat am[] = { 0.2, 0.2, 0.2 };
GLfloat diff[] = { 1.0, 1.0, 1.0 };
GLfloat xL = -20, yL = 25, zL = 15;
GLfloat phiCanh = 0;

GLfloat phi1 = 0, phi2 = 1, phi3 = 2, dx = 0, huong1 = -1, huong2 = -1, huong3 = -1, doLech = 4;
GLfloat cayTimer = 0, maxTime = 10;
bool isGio = false;

bool isLight = true;

GLfloat phiCuaChinh = 0, phiTayNamCuaChinh = 0, phiCuaSo1 = 0, phiCaiGhe = 0, phiCaiGheQuay = 0, phiCanhTuLanh1 = 0, phiPillow = 0, phiNganTuLanh1 = 0, phiCanhTuLanh2 = 0, phiNganTuLanh2 = 0, phiFan = 0;

void drawObject(GLuint vao, GLuint buffer, mat4 model, int numpoints, bool flipX = false, bool flipY = false, bool flipZ = false, bool isCustomMaterial = false, vec4 CustomMaterialAmbient = vec4(), vec4 CustomMaterialDiffuse = vec4(), vec4 CustomMaterialSpecular = vec4()) {
	glBindVertexArray(vao);
	glBindBuffer(GL_ARRAY_BUFFER, buffer);
	glUniformMatrix4fv(model_loc, 1, GL_TRUE, model);
	glUniform1i(flipX_loc, flipX);
	glUniform1i(flipY_loc, flipY);
	glUniform1i(flipZ_loc, flipZ);
	glUniform1i(isCustomMaterial_loc, isCustomMaterial);
	glUniform4fv(customMaterialAmbient_loc, 1, CustomMaterialAmbient);
	glUniform4fv(customMaterialDiffuse_loc, 1, CustomMaterialDiffuse);
	glUniform4fv(customMaterialSpecular_loc, 1, CustomMaterialSpecular);
	glDrawArrays(GL_TRIANGLES, 0, numpoints);    /*Vẽ các tam giác*/
}

void xeTai_KhungXe()
{
	// Dau xe
	instance = identity();
	drawObject(vao_xeTai_Dau, buffer_xeTai_Dau, model * instance, xetai_dau_size);
	instance = Scale(-1, 1, 1);
	drawObject(vao_xeTai_Dau, buffer_xeTai_Dau, model * instance, xetai_dau_size);

	// Noi that
	instance = identity();
	drawObject(vao_xeTai_NoiThat1, buffer_xeTai_NoiThat1, model * instance, xetai_noithat1_size);
	instance = Scale(-1, 1, 1);
	drawObject(vao_xeTai_NoiThat1, buffer_xeTai_NoiThat1, model * instance, xetai_noithat1_size);

	// Thung hang
	int cnt = 10;
	float zD = 7.05 / cnt;
	vec4 tmpAmbient = vec4(0.500000, 0.500000, 0.500000, 1);
	vec4 tmpDiffuse = vec4(0.08, 0.08, 0.08, 1);
	vec4 tmpSpecular = vec4(0.1, 0.1, 0.1, 1);
	for (float z = -cnt / 2 * zD + zD / 2; z <= cnt / 2 * zD - zD / 2; z += zD) {
		instance = Translate(0, 1.4, -1.14 + z) * Scale(1.64, 1.62, zD);
		drawObject(vao_xeTai_Cube, buffer_xeTai_Cube, model * instance, xetai_cube_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);
	}

	// Thung gam xe
	instance = Translate(-0.5, 0.42, 0.1) * Scale(0.4, 0.44, 0.9);
	drawObject(vao_xeTai_Cube, buffer_xeTai_Cube, model * instance, xetai_cube_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);

	instance = Translate(-0.5, 0.42, 0.8) * Scale(0.4, 0.44, 0.4);
	drawObject(vao_xeTai_Cube, buffer_xeTai_Cube, model * instance, xetai_cube_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);

	instance = Translate(0.5, 0.42, 0.63) * Scale(0.4, 0.44, 0.9);
	drawObject(vao_xeTai_Cube, buffer_xeTai_Cube, model * instance, xetai_cube_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);

	instance = Translate(0.5, 0.42, -0.44) * Scale(0.4, 0.44, 0.9);
	drawObject(vao_xeTai_Cube, buffer_xeTai_Cube, model * instance, xetai_cube_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);

	instance = Translate(0.5, 0.42, -3.7) * Scale(0.4, 0.44, 0.9);
	drawObject(vao_xeTai_Cube, buffer_xeTai_Cube, model * instance, xetai_cube_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);

	// Chan bun
	instance = identity();
	drawObject(vao_xeTai_Chan1, buffer_xeTai_Chan1, model * instance, xetai_chan1_size);
	drawObject(vao_xeTai_Chan2, buffer_xeTai_Chan2, model * instance, xetai_chan2_size);

	instance = Scale(-1, 1, 1);
	drawObject(vao_xeTai_Chan1, buffer_xeTai_Chan1, model * instance, xetai_chan1_size, true);
	drawObject(vao_xeTai_Chan2, buffer_xeTai_Chan2, model * instance, xetai_chan2_size, true);

	instance = Translate(0, 0, -3.3);
	drawObject(vao_xeTai_Chan1, buffer_xeTai_Chan1, model * instance, xetai_chan1_size);
	instance = Translate(0, 0, -4.3);
	drawObject(vao_xeTai_Chan2, buffer_xeTai_Chan2, model * instance, xetai_chan2_size);

	instance = Translate(0, 0, -3.3) * Scale(-1, 1, 1);
	drawObject(vao_xeTai_Chan1, buffer_xeTai_Chan1, model * instance, xetai_chan1_size, true);
	instance = Translate(0, 0, -4.3) * Scale(-1, 1, 1);
	drawObject(vao_xeTai_Chan2, buffer_xeTai_Chan2, model * instance, xetai_chan2_size, true);

	// Den sau
	instance = identity();
	drawObject(vao_xeTai_DenSau, buffer_xeTai_DenSau, model * instance, xetai_densau_size);

	// Truc guong 1
	instance = Translate(-0.6, 1.2, 3.77);
	drawObject(vao_xeTai_TrucGuong1, buffer_xeTai_TrucGuong1, model * instance, xetai_trucguong1_size);
}

vec3 vec4Tovec3(vec4 tmp)
{
	vec3 result;
	result.x = tmp.x;
	result.y = tmp.y;
	result.z = tmp.z;
	return result;
}

void xeTai()
{
	// animation
	if (isGatNuoc) {
		if (phiGatNuoc < -60 || phiGatNuoc > 0) {
			chieuGatNuoc *= -1;
		}
		phiGatNuoc += 0.1 * chieuGatNuoc;
	}

	dirBanh = vec4Tovec3(RotateY(phiBanhTruoc) * normalize(dirXetai));
	if (speed != 0) {
		dirDichuyen = dirXetai + dirBanh;
		GLfloat tmp = dot(dirXetai, dirDichuyen) / (length(dirXetai) * length(dirDichuyen));
		GLfloat deg = acos(tmp);
		if ((speed > 0 && phiBanhTruoc > 0) || (speed < 0 && phiBanhTruoc < 0)) {
			dirXetai = vec4Tovec3(RotateY(deg) * dirXetai);
			phi += deg;
		}	
		else if ((speed < 0 && phiBanhTruoc > 0) || (speed > 0 && phiBanhTruoc < 0)) {
			dirXetai = vec4Tovec3(RotateY(-deg) * dirXetai);
			phi -= deg;
		}
		
		model = model * Translate(posXetai) * Translate(dirDichuyen * speed * 0.3) * RotateY(phi);
		posXetai = vec4Tovec3(Translate(dirDichuyen * speed * 0.3) * posXetai);
	}
	else model = model * Translate(posXetai) * RotateY(phi);

	// Khung xe
	xeTai_KhungXe();

	// Truc guong 2
	mat4 model1 = model * Translate(-0.663, 1.287, 4.0333) * RotateY(phiTrucGuong2);
	drawObject(vao_xeTai_TrucGuong2, buffer_xeTai_TrucGuong2, model1, xetai_trucguong2_size);

	// Guong
	model1 = model1 * Translate(0, 0.25, 0.2222) * RotateY(phiGuong);
	drawObject(vao_xeTai_Guong, buffer_xeTai_Guong, model1, xetai_guong_size);

	// Gat nuoc
	mat4 model2 = model * Translate(-0.1633, 1.1361, 4.143) * RotateX(-16.5) * RotateZ(phiGatNuoc);
	drawObject(vao_xeTai_GatNuoc, buffer_xeTai_GatNuoc, model2, xetai_gatnuoc_size);

	model2 = model * Translate(0.3384, 1.1361, 4.143) * RotateX(-16.5) * RotateZ(phiGatNuoc);
	drawObject(vao_xeTai_GatNuoc, buffer_xeTai_GatNuoc, model2, xetai_gatnuoc_size);

	// Cua phai
	mat4 model3 = model * Translate(-0.7583, 1.1, 3.8628) * RotateY(phiCuaPhai);
	drawObject(vao_xeTai_Cua, buffer_xeTai_Cua, model3, xetai_cua_size);

	mat4 model31 = model3 * Translate(0, dKinhPhai, 0);
	drawObject(vao_xeTai_Kinh, buffer_xeTai_Kinh, model31, xetai_kinh_size);

	model31 = model3 * Translate(-0.038, -0.1, -0.6626) * RotateX(17.75) * RotateZ(phiTayNamPhai);
	drawObject(vao_xeTai_TayNam, buffer_xeTai_TayNam, model31, xetai_taynam_size);

	//Cua trai
	mat4 model4 = model * Translate(0.7583, 1.1, 3.8628) * RotateY(phiCuaTrai);
	drawObject(vao_xeTai_Cua, buffer_xeTai_Cua, model4 * Scale(-1, 1, 1), xetai_cua_size);

	mat4 model41 = model4 * Translate(0, dKinhTrai, 0);
	drawObject(vao_xeTai_Kinh, buffer_xeTai_Kinh, model41 * Scale(-1, 1, 1), xetai_kinh_size);

	model41 = model4 * Translate(0.038, -0.1, -0.6626) * RotateX(17.75) * RotateZ(phiTayNamTrai);
	drawObject(vao_xeTai_TayNam, buffer_xeTai_TayNam, model41 * Scale(-1, 1, 1), xetai_taynam_size, true);

	// Banh dau phai
	vec4 tmpAmbient = vec4(0.500000, 0.500000, 0.500000, 1);
	vec4 tmpDiffuse = vec4(0.084376, 0.084376, 0.084376, 1);
	vec4 tmpSpecular = vec4(0.300000, 0.300000, 0.300000, 1);;
	mat4 model5 = model * Translate(-0.71, 0.2, 3.025) * RotateY(phiBanhTruoc) * RotateX(phiBanh) * RotateY(90);
	drawObject(vao_xeTai_BanhXe, buffer_xeTai_BanhXe, model5, xetai_banhxe_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);

	// Banh dau trai
	mat4 model6 = model * Translate(0.71, 0.2, 3.025) * RotateY(phiBanhTruoc) * RotateX(phiBanh) * RotateY(90);
	drawObject(vao_xeTai_BanhXe, buffer_xeTai_BanhXe, model6, xetai_banhxe_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);

	// Banh sau
	mat4 model7 = model * Translate(-0.71, 0.2, 1.73) * RotateX(phiBanh) * RotateY(90);
	drawObject(vao_xeTai_BanhXe, buffer_xeTai_BanhXe, model7, xetai_banhxe_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);

	model7 = model * Translate(0.71, 0.2, 1.73) * RotateX(phiBanh) * RotateY(90);
	drawObject(vao_xeTai_BanhXe, buffer_xeTai_BanhXe, model7, xetai_banhxe_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);

	model7 = model * Translate(-0.71, 0.2, -1.59) * RotateX(phiBanh) * RotateY(90);
	drawObject(vao_xeTai_BanhXe, buffer_xeTai_BanhXe, model7, xetai_banhxe_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);

	model7 = model * Translate(0.71, 0.2, -1.59) * RotateX(phiBanh) * RotateY(90);
	drawObject(vao_xeTai_BanhXe, buffer_xeTai_BanhXe, model7, xetai_banhxe_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);

	model7 = model * Translate(-0.71, 0.2, -2.54) * RotateX(phiBanh) * RotateY(90);
	drawObject(vao_xeTai_BanhXe, buffer_xeTai_BanhXe, model7, xetai_banhxe_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);

	model7 = model * Translate(0.71, 0.2, -2.54) * RotateX(phiBanh) * RotateY(90);
	drawObject(vao_xeTai_BanhXe, buffer_xeTai_BanhXe, model7, xetai_banhxe_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);

}

void customCube(int xC, int zC, bool isCustomMaterial = false, vec4 CustomMaterialAmbient = vec4(), vec4 CustomMaterialDiffuse = vec4(), vec4 CustomMaterialSpecular = vec4())
{
	for (float x = -xC * 2 + 2; x <= xC * 2 - 2; x += 4) {
		for (float z = -zC / 2 + 0.5; z <= zC / 2 - 0.5; z++) {
			instance = model * Translate(x, -0.75, z) * Scale(4, 1, 1);
			drawObject(vao_xeTai_Cube, buffer_xeTai_Cube, instance, xetai_cube_size, false, false, false, isCustomMaterial, CustomMaterialAmbient, CustomMaterialDiffuse, CustomMaterialSpecular);
		}
	}
}

void san() {
	model = identity();
	customCube(14, 40, true, vec4(0.8, 0.8, 0.8, 1), vec4(0.2, 0.2, 0.2, 1), vec4(0.12, 0.12, 0.12, 1));
	model = Translate(-6, 0.1, 0);
	customCube(2, 40, true, vec4(0.55, 0.55, 0.55, 1), vec4(0.1, 0.1, 0.1, 1), vec4(0.05, 0.05, 0.05, 1));
	
}

string intToString(int n) {
	if (n == 0)
		return "0";
	string result = "";
	while (n > 0) {
		int tmp = n % 10;
		n = n / 10;
		result = (char)(tmp + 48) + result;
	}
	return result;
}

void canhQuat(mat4 model) {
	vec4 tmpAmbient = vec4(0.6, 0.6, 0.6, 1);
	vec4 tmpDiffuse = vec4(0.5, 0.5, 0.5, 1);
	vec4 tmpSpecular = vec4(0.3, 0.3, 0.3, 1);
	instance = Translate(0, 0, 1) * RotateZ(phiCanh) * RotateY(-20);
	drawObject(vao_coiXayCanhQuat, buffer_coiXayCanhQuat, model * instance, coixaycanhquat_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);
	instance = Translate(0, 0, 1) * RotateZ(120 + phiCanh) * RotateY(-20);
	drawObject(vao_coiXayCanhQuat, buffer_coiXayCanhQuat, model * instance, coixaycanhquat_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);
	instance = Translate(0, 0, 1) * RotateZ(240 + phiCanh) * RotateY(-20);
	drawObject(vao_coiXayCanhQuat, buffer_coiXayCanhQuat, model * instance, coixaycanhquat_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);
}

void coiXay() {
	vec4 tmpAmbient = vec4(0.6, 0.6, 0.6, 1);
	vec4 tmpDiffuse = vec4(0.5, 0.5, 0.5, 1);
	vec4 tmpSpecular = vec4(0.3, 0.3, 0.3, 1);
	instance = model;
	drawObject(vao_coiXayThan, buffer_coiXayThan, instance, coixaythan_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);
	instance = instance * Translate(0, 0, 16) * RotateX(90);
	drawObject(vao_coiXayTuabin, buffer_coiXayTuabin, instance, coixaytuabin_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);

	canhQuat(instance);
}

void battery(float phiY, float phiZ) {
	instance = RotateY(phiY) * RotateZ(phiZ) * Scale(1.8, 1.8, 1.8);
	drawObject(vao_battery, buffer_battery, model * instance, battery_size);
}

void thanBattery() {
	instance = Scale(1.8, 1.8, 1.8);
	drawObject(vao_than_battery, buffer_than_battery, model * instance, than_battery_size);
}

void fullBattery(float x, float y, float z) {
	model = Translate(x, y, z);
	thanBattery();

	vec3 dir = vec3(xL, yL, zL) - vec3(x, y, z);
	float phiY = acos(dot(normalize(vec3(dir.x, 0, dir.z)), vec3(1, 0, 0))) / DegreesToRadians;
	phiY = isnan(phiY) ? 0 : phiY;
	phiY = normalize(vec3(dir.x, 0, dir.z)).z > 0 ? -phiY : phiY;

	vec4 tmp = RotateY(-phiY) * vec4(dir, 1);
	float phiZ = acos(dot(normalize(vec3(tmp.x, tmp.y, 0)), vec3(0, 1, 0))) / DegreesToRadians;
	phiZ = normalize(vec3(tmp.x, tmp.y, 0)).x > 0 ? -phiZ : phiZ;
	phiZ = isnan(phiZ) ? 0 : phiZ;
	phiZ = phiZ > 30 ? 30 : phiZ;
	phiZ = phiZ < -30 ? -30 : phiZ;

	battery(phiY, phiZ);
}

void batteries() {
	fullBattery(-15, 1.2, 16);
	fullBattery(-15, 1.2, 8);
	fullBattery(-15, 1.2, 0);
	fullBattery(-15, 1.2, -8);
	fullBattery(-15, 1.2, -16);

	fullBattery(-25, 1.2, 16);
	fullBattery(-25, 1.2, 8);
	fullBattery(-25, 1.2, 0);
	fullBattery(-25, 1.2, -8);
	fullBattery(-25, 1.2, -16);
}

void cay(float scale)
{
	model = model * Scale(scale);
	drawObject(vao_than, buffer_than, model, than_size);

	mat4 model1 = model * Translate(0.19714, 3.7358, 0.38079) * RotateZ(phi1);
	drawObject(vao_canh1, buffer_canh1, model1, canh1_size);

	mat4 model11 = model1 * Translate(-0.19714, 3.6944, 2.71327);
	drawObject(vao_la, buffer_la, model11, la_size);

	mat4 model12 = model1 * Translate(-1.68724, 4.2678, 1.67161) * Scale(0.6, 0.6, 0.6);
	drawObject(vao_la, buffer_la, model12, la_size);

	mat4 model2 = model * Translate(-0.03071, 3.598, 0.2301) * RotateZ(phi2);
	drawObject(vao_canh2, buffer_canh2, model2, canh2_size);

	mat4 model21 = model2 * Translate(-2.25326, 3.97352, -0.05575);
	drawObject(vao_la, buffer_la, model21, la_size);

	mat4 model22 = model2 * Translate(-1.58299, 2.9345, 0.71943) * Scale(0.6, 0.6, 0.6);
	drawObject(vao_la, buffer_la, model22, la_size);

	mat4 model3 = model * Translate(0.49338, 4.682, 0.30035) * RotateZ(phi3);
	drawObject(vao_canh3, buffer_canh3, model3, canh3_size);

	mat4 model31 = model3 * Translate(1.48449, 2.77179, -2.40997);
	drawObject(vao_la, buffer_la, model31, la_size);

	mat4 model32 = model3 * Translate(0.34433, 2.1116, -1.71305 ) * Scale(0.6, 0.6, 0.6);
	drawObject(vao_la, buffer_la, model32, la_size);
	
	mat4 model4 = model * Translate(0.14128, 9.29038, 0.7865) * Translate(dx, 0, 0);
	drawObject(vao_la, buffer_la, model4, la_size);
}

void animationCay(GLfloat& phi, GLfloat& huong)
{
	if (isGio && cayTimer >= maxTime) {
		if (phi < -doLech || phi > doLech)
			huong *= -1;
		phi += 2 * huong;
		dx += -0.06 * huong1;
	}
}

void trees() {
	mat4 model1 = Translate(10, -0.2, -4);
	model = model1 * Translate(0, 0, 0);
	cay(0.3);
	model = model1 * Translate(2, 0, 1);
	cay(0.36);
	model = model1 * Translate(2.5, 0, -1);
	cay(0.31);
	model = model1 * Translate(1, 0, 2.5);
	cay(0.33);
	model = model1 * Translate(4, 0, 5);
	cay(0.28);
	model = model1 * Translate(6, 0, 8);
	cay(0.31);
	model = model1 * Translate(2, 0, 7);
	cay(0.34);
	model = model1 * Translate(10, 0, 2);
	cay(0.26);
	model = model1 * Translate(12, 0, 10);
	cay(0.29);
	model = model1 * Translate(6, 0, 20);
	cay(0.28);
	model = model1 * Translate(4.5, 0, 18);
	cay(0.31);
	model = model1 * Translate(8, 0, 19);
	cay(0.29);
}

void house(float scale)
{
	//khung nhà
	model = Translate(8, 0, -10) * Scale(scale) * RotateY(90);
	drawObject(vao_house, buffer_house, model, house_size);

	//cửa chính
	mat4 model1 = model * Translate(-0.3, 7.6558, 10.25) * RotateY(-145.45) * RotateX(90) * RotateZ(phiCuaChinh) * Translate(2.4, 0, 0);
	drawObject(vao_cuachinh, buffer_cuachinh, model1, cuachinh_size);
	model1 = model1 * Translate(1.5, 0.25, 1.5) * RotateX(-90) * RotateZ(phiTayNamCuaChinh) * Translate(-0.158, 0, 0);
	drawObject(vao_tayNamCuaChinh, buffer_tayNamCuaChinh, model1, taynamcuachinh_size);

	//cửa sổ 1
	mat4 model2 = model * Translate(0, phiCuaSo1, 0) * Translate(-10.7295, 4.9043, 22.6862);
	drawObject(vao_cuaSo1, buffer_cuaSo1, model2, cuaso1_size);

	//cái ghế
	mat4 model3 = model * Translate(phiCaiGhe, 0, 0) * Translate(-7.3154, 0, 22.819) * RotateY(phiCaiGheQuay);
	drawObject(vao_caighe, buffer_caighe, model3, caighe_size);

	//tủ lạnh
	mat4 model4 = model * Translate(13.891, 0.54193, 12.708);
	drawObject(vao_khungfridge, buffer_khungfridge, model4, khungfridge_size);
	model4 = model4 * Translate(-2.3, 0, 2.6) * RotateY(phiCanhTuLanh1) * Translate(2.3, 0, -2.6);
	drawObject(vao_canhtulanh1, buffer_canhtulanh1, model4, canhtulanh1_size);
	mat4 model5 = model * Translate(phiNganTuLanh1, 0, 0) * Translate(13.915, 0.54193, 12.708);
	drawObject(vao_ngantulanh1, buffer_ngantulanh1, model5, ngantulanh1_size);
	model4 = model4 * RotateY(phiCanhTuLanh2);
	drawObject(vao_canhtulanh2, buffer_canhtulanh2, model4, canhtulanh2_size);
	//model5 = model * Translate(phiNganTuLanh2, 0, 0);
	model5 = model * Translate(phiNganTuLanh1, 0, 0) * Translate(13.915, 0.54193, 12.708);
	drawObject(vao_ngantulanh2, buffer_ngantulanh2, model5, ngantulanh2_size);

	//quạt trần
	mat4 model6 = model * Translate(2.01921, 10.411, 21.017) * RotateY(phiFan);
	drawObject(vao_fan, buffer_fan, model6, fan_size);

	//giường
	mat4 model7 = model * Translate(8.19353, 0.180542, 26.3128);
	drawObject(vao_bed, buffer_bed, model7, bed_size);
}

void den(int index, float rY)
{
	vec4 tmpAmbient = vec4(0.500000, 0.500000, 0.500000, 1);
	vec4 tmpDiffuse = vec4(0.08, 0.08, 0.08, 1);
	vec4 tmpSpecular = vec4(0.1, 0.1, 0.1, 1);
	instance = Translate(0, 2, 0) * Scale(0.15, 5, 0.15);
	drawObject(vao_xeTai_Cube, buffer_xeTai_Cube, model * instance, xetai_cube_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);

	instance = Translate(0, 4.4, 0) * RotateZ(60) * Translate(0.075, 0.3, 0) * Scale(0.1, 0.6, 0.3);
	drawObject(vao_xeTai_Cube, buffer_xeTai_Cube, model * instance, xetai_cube_size, false, false, false, true, tmpAmbient, tmpDiffuse, tmpSpecular);

	instance = Translate(0, 4.3, 0) * RotateZ(60) * Translate(0.075, 0.3, 0) * Scale(0.1, 0.5, 0.25);
	drawObject(vao_xeTai_Cube, buffer_xeTai_Cube, model * instance, xetai_cube_size, true, true, true, true, vec4(0.9, 0.7, 0.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0), vec4(1.0, 1.0, 1.0, 1.0));

	vec4 light_ambient;
	vec4 light_diffuse;
	vec4 light_specular;
	light_ambient = isLight ? vec4(0.6, 0.55, 0.0, 1.0) : vec4();
	light_diffuse = isLight ? vec4(0.7, 0.7, 0.7, 1.0) : vec4();
	light_specular = isLight ? vec4(0.8, 0.5, 0.5, 1.0) : vec4();
	spotLightPos[index] = model * Translate(-0.07, 5, 0) * RotateZ(60) * Translate(0.075, 0.2, 0) * vec4(0, 0, 0, 1.0);
	vec4 light_dir = RotateY(rY) * RotateZ(-27) * vec4(0.0, -1.0, 0.0, 1.0);
	float shininess = 100;
	float distance = 60;
	float light_cutoff = cos(45 * DegreesToRadians);

	string uSpotLightA = "SpotLights[" + intToString(index) + "].AmbientLight";
	string uSpotLightD = "SpotLights[" + intToString(index) + "].DiffuseLight";
	string uSpotLightS = "SpotLights[" + intToString(index) + "].SpecularLight";
	string uSpotLightLpos = "SpotLights[" + intToString(index) + "].LightPosition";
	string uSpotLightLdir = "SpotLights[" + intToString(index) + "].LightDirection";
	string uSpotLightShininess = "SpotLights[" + intToString(index) + "].Shininess";
	string uSpotLightDistance = "SpotLights[" + intToString(index) + "].Distance";
	string uSpotLightCutoff = "SpotLights[" + intToString(index) + "].SpotCutOff";

	glUniform4fv(glGetUniformLocation(program, uSpotLightA.c_str()), 1, light_ambient);
	glUniform4fv(glGetUniformLocation(program, uSpotLightD.c_str()), 1, light_diffuse);
	glUniform4fv(glGetUniformLocation(program, uSpotLightS.c_str()), 1, light_specular);
	glUniform4fv(glGetUniformLocation(program, uSpotLightLpos.c_str()), 1, spotLightPos[index]);
	glUniform4fv(glGetUniformLocation(program, uSpotLightLdir.c_str()), 1, light_dir);
	glUniform1f(glGetUniformLocation(program, uSpotLightShininess.c_str()), shininess);
	glUniform1f(glGetUniformLocation(program, uSpotLightDistance.c_str()), distance);
	glUniform1f(glGetUniformLocation(program, uSpotLightCutoff.c_str()), light_cutoff);
}

void PointLight(int index, float x, float y, float z, float distance)
{
	// -------------------------Tạo chiếu sáng--------------------------
	vec4 light_position = Translate(x, y, z) * vec4(0, 0, 0, 1.0);
	float material_shininess = 50;
	vec4 light_ambient(am[0], am[1], am[2], 1.0);
	vec4 light_diffuse(diff[0], diff[1], diff[2], 1.0);
	vec4 light_specular(1.0, 1.0, 1.0, 1.0);

	instance = identity() * Translate(x, y, z);
	drawObject(vao_matTroi, buffer_matTroi, instance, mattroi_size);

	string uPointLightA = "PointLights[" + intToString(index) + "].AmbientLight";
	string uPointLightD = "PointLights[" + intToString(index) + "].DiffuseLight";
	string uPointLightS = "PointLights[" + intToString(index) + "].SpecularLight";
	string uPointLpos = "PointLights[" + intToString(index) + "].LightPosition";
	string uPointLightShininess = "PointLights[" + intToString(index) + "].Shininess";
	string uPointLightDistance = "PointLights[" + intToString(index) + "].Distance";

	glUniform4fv(glGetUniformLocation(program, uPointLightA.c_str()), 1, light_ambient);
	glUniform4fv(glGetUniformLocation(program, uPointLightD.c_str()), 1, light_diffuse);
	glUniform4fv(glGetUniformLocation(program, uPointLightS.c_str()), 1, light_specular);
	glUniform4fv(glGetUniformLocation(program, uPointLpos.c_str()), 1, light_position);
	glUniform1f(glGetUniformLocation(program, uPointLightShininess.c_str()), material_shininess);
	glUniform1f(glGetUniformLocation(program, uPointLightDistance.c_str()), distance);
}

void SpotLight()
{
	int cnt = 2;
	int index = 0;
	for (int i = -cnt; i <= cnt; i++) {
		model = Translate(4.5 - 6, 0, i * 9) * RotateY(0);
		den(index * 2, 0);
		if (i != cnt) {
			model = Translate(-4.5 - 6, 0, i * 9 + 4.5) * RotateY(180);
			den(index * 2 + 1, 180);
		}
		index++;
	}
}

void MultiLight() {
	PointLight(0, xL, yL, zL, 500);
	SpotLight();
}

void Eye()
{
	// -------------------------Tạo mắt nhìn--------------------------
	vec4 at = eye + normalize(mat_dinhhuong);
	eye.w = at.w = 1;
	vec4 up(0, 1, 0, 1);
	mat4 view = LookAt(eye, at, up);
	glUniformMatrix4fv(view_loc, 1, GL_TRUE, view);

	// -------------------------Tạo khung nhìn--------------------------
	mat4 projection = Frustum(l, r, b, t, zN, zF);
	glUniformMatrix4fv(projection_loc, 1, GL_TRUE, projection);
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	Eye();
	MultiLight();

	model = Translate(-6, 0, 0);
	xeTai();

	san();

	if (isGio) {
		phiCanh -= 0.9;
		if (phiCanh < 0) phiCanh += 360;
	}
	model = Translate(4, 0, 15) * RotateY(-90) * RotateX(-90);
	coiXay();
	model = Translate(4, 0, -15) * RotateY(-90) * RotateX(-90);
	coiXay();

	batteries();

	cayTimer += 1;
	animationCay(phi1, huong1);
	animationCay(phi2, huong2);
	animationCay(phi3, huong3);
	if (cayTimer >= maxTime)
		cayTimer = 0;
	trees();

	phiFan += 0.4;
	if (phiFan > 360)
		phiFan = -360;
	house(0.25);

	glutSwapBuffers();
}
#pragma endregion

#pragma region Keyboard, Điều khiển camera

GLfloat speedTranslate = 0.3;
GLfloat speedRotate = 5;

enum Controller {
	eyeController, sunController, carController, houseController, airController, lightController
};

Controller controller = Controller::eyeController;

void keyboard(unsigned char key, int x, int y)
{
	// keyboard handler

	switch (key) {
	case 033:			// 033 is Escape key octal value
		exit(1);		// quit program
		break;
		// Chỉnh tọa độ của mắt
	case '1':
		controller = Controller::eyeController;
		break;
	case '2':
		controller = Controller::sunController;
		break;
	case '3':
		controller = Controller::carController;
		break;
	case '4':
		controller = Controller::houseController;
		break;
	case '5':
		controller = Controller::airController;
		break;
	case '6':
		controller = Controller::lightController;
		break;
	case 'o':
		if (controller == Controller::sunController) {
			xL += 1;
		}
		else if (controller == Controller::carController) {
			speed = 0;
		}
		break;
	case 'u':
		if (controller == Controller::sunController) {
			xL -= 1;
		}
		break;
	case 'i':
		if (controller == Controller::sunController) {
			yL += 1;
		}
		else if (controller == Controller::carController) {
			speed = 0.01;
		}
		else if (controller == Controller::eyeController) {
			matXY[0] -= speedRotate;
			if (matXY[0] < -75) {
				matXY[0] = -75;
			}
		}
		break;
	case 'k':
		if (controller == Controller::sunController) {
			yL -= 1;
		}
		else if (controller == Controller::carController) {
			speed = -0.01;
		}
		else if (controller == Controller::eyeController) {
			matXY[0] += speedRotate;
			if (matXY[0] > 75) {
				matXY[0] = 75;
			}
		}
		break;
	case 'j':
		if (controller == Controller::sunController) {
			zL -= 1;
		}
		else if (controller == Controller::carController) {
			if (phiBanhTruoc < 30) phiBanhTruoc += 5;
		}
		else if (controller == Controller::eyeController) {
			matXY[1] += speedRotate;
			if (matXY[1] >= 360) matXY[1] -= 360;
			if (matXY[1] < 0) matXY[1] += 360;
		}
		break;
	case 'l':
		if (controller == Controller::sunController) {
			zL += 1;
		}
		else if (controller == Controller::carController) {
			if (phiBanhTruoc > -30) phiBanhTruoc -= 5;
		}
		else if (controller == Controller::eyeController) {
			matXY[1] -= speedRotate;
			if (matXY[1] >= 360) matXY[1] -= 360;
			if (matXY[1] < 0) matXY[1] += 360;
		}
		break;
	case 'w':
		if (controller == Controller::eyeController) {
			eye = eye + RotateY(matXY[1]) * vec4(0.0, 0.0, speedTranslate, 1);
		}
		break;
	case 's':
		if (controller == Controller::eyeController) {
			eye = eye - RotateY(matXY[1]) * vec4(0.0, 0.0, speedTranslate, 1);
		}
		break;
	case 'd':
		if (controller == Controller::eyeController) {
			eye = eye - RotateY(matXY[1]) * vec4(speedTranslate, 0.0, 0.0, 1);
		}
		break;
	case 'a':
		if (controller == Controller::eyeController) {
			eye = eye + RotateY(matXY[1]) * vec4(speedTranslate, 0.0, 0.0, 1);
		}
		break;
	case 'q':
		if (controller == Controller::eyeController) {
			eye = eye - RotateY(matXY[1]) * vec4(0.0, speedTranslate, 0.0, 1);
		}
		else if (controller == Controller::airController) {
			isGio = !isGio;
		}
		else if (controller == Controller::lightController) {
			isLight = !isLight;
		}
		break;
	case 'e':
		if (controller == Controller::eyeController) {
			eye = eye + RotateY(matXY[1]) * vec4(0.0, speedTranslate, 0.0, 1);
		}
		break;
	case 'r':
		if (controller == Controller::carController) {
			if (phiTrucGuong2 < 0)	phiTrucGuong2 += 5;
		}
		break;
	case 'R':
		if (controller == Controller::carController) {
			if (phiTrucGuong2 > -135)	phiTrucGuong2 -= 5;
		}
		break;
	case 't':
		if (controller == Controller::carController) {
			if (phiGuong < 135) phiGuong += 5;
		}
		else if (controller == Controller::houseController) {
			if (phiCanhTuLanh1 <= 90)
				phiCanhTuLanh1 += 5;
		}
		break;
	case 'T':
		if (controller == Controller::carController) {
			if (phiGuong > 0) phiGuong -= 5;
		}
		else if (controller == Controller::houseController) {
			if (phiCanhTuLanh1 > 0)
				phiCanhTuLanh1 -= 5;
		}
		break;
	case 'y':
		if (controller == Controller::carController) {
			isGatNuoc = !isGatNuoc;
		}
		else if (controller == Controller::houseController) {
			if (phiNganTuLanh1 >= -2.05)
				phiNganTuLanh1 -= 0.35;
		}
		break;
	case 'Y':
		if (controller == Controller::houseController) {
			if (phiNganTuLanh1 < 0)
				phiNganTuLanh1 += 0.35;
		}
		break;
	case 'z':
		if (controller == Controller::carController) {
			if (phiTayNamTrai < 45) phiTayNamTrai += 5;
		}
		else if (controller == Controller::houseController) {
			if (phiCuaChinh <= 90)
				phiCuaChinh += 5;
		}
		break;
	case 'Z':
		if (controller == Controller::carController) {
			if (phiTayNamTrai > 0) phiTayNamTrai -= 5;
		}
		else if (controller == Controller::houseController) {
			if (phiCuaChinh > 0)
				phiCuaChinh -= 5;
		}
		break;
	case 'b':
		if (controller == Controller::carController) {
			if (phiTayNamPhai > -45) phiTayNamPhai -= 5;
		}
		else if (controller == Controller::houseController) {
			phiCaiGhe += 0.35;
		}
		break;
	case 'B':
		if (controller == Controller::carController) {
			if (phiTayNamPhai < 0) phiTayNamPhai += 5;
		}
		else if (controller == Controller::houseController) {
			phiCaiGhe -= 0.35;
		}
		break;
	case 'x':
		if (controller == Controller::carController) {
			if (phiCuaTrai > -60 && phiTayNamTrai > 30) phiCuaTrai -= 5;
			else if (phiCuaTrai <= 0) phiTayNamTrai = 0;
		}
		break;
	case 'X':
		if (controller == Controller::carController) {
			if (phiCuaTrai < 0) phiCuaTrai += 5;
		}
		break;
	case 'n':
		if (controller == Controller::carController) {
			if (phiCuaPhai < 60 && phiTayNamPhai < -30) phiCuaPhai += 5;
		}
		else if (controller == Controller::houseController) {
			phiCaiGheQuay += 5;
		}
		break;
	case 'N':
		if (controller == Controller::carController) {               
			if (phiCuaPhai > 0) phiCuaPhai -= 5;
			else if (phiCuaPhai <= 0) phiTayNamPhai = 0;
		}
		else if (controller == Controller::houseController) {
			phiCaiGheQuay -= 5;
		}
		break;
	case 'c':
		if (controller == Controller::carController) {
			if (dKinhTrai > -0.38) dKinhTrai -= 0.05;
		}
		else if (controller == Controller::houseController) {
			if (phiTayNamCuaChinh <= 90)
				phiTayNamCuaChinh += 5;
		}
		break;
	case 'C':
		if (controller == Controller::carController) {
			if (dKinhTrai < -0.05) dKinhTrai += 0.05;
		}
		else if (controller == Controller::houseController) {
			if (phiTayNamCuaChinh > 0)
				phiTayNamCuaChinh -= 5;
		}
		break;
	case 'm':
		if (controller == Controller::carController) {
			if (dKinhPhai > -0.38) dKinhPhai -= 0.05;
		}
		break;
	case 'M':
		if (controller == Controller::carController) {
			if (dKinhPhai < -0.05) dKinhPhai += 0.05;
		}
		break;
	case 'v':
		if (controller == Controller::houseController) {
			if (phiCuaSo1 >= -2.05)
				phiCuaSo1 -= 0.35;
		}
		break;
	case 'V':
		if (controller == Controller::houseController) {
			if (phiCuaSo1 < 0)
				phiCuaSo1 += 0.35;
		}
		break;
	}

	mat_dinhhuong = RotateY(matXY[1]) * RotateX(matXY[0]) * vec4(0, 0, 1, 1);
	mat_dinhhuong.w = 1;


	glutPostRedisplay();
}

#pragma endregion

#pragma region Main

int main(int argc, char** argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
	glutInitWindowSize(850, 800);
	glutInitWindowPosition(0, 0);
	glutCreateWindow("Canh Dong Gio");


	glewInit();

	shaderSetup();

	glutDisplayFunc(display);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(display);

	glutMainLoop();
	return 0;
}

#pragma endregion
