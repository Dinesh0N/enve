#include "shadereffectprogram.h"

ShaderEffectProgram ShaderEffectProgram::sCreateProgram(
        QGL33 * const gl, const QString &fragPath,
        const QString& marginScript,
        const QList<stdsptr<ShaderPropertyCreator>>& propCs,
        const UniformSpecifierCreators& uniCs,
        const QList<stdsptr<ShaderValueHandler>>& values) {
    ShaderEffectProgram program;
    try {
        iniProgram(gl, program.fId, GL_TEXTURED_VERT, fragPath);
    } catch(...) {
        RuntimeThrow("Could not initialize a program for ShaderEffectProgram");
    }

    for(const auto& propC : propCs) {
        if(propC->fGLValue) {
            const GLint loc = gl->glGetUniformLocation(program.fId,
                                                       propC->fName.toLatin1());
            if(loc < 0) {
                gl->glDeleteProgram(program.fId);
                RuntimeThrow("'" + propC->fName +
                             "' does not correspond to an active uniform variable.");
            }
            program.fPropUniLocs.append(loc);
        } else program.fPropUniLocs.append(-1);
    }
    program.fPropUniCreators = uniCs;
    for(const auto& value : values) {
        if(value->fGLValue) {
            const GLint loc = gl->glGetUniformLocation(program.fId,
                                                       value->fName.toLatin1());
            if(loc < 0) {
                gl->glDeleteProgram(program.fId);
                RuntimeThrow("'" + value->fName +
                             "' does not correspond to an active uniform variable.");
            }
            program.fValueLocs.append(loc);
        } else program.fValueLocs.append(-1);
    }
    program.fValueHandlers = values;

    program.fGPosLoc = gl->glGetUniformLocation(program.fId, "eGlobalPos");
    program.fTexLocation = gl->glGetUniformLocation(program.fId, "texture");
    if(program.fTexLocation < 0) {
        gl->glDeleteProgram(program.fId);
        RuntimeThrow("Invalid location received for 'texture'.");
    }
    gl->glUniform1i(program.fTexLocation, 0);

    program.fMarginScript = marginScript;
    return program;
}