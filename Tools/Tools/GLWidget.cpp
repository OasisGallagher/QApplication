#include <windows.h>

#include <QDebug>

#include "GLWidget.h"
#include "3rdParty/glew-2.1.0/include/GL/glew.h"

#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "3rdParty/glew-2.1.0/lib/Release/x64/glew32.lib")

void GLAPIENTRY debugOutputCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const GLvoid* userParam) {
	// Shader.
	if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB) {
		return;
	}

	QString text = "OpenGL Debug Output message:\n";

	if (source == GL_DEBUG_SOURCE_API_ARB)					text += "Source: API.\n";
	else if (source == GL_DEBUG_SOURCE_WINDOW_SYSTEM_ARB)	text += "Source: WINDOW_SYSTEM.\n";
	else if (source == GL_DEBUG_SOURCE_SHADER_COMPILER_ARB)	text += "Source: SHADER_COMPILER.\n";
	else if (source == GL_DEBUG_SOURCE_THIRD_PARTY_ARB)		text += "Source: THIRD_PARTY.\n";
	else if (source == GL_DEBUG_SOURCE_APPLICATION_ARB)		text += "Source: APPLICATION.\n";
	else if (source == GL_DEBUG_SOURCE_OTHER_ARB)			text += "Source: OTHER.\n";

	if (type == GL_DEBUG_TYPE_ERROR_ARB)					text += "Type: ERROR.\n";
	else if (type == GL_DEBUG_TYPE_DEPRECATED_BEHAVIOR_ARB)	text += "Type: DEPRECATED_BEHAVIOR.\n";
	else if (type == GL_DEBUG_TYPE_UNDEFINED_BEHAVIOR_ARB)	text += "Type: UNDEFINED_BEHAVIOR.\n";
	else if (type == GL_DEBUG_TYPE_PORTABILITY_ARB)			text += "Type: PORTABILITY.\n";
	else if (type == GL_DEBUG_TYPE_PERFORMANCE_ARB)			text += "Type: PERFORMANCE.\n";
	else if (type == GL_DEBUG_TYPE_OTHER_ARB)				text += "Type: OTHER.\n";

	if (severity == GL_DEBUG_SEVERITY_HIGH_ARB)				text += "Severity: HIGH.\n";
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB)		text += "Severity: MEDIUM.\n";
	else if (severity == GL_DEBUG_SEVERITY_LOW_ARB)			text += "Severity: LOW.\n";

	text += message;

	Q_ASSERT_X(severity != GL_DEBUG_SEVERITY_HIGH_ARB, "", text.toLatin1());

	if (severity == GL_DEBUG_SEVERITY_HIGH_ARB) {
		qCritical() << text;
	}
	else if (severity == GL_DEBUG_SEVERITY_MEDIUM_ARB) {
		qWarning() << text;
	}
	else {
		qDebug() << text;
	}
}

GLWidget::GLWidget(QWidget* parent) :QWidget(parent) {
	setAttribute(Qt::WA_PaintOnScreen);
	setAttribute(Qt::WA_NoSystemBackground);
	setAutoFillBackground(true);

	HWND hWnd = (HWND)winId();
	dc_ = GetDC(hWnd);

	PIXELFORMATDESCRIPTOR pfd;

	SetPixelFormat(dc_, 1, &pfd);

	HGLRC hRC = wglCreateContext(dc_);

	wglMakeCurrent(dc_, hRC);

	glewExperimental = true;
	if (glewInit() != GLEW_OK) {
		qFatal("failed to initialize GLEW\n");
		return;
	}

	if (GLEW_ARB_debug_output) {
		glDebugMessageCallback(debugOutputCallback, NULL);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS_ARB);
	}
}

GLWidget::~GLWidget() {
}

void GLWidget::paintEvent(QPaintEvent *event) {
	glClearColor(1.0, 0, 0, 0.0);
	/*
	glMatrixMode(GL_PROJECTION);
	glOrtho(0.0, 200.0, 0.0, 150.0, 1, 500);
	*/
	glClear(GL_COLOR_BUFFER_BIT);
	/*
	glEnable(GL_LINE_STIPPLE);
	GLushort  patn = 0xFAFA;
	glLineStipple(3, patn);
	glColor3f(1.0, 0.0, 0.0);

	glBegin(GL_LINE_LOOP);
	glVertex2i(10, 10);
	glVertex2f(100.0, 75.3);
	glColor3f(0.0, 1.0, 0.0);
	glVertex2i(70, 80);
	glEnd();
	*/
	glFlush();
	SwapBuffers(dc_);

	repaint();
}

QPaintEngine*  GLWidget::paintEngine() {
	return Q_NULLPTR;
}
