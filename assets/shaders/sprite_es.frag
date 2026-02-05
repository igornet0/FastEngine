#version 300 es
precision mediump float;

in vec2 TexCoord;

uniform sampler2D uTexture;
uniform vec4 uColor;
uniform bool uUseTexture;

out vec4 fragColor;

void main() {
    if (uUseTexture) {
        fragColor = texture(uTexture, TexCoord) * uColor;
    } else {
        fragColor = uColor;
    }
}
