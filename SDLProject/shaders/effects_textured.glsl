uniform sampler2D diffuse;

varying vec2 texCoordVar;

void main() {
    vec4 color = texture2D(diffuse, texCoordVar);
    gl_FragColor = vec4(color.r,color.g, color.b,color.a);//texture2D(diffuse, texCoordVar);
    
    //SATURATION
    //vec3 luminance = vec3(dot(vec3(0.2126, 0.7152, 0.0722), color.rgb));
    //vec3 m = mix(luminance, color.rgb, 1.0); changing the last value will change saturation (less = more grayscale)
    
    //LUMINANCE GRAYSCALE
    //vec3 luminance = vec3(dot(vec3(0.2126, 0.7152, 0.0722), color.rgb)); //luminance (more realistic grayscale)
    //gl_FragColor = vec4(luminance, color.a);
    
    //GRAYSCALE
    //float c = (color.r + color.g + color.b) / 3.0; //grayscale
    //gl_GragColor = vec4(c, c, c, color.a); //grayscale
    
    //DARKENING: divide the color by a float (e.g. 1.0-5.0)
    
    //INVERTING: do 1 - color.x for each value
    
}
