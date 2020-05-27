#version 330

uniform sampler2D texture0;
uniform vec2 texOffset;
uniform vec2 resolution;
uniform int SSLevel;

in vec2 texCoord;

layout (location=0) out vec4 fragColour;

void main(void) {
	float tx = 1.0 / resolution.x;
	float ty = 1.0 / resolution.y;
	vec2 texelSize = vec2(tx, ty);

	vec3 outColour = vec3(0.0f,0.0f,0.0f);
	float divider;
	int sampleCount = SSLevel / 2;


	////FlipTri
    //bool xOdd = (floor(mod(texCoord.x,2.0)) == 1.0);
    //bool yOdd = (floor(mod(texCoord.y,2.0)) == 1.0);
	//
	//vec2 offsets[3] = vec2[](
	//	vec2(xOdd ? -0.5 : 0.5, yOdd ? -0.5  : 0.5 ),
	//	vec2(0.0, yOdd ? 0.5 : -0.5),
	//	vec2(xOdd ? 0.5 : -0.5, 0.0 )
	//);
	//
	//float percentage[3] = float[](0.2, 0.4, 0.4);
	//
	//for(int x = -sampleCount; x <= sampleCount; ++x)
	//{
	//	for(int y = -sampleCount; y<= sampleCount; ++y)
	//	{
	//		for(int i = 0; i < 3; i++)
	//		{
	//			//Offset to different texel and offset to a different position inside the texel
	//			outColour += texture(texture0, texCoord + vec2(x,y) * texelSize + texelSize * offsets[i]).rgb * percentage[i];
	//		}
	//	}
	//}
	//
	//divider = pow(SSLevel+1, 2);

	//---------------------------------------------------------

	////FlipQuad
	//bool xOdd = (floor(mod(texCoord.x,2.0)) == 1.0);
    //bool yOdd = (floor(mod(texCoord.y,2.0)) == 1.0);
	//vec2 offsets[4] = vec2[](
	//	vec2(xOdd ? 0.25 : -0.25, yOdd ? -0.5  :  0.5 ),
	//	vec2(xOdd ? 0.5  : -0.5 , yOdd ?  0.25 : -0.25 ),
	//	-vec2(xOdd ? 0.25 : -0.25, yOdd ? -0.5  :  0.5 ),
	//	-vec2(xOdd ? 0.5  : -0.5 , yOdd ?  0.25 : -0.25 )
	//);
	//
	//for(int x = -sampleCount; x <= sampleCount; ++x)
	//{
	//	for(int y = -sampleCount; y<= sampleCount; ++y)
	//	{
	//		for(int i = 0; i < 4; i++)
	//		{
	//			//Offset to different texel and offset to a different position inside the texel
	//			outColour += texture(texture0, texCoord + vec2(x,y) * texelSize + texelSize * offsets[i]).rgb * 0.25;
	//		}
	//	}
	//}
	//
	//divider = pow(SSLevel+1, 2);

	//---------------------------------------------------------

	////RGSS
	//vec2 offsets[4] = vec2[](
	//	vec2(  0.125,  0.375 ),
	//	vec2( -0.125, -0.375 ),
	//	vec2( -0.375,  0.125 ),
	//	vec2(  0.375, -0.125 )
	//);
	//
	//for(int x = -sampleCount; x <= sampleCount; ++x)
	//{
	//	for(int y = -sampleCount; y<= sampleCount; ++y)
	//	{
	//		for(int i = 0; i < 4; i++)
	//		{
	//			//Offset to different texel and offset to a different position inside the texel
	//			outColour += texture(texture0, texCoord + vec2(x,y) * texelSize + texelSize * offsets[i]).rgb * 0.25;
	//		}
	//	}
	//}
	//
	//divider = pow(SSLevel+1, 2);

	//---------------------------------------------------------
	
	////Quincux
	//vec2 offsets[4] = vec2[](
	//	vec2(-0.5f,  0.5f),
	//	vec2(-0.5f,  -0.5f),
	//	vec2(0.5f,  0.5f),
	//	vec2(0.5f,  -0.5f)
	//);
	//
	//for(int x = -sampleCount; x <= sampleCount; ++x)
	//{
	//	for(int y = -sampleCount; y<= sampleCount; ++y)
	//	{
	//		outColour += texture(texture0, texCoord + vec2(x,y) * texelSize).rgb * 0.5;
	//		for(int i = 0; i < 4; i++)
	//		{
	//			//Offset to different texel and offset to a different position inside the texel
	//			outColour += texture(texture0, texCoord + vec2(x,y) * texelSize + texelSize * offsets[i]).rgb * 0.125;
	//		}
	//	}
	//}
	//
	//divider = pow(SSLevel+1, 2);
	
	//---------------------------------------------------------

	
	for(int x = -sampleCount; x <= sampleCount; ++x)
	{
		for(int y = -sampleCount; y<= sampleCount; ++y)
		{
			//Offset to different texel and offset to a different position inside the texel
			outColour += texture(texture0, texCoord + vec2(x,y) * texelSize).rgb;
		}
	}
	
	divider = pow(SSLevel+1, 2);

	
	//---------------------------------------------------------

	fragColour = vec4(outColour / divider,1.0f); 
}