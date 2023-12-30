#version 400
in vec4 vPosition;
in vec4 vColor;
in vec3 vNormal;
out vec4 color;

in vec4 MaterialAmbient;
in vec4 MaterialDiffuse;
in vec4 MaterialSpecular;

// Point light
const int numPointLight = 1;
struct PointLight {
	vec4 AmbientLight, DiffuseLight, SpecularLight;
	vec4 LightPosition;
	float Shininess;
	float Distance;
};

uniform PointLight PointLights[numPointLight];

// Spot light
const int numSpotLight = 9;
struct SpotLight {
	vec4 AmbientLight, DiffuseLight, SpecularLight;
	vec4 LightPosition;
	vec4 LightDirection;
	float Shininess;
	float Distance;
	float SpotCutOff;
};

uniform SpotLight SpotLights[numSpotLight];

uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform bool FlipX;
uniform bool FlipY;
uniform bool FlipZ;

uniform bool IsCustomMaterial;
uniform vec4 CustomMaterialAmbient, CustomMaterialDiffuse, CustomMaterialSpecular;

vec4 ambient, diffuse, specular;

vec4 calculatePointLight(vec4 ambientL, vec4 diffuseL, vec4 specularL, vec4 Lpos, float Shininess, float Distance, float itensity)
{
	float x = FlipX ? -vNormal.x : vNormal.x;
	float y = FlipY ? -vNormal.y : vNormal.y;
	float z = FlipZ ? -vNormal.z : vNormal.z;
	vec3 newVNormal = vec3(x, y, z);

	vec4 finalMaterialAmbient = IsCustomMaterial ? CustomMaterialAmbient : MaterialAmbient;
	vec4 finalMaterialDiffuse = IsCustomMaterial ? CustomMaterialDiffuse : MaterialDiffuse;
	vec4 finalMaterialSpecular = IsCustomMaterial ? CustomMaterialSpecular : MaterialSpecular;

	// Transform vertex position into eye coordinates
	vec3 pos = (View * Model * vPosition).xyz;
	vec3 L = normalize( (View * Lpos).xyz - pos );

	float dis = length((View * Lpos).xyz - pos);
	if (dis <= Distance) {
		float newItensity = itensity * (Distance - dis) / Distance;
		
		vec3 E = normalize( -pos );
		vec3 H = normalize(L+E);
	
		// Transform vertex normal into eye coordinates
		vec3 N = normalize( View * Model * vec4(newVNormal, 0.0) ).xyz; 
	
		vec4 AmbientProduct = ambientL * finalMaterialAmbient * newItensity;
		vec4 DiffuseProduct = diffuseL * finalMaterialDiffuse * newItensity;
		vec4 SpecularProduct = specularL * finalMaterialSpecular * newItensity;


		// Compute terms in the illumination equation
		ambient = AmbientProduct;
		float Kd = max( dot(L, N), 0.0 );
		diffuse = Kd * DiffuseProduct;
		float Ks = pow( max(dot(N, H), 0.0), Shininess );
		specular = Ks * SpecularProduct * 2;
		if( dot(L, N) < 0.0 ) specular = vec4(0.0, 0.0, 0.0, 1.0);
		return ambient + diffuse + specular;
	}
	return vec4(0.0, 0.0, 0.0, 1.0);
}

vec4 calculateSpotLight(SpotLight spotLight)
{
	vec3 pos = (Model * vPosition).xyz;
	vec3 LightToPixel = normalize(pos - (spotLight.LightPosition).xyz);
    float SpotFactor = dot(LightToPixel, normalize((spotLight.LightDirection).xyz));
	if (SpotFactor > spotLight.SpotCutOff) {
		float itensity = (1.0 - (1.0 - SpotFactor)/(1.0 - spotLight.SpotCutOff)) * 2 / 4;
		return calculatePointLight(spotLight.AmbientLight, spotLight.DiffuseLight, spotLight.SpecularLight, spotLight.LightPosition, spotLight.Shininess, spotLight.Distance, itensity);
	}
	return vec4(0, 0, 0, 0);
}

void main()
{	
//Mo hinh chieu sang Blinn - Phong (Phong sua doi)
	color = vec4(0.0);

	for (int i = 0; i < numPointLight; i++) {
		color += calculatePointLight(PointLights[i].AmbientLight, PointLights[i].DiffuseLight, PointLights[i].SpecularLight, PointLights[i].LightPosition, PointLights[i].Shininess, PointLights[i].Distance, 1.0);
	}
	
	for (int i = 0; i < numSpotLight; i++) {
		color += calculateSpotLight(SpotLights[i]);
	}
	
	color.a = 1.0;

    gl_Position = Projection * View * Model * vPosition/vPosition.w;
		
}//