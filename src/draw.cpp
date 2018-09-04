enum {
#define shader(name) DEFAULT_SHADER_ ##name,
#include <assets/default_shaders.cpp>
  MAX_DEFAULT_SHADER
#undef shader
};

struct Draw {
  vec3 clear;

  mat4 view;
  mat4 projection;

  Shader activeShader;

  Shader defaultShaders[MAX_DEFAULT_SHADER];

  GLuint quadVAO;
  GLuint quadVertexBuffer;
  GLuint quadUVBuffer;

  GLuint virtualFBO;
  Texture virtualTex;
};

Draw draw = {0};

void draw_init() {
  draw.view = mat4d(1.0f);
  draw.projection = mat4d(1.0f);

  glGenFramebuffers(1, &draw.virtualFBO);
  glBindFramebuffer(GL_FRAMEBUFFER, draw.virtualFBO);

  glGenTextures(1, &draw.virtualTex.id);

  glBindTexture(GL_TEXTURE_2D, draw.virtualTex.id);

  // glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, SHADOW_WIDTH, SHADOW_HEIGHT, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, GAME_VIRTUAL_WIDTH, GAME_VIRTUAL_HEIGHT, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glGenerateMipmap(GL_TEXTURE_2D);

  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, draw.virtualTex.id, 0);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  GLfloat vertices[] = { 
    0.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f,
    0.0f, 0.0f, 0.0f,

    0.0f, 1.0f, 0.0f,
    1.0f, 1.0f, 0.0f,
    1.0f, 0.0f, 0.0f
  };

  GLfloat uvs[] = {
    0.0f, 1.0f,
    1.0f, 0.0f,
    0.0f, 0.0f,

    0.0f, 1.0f,
    1.0f, 1.0f,
    1.0f, 0.0f
  };

  glGenVertexArrays(1, &draw.quadVAO);
  glBindVertexArray(draw.quadVAO);

  {
    glGenBuffers(1, &draw.quadVertexBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, draw.quadVertexBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenBuffers(1, &draw.quadUVBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, draw.quadUVBuffer);

    glBufferData(GL_ARRAY_BUFFER, sizeof(uvs), uvs, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, draw.quadVertexBuffer);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, draw.quadUVBuffer);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
  }
  glBindVertexArray(0);

#define shader(name) \
  draw.defaultShaders[DEFAULT_SHADER_ ## name ] = shader_load(# name);
#include <assets/default_shaders.cpp>
#undef shader
}

void draw_setShader(Shader s) {
  if (s.id == draw.activeShader.id) {
    return;
  }

  glUseProgram(s.id);

  draw.activeShader = s;
}

void draw_clean() {
  glDeleteBuffers(1, &draw.quadVertexBuffer);
  glDeleteBuffers(1, &draw.quadUVBuffer);

  glDeleteVertexArrays(1, &draw.quadVAO);

#define shader(name) \
  shader_clean(&draw.defaultShaders[DEFAULT_SHADER_ ## name ]);
#include <assets/default_shaders.cpp>
#undef shader
}

void draw_virtual_begin() {
  glBindFramebuffer(GL_FRAMEBUFFER, draw.virtualFBO);

  glViewport(0, 0, GAME_VIRTUAL_WIDTH, GAME_VIRTUAL_HEIGHT);

  // glClearColor(draw.clear.x/255.0f, draw.clear.y/255.0f, draw.clear.z/255.f, 1.0f);
  glClearColor(1.0f, 0.0f, 1.0f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT);

  draw.view = mat4d(1.0f);
  draw.projection = mat4Orthographic(0, GAME_VIRTUAL_WIDTH, GAME_VIRTUAL_HEIGHT, 0, -1.0f, 1.0f);
}

void draw_virtual_end() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void draw_begin() {
  glBindFramebuffer(GL_FRAMEBUFFER, 0);

  // TODO(harrison): letterbox if needed
  glViewport(0, 0, getWindowWidth(), getWindowHeight());

  glClearColor(draw.clear.x/255.0f, draw.clear.y/255.0f, draw.clear.z/255.f, 1.0f);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

  draw.view = mat4d(1.0f);
  draw.projection = mat4Orthographic(0, (real32) getWindowWidth(), (real32) getWindowHeight(), 0, -1.0f, 1.0f);
}

void draw_rectangle(Rect rect, vec4 color) {
  Shader returnTo = {0};

  if (draw.activeShader.id != draw.defaultShaders[DEFAULT_SHADER_rectangle].id) {
    returnTo = draw.activeShader;

    draw_setShader(draw.defaultShaders[DEFAULT_SHADER_rectangle]);
  }

  mat4 model = mat4d(1.0f);
  model = mat4Translate(model, vec3(rect.x, rect.y, 0));
  model = mat4Scale(model, vec3(rect.w, rect.h, 1));

  shader_setMat4(&draw.activeShader, "model", model);
  shader_setMat4(&draw.activeShader, "view", draw.view);
  shader_setMat4(&draw.activeShader, "projection", draw.projection);

  shader_setVec4(&draw.activeShader, "color", vec4(color.x/255.0f, color.y/255.0f, color.z/255.0f, color.w));

  glBindVertexArray(draw.quadVAO);

  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);

  if (returnTo.id != 0) {
    draw_setShader(returnTo);
  }
}

void draw_rectangle(Rect rect, vec3 color) {
  draw_rectangle(rect, vec4(color.x, color.y, color.z, 1.0f));
}

void draw_sprite(Rect rect, Texture t) {
  Shader returnTo = {0};

  if (draw.activeShader.id != draw.defaultShaders[DEFAULT_SHADER_sprite].id) {
    returnTo = draw.activeShader;

    draw_setShader(draw.defaultShaders[DEFAULT_SHADER_sprite]);
  }

  mat4 model = mat4d(1.0f);
  model = mat4Translate(model, vec3(rect.x, rect.y, 0));
  model = mat4Scale(model, vec3(rect.w, rect.h, 1));

  shader_setMat4(&draw.activeShader, "model", model);
  shader_setMat4(&draw.activeShader, "view", draw.view);
  shader_setMat4(&draw.activeShader, "projection", draw.projection);

  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, t.id);

  glBindVertexArray(draw.quadVAO);

  glDrawArrays(GL_TRIANGLES, 0, 6);
  glBindVertexArray(0);

  if (returnTo.id != 0) {
    draw_setShader(returnTo);
  }
}
