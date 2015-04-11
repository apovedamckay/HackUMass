#include <algorithm>
#include <memory>
#include <sstream>

#include "Views.h"
#include "Events.h"
#include "main.h"
#include "SDL_image.h"

Sprite LoadSprite(const char* file, SDL_Renderer* renderer)
{
	Sprite result;
	result.texture = NULL;
	result.position.w = 0;
	result.position.h = 0;
	result.position.x = 0;
	result.position.y = 0;
	result.scale = 1.f;
	result.angle = 0.f;

    SDL_Surface* temp;

    /* Load the sprite image */
    temp = IMG_Load(file);
    if (temp == NULL)
	{
        fprintf(stderr, "Couldn't load %s: %s\n", file, SDL_GetError());
        return result;
    }
    result.position.w = temp->w;
    result.position.h = temp->h;

    /* Create texture from the image */
    result.texture = SDL_CreateTextureFromSurface(renderer, temp);
    if (!result.texture) {
        fprintf(stderr, "Couldn't create texture: %s\n", SDL_GetError());
        SDL_FreeSurface(temp);
        return result;
    }
    SDL_FreeSurface(temp);

    return result;
}

LoadingView::LoadingView(EventController* controller)
	: begtime(0), loadingWheel(LoadSprite("loadingWheel.png", renderer))
{
    screen = loadImage("loadingScreen.png");
    SDL_GetWindowSize(window, &w, &h);
    loadingWheel.position = {7*w/16, 3*h/4, w/8, w/8};
}

bool LoadingView::activate(){
    //views.push_back(std::make_shared<CurLocationView>(&viewController));
    begtime = SDL_GetTicks();
    activated = true;
}

bool LoadingView::deactivate(){
    activated = false;
}

bool LoadingView::drawWorld(){
    if(!screen) return false;
    SDL_RenderCopy(renderer, screen, nullptr, nullptr);
    loadingWheel.draw();

    return !done;
}

bool LoadingView::updateWorld(){
    if(!activated) return false;
    SDL_Delay(15);
    static int counting;
    if(!loading) done = true;
    if (!counting) loadingWheel.angle += 30.f;
    counting = ++counting % 5;
    return !done;
}

WelcomeView::WelcomeView(EventController* controller)
: myController(controller), screen(loadImage("screen.png"))
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    meds.box = loadImage("medsBox.png");
    meds.position = SDL_Rect{w/8, 3*h/4, 3*w/4, h/4};
    tests.box = loadImage("testsBox.png");
    tests.position = SDL_Rect{w/8, h/2, 3*w/4, h/4};
}

WelcomeView::~WelcomeView(){
    SDL_DestroyTexture(screen);
    SDL_DestroyTexture(meds.box);
    SDL_DestroyTexture(tests.box);
}

bool WelcomeView::activate(){
    myEvents.push_back(std::make_shared<SelFDownEventProcesor>(myController, &meds));
    myEvents.push_back(std::make_shared<SelFDownEventProcesor>(myController, &tests));
    myEvents.push_back(std::make_shared<QuitKeyEventProcessor>(myController, this));
}

bool WelcomeView::updateWorld(){
    if(meds.selected){
    //    views.push_back(std::make_shared<MedsView>(myController));
    }
    else if(tests.selected){
        views.push_back(std::make_shared<TestView>(myController));
    }

    return !(meds.selected || tests.selected);
}

bool WelcomeView::drawWorld(){
    SDL_RenderCopy(renderer, screen, nullptr, nullptr);
    meds.draw();
    tests.draw();
    return !done;
}

bool WelcomeView::deactivate(){
}

TestView::TestView(EventController* controller)
: myController(controller), screen(loadImage("screenTest.png"))
{
}

TestView::~TestView(){
    SDL_DestroyTexture(screen);
    for(auto& b : tests.elements){
        SDL_DestroyTexture(b->box);
    }
}

