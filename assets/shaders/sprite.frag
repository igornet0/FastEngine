#version 120

varying vec2 TexCoord;

uniform sampler2D uTexture;
uniform vec4 uColor;
uniform bool uUseTexture;

void main() {
    if (uUseTexture) {
        gl_FragColor = texture2D(uTexture, TexCoord) * uColor;
    } else {
        gl_FragColor = uColor;
    }
}