#ifndef _VIEWS_H_
#define _VIEWS_H_

#include "View.h"
#include "main.h"
#include "SDL.h"
#include "GUI.h"

class LoadingView : public View {
public:
    int w,h;
	SDL_Texture* screen;
	int begtime;
	Sprite loadingWheel;

	LoadingView(EventController* controller);

    virtual bool activate();
    virtual bool updateWorld();
    virtual bool drawWorld();
    virtual bool deactivate();

    virtual ~LoadingView(){
        if(screen) SDL_DestroyTexture(screen);
        screen = nullptr;
    }
};

class WelcomeView : public View{
protected:
    EventController* myController;
    SDL_Texture* screen;
    SelectionBox meds;
    SelectionBox tests;

public:
    WelcomeView(EventController* controller);
    ~WelcomeView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

class TestView : public View{
protected:
    EventController* myController;
    SDL_Texture* screen;
    VList tests;
    int w,h;

public:
    TestView(EventController* controller);
    ~TestView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

class RankView : public View{
protected:
    EventController* myController;
    SDL_Texture* screen;

public:
    RankView(EventController* controller);
    ~RankView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

class SleepingView : public View{
protected:
    EventController* myController;
    SDL_Texture* screen;

public:
    SleepingView(EventController* controller);
    ~SleepingView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

class RestrictionsView : public View{
protected:
    EventController* myController;
    SDL_Texture* screen;

public:
    RestrictionsView(EventController* controller);
    ~RestrictionsView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

class SpecificationsView : public View{
protected:
    EventController* myController;
    SDL_Texture* screen;
    SelectionBox subject;
    std::vector<RadioButton*> buttons;
    InputBox timeBox;
    int place;
    SDL_Texture* selected;
    SDL_Texture* unselected;

public:
    SpecificationsView(EventController* controller, int _place);
    ~SpecificationsView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

class FinalTimeView : public View{
protected:
    EventController* myController;
    SDL_Texture* screen;

public:
    FinalTimeView(EventController* controller);
    ~FinalTimeView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

class MedsView : public View{
protected:
    EventController* myController;
    SDL_Texture* screen;

public:
    MedsView(EventController* controller);
    ~MedsView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

class IntAndSideView : public View{
protected:
    EventController* myController;
    SDL_Texture* screen;

public:
    IntAndSideView(EventController* controller);
    ~IntAndSideView();

    bool activate();
    bool updateWorld();
    bool drawWorld();
    bool deactivate();
};

#endif // _VIEWS_H_
