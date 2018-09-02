struct Shader {
    GLuint id;
};

Shader shader_init(void* vert, uint32 vertLen, void* frag, uint32 fragLen) {
  Shader s = {0};

  // Create shaders
  GLuint vertShaderID = glCreateShader(GL_VERTEX_SHADER);
  GLuint fragShaderID = glCreateShader(GL_FRAGMENT_SHADER);

  bool failed = false;
  GLint errorLogLen = 0;

  {
    const char* code = (char*) vert;

    logfln("INFO: compiling vert shader: '%s'", code);

    glShaderSource(vertShaderID, 1, &code, 0);
    glCompileShader(vertShaderID);
  }

  {
    GLint res;
    glGetShaderiv(vertShaderID, GL_COMPILE_STATUS, &res);

    failed |= res == GL_FALSE;

    glGetShaderiv(vertShaderID, GL_INFO_LOG_LENGTH, &errorLogLen);
    if (errorLogLen) {
      char errorLog[512];
      glGetShaderInfoLog(vertShaderID, errorLogLen, 0, errorLog);

      log("ERROR: Could not compile vertex shader: %s", errorLog);
    }
  }

  {
    const char* code = (char*) frag;

    logfln("INFO: compiling vert shader: '%s'", code);

    glShaderSource(fragShaderID, 1, &code, 0);
    glCompileShader(fragShaderID);
  }

  {
    GLint res;
    glGetShaderiv(fragShaderID, GL_COMPILE_STATUS, &res);

    failed |= res == GL_FALSE;

    glGetShaderiv(fragShaderID, GL_INFO_LOG_LENGTH, &errorLogLen);

    if (errorLogLen) {
      char errorLog[512]; 
      glGetShaderInfoLog(vertShaderID, errorLogLen, 0, errorLog);

      log("ERROR: Could not compile fragment shader: %s", errorLog);
    }
  }

  GLuint programID = glCreateProgram();
  glAttachShader(programID, vertShaderID);
  glAttachShader(programID, fragShaderID);
  glLinkProgram(programID);

  {
    GLint res;
    glGetProgramiv(programID, GL_LINK_STATUS, &res);

    failed = res == GL_FALSE;

    glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &errorLogLen);

    if (errorLogLen) {
      char errorLog[512]; 
      glGetProgramInfoLog(programID, errorLogLen, 0, errorLog);

      log("ERROR: Could not link shader program: %s", errorLog);
    }
  }

  assert(failed != true);

  glDeleteShader(fragShaderID);
  glDeleteShader(vertShaderID);

  s.id = programID;

  return s;
}

Shader shader_load(const char* name) {
  Shader s = {0};

  void* vertData;
  MemoryIndex vertLen;
  void* fragData;
  MemoryIndex fragLen;

  char vertFilename[MAX_PATH];
  char fragFilename[MAX_PATH];

  snprintf(vertFilename, MAX_PATH, "shaders/%s.vert", name);
  snprintf(fragFilename, MAX_PATH, "shaders/%s.frag", name);

  mustLoadFromFile(vertFilename, &vertData, &vertLen);
  mustLoadFromFile(fragFilename, &fragData, &fragLen);

  s = shader_init(vertData, vertLen, fragData, fragLen);

  return s;
}

bool shader_setMat4(Shader* shader, const char* name, mat4 m) {
  GLint loc = glGetUniformLocation(shader->id, name);
  if (loc == -1) {
    logfln("ERROR: cannot find uniform location %s", name);

    return false;
  }

  glUniformMatrix4fv(loc, 1, GL_FALSE, &m.Elements[0][0]);

  return true;
}

bool shader_setVec4(Shader* shader, const char* name, vec4 v) {
  GLint loc = glGetUniformLocation(shader->id, name);
  if (loc == -1) {
    logfln("ERROR: cannot find uniform location %s", name);

    return false;
  }

  glUniform4f(loc, v.x, v.y, v.z, v.w);

  return true;
}

bool shader_setVec3(Shader* shader, const char* name, vec3 v) {
  GLint loc = glGetUniformLocation(shader->id, name);
  if (loc == -1) {
    logfln("ERROR: cannot find uniform location %s", name);

    return false;
  }

  glUniform3f(loc, v.x, v.y, v.z);

  return true;
}

bool shader_setVec2(Shader* shader, const char* name, vec2 v) {
  GLint loc = glGetUniformLocation(shader->id, name);
  if (loc == -1) {
    logfln("ERROR: cannot find uniform location %s", name);

    return false;
  }

  glUniform2f(loc, v.x, v.y);

  return true;
}

bool shader_setInt(Shader* shader, const char* name, int v) {
  GLint loc = glGetUniformLocation(shader->id, name);
  if (loc == -1) {
    logfln("ERROR: cannot find uniform location %s", name);

    return false;
  }

  glUniform1i(loc, v);

  return true;
}

bool shader_setFloat(Shader* shader, const char* name, float v) {
  GLint loc = glGetUniformLocation(shader->id, name);
  if (loc == -1) {
    logfln("ERROR: cannot find uniform location %s", name);

    return false;
  }

  glUniform1f(loc, v);

  return true;
}

void shader_clean(Shader* shader) {
  glDeleteProgram(shader->id);
  shader->id = 0;
}
