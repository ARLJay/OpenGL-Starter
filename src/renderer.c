#include <renderer.h>
#include <texture.h>
#include <stdio.h>

const unsigned int maxQuadCount = 1000;
const unsigned int maxVertexCount = 4 * maxQuadCount;
const unsigned int maxIndexCount = 6 * maxQuadCount;
unsigned int slots;
unsigned int tex0;
unsigned int tex1;

typedef struct {
	vec3 position;
	vec4 colour;
	vec2 textureCoords;
	float textureIndex;
} vertex;

struct {
	unsigned int vertexArray;
	unsigned int vertexBuffer;
	unsigned int indexBuffer;
	unsigned int shader;
} rendererData;

struct {
	float vertices[200];
	unsigned int* textures; 
	unsigned int count;
	unsigned int indexCount;
	unsigned int textureCount;
	unsigned int endPtr;
} data;

unsigned int getShader(){
	return rendererData.shader;
}

void renderer_draw_quad_texture(vec2 position, vec2 size, unsigned int textureID){
	if(data.indexCount >= maxIndexCount || data.textureCount >= slots){
		renderer_end_batch();
		renderer_flush();
		renderer_begin_batch();
	}

	float textureIndex = 0.0f;
	for(unsigned int i = 1; i < slots; i++){
		if(data.textures[i] == textureID){
			textureIndex = (float)i;
			break;
		}
	}

	vec4 colour = {1.0f, 1.0f, 1.0f, 1.0f};

	vertex* objectPtr = (void*) &data.vertices + (data.count * 4 * sizeof(vertex));


	objectPtr->position.x = position.x;
	objectPtr->position.y = position.y;
	objectPtr->position.z = 0.0f;
	objectPtr->colour = colour;
	objectPtr->textureCoords.x = 0.0f;
	objectPtr->textureCoords.y = 0.0f;
	objectPtr->textureIndex = textureIndex;
	objectPtr++;

	objectPtr->position.x = position.x + size.x;
	objectPtr->position.y = position.y;
	objectPtr->position.z = 0.0f;
	objectPtr->colour = colour;
	objectPtr->textureCoords.x = 1.0f;
	objectPtr->textureCoords.y = 0.0f;
	objectPtr->textureIndex = textureIndex;
	objectPtr++;

	objectPtr->position.x = position.x + size.x;
	objectPtr->position.y = position.y + size.y;
	objectPtr->position.z = 0.0f;
	objectPtr->colour = colour;
	objectPtr->textureCoords.x = 1.0f;
	objectPtr->textureCoords.y = 1.0f;
	objectPtr->textureIndex = textureIndex;
	objectPtr++;

	objectPtr->position.x = position.x;
	objectPtr->position.y = position.y + size.y;
	objectPtr->position.z = 0.0f;
	objectPtr->colour = colour;
	objectPtr->textureCoords.x = 0.0f;
	objectPtr->textureCoords.y = 1.0f;
	objectPtr->textureIndex = textureIndex;
	objectPtr++;

	data.endPtr = objectPtr;

	data.count++;
	data.indexCount += 6;
}

