#include "cameracontroller.h"

CameraController::CameraController() :mpressed_(false), rpressed(false) {
}

void CameraController::setCamera(Camera value) {
	camera_ = value;
}

void CameraController::onMouseWheel(int delta) {
	glm::vec3 fwd = camera_->GetForward();
	float fDelta = 0.005f * delta;
	camera_->SetPosition(camera_->GetPosition() + fwd * fDelta);
}

void CameraController::onMouseMove(const QPoint& pos) {
	if (mpressed_) {
		QPoint delta = pos - mpos_;
		mpos_ = pos;
		glm::vec3 up = camera_->GetUp();
		glm::vec3 right = camera_->GetRight();

		up *= 0.005f * (float)delta.y();
		right *= 0.005f * (float)delta.x();

		camera_->SetPosition(camera_->GetPosition() + up + right);
	}

	if (rpressed) {
		QPoint delta = pos - rpos_;
		rpos_ = pos;
		glm::vec3 euler = camera_->GetEulerAngles();
		euler.x += 0.005f * delta.y();
		euler.y += 0.005f * delta.x();
		camera_->SetEulerAngles(euler);
	}
}

void CameraController::onMouseRelease(Qt::MouseButton button) {
	if (button == Qt::MiddleButton) {
		mpressed_ = false;
	}
	else if (button == Qt::RightButton) {
		rpressed = false;
	}
}

void CameraController::onMousePress(Qt::MouseButton button, const QPoint & pos) {
	if (button == Qt::MiddleButton) {
		mpos_ = pos;
		mpressed_ = true;
	}

	if (button == Qt::RightButton) {
		rpos_ = pos;
		rpressed = true;
	}
}