bool TestView::activate(){
    SDL_GetWindowSize(window, &w, &h);
    for(int i=0; i<3; i++){
        InputBox* tBox = new InputBox;
        tBox->position = SDL_Rect{0, i*h/8 + h/8, 5*w/8, h/8};
        tBox->box = loadImage("textBox.png");
        tBox->font = TTF_OpenFont("Font.otf", h/12);
        myEvents.push_back(std::make_shared<InputEventProcessor>(myController, tBox));
        myEvents.push_back(std::make_shared<EditEventProcessor>(myController, tBox));
        myEvents.push_back(std::make_shared<InFDownEventProcessor>(myController, tBox));
        myEvents.push_back(std::make_shared<InKeyEventProcessor>(myController, tBox));
        tests.elements.push_back(tBox);
        tBox = new InputBox;
        tBox->position = SDL_Rect{5*w/8, i*h/8 + h/8, 3*w/8, h/8};
        tBox->box = loadImage("textBox.png");
        tBox->font = TTF_OpenFont("Font.otf", h/12);
        myEvents.push_back(std::make_shared<InputEventProcessor>(myController, tBox));
        myEvents.push_back(std::make_shared<EditEventProcessor>(myController, tBox));
        myEvents.push_back(std::make_shared<InFDownEventProcessor>(myController, tBox));
        myEvents.push_back(std::make_shared<InKeyEventProcessor>(myController, tBox));
        tests.elements.push_back(tBox);
    }

    //views.push_back(std::make_shared<RankView>(myController));
    views.push_back(std::make_shared<SleepingView>(myController));
    myEvents.push_back(std::make_shared<FVMotionEventProcessor>(myController, &tests));
    myEvents.push_back(std::make_shared<SwipeDownEventProcesor>(myController, this));
    myEvents.push_back(std::make_shared<QuitKeyEventProcessor>(myController, this));
}

bool TestView::updateWorld(){
    if(!tests.elements.back()->text.empty()){
        InputBox* tBox = new InputBox;
        tBox->position = SDL_Rect{0, tests.elements.size()*h/8 + h/4, 5*w/8, h/8};
        tBox->box = loadImage("textBox.png");
        tBox->font = TTF_OpenFont("Font.otf", h/12);
        myEvents.push_back(std::make_shared<InputEventProcessor>(myController, tBox));
        myEvents.push_back(std::make_shared<EditEventProcessor>(myController, tBox));
        myEvents.push_back(std::make_shared<InFDownEventProcessor>(myController, tBox));
        myEvents.push_back(std::make_shared<InKeyEventProcessor>(myController, tBox));
        tests.elements.push_back(tBox);
        tBox = new InputBox;
        tBox->position = SDL_Rect{5*w/8, (tests.elements.size()/2)*h/8 + h/4, 3*w/8, h/8};
        tBox->box = loadImage("textBox.png");
        tBox->font = TTF_OpenFont("Font.otf", h/12);
        myEvents.push_back(std::make_shared<InputEventProcessor>(myController, tBox));
        myEvents.push_back(std::make_shared<EditEventProcessor>(myController, tBox));
        myEvents.push_back(std::make_shared<InFDownEventProcessor>(myController, tBox));
        myEvents.push_back(std::make_shared<InKeyEventProcessor>(myController, tBox));
        tests.elements.push_back(tBox);

    }
    return !done;
}

bool TestView::drawWorld(){
    SDL_RenderCopy(renderer, screen, nullptr, nullptr);
    tests.draw();
    return !done;
}

bool TestView::deactivate(){
    for(int i=0; i<tests.elements.size()-1; i+=2){
        if(tests.elements[i]->text.empty() || tests.elements[i+1]->text.empty()) continue;
        testsJson[i/2]["subject"] = tests.elements[i]->text;
        testsJson[i/2]["time"] = tests.elements[i+1]->text;
    }
}

SpecificationsView::SpecificationsView(EventController* controller, int _place)
: myController(controller), screen(loadImage("screenSpecifications.png")), place(_place)
{
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    selected = loadImage("radioSelected.png");
    unselected = loadImage("radioUnselected.png");
    for(int i=0; i<3; i++){
        RadioButton* tButton = new RadioButton;
        tButton->position = SDL_Rect{w/8, 2*h/8 + i*h/8, 3*w/4, h/8};
        switch(i){
        case 0:
            tButton->box = loadImage("computer.png");
            break;
        case 1:
            tButton->box = loadImage("library.png");
            break;
        case 2:
            tButton->box = loadImage("tutor.png");
        }
        buttons.push_back(tButton);
    }
    subject.position = SDL_Rect{w/8, h/8, 7*w/8, h/8};
    subject.font = TTF_OpenFont("Font.otf", h/16);
    subject.boxColor = {0x1B, 0x51, 0x51, 0xFF};
    timeBox.position = SDL_Rect{w/8, 5*h/8, 3*w/4, h/8};
    timeBox.box = loadImage("textBox.png");
    timeBox.font = TTF_OpenFont("Font.otf", h/12);
}

