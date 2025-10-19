#include <SDL.h>
#include <SDL_ttf.h>
#include <string>
#include <stack>
#include <cmath>
#include <cctype>

const int SCREEN_WIDTH = 1024;
const int SCREEN_HEIGHT = 750;
const int MARGIN_X = 20;
const int MARGIN_Y = 20;
const int BUTTON_SPACING_X = 15;
const int BUTTON_SPACING_Y = 15;

struct Button {
    SDL_Rect rect;
    std::string label;
    SDL_Texture* texture;  // Cached texture
};

Button buttons[20];
std::string inputText = "";
int selectedButton = 0;

// --- Draw filled circle for smooth corners
void DrawFilledCircle(SDL_Renderer* renderer, int x0, int y0, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    for (int w = -radius; w <= radius; w++) {
        for (int h = -radius; h <= radius; h++) {
            if (w*w + h*h <= radius*radius) SDL_RenderDrawPoint(renderer, x0 + w, y0 + h);
        }
    }
}

// --- Rounded rectangle drawing
void DrawSmoothRoundedRect(SDL_Renderer* renderer, const SDL_Rect& rect, int radius, SDL_Color color) {
    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
    SDL_Rect mid = { rect.x + radius, rect.y, rect.w - 2*radius, rect.h };
    SDL_RenderFillRect(renderer, &mid);
    SDL_Rect left = { rect.x, rect.y + radius, radius, rect.h - 2*radius };
    SDL_Rect right = { rect.x + rect.w - radius, rect.y + radius, radius, rect.h - 2*radius };
    SDL_RenderFillRect(renderer, &left);
    SDL_RenderFillRect(renderer, &right);
    DrawFilledCircle(renderer, rect.x + radius, rect.y + radius, radius, color);
    DrawFilledCircle(renderer, rect.x + rect.w - radius - 1, rect.y + radius, radius, color);
    DrawFilledCircle(renderer, rect.x + radius, rect.y + rect.h - radius - 1, radius, color);
    DrawFilledCircle(renderer, rect.x + rect.w - radius - 1, rect.y + rect.h - radius - 1, radius, color);
}

// --- Init buttons (5×4 grid)
void InitButtons() {
    int cols = 5, rows = 4;
    int btnW = (SCREEN_WIDTH - 2*MARGIN_X - (cols-1)*BUTTON_SPACING_X) / cols;
    int btnH = (SCREEN_HEIGHT - 2*MARGIN_Y - 150 - (rows-1)*BUTTON_SPACING_Y) / rows;

    std::string labels[20] = {
        "7","8","9","/","C",
        "4","5","6","*","%",
        "1","2","3","-","(",
        "0",".","=","+",")"
    };

    for(int i=0;i<20;i++){
        int row = i / cols;
        int col = i % cols;
        buttons[i].rect = {
            MARGIN_X + col*(btnW + BUTTON_SPACING_X),
            MARGIN_Y + 150 + row*(btnH + BUTTON_SPACING_Y),
            btnW, btnH
        };
        buttons[i].label = labels[i];
        buttons[i].texture = nullptr;  // Will cache later
    }
}

// --- Expression evaluator
double EvaluateExpression(const std::string& s) {
    std::stack<double> values;
    std::stack<char> ops;

    auto applyOp = [&](char op) {
        if(values.size() < 2) return;
        double b = values.top(); values.pop();
        double a = values.top(); values.pop();
        switch(op){
            case '+': values.push(a+b); break;
            case '-': values.push(a-b); break;
            case '*': values.push(a*b); break;
            case '/': values.push(b != 0 ? a/b : 0); break;
            case '%': values.push(fmod(a,b)); break;
        }
    };

    auto prec = [](char op){ return (op=='+'||op=='-')?1:(op=='*'||op=='/'||op=='%')?2:0; };

    for(size_t i=0;i<s.size();){
        if(isspace(s[i])){ i++; continue; }
        if(isdigit(s[i]) || s[i]=='.'){
            size_t len=0;
            double val = std::stod(s.substr(i), &len);
            values.push(val);
            i += len;
        } else if(s[i]=='('){ ops.push('('); i++; }
        else if(s[i]==')'){
            while(!ops.empty() && ops.top()!='('){ applyOp(ops.top()); ops.pop(); }
            if(!ops.empty()) ops.pop();
            i++;
        } else {
            char op = s[i];
            while(!ops.empty() && prec(ops.top())>=prec(op)){ applyOp(ops.top()); ops.pop(); }
            ops.push(op);
            i++;
        }
    }
    while(!ops.empty()){ applyOp(ops.top()); ops.pop(); }
    return values.empty()?0:values.top();
}

