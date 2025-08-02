#pragma once

#include "Test.h"

namespace test {
    class TestClearColor : public Test
    {
    public:
        TestClearColor();
        ~TestClearColor();

        virtual void OnUpdate(float deltaTime);
        virtual void OnRender();
        virtual void OnImGuiRender();
    private:
        float m_ClearColor[4];
    };
}