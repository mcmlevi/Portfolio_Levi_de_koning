#version 330 core
out vec4 FragColor;
  
in vec2 TexCoords;

uniform sampler2D screenTexture;
uniform float exposure = 0.1;

void main()
{ 
    const float gamma = 2.2;
    vec3 hdrColor = texture(screenTexture, TexCoords).rgb;
  
    // exposure tone mapping
    //vec3 mapped = vec3(1.0) - exp(-hdrColor * exposure);
    
    // gamma correction 
    vec3 mapped = pow(hdrColor, vec3(1.0 / gamma));

    FragColor = vec4(mapped, 1.0);
}