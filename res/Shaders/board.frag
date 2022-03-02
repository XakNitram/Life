#version 460 core

in flat uint alive;

out vec4 final;

void main() {
    if (alive == 1) {
//        final = vec4(0.71373, 0.09020, 0.29412, 1.0);  // boid pictoral carmine
        final = vec4(0.76471, 0.04314, 0.30588, 1.0); // actual pictoral carmine
    } else {
        final = vec4(0.0, 0.0, 0.0, 1.0);
    }
}
