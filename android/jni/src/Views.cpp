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
}