void renderer_draw_quad_colour(vec2 position, vec2 size, vec4 colour){
	if(data.indexCount >= maxIndexCount){
		renderer_end_batch();
		renderer_flush();
		renderer_begin_batch();
	}

	float textureIndex = 0.0f;


	vertex* objectPtr = (void*) &data.vertices + (data.count * 4 * sizeof(vertex));


	objectPtr->position.x = position.x;
	objectPtr->position.y = position.y;
	objectPtr->position.z = 0.0f;
	objectPtr->colour = colour;
	objectPtr->textureCoords.x = 0.0f;
	objectPtr->textureCoords.y = 0.0f;
	objectPtr->textureIndex = textureIndex;
	objectPtr++;

	objectPtr->position.x = position.x + size.x;
	objectPtr->position.y = position.y;
	objectPtr->position.z = 0.0f;
	objectPtr->colour = colour;
	objectPtr->textureCoords.x = 1.0f;
	objectPtr->textureCoords.y = 0.0f;
	objectPtr->textureIndex = textureIndex;
	objectPtr++;

	objectPtr->position.x = position.x + size.x;
	objectPtr->position.y = position.y + size.y;
	objectPtr->position.z = 0.0f;
	objectPtr->colour = colour;
	objectPtr->textureCoords.x = 1.0f;
	objectPtr->textureCoords.y = 1.0f;
	objectPtr->textureIndex = textureIndex;
	objectPtr++;

	objectPtr->position.x = position.x;
	objectPtr->position.y = position.y + size.y;
	objectPtr->position.z = 0.0f;
	objectPtr->colour = colour;
	objectPtr->textureCoords.x = 0.0f;
	objectPtr->textureCoords.y = 1.0f;
	objectPtr->textureIndex = textureIndex;
	objectPtr++;

	data.endPtr = objectPtr;

	data.count++;
	data.indexCount += 6;
}
void renderer_init(){
	data.count = 0;
	data.indexCount = 0;
	data.textureCount = 1;

	glCreateVertexArrays(1, &rendererData.vertexArray);
	glBindVertexArray(rendererData.vertexArray);

	glGenBuffers(1, &rendererData.vertexBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, rendererData.vertexBuffer);

	glVertexAttribPointer(0,3, GL_FLOAT, GL_FALSE, sizeof(vertex), 0);
  glEnableVertexAttribArray(0);                  
  glVertexAttribPointer(1,4, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);                  
  glVertexAttribPointer(2,2, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(7 * sizeof(float)));
  glEnableVertexAttribArray(2);                  
	glVertexAttribPointer(3,1, GL_FLOAT, GL_FALSE, sizeof(vertex), (void*)(9 * sizeof(float)));
	glEnableVertexAttribArray(3);

	glGenBuffers(1, &rendererData.indexBuffer);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, rendererData.indexBuffer);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &slots);
	data.textures =	(unsigned int*)malloc(slots*sizeof(unsigned int));

	unsigned int location=glGetUniformLocation(rendererData.shader,"u_Texture");
	int *samplers = (int*)malloc(slots*sizeof(int));

	for(int i = 0; i < slots; i++){
		samplers[i] = i;
	}
	glUniform1iv(location, slots, samplers);
	free(samplers);

	glBufferData(GL_ARRAY_BUFFER, maxVertexCount * sizeof(vertex), NULL , GL_DYNAMIC_DRAW);

	unsigned int indices[maxIndexCount];
	unsigned int offset = 0;
	for(unsigned int i = 0; i < maxIndexCount; i+=6){
		indices[i + 0] = 0 + offset;
		indices[i + 1] = 1 + offset;
		indices[i + 2] = 2 + offset;

		indices[i + 3] = 2 + offset;
		indices[i + 4] = 3 + offset;
		indices[i + 5] = 0 + offset;
		offset += 4;
	}

	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	unsigned int whiteTexture;
	glCreateTextures(GL_TEXTURE_2D, 1, &whiteTexture);
	glBindTexture(GL_TEXTURE_2D, whiteTexture);
	unsigned int white = 0xffffffff;
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, 1, 1, 0, GL_RGBA, GL_UNSIGNED_BYTE, &white);
	data.textures[0] = whiteTexture;

	for(unsigned int i = 1; i < slots; i++){
		data.textures[i] = 0;
	}

}

void renderer_shader_init(const char* vertexShaderPath, const char* fragmentShaderPath){
	rendererData.shader = CreateShader(vertexShaderPath, fragmentShaderPath);
	glUseProgram(rendererData.shader);
}

void renderer_terminate(){
	glDeleteVertexArrays(1, &rendererData.vertexArray);
	glDeleteBuffers(1, &rendererData.vertexBuffer);
	glDeleteBuffers(1, &rendererData.indexBuffer);
}


void renderer_begin_batch(){
}

void renderer_add_texture(GLuint texture){
	data.textures[data.textureCount] = texture;
	data.textureCount++;
}

void renderer_end_batch(){

	glBindBuffer(GL_ARRAY_BUFFER, rendererData.vertexBuffer);

	glBufferSubData(GL_ARRAY_BUFFER, 0,data.count * 4 * sizeof(vertex) ,data.vertices);
}

void renderer_flush(){
	glClear(GL_COLOR_BUFFER_BIT);

	glUseProgram(rendererData.shader);
	glBindVertexArray(rendererData.vertexArray);
	glBindTextures(0, data.textureCount, data.textures);
	glDrawElements(GL_TRIANGLES, data.indexCount, GL_UNSIGNED_INT, 0);
	data.count = 0;
	data.textureCount = 1;
	data.indexCount = 0;
}
