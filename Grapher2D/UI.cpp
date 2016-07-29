#include "UI.h"

bool UI::open = true;
std::thread* UI::thread = 0;

using namespace glui;

TextBox** UI::texts = new TextBox*[5];
Button** UI::buttons = new Button*[5];

void func0() {
	UI::addGraph(UI::texts[0]->m_text, 0);
}

void func1() {
	UI::addGraph(UI::texts[1]->m_text, 1);
}

void func2() {
	UI::addGraph(UI::texts[2]->m_text, 2);
}

void func3() {
	UI::addGraph(UI::texts[3]->m_text, 3);
}

void func4() {
	UI::addGraph(UI::texts[4]->m_text, 4);
}

void run() {
	GLUI::init();

	Window win("Editor", 500, 270);

	Renderer::init(&win);

	Font* font16 = new Font("arial.ttf", 16);
	Font* font20 = new Font("arial.ttf", 20);

	Rectangle rect = { 410, 220, 70, 30 };
	Layout* layout = new AbsoluteLayout(&win, 500, 270);

	Color white = { 1,1,1 };
	Color black = { 0,0,0 };
	Color grey = {0.8f, 0.8f, 0.8f};

	TextStyle bstyle = {
		20, font20, white
	};

	BasicButtonDescriptor bdesc = {
		bstyle, layout, func0
	};
	
	UI::buttons[0] = new Button(rect, "Submit", bdesc);
	rect.y -= 50;
	bdesc.callBack = func1;
	UI::buttons[1] = new Button(rect, "Submit", bdesc);
	rect.y -= 50;
	bdesc.callBack = func2;
	UI::buttons[2] = new Button(rect, "Submit", bdesc);
	rect.y -= 50;
	bdesc.callBack = func3;
	UI::buttons[3] = new Button(rect, "Submit", bdesc);
	rect.y -= 50;
	bdesc.callBack = func4;
	UI::buttons[4] = new Button(rect, "Submit", bdesc);
	rect.y -= 50;
	
	TextStyle tstyle = {
		16, font16, black
	};

	TextBoxDescriptor tdesc = {
		tstyle, layout,
		{ 0.75f, 0.75f, 0.75f },
		{ 0.2f, 0.2f, 0.2f },
		{ 0.2f, 0.2f, 0.2f },
		1, 2
	};

	rect.x = 20;
	rect.y = 220;
	rect.w = 370;
	rect.h = 30;
	
	for (int i = 0; i < 5;i++) {
		UI::texts[i] = new TextBox(rect, tdesc);
		rect.y -= 50;
	}
	
	while (UI::open && win.isOpen()) {
		win.poll();
		Renderer::clear(grey);

		for (int i = 0; i < 5;i++) {
			UI::buttons[i]->poll();
			UI::buttons[i]->render();

			UI::texts[i]->poll();
			UI::texts[i]->render();
		}

		win.swap();
	}

	UI::open = false;

	win.destroy();

	GLUI::destroy();
}

void UI::start() {
	thread =  new std::thread(run);
}

void UI::stop() {
	open = false;
	thread->join();
}