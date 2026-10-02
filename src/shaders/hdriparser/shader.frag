#version 450 core
in vec2 FragPos;
// color attachments for each face of cube.
layout(location = 0) out vec4 faceColors[6];
// Order:
// POSITIVE_X   (i = 0)
// NEGATIVE_X   (i = 1)
// POSITIVE_Y   (i = 2)
// NEGATIVE_Y   (i = 3)
// POSITIVE_Z   (i = 4)
// NEGATIVE_Z   (i = 5)

uniform sampler2D hdriTexture;
uniform int hdriWidth;
uniform int hdriHeight;

vec2 angleToUv(vec2 angles) { // degrees
	float lat = clamp(angles.y, -90.0f, 90.0f);
	float lon = clamp(angles.x, -180.0f, 180.0f);

	return vec2(0.5f * (1.0f + lon / 180.0f), 0.5f * (1.0f - lat / 90.0f));
}

void main() {
	// front face (+z)
	for (int i = 0; i < 6; i++) { 
	    vec2 fragangle = vec2(FragPos.x * 45.0f, FragPos.y * 45.0f);

		switch (i) {
			case 0: // right face (+x)
				fragangle += vec2(90.0f, 0.0f);
				break;
			case 1: // left face (-x)
				fragangle += vec2(-90.0f, 0.0f);
				break;
			case 2: // top face (+y)
			    fragangle += vec2(0.0f, 90.0f);
				break;
			case 3: // bottom face (-y)
				fragangle += vec2(0.0f, -90.0f);
				break;
			case 4: // front face (+z)
				fragangle += vec2(0.0f, 0.0f);
				break;
			case 5: // back face (-z)
				fragangle += vec2(180.0f, 0.0f);
				break;
		}

 		vec2 uv = angleToUv(fragangle);
		faceColors[i] = texture(hdriTexture, uv);
	}
}