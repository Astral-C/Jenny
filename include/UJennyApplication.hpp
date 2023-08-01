#pragma once

#include "UApplication.hpp"

class UJennyApplication : public UApplication {
	struct GLFWwindow* mWindow;
	class UJennyContext* mContext;

	virtual bool Execute(float deltaTime) override;

public:
	UJennyApplication();
	virtual ~UJennyApplication() {}

	virtual bool Setup() override;
	virtual bool Teardown() override;
};
