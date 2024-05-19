#version 460 core

struct DirectionalLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Spotlight {
    vec3 position;
    vec3 direction;
    float inner;
    float outer;
    float constant;
    float linear;
    float quadratic;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct AmbientLight {
    vec3 color;
    float intensity;
};

in vec2 UV;
in vec3 Position_worldspace;
in vec3 Normal_cameraspace;
in vec3 EyeDirection_cameraspace;
in vec3 LightDirection_cameraspace;

out vec4 color;

uniform sampler2D myTextureSampler;
uniform mat4 MV;
uniform vec3 LightPosition_worldspace;
uniform float Alpha;
uniform DirectionalLight directionalLights[5];
uniform AmbientLight ambientLights[5];
uniform Spotlight spotlights[5];

void main(){

	vec3 LightColor = vec3(1,1,1);
	float LightPower = 10.0f;
	vec3 suma = vec3(0.0);

	// Material properties
	vec3 MaterialDiffuseColor = texture( myTextureSampler, UV ).rgb;
	vec3 MaterialAmbientColor = vec3(0.1,0.1,0.1) * MaterialDiffuseColor;
	vec3 MaterialSpecularColor = vec3(0.3,0.3,0.3);

	float distance = length( LightPosition_worldspace - Position_worldspace );

	vec3 n = normalize( Normal_cameraspace );
	vec3 l = normalize( LightDirection_cameraspace );
	float cosTheta = clamp( dot( n,l ), 0,1 );

	vec3 E = normalize(EyeDirection_cameraspace);
	vec3 R = reflect(-l,n);
	float cosAlpha = clamp( dot( E,R ), 0,1 );

	suma = 		MaterialAmbientColor +
           		MaterialDiffuseColor * LightColor * LightPower * cosTheta / (distance*distance) +
           		MaterialSpecularColor * LightColor * LightPower * pow(cosAlpha,5) / (distance*distance);


    for (int i = 0; i < 5; i++) {
        if (directionalLights[i].diffuse == vec3(0.0f)){
         continue;
        }
        vec3 n = normalize( Normal_cameraspace );
        vec3 lightDir = normalize(-directionalLights[i].direction);
        vec3 halfwayDir = normalize(lightDir + EyeDirection_cameraspace);
        float diff = max(dot(n, lightDir), 0.0);
        float spec = max(dot(n, halfwayDir), 0.0);
        vec3 ambient = directionalLights[i].ambient * MaterialDiffuseColor;
        vec3 diffuse = directionalLights[i].diffuse * diff * MaterialDiffuseColor;
        vec3 specular = directionalLights[i].specular * spec * MaterialSpecularColor;
        suma += (ambient + diffuse + specular)/10;
    }
    for (int i = 0; i < 5; i++) {
        if (spotlights[i].diffuse == vec3(0.0f)){
          continue;
        }
            vec3 n = normalize( Normal_cameraspace );
            vec3 lightDir = normalize(spotlights[i].position - Position_worldspace);
            vec3 halfwayDir = normalize(lightDir + EyeDirection_cameraspace);

            float diff = max(dot(n, lightDir), 0.0);

            float spec = max(dot(n, halfwayDir), 0.0);

            float distance = length(spotlights[i].position - Position_worldspace);
            float attenuation = 1.0 / (spotlights[i].constant + spotlights[i].linear * distance + spotlights[i].quadratic * (distance * distance));

            float theta = dot(lightDir, normalize(-spotlights[i].direction));
            float epsilon = spotlights[i].inner - spotlights[i].outer;
            float intensity = clamp((theta - spotlights[i].outer) / epsilon, 0.0, 1.0);

            vec3 ambient = spotlights[i].ambient * MaterialDiffuseColor;
            vec3 diffuse = spotlights[i].diffuse * diff * MaterialDiffuseColor;
            vec3 specular = spotlights[i].specular * spec * MaterialSpecularColor;

            ambient *= attenuation * intensity;
            diffuse *= attenuation * intensity;
            specular *= attenuation * intensity;

            suma += (ambient + diffuse + specular);
    }
    suma += ambientLights[0].color * ambientLights[0].intensity * MaterialDiffuseColor;



	color.rgb = suma;
	color.a = Alpha;
}