SpecificationsView::~SpecificationsView(){
    SDL_DestroyTexture(screen);
}

bool SpecificationsView::activate(){
    subject.text = std::string("Final: ")+testsJson[place]["subject"].asString();
    for(auto& b: buttons) myEvents.push_back(std::make_shared<SelFDownEventProcesor>(myController, b));
    if(testsJson.size() > place+1) views.push_back(std::make_shared<SpecificationsView>(myController, place+1));
    myEvents.push_back(std::make_shared<InputEventProcessor>(myController, &timeBox));
    myEvents.push_back(std::make_shared<EditEventProcessor>(myController, &timeBox));
    myEvents.push_back(std::make_shared<InFDownEventProcessor>(myController, &timeBox));
    myEvents.push_back(std::make_shared<InKeyEventProcessor>(myController, &timeBox));
    myEvents.push_back(std::make_shared<SwipeDownEventProcesor>(myController, this));
    myEvents.push_back(std::make_shared<QuitKeyEventProcessor>(myController, this));
}

bool SpecificationsView::updateWorld(){
    for(auto& b : buttons){
        if(b->selected) b->radio = selected;
        else b->radio = unselected;
    }
    return !done;
}

SleepingView::SleepingView(EventController* controller)
: myController(controller), screen(loadImage("screenSleeping.png")),unselected(loadImage("radioUnselected.png")),selected(loadImage("radioSelected.png"))
{
    int w,h;
    SDL_GetWindowSize(window,&w,&h);
        RadioButton tRButton;
        tRButton.radio = unselected;
        tRButton.position = {w/8,3*h/8,3*w/4,h/8};
        tRButton.box = loadImage("insomnia.png");
        buttons.push_back(tRButton);
        tRButton.position = {w/8,4*h/8,3*w/4,h/8};
        tRButton.box = loadImage("naps.png");
        buttons.push_back(tRButton);
        tRButton.position = {w/8,5*h/8,3*w/4,h/8};
        tRButton.box = loadImage("daytime.png");
        buttons.push_back(tRButton);
}

SleepingView::~SleepingView(){
    SDL_DestroyTexture(screen);
    SDL_DestroyTexture(selected);
    SDL_DestroyTexture(unselected);
}

bool SleepingView::activate(){
    views.push_back(std::make_shared<SpecificationsView>(myController,0));
    myEvents.push_back(std::make_shared<QuitKeyEventProcessor>(myController, this));
    for(auto& r : buttons) myEvents.push_back(std::make_shared<SelFDownEventProcesor>(myController, &r));
    myEvents.push_back(std::make_shared<SwipeDownEventProcesor>(myController, this));

}

bool SleepingView::updateWorld(){
    bool sel = false;
    for(int i = 0; i<buttons.size();i++){
        if(buttons[i].selected){
            buttons[i].radio = selected;
        }else{
            buttons[i].radio = unselected;
        }
    }
    return !done;
}

bool SpecificationsView::drawWorld(){
    SDL_RenderCopy(renderer, screen, nullptr, nullptr);
    for(auto& b : buttons){
        b->draw();
    }
    subject.draw();
    timeBox.draw();
    return !done;
}

bool SpecificationsView::deactivate(){
    testsJson[place]["computer"] = buttons[0]->selected;
    testsJson[place]["library"] = buttons[0]->selected;
    testsJson[place]["tutor"] = buttons[0]->selected ? timeBox.text : std::string();
}

bool SleepingView::drawWorld(){
    SDL_RenderCopy(renderer,screen,nullptr,nullptr);
    for(auto& r : buttons){
        r.draw();
    }
    return !done;
}



bool SleepingView::deactivate(){
    insomnia = buttons[0].selected;
    naps = buttons[1].selected;
    daytime = buttons[2].selected;
}
