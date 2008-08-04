/*	Graphics Library of Views (GLV) - GUI Building Toolkit
	See COPYRIGHT file for authors and license information */

#include "test_glv.h"
#include <iostream>

class TestWindow : public glv::View
{
public:
    TestWindow()
        : glv::View(::glv::Rect(640, 640))
        , other(NULL)
    {
		stretch(1,1);
        glv::LayoutGrid layout(*this, 2, 2, 10);
        layout << *new glv::CharView;
        slider = new glv::Slider(glv::Rect(300,300));
        *slider << new glv::Label("This one causes change in other window");
        layout << *slider;
        passive_slider = new glv::Slider(glv::Rect(300,300));
        layout << *passive_slider;
        *passive_slider << new glv::Label("This one is manipulated by other window");
        slider->attachHandler(onNotify, this);
    }
    static void onNotify(Notifier * sender, void * userdata)
    {
        TestWindow *this_ = static_cast<TestWindow *>(userdata);
        std::cout << this_->slider->value() << std::endl;
        if(this_->other)
            this_->other->passive_slider->value(this_->slider->value());
    }
    glv::Slider *slider;
    glv::Slider *passive_slider;
    TestWindow *other;
};

int main(int argc, char ** argv){

    using namespace glv;
    
	GLV one;
  	one.colors().back.set(0);
	Window win_one(640, 640, "Window One", &one);
    TestWindow *test_one = new TestWindow;
    one << test_one;
    
	GLV two;
  	two.colors().back.set(0);
	Window win_two(640, 640, "Window Two", &two);
    TestWindow *test_two = new TestWindow;
    two << test_two;
    
    test_one->other = test_two;
    test_two->other = test_one;
    
	Application::run();
	return 0;
}
