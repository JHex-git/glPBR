#version 460 core
out vec4 FragColor;
in vec2 TexCoords;
in vec3 WorldPos;
in vec3 Normal;

// material parameters
uniform sampler2D albedoMap1;
uniform sampler2D normalMap1;
uniform sampler2D metallicMap1;
// uniform sampler2D roughnessMap1;
// uniform sampler2D aoMap1;
// uniform sampler2D emissiveMap1;

// IBL
uniform samplerCube irradianceMap;
uniform samplerCube prefilterMap;
uniform sampler2D brdfLUT;

uniform vec3 camPos;

const float PI = 3.14159265359;
vec3 getNormalFromMap()
{
    vec3 tangentNormal = texture(normalMap1, TexCoords).xyz * 2.0 - 1.0;

    vec3 Q1  = dFdx(WorldPos);
    vec3 Q2  = dFdy(WorldPos);
    vec2 st1 = dFdx(TexCoords);
    vec2 st2 = dFdy(TexCoords);

    vec3 N   = normalize(Normal);
    vec3 T  = normalize(Q1*st2.t - Q2*st1.t);
    vec3 B  = -normalize(cross(N, T));
    mat3 TBN = mat3(T, B, N);

    return normalize(TBN * tangentNormal);
}

vec3 fresnelSchlickRoughness(float cosTheta, vec3 F0, float roughness)
{
    return F0 + (max(vec3(1.0 - roughness), F0) - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}

void main()
{		
    // material properties
    vec3 albedo = pow(texture(albedoMap1, TexCoords).rgb, vec3(2.2));
    float metallic = texture(metallicMap1, TexCoords).b;
    float roughness = texture(metallicMap1, TexCoords).g;
    // float ao = texture(aoMap1, TexCoords).r;
    // vec3 emissive = texture(emissiveMap1, TexCoords).rgb;
       
    // input lighting data
    vec3 N = getNormalFromMap();
    vec3 V = normalize(camPos - WorldPos);
    vec3 R = reflect(-V, N); 

    // calculate reflectance at normal incidence; if dia-electric (like plastic) use F0 
    // of 0.04 and if it's a metal, use the albedo color as F0 (metallic workflow)    
    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    // reflectance equation
    vec3 Lo = vec3(0.0);
    // ambient lighting (we now use IBL as the ambient term)
    vec3 F = fresnelSchlickRoughness(max(dot(N, V), 0.0), F0, roughness);
    
    vec3 kS = F;
    vec3 kD = 1.0 - kS;
    kD *= 1.0 - metallic;	  
    
    vec3 irradiance = texture(irradianceMap, N).rgb;
    vec3 diffuse = irradiance * albedo;
    
    // sample both the pre-filter map and the BRDF lut and combine them together as per the Split-Sum approximation to get the IBL specular part.
    const float MAX_REFLECTION_LOD = 4.0;
    vec3 prefilteredColor = textureLod(prefilterMap, R,  roughness * MAX_REFLECTION_LOD).rgb;    
    vec3 brdf  = texture(brdfLUT, vec2(max(dot(N, V), 0.0), roughness)).rgb;
    vec3 specular = prefilteredColor * (F * brdf.r + brdf.g);

    // vec3 ambient = (kD * diffuse + specular) * ao;
    vec3 ambient = kD * diffuse + specular;
    
    vec3 color = ambient + Lo;
    // vec3 color = ambient + Lo + pow(emissive, vec3(2.2));

    // HDR tonemapping
    color = color / (color + vec3(1.0));
    // gamma correct
    color = pow(color, vec3(1.0/2.2)); 

    FragColor = vec4(color , 1.0);
}