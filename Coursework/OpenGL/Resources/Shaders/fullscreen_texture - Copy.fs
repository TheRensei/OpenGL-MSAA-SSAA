#version 330

uniform sampler2D texture0;
uniform vec2 texOffset;
uniform vec2 resolution;

in vec2 texCoord;

layout (location=0) out vec4 fragColour;

void main(void) {

	float tx = 1.0 / resolution.x;
	float ty = 1.0 / resolution.y;
	vec2 texelSize = vec2(tx, ty);
	//Sample Patterns Used from https://mynameismjp.wordpress.com/2012/10/24/msaa-overview/

	//Sample count depending on resolution, 2x, 4x, 8x
	//for -sampleCount/2 to samplecount/2
	//pattern for ssaaa
	//out colour

	//vec4 col;
	//for(int i = 0; i < 4; i++)
	//{
	//	//4x4 Checker
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(-0.75f + i,  1.75 + i)), 0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(-0.75f + i, -0.25 + i)),0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(-0.25f + i,  0.25 + i)), 0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(-0.25f + i, -0.75 + i)), 0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(0.25f  + i,  0.75 + i)), 0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(0.25f  + i, -0.25 + i)), 0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(0.75f  + i,  0.25 + i)), 0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(0.75f  + i, -0.75 + i)), 0);
	//}
	//fragColour = col / 32;

	//vec4 col = texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(-1.0f, 1.0f)), 0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(-1.0f, -1.0f)), 0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(1.0f, 1.0f)), 0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(1.0f, -1.0f)), 0);
	//fragColour = col * 0.25;

	//vec4 col = texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(-0.375f, 0.125f)), 0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(-0.125f, -0.375f)), 0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(0.125f, 0.375f)), 0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(0.375f, -0.125f)), 0);
	//fragColour = col * 0.25;


	////Quincunx
	//vec4 col =  texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(-1.0f, 1.0f)), 0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(-1.0f, -1.0f)), 0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(1.0f, 1.0f)), 0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y) + texelSize * vec2(1.0f, -1.0f)), 0);
	//	col += texelFetch(texture0, ivec2(texCoord * vec2(resolution.x, resolution.y)), 0);
	//fragColour = col * 0.2;

	//-----------------------------------------------------------------------
		////4x4 Grid
	//	//Column 1
	//vec4 col = texture(texture0, texCoord + texelSize * vec2(-0.75f,0.75f));
	//col += texture(texture0, texCoord + texelSize * vec2(-0.75f,0.25f));
	//col += texture(texture0, texCoord + texelSize * vec2(-0.75f,-0.25f));
	//col += texture(texture0, texCoord + texelSize * vec2(-0.75f,-0.75f));
	//	//Column 2
	//col += texture(texture0, texCoord + texelSize * vec2(-0.25f,0.75f));
	//col += texture(texture0, texCoord + texelSize * vec2(-0.25f,0.25f));
	//col += texture(texture0, texCoord + texelSize * vec2(-0.25f,-0.25f));
	//col += texture(texture0, texCoord + texelSize * vec2(-0.25f,-0.75f));
	//	//Column 3
	//col += texture(texture0, texCoord + texelSize * vec2(0.25f,0.75f));
	//col += texture(texture0, texCoord + texelSize * vec2(0.25f,0.25f));
	//col += texture(texture0, texCoord + texelSize * vec2(0.25f,-0.25f));
	//col += texture(texture0, texCoord + texelSize * vec2(0.25f,-0.75f));
	//	//Column 4
	//col += texture(texture0, texCoord + texelSize * vec2(0.75f,0.75f));
	//col += texture(texture0, texCoord + texelSize * vec2(0.75f,0.25f));
	//col += texture(texture0, texCoord + texelSize * vec2(0.75f,-0.25f));
	//col += texture(texture0, texCoord + texelSize * vec2(0.75f,-0.75f));
	//
	//fragColour = col * 0.0625f;

	
	////4x4 Checker
	//vec4 col = texture(texture0, texCoord + texelSize * vec2(-0.75f, 0.75));
	//col += texture(texture0, texCoord + texelSize * vec2(-0.75f, -0.25));
	//col += texture(texture0, texCoord + texelSize * vec2(-0.25f, 0.25));
	//col += texture(texture0, texCoord + texelSize * vec2(-0.25f, -0.75));
	//col += texture(texture0, texCoord + texelSize * vec2(0.25f, 0.75));
	//col += texture(texture0, texCoord + texelSize * vec2(0.25f, -0.25));
	//col += texture(texture0, texCoord + texelSize * vec2(0.75f, 0.25));
	//col += texture(texture0, texCoord + texelSize * vec2(0.75f, -0.75));
	//
	//fragColour = col * 0.125;

	////https://blog.demofox.org/2015/04/23/4-rook-antialiasing-rgss/
	////2x2 RGSS
	//vec4 col = texture(texture0, texCoord + texelSize *	vec2(-0.375f, 0.125f));
	//col += texture(texture0, texCoord + texelSize *		vec2(-0.125f, -0.375f));
	//col += texture(texture0, texCoord + texelSize *		vec2(0.125f, 0.375f));
	//col += texture(texture0, texCoord + texelSize *		vec2(0.375f, -0.125f));
	//fragColour = col * 0.25;

	//----------------------------------------------------

	//vec4 col;
	//for(int i = -2; i <= 2; i++)
	//{
	//	//4x4 Checker
	//	//col += texture(texture0, texCoord + texelSize * vec2(-0.75f + i,  0.75 + i));
	//	//col += texture(texture0, texCoord + texelSize * vec2(-0.75f + i, -0.25 + i));
	//	//col += texture(texture0, texCoord + texelSize * vec2(-0.25f + i,  0.25 + i));
	//	//col += texture(texture0, texCoord + texelSize * vec2(-0.25f + i, -0.75 + i));
	//	col += texture(texture0, texCoord + texelSize * vec2(0.25f  + i,  0.75 + i));
	//	col += texture(texture0, texCoord + texelSize * vec2(0.25f  + i, -0.25 + i));
	//	col += texture(texture0, texCoord + texelSize * vec2(0.75f  + i,  0.25 + i));
	//	col += texture(texture0, texCoord + texelSize * vec2(0.75f  + i, -0.75 + i));
	//}
	//fragColour = col / 20;

	//for(int x = -2; x <=2;x++)
	//{
	//	for(int y = -2; y <= 2;y++)
	//	{
	//		col += texture(texture0, texCoord + texelSize * vec2(0.25f  + i,  0.75 + i));
	//	}
	//}

	//	//Column 1
	//vec4 col = texture(texture0, texCoord + texelSize * vec2(-1.75f,1.75f));
	//col += texture(texture0, texCoord + texelSize * vec2(-1.75f,1.25f));
	//col += texture(texture0, texCoord + texelSize * vec2(-1.75f,-1.25f));
	//col += texture(texture0, texCoord + texelSize * vec2(-1.75f,-1.75f));
	//	//Column 2										  1
	//col += texture(texture0, texCoord + texelSize * vec2(-1.25f,1.75f));
	//col += texture(texture0, texCoord + texelSize * vec2(-1.25f,1.25f));
	//col += texture(texture0, texCoord + texelSize * vec2(-1.25f,-1.25f));
	//col += texture(texture0, texCoord + texelSize * vec2(-1.25f,-1.75f));
	//	//Column 3
	//col += texture(texture0, texCoord + texelSize * vec2(1.25f,1.75f));
	//col += texture(texture0, texCoord + texelSize * vec2(1.25f,1.25f));
	//col += texture(texture0, texCoord + texelSize * vec2(1.25f,-1.25f));
	//col += texture(texture0, texCoord + texelSize * vec2(1.25f,-1.75f));
	//	//Column 4										 1
	//col += texture(texture0, texCoord + texelSize * vec2(1.75f,1.75f));
	//col += texture(texture0, texCoord + texelSize * vec2(1.75f,1.25f));
	//col += texture(texture0, texCoord + texelSize * vec2(1.75f,-1.25f));
	//col += texture(texture0, texCoord + texelSize * vec2(1.75f,-1.75f));
	//
	//fragColour = col * 0.0625f;
	//
	//vec4 col = texture(texture0, texCoord + texelSize *	vec2(-1.375f, 1.125f));
	//col += texture(texture0, texCoord + texelSize *		vec2(-1.125f, -1.375f));
	//col += texture(texture0, texCoord + texelSize *		vec2(1.125f, 1.375f));
	//col += texture(texture0, texCoord + texelSize *		vec2(1.375f, -1.125f));
	//fragColour = col * 0.25;

	//vec4 col = texture(texture0, texCoord + texelSize * vec2(-1.5f, 1.5f));
	//col += texture(texture0, texCoord + texelSize * vec2(-1.5f, -1.5f));
	//col += texture(texture0, texCoord + texelSize * vec2(1.5f, 1.5f));
	//col += texture(texture0, texCoord + texelSize * vec2(1.5f, -1.5f));
	//fragColour = col * 0.25;

	//---------------------------------------------------------------------

	////2x2 Grid
	//vec4 col = texture(texture0, texCoord + texelSize * vec2(-0.5f, 0.5f));
	//col += texture(texture0, texCoord + texelSize * vec2(-0.5f, -0.5f));
	//col += texture(texture0, texCoord + texelSize * vec2(0.5f, 0.5f));
	//col += texture(texture0, texCoord + texelSize * vec2(0.5f, -0.5f));
	//fragColour = col * 0.25;


	////Quincunx
	//vec4 col = texture(texture0, texCoord + texelSize * vec2(-1.0f, 1.0f));
	//	col += texture(texture0, texCoord + texelSize * vec2(-1.0f, -1.0f));
	//	col += texture(texture0, texCoord + texelSize * vec2(1.0f, 1.0f));
	//	col += texture(texture0, texCoord + texelSize * vec2(1.0f, -1.0f));
	//	col += texture(texture0, texCoord);
	//fragColour = col * 0.2;

	////1x2 Sample
	//vec4 col =  texture(texture0, texCoord + texelSize * vec2(0.0f, 0.25f));
	//col +=  texture(texture0, texCoord + texelSize * vec2(0.0f, -0.25f));
	//
	//fragColour = col * 0.5;

	//1 sample
	//fragColour = texture(texture0, texCoord);

	//---------------------------------------------------------------------

	//int sampleCount = 5;
	
	//vec2 offsets[9] = vec2[](
    //   vec2(-tx,  ty), // top-left
    //   vec2( 0.0f,    ty), // top-center
    //   vec2( tx,  ty), // top-right
    //   vec2(-tx,  0.0f),   // center-left
    //   vec2( 0.0f,    0.0f),   // center-center
    //   vec2( tx,  0.0f),   // center-right
    //   vec2(-tx, -ty), // bottom-left
    //   vec2( 0.0f,   -ty), // bottom-center
    //   vec2( tx, -ty)  // bottom-right    
    //);
	//
	//vec2 pos[9] = vec2[](
    //   vec2(-1,  1), // top-left
    //   vec2( 0.0f,    1), // top-center
    //   vec2( 1,  1), // top-right
    //   vec2(-1,  0.0f),   // center-left
    //   vec2( 0.0f,    0.0f),   // center-center
    //   vec2( 1,  0.0f),   // center-right
    //   vec2(-1, -1), // bottom-left
    //   vec2( 0.0f,   -1), // bottom-center
    //   vec2( 1, -1)  // bottom-right    
    //);
	
	//2x2 RGSS +1 center
	//vec2 offsets[5] = vec2[](
	//	vec2(texelSize * vec2(  0.4,  0.9 )),
	//	vec2(texelSize * vec2( -0.4, -0.9 )),
	//	vec2(0,0),
	//	vec2(texelSize * vec2( -0.9,  0.4 )),
	//	vec2(texelSize * vec2(  0.9, -0.4 ))
	//);
	
	////2x2 Grid + 1 center
	//vec2 offsets[5] = vec2[](
	//	vec2(texelSize * vec2(  0.25,   0.75)),
	//	vec2(texelSize * vec2(  0.75,   0.75)),
	//	vec2(0,0),		 
	//	vec2(texelSize * vec2(  0.25,   0.25)),
	//	vec2(texelSize * vec2(  0.75,   0.25))
	//);
	
	//vec3 sampleTex[5];
	//vec3 col = vec3(0.0);
	//
	//for(int i = 0; i < sampleCount; i++)
    //{
    //    sampleTex[i] = vec3(texture(texture0, texCoord.st + offsets[i]));
    //}
	//
	//for(int i = 0; i < sampleCount; i++)
	//{
	//	col += sampleTex[i];
	//}
	//
	//fragColour = vec4(col/sampleCount, 1.0);
	
	//int samples = 4;
	//vec4 outColour = vec4(0.0, 0.0, 0.0, 0.0);
	//vec4 newColour;
	//
	//int size = samples / 2;
	//
	//int sample2 = ((samples + 1) * (samples+1));
	//
	//for (int y = -size; y <= size; y++) {
    //    for (int x = -size; x <= size; x++) {
	//		vec2 position = (texCoord + vec2(x, y)) + vec2(tx * x, ty * y);
	//		newColour = (texture(texture0, position) / sample2);
	//
	//		outColour += newColour;
	//	}
	//}
	//fragColour = outColour;
}