int main(){
    SDL_Init(SDL_INIT_VIDEO | SDL_INIT_GAMECONTROLLER);
    TTF_Init();

    SDL_Window* win = SDL_CreateWindow("SDL Calculator", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
                                       SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_SHOWN);
    SDL_Renderer* ren = SDL_CreateRenderer(win, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    TTF_Font* font = TTF_OpenFont("./font.ttf", 48);
    if(!font){ printf("Font load failed: %s\n", TTF_GetError()); return 1; }

    InitButtons();

    // --- Cache button textures
    for(int i=0;i<20;i++){
        SDL_Surface* s = TTF_RenderText_Blended(font, buttons[i].label.c_str(), {255,255,255,255});
        buttons[i].texture = SDL_CreateTextureFromSurface(ren,s);
        SDL_FreeSurface(s);
    }

    SDL_GameController* ctrl = nullptr;
    for(int i=0;i<SDL_NumJoysticks();i++)
        if(SDL_IsGameController(i)){
            ctrl = SDL_GameControllerOpen(i);
            if(ctrl) printf("Controller: %s\n", SDL_GameControllerName(ctrl));
        }

    bool quit=false;
    SDL_Event e;
    Uint32 lastNav=0;
    int total=20;

    // --- Display caching
    bool dirty = true;
    SDL_Texture* inputTex = nullptr;
    SDL_Rect disp={MARGIN_X,MARGIN_Y,SCREEN_WIDTH-2*MARGIN_X,120};

    while(!quit){
        Uint32 now = SDL_GetTicks();

        while(SDL_PollEvent(&e)){
            if(e.type==SDL_QUIT) quit=true;

            if(e.type==SDL_CONTROLLERBUTTONDOWN){
                if(e.cbutton.button==SDL_CONTROLLER_BUTTON_B || e.cbutton.button==SDL_CONTROLLER_BUTTON_START) quit=true;

                if(now - lastNav > 150){
                    if(e.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_UP){ selectedButton-=5; if(selectedButton<0) selectedButton=0; dirty=true; lastNav=now; }
                    else if(e.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_DOWN){ selectedButton+=5; if(selectedButton>=total) selectedButton=total-1; dirty=true; lastNav=now; }
                    else if(e.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_LEFT){ selectedButton-=1; if(selectedButton<0) selectedButton=0; dirty=true; lastNav=now; }
                    else if(e.cbutton.button==SDL_CONTROLLER_BUTTON_DPAD_RIGHT){ selectedButton+=1; if(selectedButton>=total) selectedButton=total-1; dirty=true; lastNav=now; }
                }

                if(e.cbutton.button==SDL_CONTROLLER_BUTTON_A){
                    auto& b=buttons[selectedButton];
                    std::string lbl=b.label;
                    if(lbl=="C") inputText="";
                    else if(lbl=="<" && !inputText.empty()) inputText.pop_back();
                    else if(lbl=="="){
                        try { inputText = std::to_string(EvaluateExpression(inputText)); }
                        catch(...) { inputText="Error"; }
                    } else inputText += lbl;
                    dirty = true;
                }
            }

            if(e.type==SDL_MOUSEBUTTONDOWN){
                int mx=e.button.x,my=e.button.y;
                for(int i=0;i<total;i++){
                    auto& b=buttons[i];
                    if(mx>=b.rect.x && mx<=b.rect.x+b.rect.w && my>=b.rect.y && my<=b.rect.y+b.rect.h){
                        selectedButton=i;
                        std::string lbl=b.label;
                        if(lbl=="C") inputText="";
                        else if(lbl=="<" && !inputText.empty()) inputText.pop_back();
                        else if(lbl=="="){
                            try { inputText=std::to_string(EvaluateExpression(inputText)); }
                            catch(...) { inputText="Error"; }
                        } else inputText+=lbl;
                        dirty = true;
                    }
                }
            }
        }

        // --- Render only if dirty
        if(dirty){
            SDL_SetRenderDrawColor(ren,230,230,230,255);
            SDL_RenderClear(ren);

            // Display
            DrawSmoothRoundedRect(ren,disp,20,{255,255,255,255});
            if(inputTex) { SDL_DestroyTexture(inputTex); inputTex=nullptr; }
            if(!inputText.empty()){
                SDL_Surface* s=TTF_RenderText_Blended_Wrapped(font,inputText.c_str(),{0,0,0,255},disp.w-40);
                inputTex=SDL_CreateTextureFromSurface(ren,s);
                SDL_Rect tr={disp.x+20,disp.y+40,s->w,s->h};
                SDL_RenderCopy(ren,inputTex,NULL,&tr);
                SDL_FreeSurface(s);
            }

            // Buttons
            for(int i=0;i<total;i++){
                SDL_Color c = (i==selectedButton) ? SDL_Color{255,165,0,255} : SDL_Color{90,90,90,255};
                DrawSmoothRoundedRect(ren,buttons[i].rect,25,c);
                SDL_Rect tr={buttons[i].rect.x+(buttons[i].rect.w-0)/2,buttons[i].rect.y+(buttons[i].rect.h-0)/2,0,0};
                SDL_QueryTexture(buttons[i].texture,NULL,NULL,&tr.w,&tr.h);
                tr.x = buttons[i].rect.x + (buttons[i].rect.w - tr.w)/2;
                tr.y = buttons[i].rect.y + (buttons[i].rect.h - tr.h)/2;
                SDL_RenderCopy(ren,buttons[i].texture,NULL,&tr);
            }

            SDL_RenderPresent(ren);
            dirty=false;
        }

        SDL_Delay(10); // sleep to reduce CPU usage
    }

    // Cleanup
    for(int i=0;i<20;i++) if(buttons[i].texture) SDL_DestroyTexture(buttons[i].texture);
    if(inputTex) SDL_DestroyTexture(inputTex);
    if(ctrl) SDL_GameControllerClose(ctrl);
    TTF_CloseFont(font);
    SDL_DestroyRenderer(ren);
    SDL_DestroyWindow(win);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
